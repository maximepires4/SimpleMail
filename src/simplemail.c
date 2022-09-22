/* SimpleMail.c */
/* Copyright 2022, Maxime Pires, All rights reserved */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>

#include <curl/curl.h>
#include <time.h>
#include <stdbool.h>

#define OPTSTR "rhvc:b:a:"
#define ERR_FOPEN_CONFIG "fopen(config, r)"
#define ERR_SENDMAIL "Error sending the mail"
#define DEFAULT_PROGNAME "simplemail"
#define DATE_LENGTH 31
#define MAX_SIZE 100

extern int errno;
extern char *optarg;
extern int opterr, optind;

typedef struct {
	char	*name;
	char	*username;
	char	*password;
	char	*smtp;
	char	*from;
	char	*to;
	char	*cc;
	char	*bcc;
	char	*subject;
	char	*body;
	char	*attachment;
} mail_t;

void usage(int status);
int sendmail(const mail_t *mail, bool verbose);
char* reformat_mail(const char* str, bool verbose);
char* generate_header_text(const mail_t *mail, bool verbose);
void generate_mail_from_config(mail_t *mail, bool reload);
void read_config(mail_t *mail, const char *config_file);
void create_config(const char *config_file);
void remove_trailing_space(char *str);

int main(int argc, char *argv[]){
	int opt;
	bool verbose = false;
	bool reload = false;

	mail_t *mail = calloc(1, sizeof(mail_t));

	opterr = 0;

	while((opt = getopt(argc, argv, OPTSTR)) != EOF){
		switch(opt) {
			case 'h':
				usage(EXIT_SUCCESS);
				/* NOTREACHED */
				break;

			case 'r':
				reload = true;
				break;
			
			case 'v':
				verbose = true;
				break;
			
			case 'c':
				mail->cc = reformat_mail(optarg, verbose);
				break;

			case 'b':
				mail->bcc = reformat_mail(optarg, verbose);
				break;

			case 'a':
				mail->attachment = malloc(strlen(optarg) + 1);
				mail->attachment = optarg;
				break;
				
			default:
				fprintf(stderr, DEFAULT_PROGNAME": Invalid option -- '%s'\n",optarg);
				usage(EXIT_FAILURE);
		}
	}

	generate_mail_from_config(mail, reload);

	argc -= optind;
	argv += optind;

	if(argc != 3){
		fprintf(stderr, DEFAULT_PROGNAME": Too few arguments\n");
		usage(EXIT_FAILURE);
		/* NOTREACHED */
	}

	mail->to = reformat_mail(argv[0], verbose);
	mail->subject = argv[1];
	mail->body = argv[2];

	if(verbose) {
		printf("SENDING MAIL...\n");
	}

	if(sendmail(mail, verbose) != EXIT_SUCCESS) {
		perror(ERR_SENDMAIL);
		exit(EXIT_FAILURE);
	}

	free(mail->name);
	free(mail->username);
	free(mail->password);
	free(mail->from);
	free(mail->smtp);
	free(mail->to);
	if(mail->cc) free(mail->cc);
	if(mail->bcc) free(mail->bcc);
	if(mail->attachment) free(mail->attachment);
	free(mail);

	printf(DEFAULT_PROGNAME": Successful\n");

	return EXIT_SUCCESS;
}

void usage(int status){
	if(status != EXIT_SUCCESS) {
		fprintf(stderr, "Try '"DEFAULT_PROGNAME" -h' for more information\n");
	} else {
		printf("Usage: "DEFAULT_PROGNAME" [OPTION]... TO SUBJECT CONTENT\n");
		printf("simplemail send an email to the address specified in TO, with the subject specified in SUBJECT and the content specified in CONTENT. Running simplemail for the first time causes the program to ask for the mail informations. These informations are saved at ~/.simplemailrc and won't be re-asked unless parameter -r is specified.\n");
		printf("\nParameters:\n\
			       	-r			Erases the config file and ask for the mail informations.\n\
				-h			Shows help.\n\
				-v			Shows what the program is doing.\n\
				-c [carbon-copy]	Add the specified address to the carbon copy.\n\
				-b [blind-carbon-copy]	Add the specified address to the blind carbon.\n\
				-a [attachment-file]	Add the specified file to the attachment.\n\
				");
		printf("\nPlease report any issues at the following issue tracker: https://github.com/Marouk4/SimpleMail/issues\n");
		printf("\nCopyright © 2022 Maxime Pires. License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\nThis is free software: you are free to change and redistribute it.  There is NO WARRANTY, to the extent permitted by law.\n");
		printf("\nGitHub page: https://github.com/Marouk4/SimpleMail\n");
	}

	exit(status);
	/* NOTREACHED */
}

int sendmail(const mail_t *mail, bool verbose){
	if(!mail){
		errno = EINVAL;
		return EXIT_FAILURE;
	}

	CURL *curl;
	CURLcode res = CURLE_OK;
	struct curl_slist *headers = NULL;
	struct curl_slist *recipients = NULL;
	struct curl_slist *slist = NULL;
	curl_mime *mime;
	curl_mime *alt;
	curl_mimepart *part;

	char *header_buffer = generate_header_text(mail, verbose);
	
	curl = curl_easy_init();
	if(curl) {
        	curl_easy_setopt(curl, CURLOPT_USERNAME, mail->username);
	        curl_easy_setopt(curl, CURLOPT_PASSWORD, mail->password);

	        curl_easy_setopt(curl, CURLOPT_URL, mail->smtp);

        	curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

	        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, mail->from);

        	recipients = curl_slist_append(recipients, mail->to);
	        if(mail->cc) recipients = curl_slist_append(recipients, mail->cc);
	        if(mail->bcc) recipients = curl_slist_append(recipients, mail->bcc);
	        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

		headers = curl_slist_append(headers, header_buffer);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		mime = curl_mime_init(curl);
		alt = curl_mime_init(curl);

		part = curl_mime_addpart(alt);
		curl_mime_data(part, mail->body, CURL_ZERO_TERMINATED);
		curl_mime_type(part, "text/plain");

		part = curl_mime_addpart(mime);
		curl_mime_subparts(part, alt);
		curl_mime_type(part, "multipart/alternative");
		slist = curl_slist_append(NULL, "Content-Disposition: inline");
		curl_mime_headers(part, slist, 1);

		if(mail->attachment) {
			part = curl_mime_addpart(mime);
			curl_mime_filedata(part, mail->attachment);
		}
		
		curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

	        res = curl_easy_perform(curl);

        	if(res != CURLE_OK) fprintf(stderr, "curl_easy_perform() failed: %s (%d)\n",curl_easy_strerror(res), res);

	        curl_slist_free_all(recipients);
		curl_slist_free_all(headers);

		curl_easy_cleanup(curl);

		curl_mime_free(mime);
	}

	free(header_buffer);
	return (int)res;
}

char* generate_header_text(const mail_t *mail, bool verbose){
	if (verbose) printf("GENERATING HEADER...\n");
		
	char *header_buffer = calloc(43 + DATE_LENGTH + strlen(mail->to) + strlen(mail->from) + strlen(mail->name) + (mail->cc ? strlen(mail->cc) : 0) + (mail->bcc ? strlen(mail->bcc) : 0) + strlen(mail->subject) + 1, sizeof(char));

	if(!header_buffer) {
		fprintf(stderr, "Error allocating memory for header_buffer");
	}
	
	time_t timestamp = time(NULL);
        struct tm *pTime = localtime(&timestamp);

        char date_buffer[DATE_LENGTH];
        strftime(date_buffer, DATE_LENGTH, "%a, %d %b %Y %H:%M:%S %z", pTime);

	sprintf(header_buffer, "Date: %s\nTo: %s\nFrom: %s (%s)\nCc: %s\nBcc: %s\nSubject: %s\n", date_buffer, mail->to, mail->from, mail->name, mail->cc ? mail->cc : "", mail->bcc ? mail->bcc : "", mail->subject);

	if(verbose) {
		printf("%s\nGENERATING HEADER: Done\n", header_buffer);
	}

	return header_buffer;
}

char* reformat_mail(const char* str, bool verbose){
	char *mail_address;

	if(str[0] == '<' && str[strlen(str)-1] == '>') {	
		mail_address = strdup(str);
	} else {
		if(verbose) printf("CONVERTING: %s -> <%s>\n",str,str);
		
		mail_address = calloc(strlen(str) + 2 +1, sizeof(char));

		if(!mail_address){
			fprintf(stderr, "Error allocating memory while reformating mail");
			exit(EXIT_FAILURE);
			/* NOTREACHED */	
		}

		sprintf(mail_address,"<%s>",str);	

		if(verbose) printf("%s\nCONVERTING: Done\n", mail_address);

	}
	
	return mail_address;
}

void generate_mail_from_config(mail_t *mail, bool reload){

	char *config_file = calloc(strlen(getenv("HOME")) + strlen(DEFAULT_PROGNAME) + 4 + 1, sizeof(char));
	sprintf(config_file, "%s/."DEFAULT_PROGNAME"rc", getenv("HOME"));

	if(access(config_file, F_OK) == -1 || reload) {
		create_config(config_file);
	}

	read_config(mail, config_file);

	free(config_file);
}

void read_config(mail_t *mail, const char *config_file){
	FILE *fp;
	char buffer[MAX_SIZE];

	if(!(fp = fopen(config_file,"r"))) {
		perror(ERR_FOPEN_CONFIG);
		exit(EXIT_FAILURE);
	}

	while(!feof(fp)){
		fgets(buffer, MAX_SIZE, fp);
		
		if(buffer[0] == '#') continue;

		char *key = calloc(MAX_SIZE, sizeof(char)), *value = calloc(MAX_SIZE, sizeof(char));
		bool line_started = false;
		int separator_index = -1;
		int i = 0, start_offset = 0;

		for(; buffer[i] != '\n'; i++){

			if(buffer[i] == ' ' && !line_started) {
				start_offset++;
				continue;
			}
			if(buffer[i] == '#') break;
			if(buffer[i] == '=') {
				remove_trailing_space(key);
				key = realloc(key, strlen(key) + 1);
				separator_index = i;
				line_started = false;
				start_offset = 0;
				continue;
			}

			line_started = true;

			if(separator_index == -1){
				key[i-start_offset] = buffer[i];
			} else {
				value[i-start_offset-separator_index-1] = buffer[i];
			}
		}

		value = realloc(value, strlen(value) + 1);
		remove_trailing_space(value);

		if(strcmp(key,"NAME") == 0) {
			mail->name = strdup(value);

		} else if(strcmp(key,"USERNAME") == 0) {
			mail->username = strdup(value);

		} else if(strcmp(key,"PASSWORD") == 0) {
			mail->password = strdup(value);

		} else if(strcmp(key,"MAIL") == 0) {
			mail->from = reformat_mail(value, false);

		} else if(strcmp(key,"SMTP") == 0) {
			mail->smtp = strdup(value);
		}

		free(key);
		free(value);
	}

	fclose(fp);

	if(!mail->name || !mail->username || !mail->password || !mail->from || !mail->smtp){
		fprintf(stderr, DEFAULT_PROGNAME": Error parsing config file\nTry '"DEFAULT_PROGNAME" -r' for reloading the config file\n");
		exit(EXIT_FAILURE);
	}
}

void remove_trailing_space(char *str){

	if(!str) return;

        int trailing_space_count = 0;

        while(str[strlen(str)-trailing_space_count-1] == ' '){
                trailing_space_count++;
        }

	str[strlen(str)-trailing_space_count] = '\0';
}

void create_config(const char *config_file){
	
	printf("***** No config file detected, creating one *****\n");
	
	FILE *fp;

	if(!(fp = fopen(config_file,"w"))) {
                perror(ERR_FOPEN_CONFIG);
                exit(EXIT_FAILURE);
        }

	char buffer[MAX_SIZE];

	printf("Please enter your name: ");
	fgets(buffer, MAX_SIZE, stdin);
	buffer[strcspn(buffer, "\n")] = 0;
	fprintf(fp, "NAME=%s", buffer);

	printf("Please enter your username: ");
	fgets(buffer, MAX_SIZE, stdin);
	buffer[strcspn(buffer, "\n")] = 0;
	fprintf(fp, "\nUSERNAME=%s", buffer);

	printf("Please enter your password: ");
	fgets(buffer, MAX_SIZE, stdin);
	buffer[strcspn(buffer, "\n")] = 0;
	fprintf(fp, "\nPASSWORD=%s", buffer);

	printf("Please enter your mail: ");
	fgets(buffer, MAX_SIZE, stdin);
	buffer[strcspn(buffer, "\n")] = 0;
	fprintf(fp, "\nMAIL=%s", buffer);

	printf("Please enter the smtp server: ");
	fgets(buffer, MAX_SIZE, stdin);
	buffer[strcspn(buffer, "\n")] = 0;
	fprintf(fp, "\nSMTP=%s", buffer);

	printf("Please enter the port: ");
	fgets(buffer, MAX_SIZE, stdin);
	buffer[strcspn(buffer, "\n")] = 0;
	fprintf(fp,":%s", buffer);

	fclose(fp);
	printf("The config file has been successfully created, feel free to edit it at ~/.simplemailrc (becareful of unwanted spaces!)\n");

}
