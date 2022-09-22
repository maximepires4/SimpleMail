# Testing cURL
# MAKEFILE

# Commands
CP = cp
RM = rm
RMFLAG = -f
GZIP = gzip

# C Compiler (Default: gcc)
CC = gcc
CFLAG = -Wall

# Library 
LIBCURL = -lcurl

# Details
SOURCE = src/simplemail.c
OUT = simplemail
MANLOC = /usr/share/man/man1
PROGLOC = /usr/local/bin
PROGNAME = simplemail

all: build maninstall install

build: $(SOURCE)
	$(CC) $(SOURCE) $(CFLAG) $(LIBCURL) -o $(OUT)

maninstall:
	$(CP) $(PROGNAME).man1 $(PROGNAME).1
	$(GZIP) $(PROGNAME).1
	$(CP) $(PROGNAME).1.gz $(MANLOC)

install: build
	$(CP) $(OUT) $(PROGLOC)

clean:
	$(RM) $(RMFLAG) $(OUT) $(PROGNAME).1.gz

uninstall:
	$(RM) $(PROGLOC)/$(OUT) $(MANLOC)/$(PROGNAME).1.gz
