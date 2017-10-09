#include "server.h"

using namespace std;

void start_listening(int port, const char* htdocs_dir) {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
      std::cerr << "ERROR opening socket" << std::endl;
      return;
    }

    struct sockaddr_in server_address;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    if(bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
      std::cerr << "ERROR binding socket" << std::endl;
      return;
    }
    std::cout << "Listening " << port << " port" << std::endl;

    if (listen(server_socket, 25) < 0) {
      std::cerr << "ERROR listen" << std::endl;
      return;
    }

    int client_socket;
    struct sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);
    while(true) {
      client_socket = accept(server_socket, (struct sockaddr*) &client_address, &client_address_size);
      if(client_socket < 0) {
        std::cerr << "ERROR " << std::endl;
          return;
      }
      process_connection(client_socket, &client_address, htdocs_dir);
    }
}


void process_connection(int client_socket, const struct sockaddr_in *client_address, const char* htdocs_dir) {
    char client_ip[INET_ADDRSTRLEN];
    const struct in_addr client_ip_addr = client_address->sin_addr;
    inet_ntop(AF_INET, &(client_ip_addr), client_ip, sizeof(client_ip));
    const uint16_t client_port = ntohs(client_address->sin_port);
    pid_t pid = fork();
    if (0 == pid) {
      interact_connection(client_socket, client_ip, client_port, htdocs_dir);
      exit(0);
    } else {
      close(client_socket);
    }
}


void interact_connection(int client_socket, const char* client_ip, const uint16_t client_port, const char* htdocs_dir) {
    char recv_buffer[16536];
    memset(recv_buffer, 0, sizeof(recv_buffer));
    ssize_t bytes_received;
    char requested_url[2000];
    enum request_method method = UNKNOWN;
    bytes_received = read(client_socket, recv_buffer, sizeof(recv_buffer));
    parse_request(recv_buffer, &method, requested_url, sizeof(requested_url));
    write_access_log(client_ip, client_port, method, requested_url);

    if (strcmp(requested_url + strlen(requested_url) - 3, "cgi") == 0) {
        char** argv = (char**)malloc(sizeof(char**));
        char** envp;
        char str_socket[20];
        sprintf(str_socket, "%d", client_socket);
        argv[0] = str_socket;

        char script_name[2048];
        memcpy(script_name, htdocs_dir, strlen(htdocs_dir));
        strcat(script_name, requested_url);

        execve(script_name, argv, envp);
    } else if (method == GET) do_GET(client_socket, requested_url, client_ip, client_port, htdocs_dir);
      else if (method == POST) do_POST(client_socket, requested_url, client_ip, client_port, htdocs_dir);

    close(client_socket);
}
