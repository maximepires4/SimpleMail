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

#define OPTSTR "hvc:b:a:"
#define USAGE_FMT "%s [-h] [-v] [-c carbonCopy] [-b blindCarbonCopy] [-a attachmentFile] TO SUBJECT CONTENT\n"
#define ERR_FOPEN_CONFIG "fopen(config, r)"
#define ERR_SENDMAIL "Error sending the mail"
#define DEFAULT_PROGNAME "simplemail"
#define DATE_LENGTH 31

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

void usage(char *progname, int opt);
int sendmail(const mail_t *mail, bool verbose);
char* reformat_mail(const char* str, bool verbose);
char* generate_header_text(const mail_t *mail, bool verbose);
mail_t *handle_config();
mail_t *read_config(const char *config_file);
void create_config();
void remove_spaces(char *str);

int main(int argc, char *argv[]){
	int opt;
	bool verbose = false;
	mail_t *mail = handle_config();

	opterr = 0;

	while((opt = getopt(argc, argv, OPTSTR)) != EOF){
		switch(opt) {
			case 'h':
				usage(basename(argv[0]), opt);
				/* NOTREACHED */
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
				mail->attachment = optarg;
				break;
		}
	}

	argc -= optind;
	argv += optind;

	if(argc != 3){
		usage(basename(argv[0]), opt);
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
	free(mail);

	if(verbose) {
		printf("SENDING MAIL: Successful\n");
	}

	return EXIT_SUCCESS;
}

void usage(char *progname, int opt){
	fprintf(stderr, USAGE_FMT, progname?progname:DEFAULT_PROGNAME);
	exit(EXIT_FAILURE);
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

	char *cc_buffer;
	char *bcc_buffer;

	if(mail->cc){
		cc_buffer = mail->cc;
	} else {
		cc_buffer = malloc(sizeof(char));
		if(!cc_buffer) {
			fprintf(stderr, "Error allocating memory for cc_buffer");
		}
		cc_buffer = "";
	}

	if(mail->bcc){
		bcc_buffer = mail->bcc;
	} else {
		bcc_buffer = malloc(sizeof(char));
		if(!bcc_buffer) {
			fprintf(stderr, "Error allocating memory for bcc_buffer");
		}
		bcc_buffer = "";
	}

	char *header_buffer = malloc((43 + DATE_LENGTH + strlen(mail->to) + strlen(mail->from) + strlen(mail->name) + strlen(cc_buffer) + strlen(bcc_buffer) + strlen(mail->subject) + 1) * sizeof(char));

	if(!header_buffer) {
		fprintf(stderr, "Error allocating memory for header_buffer");
	}
	
	time_t timestamp = time(NULL);
        struct tm *pTime = localtime(&timestamp);

        char date_buffer[DATE_LENGTH];
        strftime(date_buffer, DATE_LENGTH, "%a, %d %b %Y %H:%M:%S %z", pTime);

	sprintf(header_buffer, "Date: %s\nTo: %s\nFrom: %s (%s)\nCc: %s\nBcc: %s\nSubject: %s\n", date_buffer, mail->to, mail->from, mail->name, cc_buffer, bcc_buffer, mail->subject);

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
		
		mail_address = malloc(sizeof(char) * (strlen(str) + 2) +1);

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

mail_t *handle_config(){

	char *config_file = malloc(strlen(getenv("HOME") + strlen(DEFAULT_PROGNAME) + 4 + 1));
	strcat(config_file, getenv("HOME"));
	strcat(config_file, "/."DEFAULT_PROGNAME"rc");

//	if(!access(DEFAULT_CONFIG_FILE, F_OK)) create_config();

	return read_config(config_file);
}

mail_t *read_config(const char *config_file){
	FILE *fp;
	char buffer[256];

	if(!(fp = fopen(config_file,"r"))) {
		perror(ERR_FOPEN_CONFIG);
		exit(EXIT_FAILURE);
	}

	mail_t *mail = malloc(sizeof(mail_t));

	while(!feof(fp)){
		fgets(buffer, 256, fp);

		char *key = malloc(256), *value = malloc(256);
		
		if(buffer[0] == '#') continue;

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
				key = (char *)realloc(key, i * sizeof(char));
				remove_spaces(key);
				separator_index = i;
				continue;
			}

			line_started = true;

			if(separator_index == -1){
				key[i-start_offset] = buffer[i];
			} else {
				value[i-separator_index-1] = buffer[i];
			}
		}

		value = (char *)realloc(value, (i-separator_index-1));
		printf("|%s|\n",value);
		remove_spaces(value);
		printf("|%s|\n",value);

		//printf("|%s| |%s|\n",key,value);

		if(strcmp(key,"NAME") == 0) {
			mail->name = malloc(strlen(value) + 1);
			mail->name = strdup(value);

		} else if(strcmp(key,"USERNAME") == 0) {
			mail->username = malloc(strlen(value) + 1);
			mail->username = strdup(value);

		} else if(strcmp(key,"PASSWORD") == 0) {
			mail->password = malloc(strlen(value) + 1);
			mail->password = strdup(value);

		} else if(strcmp(key,"MAIL") == 0) {
			mail->from = reformat_mail(value, false);

		} else if(strcmp(key,"SMTP") == 0) {
			mail->smtp = malloc(strlen(value) + 1);
			mail->smtp = strdup(value);
		}

	}

	return mail;
}

void remove_spaces(char *str){

        int spaces_start = 0, spaces_end = 0;

	printf("<%c>\n",str[0]);

        while(str[spaces_start] == ' '){
                spaces_start++;
        }

        while(str[strlen(str)-spaces_end-1] == ' '){
                spaces_end++;
        }

        str += spaces_start;
        str[strlen(str)-spaces_end] = '\0';
}
