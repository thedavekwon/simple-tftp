//
// Created by Do Hyung Kwon on 4/24/20.
//

#ifndef SIMPLE_TFTP_TFTP_SERVER_H
#define SIMPLE_TFTP_TFTP_SERVER_H

#include <fstream>
#include <netinet/in.h>
#include <strings.h>

#include "tftp_packet.h"

class tftp_server {
private:
    int server_socket_fd, new_socket_fd{};
    struct sockaddr_in server_addr{}, client_addr{};
    char data[TFTP_DATA_SIZE]{};
    wchar_t packet_num;
    bool WRQ{};

    std::ifstream file_read;
    std::ofstream file_write;

    tftp_packet current_packet;
public:
    tftp_server(int port);
    bool prepare_packet();
    void run();
};


#endif //SIMPLE_TFTP_TFTP_SERVER_H
