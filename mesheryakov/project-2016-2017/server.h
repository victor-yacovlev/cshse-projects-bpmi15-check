#include "service.h"

#include <arpa/inet.h>
#include <unistd.h>

void start_listening(int port, const char* htdocs_dir);
void process_connection(int client_socket, const struct sockaddr_in *client_address, const char* htdocs_dir);
void interact_connection(int client_socket, const char* client_ip, const uint16_t client_port, const char* htdocs_dir);
