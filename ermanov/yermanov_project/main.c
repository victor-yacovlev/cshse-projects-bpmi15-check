#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/wait.h>

static void parse_command(const char * recv_buffer, char * command, char * command_url, char * command_arg, int command_argc);
static void write_headers_or_error(int client_socket, int status, size_t content_length, const char * restrict data);
static void do_POST(int client_socket, const char * restrict url, const char * restrict arg, const char * restrict client_ip, const uint16_t client_port, const char * restrict htdocs_dir);
static void do_HEAD(int client_socket, const char * restrict url, const char * restrict client_ip, const uint16_t client_port, const char * restrict htdocs_dir);
static void do_GET(int client_socket, const char * restrict url, const char * restrict arg, const char * restrict client_ip, const uint16_t client_port, const char * restrict htdocs_dir);
static void interact_connection(int client_socket, const char * restrict client_ip, const uint16_t client_port, const char * restrict htdocs_dir);
static void process_connection(int client_socket, const struct sockaddr_in * restrict client_address, const char * restrict htdocs_dir);

static void start_listening(
    const uint16_t port_no,
    const char* restrict htdocs_dir)
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0); 
    struct sockaddr_in server_address; 
    server_address.sin_addr.s_addr = INADDR_ANY; 
    server_address.sin_family = AF_INET; 
    server_address.sin_port = htons(port_no); 
    
    bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));
    listen(server_socket, SOMAXCONN); 
    int client_socket;
    struct sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);
    while (1) {
        client_socket = accept( 
            server_socket, 
            (struct sockaddr*) &client_address, 
            &client_address_size);
        process_connection(client_socket, &client_address, htdocs_dir);
    }
}

static void process_connection(
    int client_socket,
    const struct sockaddr_in * restrict client_address,
    const char * restrict htdocs_dir)
{
    char client_ip[INET_ADDRSTRLEN]; 
    const uint32_t client_ip_addr = client_address->sin_addr.s_addr; 
    inet_ntop(AF_INET, &(client_ip_addr), client_ip, sizeof(client_ip));
    const uint16_t client_port = ntohs(client_address->sin_port); 
    //write_access_log(client_ip, client_port, "CONNECT", "", "OK");          ????????????????????
    interact_connection(client_socket, client_ip, client_port, htdocs_dir);
    close(client_socket); 
}

static void interact_connection(
    int client_socket,
    const char * restrict client_ip,
    const uint16_t client_port,
    const char * restrict htdocs_dir)
{
    char recv_buffer[16536];
    memset(recv_buffer, 0, sizeof(recv_buffer));
    ssize_t bytes_received;
    char command_url[2000],  command[30] = "UNKNOWN", command_arg[2000];
    bytes_received = read(client_socket, recv_buffer, sizeof(recv_buffer));
    //write(client_socket, recv_buffer, sizeof(recv_buffer));
    parse_command(recv_buffer, command, command_url, command_arg, bytes_received);
    //printf("%s\n", command);
    if (strcmp("GET", command) == 0) {
        do_GET(client_socket, command_url, command_arg, client_ip, client_port, htdocs_dir);
    } else if (strcmp("POST", command) == 0) {
        do_POST(client_socket, command_url, command_arg, client_ip, client_port, htdocs_dir);
    } else if (strcmp("HEAD", command) == 0) {
        do_HEAD(client_socket, command_url, client_ip, client_port, htdocs_dir);
    }
}

static void do_GET(
    int client_socket,
    const char * restrict url,
    const char * restrict arg,
    const char * restrict client_ip,
    const uint16_t client_port,
    const char * restrict htdocs_dir)
{
    char file_name[2048];
    memset(file_name, 0, sizeof(file_name));
    strcpy(file_name, htdocs_dir);
    //printf("%s GET\n", file_name);
    printf("%s\n", arg);
    if (0==strcmp("/",url) || 0 == strlen(url)) {
        strcat(file_name, "/index.html");
    } else {
        strcat(file_name, "/");
        strcat(file_name, url);
    }

    int f_id = open(file_name, O_RDONLY);
    printf("%s GET\n\n", file_name);
    if (access(file_name, X_OK) == 0) {
        int pid1;
        pid1 = fork();
        if (!pid1) {
            const char *envp[] = {arg,NULL};
            dup2(client_socket, 1);
            close(client_socket);
            execle(file_name, file_name, NULL, envp);
            //free(envp);
        }
        close(client_socket);
        int status;
        wait(&status);
    } else {
        if (-1 == f_id) {
            write_headers_or_error(client_socket, 404, 0, "Not Found");
        } else {
            write_headers_or_error(client_socket, 200, 0, "file");
            char file_buffer[65536];
            while (1) {
                ssize_t bytes_read = read(f_id, file_buffer, sizeof(file_buffer));
                if (bytes_read > 0) { 
                    ssize_t bytes_written = write(client_socket, file_buffer, bytes_read);
                } else {
                    break;
                }
            }
            close(f_id);
        }
    }
}

static void do_HEAD(
        int client_socket,
        const char * restrict url,
        const char * restrict client_ip,
        const uint16_t client_port,
        const char * restrict htdocs_dir)
{
    //Empty
}

static void do_POST(
        int client_socket,
        const char * restrict url,
        const char * restrict arg,
        const char * restrict client_ip,
        const uint16_t client_port,
        const char * restrict htdocs_dir)
{
    char file_name[2048];
    strcpy(file_name, htdocs_dir);
    if (0==strcmp("/",url) || 0 == strlen(url)) {
        strcat(file_name, "/index.html");
    } else {
        strcat(file_name, "/");
        strcat(file_name, url);
    }
    printf("%s POST\n", file_name);

    int pid1;
    pid1 = fork();
    if (!pid1) {
        const char *envp[] = {arg, NULL};
        dup2(client_socket, 1);
        dup2(client_socket, 0);
        close(client_socket);
        execle(file_name, file_name, NULL, envp);
        //free(envp);
    }
    close(client_socket);
    int status;
    wait(&status);
}

static void write_headers_or_error(
    int client_socket,
    int status,
    size_t content_length,
    const char * restrict data)
{
    char headers[2048];
    if (200 == status) {
        sprintf(headers, "HTTP/1.0 200 OK\r\n" 
                        "Server: Simple HTTP Server\r\n" 
                        "Content-Type: %s\r\n" 
                        "Content-Length: %lu\r\n" 
                        "\r\n", 
                        data, content_length);
    } else {
        sprintf(headers, "HTTP/1.0 %d %s\r\n" 
                        "Server: Simple HTTP Server\r\n"
                        "Content-Type: text/plain\r\n"
                        "Content-Length: %lu\r\n"
                        "\r\n"
                        "%s", 
                        status, data, strlen(data), data);
    }
    write(client_socket, headers, strlen(headers));
}

static void parse_command(
    const char * recv_buffer,
    char * command,
    char * command_arg,
    char * command_arg2,
    int command_argc)
{
    //for (int h = 0; h < command_argc; ++h) {
    //    printf("%c", recv_buffer[h]);
    //}
    //printf("\n");
    int i = 0;
    while (i < command_argc && recv_buffer[i] == ' ') {
        ++i;
    }
    if (recv_buffer[i] == 'G') {
        strcpy(command, "GET");
    }
    if (recv_buffer[i] == 'P') {
        strcpy(command, "POST");
    }
    if (recv_buffer[i] == 'H') {
        strcpy(command, "HEAD");
    }
    //printf("%s\n", command);
    while (i < command_argc && recv_buffer[i] != ' ') {
        ++i;
    }
    while (i < command_argc && recv_buffer[i] == ' ') {
        ++i;
    }
    int j = 0;
    //printf("First: %c\n", recv_buffer[i]);
    while (i + j < command_argc && recv_buffer[i + j] != ' ' && recv_buffer[i + j] != '\n' && recv_buffer[i + j] != '\0' && recv_buffer[i + j] != '?' && recv_buffer[i + j] != '\r') {
        ++j;
    }
    //printf("Las: %d\n", recv_buffer[i + j - 1]);
    if (j > 0)
        strncpy(command_arg, recv_buffer + i, j);
    command_arg[j] = 0; 
    if (recv_buffer[i + j] == '?') {
        i += j + 1;
        j = 0;
        while (i + j < command_argc && recv_buffer[i + j] != ' ' && recv_buffer[i + j] != '\n' && recv_buffer[i + j] != '\0' && recv_buffer[i + j] != '&' && recv_buffer[i + j] != '\r') {
            ++j;
        }
        if (j > 0) {
            strncpy(command_arg2, recv_buffer + i, j);
            command_arg2[j] = 0;
        } 
	} else command_arg2[0] = 0;
    
    //printf("%s", command_arg);
}


int
main(int argc, char* argv[])
{
    //printf("%d\n", atoi(argv[2]));
    start_listening(atoi(argv[2]), argv[1]);
}
