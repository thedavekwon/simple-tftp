//
// Created by Do Hyung Kwon on 4/24/20.
//

#include "../include/tftp_packet.h"


void tftp_packet::generate_req_packet(char* filename, std::string mode, char rw) {
    buf.resize(TFTP_MAX_PACKET_SIZE, 0);
    int filename_size = strlen(filename);
    buf[1] = rw;
    buf.insert(buf.begin()+2, filename, filename+strlen(filename));
    size_t idx = 2+filename_size+1;
    buf.insert(buf.begin()+idx, mode.begin(), mode.end());
    len = 2+filename_size+1+mode.size()+1;
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
}

bool tftp_packet::receive_packet(struct sockaddr_in &server_addr, int sockfd, bool server) {
    socklen_t serverlen = sizeof(server_addr);
    int receive_status = recvfrom(sockfd, buf.data(), len, 0, reinterpret_cast<sockaddr *>(&server_addr), &serverlen);
    if (receive_status == 0) {
        if (!server) {
            perror("Connection Closed");
            exit(EXIT_FAILURE);
        }
        return false;
    }
    else if (receive_status < 0) {
        if (!server) {
            perror("Failed to receive packet");
            exit(EXIT_FAILURE);
        }
        return false;
    }
    len = receive_status;
    return true;
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

std::string tftp_packet::get_filename() const {
    return find_nth_word(buf, 1, 2);
}

std::string tftp_packet::get_mode() const {
    return find_nth_word(buf, 2, 2);
}

std::string tftp_packet::get_error_message() const {
    return find_nth_word(buf, 1, 4);
}

std::string find_nth_word(const std::vector<char> &words, int n, int start_idx) {
    int cnt = 0;
    for (int i = start_idx; i < words.size(); i++) {
        if (words[i] == 0) {
            if (++cnt == n) {
                return std::string(words.begin()+start_idx, words.begin()+i);
            }
            start_idx = i;
        }
    }
    return std::string();
}
