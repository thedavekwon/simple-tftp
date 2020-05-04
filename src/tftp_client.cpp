//
// Created by Do Hyung Kwon on 4/24/20.
//

#include "../include/tftp_client.h"

tftp_client::tftp_client(char* host, int port) {
    packet_num = 0;
    // create new socket
    client_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket_fd < 0) {
        perror("Failed to open socket");
        exit(EXIT_FAILURE);
    }
    // initialize
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(host);
}

void tftp_client::run(int mode, char* filename) {
    if (mode == TFTP_FILE_GET) {
        if (get_file(filename)) {
            return;
        }
    } else if (mode == TFTP_FILE_SEND) {
        if (send_file(filename)) {
            return;
        }
    }
    perror("Operation Failed");
    exit(EXIT_FAILURE);
}

bool tftp_client::get_file(char* filename) {
    bool last_file = false;
    file_write = std::ofstream(filename, std::ifstream::binary);
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

                // error_code position = packet_num position
                wchar_t error_code = current_packet.get_packet_num();
                switch (error_code) {
                    case TFTP_ERROR_NOTDEFINED:
                        std::cerr << current_packet.get_error_message() << std::endl;
                        break;
                    case TFTP_ERROR_FILENOTFOUND:
                        std::cerr << "File Not Found" << std::endl;
                        break;
                    case TFTP_ERROR_ACCESSVIOLATION:
                        std::cerr << "Access Violation" << std::endl;
                        break;
                    case TFTP_ERROR_DISKFULL:
                        std::cerr << "DISK FULL" << std::endl;
                        break;
                    case TFTP_ERROR_ILLEGALOPERATION:
                        std::cerr << "Illegal TFTP Operation" << std::endl;
                        break;
                    case TFTP_ERROR_UNKWONTRANSFERID:
                        std::cerr << "Unknown Transfer ID" << std::endl;
                        break;
                    case TFTP_ERROR_FILEALREADYEXIST:
                        std::cerr << "File Already Exist" << std::endl;
                        break;
                    case TFTP_ERROR_NOSUCHUSER:
                        std::cerr << "No Such User" << std::endl;
                        break;
                    default:
                        break;
                }
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
            std::cout << "received " << current_packet.len << " bytes" << std::endl;
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
bool tftp_client::send_file(char* filename) {
    file_read = std::ifstream(filename, std::ifstream::binary);
    if (!file_read.is_open() || !file_read.good()) {
        return false;
    }
    // send WRQ request
    current_packet.generate_req_packet(filename, TFTP_MODE_NETASCII, TFTP_OPCODE_WRQ);
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
    current_packet.receive_packet(server_addr, client_socket_fd, false);
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cerr << "Usage: ./program host port filename mode\n" << "mode 0: send, 1: get" << std::endl;
        exit(EXIT_FAILURE);
    }

    char* host = argv[1];
    int port = atoi(argv[2]);
    char* filename = argv[3];
    auto client = tftp_client(host, port);
    int mode = atoi(argv[4]);

    client.run(mode, filename);
}