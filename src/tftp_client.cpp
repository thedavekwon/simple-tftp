//
// Created by dodo on 4/24/20.
//

#include "../include/tftp_client.h"

tftp_client::tftp_client(std::string host, int port) {
    packet_num = 0;
    // create new socket
    client_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket_fd < 0) {
        perror("Failed to open socket");
        exit(-1);
    }
    // initialize
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(host.c_str());

    // connect to server
//     if (connect(client_socket_fd, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr)) < 0) {
//        perror("Failed to connect to server");
//        exit(-1);
//    }
}

void tftp_client::run(int mode, const std::string &filename, const std::string &dest) {
    if (mode == TFTP_FILE_GET) {
        if (get_file(filename, dest)) {
            return;
        }
    } else if (mode == TFTP_FILE_SEND) {
        if (send_file(filename, dest)) {
            return;
        }
    }
    perror("Operation Failed");
    exit(-1);
}

// TODO Error Handling
bool tftp_client::get_file(const std::string &filename, const std::string &dest) {
    bool last_file = false;
    file_write = std::ofstream(dest, std::ifstream::binary);
    if (!file_write.is_open() || !file_write.good()) {
        return false;
    }
    // send RRQ request
    current_packet.generate_req_packet(filename, TFTP_MODE_NETASCII, TFTP_OPCODE_RRQ);
    current_packet.send_packet(server_addr, client_socket_fd);

    while (true) {
        // wait for ACK
        if (wait_for_packet()) {
            if (current_packet.isERROR()) {
                std::cerr << "Error from Server" << std::endl;
                file_write.close();
                return false;
            }
            if (current_packet.get_packet_num() != ++packet_num) {
                std::cerr << "Wrong Packet Number: received " << current_packet.get_packet_num() << " expected "
                << packet_num << std::endl;
                packet_num--;
                continue;
            }
            file_write.write(current_packet.buf.data()+4, current_packet.len-4);
            std::cout << current_packet.len << std::endl;
            if (current_packet.len - 4 < TFTP_DATA_SIZE) last_file = true;
            current_packet.generate_ack_packet(packet_num);
            current_packet.send_packet(server_addr, client_socket_fd);
            if (last_file) break;
        } else {
            std::cerr << "Server Time Out" << std::endl;
            file_write.close();
            return false;
        }
    }

    file_write.close();
    return true;
}

// TODO Error Handling
bool tftp_client::send_file(const std::string &filename, const std::string &dest) {
    file_read = std::ifstream(filename, std::ifstream::binary);
    char data[TFTP_DATA_SIZE];
    if (!file_read.is_open() || !file_read.good()) {
        return false;
    }
    // send WRQ request
    current_packet.generate_req_packet(dest, TFTP_MODE_NETASCII, TFTP_OPCODE_WRQ);
    current_packet.send_packet(server_addr, client_socket_fd);

    while (true) {
        // wait for ACK
        if (wait_for_packet()) {
            if (current_packet.isERROR()) {
                std::cerr << "Error from Server" << std::endl;
                file_read.close();
                return false;
            }
            file_read.read(data, TFTP_DATA_SIZE);
            // generate data packet
            current_packet.generate_data_packet(data, packet_num++, file_read.gcount());
            current_packet.send_packet(server_addr, client_socket_fd);
            if (file_read.eof()) break;
        } else {
            std::cerr << "Server Time Out" << std::endl;
            file_read.close();
            return false;
        }
    }

    file_read.close();
    return true;
}

bool tftp_client::wait_for_packet() {
    current_packet.clear();
    current_packet.receive_packet(server_addr, client_socket_fd);
    return true;
}

int main() {
    int port = 69;
//    std::string host, filename, dest;
    std::string host = "127.0.0.1";
    std::string filename = "testfile";
    std::string dest = "testfile";
    auto client = tftp_client(host, port);
    client.run(TFTP_FILE_SEND, filename, dest);
}