#include "server.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
      std::cerr << "Usage: ./start_server port root_dir" << std::endl;
      return 1;
    }
    start_listening(std::stoi(argv[1]), argv[2]);
}
