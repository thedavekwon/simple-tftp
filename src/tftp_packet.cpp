//
// Created by dodo on 4/24/20.
//

#include "../include/tftp_packet.h"


void tftp_packet::generate_req_packet(std::string filename, std::string mode, char rw) {
    buf.resize(TFTP_MAX_PACKET_SIZE, 0);
    buf[1] = rw;
    buf.insert(buf.begin()+2, filename.begin(), filename.end());
    size_t idx = 2+filename.size()+1;
    buf.insert(buf.begin()+idx, mode.begin(), mode.end());
    len = 2+filename.size()+1+mode.size()+1;
}


void tftp_packet::generate_data_packet(char* data, wchar_t packet_num, size_t data_len) {
    buf.resize(TFTP_MAX_PACKET_SIZE, 0);
    buf[1] = TFTP_OPCODE_DATA;
    buf[2] = (packet_num >> 8) & 0xFF;
    buf[3] = packet_num & 0xFF;
    buf.insert(buf.begin()+4, data, data+data_len);
    len = 4 + data_len;
}


void tftp_packet::generate_ack_packet(wchar_t packet_num) {
    buf.resize(TFTP_MAX_PACKET_SIZE, 0);
    buf[1] = TFTP_OPCODE_ACK;
    buf[2] = (packet_num >> 8) & 0xFF;
    buf[3] = packet_num & 0xFF;
    len = 4;
}

void tftp_packet::generate_error_packet(char error_code, std::string error_msg) {
    buf.resize(TFTP_MAX_PACKET_SIZE, 0);
    buf[1] = TFTP_OPCODE_ERROR;
    buf[3] = error_code;
    buf.insert(buf.begin()+4, error_msg.begin(), error_msg.end());
    len = 4+error_msg.size()+1;
}

bool tftp_packet::isRRQ() {
    return buf[1] == TFTP_OPCODE_RRQ;
}

bool tftp_packet::isWRQ() {
    return buf[1] == TFTP_OPCODE_WRQ;
}

bool tftp_packet::isDATA() {
    return buf[1] == TFTP_OPCODE_DATA;
}

bool tftp_packet::isACK() {
    return buf[1] == TFTP_OPCODE_ACK;
}

bool tftp_packet::isERROR() {
    return buf[1] == TFTP_OPCODE_ERROR;
}

void tftp_packet::send_packet(struct sockaddr_in &server_addr, int sockfd) {
    sendto(sockfd, buf.data(), len, 0, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr));
    // TODO check send
}
void tftp_packet::receive_packet(struct sockaddr_in &server_addr, int sockfd) {
    socklen_t serverlen = sizeof(server_addr);
    int receive_status = recvfrom(sockfd, buf.data(), len, 0, reinterpret_cast<sockaddr *>(&server_addr), &serverlen);
    if (receive_status == 0)
        perror("Connection Closed");
    else if (receive_status < 0)
        perror("Failed to receive packet");
    len = receive_status;
}

tftp_packet::tftp_packet() {
    buf = std::vector<char>(TFTP_MAX_PACKET_SIZE);
    len = 0;
}

wchar_t tftp_packet::get_packet_num() {
    return buf[2] << 8 | buf[3];
}

void tftp_packet::clear() {
    buf.resize(TFTP_MAX_PACKET_SIZE, 0);
    len = TFTP_MAX_PACKET_SIZE;
}
