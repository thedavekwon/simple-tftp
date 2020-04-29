//
// Created by dodo on 4/24/20.
//

#include "../include/tftp_server.h"

tftp_server::tftp_server(int port) {
    packet_cnt = 0;
    // initiate socket
    server_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket_fd < 0)
        perror("Failed to open socket");
    // zero fill
    bzero((char *) &server_addr, sizeof(server_addr));

    // setup socket ip address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // bind socket to addr
    if (bind(server_socket_fd, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) < 0)
        perror("Failed to bind socket");

    if (listen(server_socket_fd, 1) < 0)
       perror("Failed to listen to socket");

    new_socket_fd = accept(server_socket_fd,
                           reinterpret_cast<sockaddr *>(&client_addr),
                           reinterpret_cast<socklen_t *>(sizeof(client_addr)));
    if (new_socket_fd < 0)
        perror("Failed to accept socket");
}

void tftp_server::run() {
    while (true) {
        current_packet.receive_packet(client_addr, new_socket_fd);
        if(!prepare_packet())
            continue;
        send_packet();
        if (current_packet.len == 0) break;
    }
}

bool tftp_server::prepare_packet() {
    // initial
    if (packet_cnt == 0) {
        if (current_packet.isWRQ()) {

        } else if (current_packet.isRRQ()) {

        } else {
            return false;
        }
    } else {

    }
}

int main() {
    int port;
    auto server = tftp_server(port);
    server.run();
}