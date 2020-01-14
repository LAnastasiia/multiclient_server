#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <zconf.h>
#include <arpa/inet.h>
#include <bitset>

#define BUFFER_SIZE (1024)

int main(int argc, char** argv){

    // Initialize client socket.
    int sd = socket (AF_INET, SOCK_STREAM, 0);
    if (sd == -1)
        return EXIT_FAILURE;

    struct sockaddr_in sock_addr_server;
    memset(&sock_addr_server, 0, sizeof(sock_addr_server));
    sock_addr_server.sin_family = AF_INET;
    sock_addr_server .sin_port = htons(2233);

    if (inet_aton(argv[1], &sock_addr_server.sin_addr) == 0){
        std::cerr << "Couldn't convert the server-address [xx.xx.xx.xx] from host to network(binary) form." << std::endl;
        return EXIT_FAILURE;
    }

    // request connection
    int res = connect(sd, reinterpret_cast<const sockaddr *>(&sock_addr_server), sizeof( sock_addr_server ));
    if (res == -1) {
        std::cerr << "Couldn't connect to server by " << inet_ntoa(sock_addr_server.sin_addr) << " address." << std::endl;
        return EXIT_FAILURE;
    }


    std::string response;
    while (true) {
        char* input_buf = static_cast<char *>(malloc(BUFFER_SIZE));
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