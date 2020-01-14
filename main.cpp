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
#include <fstream>

#include "services/services.h"

#define PORT (2233)
#define BACKLOG_NUM (5)
#define BUFFER_LEN (1024)
#define JOURNAL_ENTRY_LEN (50)
#define TIME_ENTRY_LEN (20)
#define DBG (true)


int main() {
    int this_sd = socket(AF_INET, SOCK_STREAM, 0);  // by default protocol for SOCK_STREAM would be TCP
    if (this_sd == -1){
        std::cerr << "Failed to initialize server socket." << std::endl;
        _exit(EXIT_FAILURE);
    }

    int reuseaddr = 1;
    if (setsockopt(this_sd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int))) {
        std::cerr << "Warning! failed setting SO_REUSEADDR option." << std::endl;
        _exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(sockaddr_in));
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(this_sd, (struct sockaddr *)(&server_addr), sizeof(sockaddr_in)) < 0) {
        std::cerr << "Couldn't bind server to address. | errno = " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    int journal_fd = open("../journal.csv", O_WRONLY | O_CREAT | O_APPEND);
    if (journal_fd == -1){
        std::cerr << "Failed to open the client journal. | errno = " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    listen(this_sd, BACKLOG_NUM);

    struct sockaddr_in handler_addr;
    socklen_t handler_addr_len;
    handler_addr_len = sizeof(sockaddr_in);

    while (true) {
        int handler_sd = accept(this_sd, (struct sockaddr *) &handler_addr, &handler_addr_len);
        if (handler_sd < 0) {
            std::cerr << "Failed to accept new client connection to " << std::endl; continue;
        }
        int handler_pid = fork();

        if (handler_pid == -1) {
            std::cerr << "Failed to fork()" << std::endl; break;

        } else if (handler_pid == 0) {
            close(this_sd);
            char message[] = "Hello";
            send(handler_sd, message, strlen(message), 0);

            // On POSIX systems, it's the write operations from several processes are safe iff the file was open with O_APPEND flag.
            char* journal_entry = static_cast<char *>(malloc(JOURNAL_ENTRY_LEN));

            // Get client's IP address.
            char* cli_addr_str = static_cast<char *>(malloc(INET_ADDRSTRLEN));
            inet_ntop(AF_INET, &handler_addr.sin_addr, cli_addr_str, INET_ADDRSTRLEN);

            // Get client's access time.
            char* cli_time_str = (static_cast<char *>(malloc(TIME_ENTRY_LEN)));
            get_time_str(cli_time_str);

            strncpy(journal_entry, cli_addr_str, strlen(cli_addr_str));
            journal_entry[strlen(cli_addr_str)] = ',';
            strncpy(journal_entry+strlen(cli_addr_str)+1, cli_time_str, strlen(cli_time_str));
            journal_entry[strlen(cli_addr_str) + strlen(cli_time_str) + 1] = '\n';

            if (write(journal_fd, journal_entry, strlen(journal_entry)) != strlen(journal_entry)){
                std::cerr << "Failed to store this entry to journal." << std::endl;
                exit(EXIT_FAILURE);
            }
            if (DBG) std::cout << cli_time_str << "\t" << cli_addr_str << std::endl;

            free(cli_addr_str); free(cli_time_str); free(journal_entry);
            close(handler_sd);
            exit(EXIT_SUCCESS);

        } else close(handler_sd);
    }

    close(this_sd);
    return EXIT_FAILURE;
}


// std::ofstream journal("./journal.csv", std::ofstream::out | std::ofstream::app);
// if ( !journal.is_open() || journal.rdstate() ){}
// journal << cli_addr_str << "\t" << get_time_str();
// journal.close();
