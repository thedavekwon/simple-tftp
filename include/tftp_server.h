//
// Created by dodo on 4/24/20.
//

#ifndef SIMPLE_TFTP_TFTP_SERVER_H
#define SIMPLE_TFTP_TFTP_SERVER_H

#include <fstream>
#include <netinet/in.h>
#include <strings.h>

#include "tftp_packet.h"

class tftp_server {
private:
    int server_socket_fd, new_socket_fd, packet_cnt;
    struct sockaddr_in server_addr{}, client_addr{};

    tftp_packet current_packet;

    char buffer[TFTP_MAX_PACKET_SIZE]{};
    std::ifstream file_read;
    std::ofstream file_write;
public:
    tftp_server(int port);
    bool prepare_packet();
    void run();
};


#endif //SIMPLE_TFTP_TFTP_SERVER_H
