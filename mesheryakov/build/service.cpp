#include "service.h"

#define HEADER_SIZE 2048


int get_file_size(const char *file_name) {
    struct stat buf;
    stat(file_name, &buf);
    return buf.st_size;
}


const char *get_mime_type(char *file_name) {
    char* ext = strchr(file_name, '.');
    while (strchr(ext+1, '.') != NULL) ext = strchr(ext+1, '.');

    if (!strcmp(ext, ".html")) return "text/html";
    if (!strcmp(ext, ".txt")) return "text/plain";
    if (!strcmp(ext, ".jpg")) return "image/jpeg";
    if (!strcmp(ext, ".jpeg")) return "image/jpeg";
    if (!strcmp(ext, ".css")) return "text/css";
    if (!strcmp(ext, ".js")) return "application/javascript";
    if (!strcmp(ext, ".exe")) return "application/x-msdownload";
}


const char* get_status_text_by_code(int status) {
    if (status == 200) return "OK";
    if (status == 404) return "Not Found";
    if (status == 500) return "Internal Server Error";
}

const char* get_method_text_by_enum(enum request_method method) {
    if (method == GET) return "GET";
    if (method == POST) return "POST";
    if (method = UNKNOWN) return "UNKNOWN";
}

void write_access_log(const char* ip, uint16_t port, enum request_method method, char* url) {
    fprintf(stderr, "%s %s %s:%" PRIu16 "\n", get_method_text_by_enum(method), url, ip, port);
}


void parse_request(const char *recv_buffer, enum request_method *method, char *requested_url, size_t bufsize) {
    if (memcmp(recv_buffer, "GET", 3) == 0) {
	    *method = GET;
	    int idx = 3;
	    while (recv_buffer[idx] && recv_buffer[idx] != '/' && recv_buffer[idx] != '?') idx++;
	    sscanf(recv_buffer + idx, "%s", requested_url);

    } else if (memcmp(recv_buffer, "POST", 4) == 0) {
      *method = POST;
    } else
      *method = UNKNOWN;
}



void do_GET(int client_socket, const char* url, const char* client_ip, const uint16_t client_port, const char* htdocs_dir) {
    char file_name[2048];
    memcpy(file_name, htdocs_dir, strlen(htdocs_dir));
    if (strcmp("/",url)==0 || strlen(url)==0) strcat(file_name, "/index.html");
    else strcat(file_name, url);

    int f_id = open(file_name, O_RDONLY);

    if (f_id == -1) {
      write_headers_or_error(client_socket, 404, 0, "text/html", "PAGE NOT FOUND BRO!");
    } else {
      write_headers_or_error(client_socket, 200, get_file_size(file_name), get_mime_type(file_name), "");
      char file_buffer[65536];
      while (1) {
        ssize_t bytes_read = read(f_id, file_buffer, sizeof(file_buffer));
        if (bytes_read > 0) {
          ssize_t bytes_written = write(client_socket, file_buffer, bytes_read);
        } else break;
      }
      close(f_id);
    }
}


void do_POST(int client_socket, const char* url, const char* client_ip, const uint16_t client_port, const char* htdocs_dir) {
    write_headers_or_error(client_socket, 200, 0, "text/html", "<?xml version=\"1.0\" encoding=\"UTF-8\"?><status>SUCCESS</status>");
}


void write_headers_or_error(int client_socket, int status, size_t content_length, const char* datatype, const char* data) {
    char headers[HEADER_SIZE];

    sprintf(headers,
      "HTTP/1.0 %d %s\r\n"
      "Server: Simple HTTP Server\r\n"
      "Content-Type: %s\r\n"
      "Content-Length: %zu\r\n"
      "\r\n"
      "%s",
      status, get_status_text_by_code(status), datatype, content_length, data);

    write(client_socket, headers, strlen(headers));
}
