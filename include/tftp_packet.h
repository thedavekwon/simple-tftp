//
// Created by dodo on 4/24/20.
//

#ifndef SIMPLE_TFTP_TFTP_PACKET_H
#define SIMPLE_TFTP_TFTP_PACKET_H

#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <fstream>
#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define TFTP_OPCODE_RRQ 1
#define TFTP_OPCODE_WRQ 2
#define TFTP_OPCODE_DATA 3
#define TFTP_OPCODE_ACK 4
#define TFTP_OPCODE_ERROR 5

#define TFTP_MAX_PACKET_SIZE 1024
#define TFTP_DATA_SIZE 512
#define TFTP_DEFAULT_TIMEOUT 1

#define TFTP_ERROR_NOTDEFINED 0
#define TFTP_ERROR_FILENOTFOUND 1
#define TFTP_ERROR_ACCESSVIOLATION 2
#define TFTP_ERROR_DISKFULL 3
#define TFTP_ERROR_ILLEGALOPERATION 4
#define TFTP_ERROR_UNKWONTRANSFERID 5
#define TFTP_ERROR_FILEALREADYEXIST 6
#define TFTP_ERROR_NOSUCHUSER 7

#define TFTP_MODE_OCTET "octet"
#define TFTP_MODE_NETASCII "netascii"
#define TFTP_MODE_MAIL "mail"

class tftp_packet {
public:
    std::vector<char> buf;
    size_t len;

    bool isRRQ();
    bool isWRQ();
    bool isDATA();
    bool isACK();
    bool isERROR();

    tftp_packet();
    wchar_t get_packet_num();
    void clear();
    void send_packet(struct sockaddr_in &server_addr, int sockfd);
    void receive_packet(struct sockaddr_in &server_addr, int sockfd);
    void generate_req_packet(std::string filename, std::string mode, char rw);
    void generate_data_packet(char* data, wchar_t packet_num, size_t len);
    void generate_ack_packet(wchar_t packet_num);
    void generate_error_packet(char error_code, std::string error_msg);
};

#endif //SIMPLE_TFTP_TFTP_PACKET_H
