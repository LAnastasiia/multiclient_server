// Варіант 4

// Написати багатокористувацький сервер привітань, який кожному клієнту передає текст “Hello” та завершує зв’язок,
// запускаючи (за допомогою fork()) новий процес для кожного клієнта.
// Сервер повинен вести журнал клієнтів – файл із списком, з якого IP, в який момент часу було з’єднання.


#include <netinet/in.h>
#include <iostream>
#include <cstring>  // bzero
#include <zconf.h>  // fork
#include <arpa/inet.h>
#include <fcntl.h>

#include "services/services.h"

#define PORT (2233)
#define BACKLOG_NUM (5)
#define BUFFER_SIZE (1024)
#define JOURNAL_NAME ("journal.txt")
#define REUSE (1)


int main() {

    int this_sd = socket(AF_INET, SOCK_STREAM, 0);  // by default protocol for SOCK_STREAM would be TCP
    if (this_sd == -1){
        std::cerr << "Failed to initialize server socket." << std::endl;
        return EXIT_FAILURE;
    }

    int so_reuseaddr = REUSE;
    if (setsockopt(this_sd, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr, sizeof(int)) < 0)
        std::cerr << "Warning! setsockopt(SO_REUSEADDR) failed.";

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    int bind_status = bind(this_sd, reinterpret_cast<const sockaddr *>(&server_addr), sizeof(server_addr));
    if (bind_status == -1) {
        std::cout << "Couldn't bind server to address." << std::endl;
        return EXIT_FAILURE;
    }

    int j_fd = open(JOURNAL_NAME, O_APPEND);

    listen(this_sd, BACKLOG_NUM);

    char input_buff[BUFFER_SIZE];
    sockaddr_in handler_addr;
    socklen_t handler_addr_len;

    while (true) {

        handler_addr_len = sizeof(handler_addr);
        int handler_sd = accept(this_sd, (struct sockaddr *) &handler_addr, &handler_addr_len);
        if (handler_sd < 0) {
            std::cerr << "Failed to accept new client connection to " << inet_ntoa(handler_addr.sin_addr) << std::endl;
            continue;
        }

        int handler_pid = fork();
        if (handler_pid < 0){
            std::cerr << "Failed to fork()" << std::endl;

        } else if (handler_pid == 0)  {
            close(this_sd);
            char message[] = "Hello!";
            send(handler_sd, message, strlen(message), 0);
            exit(0);

        } else {
            std::cout << inet_ntoa(handler_addr.sin_addr) <<  get_time_str();


//            ssize_t num_writen = write(j_fd, new_entry.c_str(), strlen(new_entry.c_str()));
//            if (num_writen != new_entry.length()){
//                std::cout << num_writen << " | " << new_entry.length();
//                std::cerr << "Failed to store new connection record." << std::endl;
//                return EXIT_FAILURE;
//            }
            close(handler_sd);
        }
    }

    return EXIT_SUCCESS;
}