    //
// Created by Do Hyung Kwon on 4/24/20.
//

#ifndef SIMPLE_TFTP_TFTP_CLIENT_H
#define SIMPLE_TFTP_TFTP_CLIENT_H

#include "tftp_packet.h"

#define TFTP_FILE_SEND 0
#define TFTP_FILE_GET  1

class tftp_client {
private:
    int client_socket_fd;
    wchar_t packet_num;
    struct sockaddr_in server_addr{};
    char data[TFTP_DATA_SIZE]{};

    std::ifstream file_read;
    std::ofstream file_write;

    tftp_packet current_packet;
public:
    tftp_client(char* host, int port);
    bool get_file(char* filename);
    bool send_file(char* filename);
    bool wait_for_packet();
    void run(int mode, char* filename);
};


#endif //SIMPLE_TFTP_TFTP_CLIENT_H
