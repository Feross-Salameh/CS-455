#include "Daiane.hpp"
#include <stdio.h>
#include <stdlib.h>
//#include <netinet/in.h>
#include <sys/types.h>
#include <time.h>

#define MAX 8192 // Maximum lenght of the text line

//Functions
void logging(struct sockaddr_in *sockaddr_in);
void file_log_proxy(char *stringlogging, struct sockaddr_in *sockaddr_in, char *url, int size);

int log() {
	//The file to be writing
	FILE *file_log_proxy;

	file_log_proxy = fopen("proxylogging.log", "a");
	file_log_proxy = fopen("proxylogging.log", "w");

	fclose(file_log_proxy);
}

void logging(struct sockaddr_in *sockaddr_in) {
	int size = 0;
	char entrylog[MAX], url[MAX];
	
	entrylogformat(entrylog, sockaddr_in, url, size);
	fprintf(file_log_proxy, "%s %d \n", entrylog, size);
	fflush(file_log_proxy);
}

void entrylogformat(char *stringlogging, struct sockaddr_in *sockaddr_in, char *url, int size) {
	time_t now;
	char strtime[MAX];
	unsigned char x, y, w, z;
	unsigned long host;
	unsigned long sockaddr;

	now = time(NULL);
	strftime(strtime, MAX, "%a %d %b %Y %H: %M: %S %Z", localtime(&now));

	host = ntohl(sockaddr);
	x = (host >> 24;
	y = (host >> 16) & 0xff;
	w = (host >> 8) & 0xff;
	z = host & 0xff;

	//Return the entire string log
	sprintf(stringlogging, "%s: %d.%d.%d.%d %s", strtime, x, y, w, z, url);
}