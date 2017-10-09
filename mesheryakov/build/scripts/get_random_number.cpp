#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "iostream"

int main (int argc, char *argv[]) {
    char random_number[100];
    srand (time(NULL));
    sprintf(random_number, "%d", rand() % 100 + 1);

    FILE * file_stream = fdopen(atoi(argv[0]), "w");
  	fprintf(file_stream,
      "HTTP/1.0 200 OK\r\n"
  		"Server: Simple HTTP Server\r\n"
  		"Content-Type: text/html\r\n"
  		"Content-Length: %ld\r\n"
  		"\r\n"
  		"%s",
  		strlen(random_number), random_number);

    return 0;
}
