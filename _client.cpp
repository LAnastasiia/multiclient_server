#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <zconf.h>
#include <arpa/inet.h>
#include <bitset>

#define BUFFER_LEN (1024)
#define NUM_TESTS (7)
#define PORT (2233)

int main(int argc, char** argv){

    if(argc < 2) {
        std::cerr << "Too few arguments." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Initialize client socket.
    int sd = socket (AF_INET, SOCK_STREAM, 0);
    if (sd == -1)
        return EXIT_FAILURE;

    struct sockaddr_in sock_addr_server;
    memset(&sock_addr_server, 0, sizeof(sock_addr_server));
    sock_addr_server.sin_family = AF_INET;
    sock_addr_server .sin_port = htons(PORT);
    // sock_addr_server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (inet_aton(argv[1], &sock_addr_server.sin_addr) == 0){
        std::cerr << "Couldn't convert the server-address [xx.xx.xx.xx] from host to network(binary) form." << std::endl;
        return EXIT_FAILURE;
    }

    for (int _ = 0; _ < NUM_TESTS; _++){

        if(fork() == 0){

            // request connection
            int res = connect(sd, reinterpret_cast<const sockaddr *>(&sock_addr_server), sizeof( sock_addr_server ));
            if (res == -1) {
                std::cerr << "Couldn't connect to server by " << inet_ntoa(sock_addr_server.sin_addr) << " address." << std::endl;
                return EXIT_FAILURE;
            }

            std::string response;
            while (true) {
                char* input_buf = static_cast<char *>(malloc(BUFFER_LEN));
                ssize_t num_read = recv(sd, input_buf, sizeof(input_buf), 0);
                if (num_read <= 0)
                    break;
                response.append(input_buf);
                free(input_buf);
            }

            std::cout << "response: " << response << std::endl;

            close(sd);
            return EXIT_SUCCESS;
        }
    }
}