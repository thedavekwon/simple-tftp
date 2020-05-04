//
// Created by Do Hyung Kwon on 4/24/20.
//

#include "../include/tftp_server.h"

tftp_server::tftp_server(int port) {
    packet_num = 0;
    // initiate socket
    server_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket_fd < 0) {
        perror("Failed to open socket");
        exit(EXIT_FAILURE);
    }
    // zero fill
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // bind socket to addr
    if (bind(server_socket_fd, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) < 0) {
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }
}

void tftp_server::run() {
    while (true) {
        current_packet.clear();
        current_packet.receive_packet(server_addr, server_socket_fd, true);
        if(!prepare_packet())
            break;
        current_packet.send_packet(server_addr, server_socket_fd);
    }
}

bool tftp_server::prepare_packet() {
    bool last_file = false;
    // initial
    if (packet_num == 0) {
        if (current_packet.isWRQ()) {
            WRQ = true;
            const std::string filename = current_packet.get_filename();
            const std::string mode = current_packet.get_mode();
            std::cout << "Receive Filename: " << filename << std::endl;
            file_write = std::ofstream(filename, std::ifstream::binary);
            if (!file_write.is_open() || !file_write.good()) {
                // generate error depending on fstream error number
                current_packet.generate_error_packet(TFTP_ERROR_NOTDEFINED, "fstream error");
                current_packet.send_packet(server_addr, server_socket_fd);
                return false;
            }
            current_packet.generate_ack_packet(packet_num);
        } else if (current_packet.isRRQ()) {
            WRQ = false;
            const std::string filename = current_packet.get_filename();
            const std::string mode = current_packet.get_mode();
            std::cout << "Send Filename: " << filename << std::endl;
            file_read = std::ifstream(filename, std::ifstream::binary);
            if (!file_read.is_open() || !file_write.good()) {
                // generate error depending on fstream error number
                current_packet.generate_error_packet(TFTP_ERROR_NOTDEFINED, "fstream error");
                current_packet.send_packet(server_addr, server_socket_fd);
                return false;
            }
            file_read.read(data, TFTP_DATA_SIZE);
            current_packet.generate_data_packet(data, ++packet_num, file_read.gcount());
        } else {
            return false;
        }
    } else {
    // after connection
        if (WRQ) {
            if (!file_write.is_open() || !file_write.good()) {
                // generate error
                current_packet.generate_error_packet(TFTP_ERROR_NOTDEFINED, "fstream error");
                current_packet.send_packet(server_addr, server_socket_fd);
                return false;
            }
            file_write.write(current_packet.buf.data()+4, current_packet.len-4);
            std::cout << "received " << current_packet.len << " bytes" << std::endl;

            // last packet
            if (current_packet.len - 4 < TFTP_DATA_SIZE) last_file = true;
            current_packet.generate_ack_packet(packet_num);
            if (last_file) return false;
        } else {
            if (!file_read.is_open() || !file_write.good()) {
                // generate error
                current_packet.generate_error_packet(TFTP_ERROR_NOTDEFINED, "fstream error");
                current_packet.send_packet(server_addr, server_socket_fd);
                return false;
            }
            // last packet
            if (file_read.eof()) return false;
            file_read.read(data, TFTP_DATA_SIZE);
            current_packet.generate_data_packet(data, packet_num, file_read.gcount());
        }
    }
    packet_num++;
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./program port(int)" << std::endl;
        exit(EXIT_FAILURE);
    }
    int port = atoi(argv[1]);
    auto server = tftp_server(port);
    server.run();
}