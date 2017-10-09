#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <iostream>
#include <fstream>

enum request_method {UNKNOWN, GET, POST};

int get_file_size(const char*);
const char *get_mime_type(char *file_name);
const char* get_status_text_by_code(int status);
const char* get_method_text_by_enum(enum request_method method);
void write_access_log(const char* ip, uint16_t port, enum request_method method, char* url);
void write_headers_or_error(int client_socket, int status, size_t content_length, const char* datatype, const char* data);
void parse_request(const char *recv_buffer, enum request_method *method, char *requested_url, size_t bufsize);
void do_GET(int client_socket, const char* url, const char* client_ip, const uint16_t client_port, const char* htdocs_dir);
void do_POST(int client_socket, const char* url, const char* client_ip, const uint16_t client_port, const char* htdocs_dir);
