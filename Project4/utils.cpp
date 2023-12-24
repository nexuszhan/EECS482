#include "utils.h"
#include <iostream>

using std::cout;

int get_port_number(int sockfd) 
{
    struct sockaddr_in addr;
    socklen_t length = sizeof(addr);
    if (getsockname(sockfd, (struct sockaddr *) &addr, &length) == -1) {
        // perror("Error getting port of socket");
        return -1;
    }
    // Use ntohs to convert from network byte order to host byte order.
    return ntohs(addr.sin_port);
}

int make_server_sockaddr(struct sockaddr_in *addr, int port) 
{
    // Step (1): specify socket family.
    // This is an internet socket.
    addr->sin_family = AF_INET;

    // Step (2): specify socket address (hostname).
    // The socket will be a server, so it will only be listening.
    // Let the OS map it to the correct address.
    addr->sin_addr.s_addr = htonl(INADDR_ANY);

    // Step (3): Set the port value.
    // If port is 0, the OS will choose the port for us.
    // Use htons to convert from local byte order to network byte order.
    addr->sin_port = htons(port);

    return 0;
}

int setup_socket(int & port, int queue_size)
{
    // (1) Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sockfd == -1) {
        // perror("Error opening stream socket");
        return -1;
    }

    // (2) Set the "reuse port" socket option
    int yesval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yesval, sizeof(yesval)) == -1) {
        // perror("Error setting socket options");
        return -1;
    }

    // (3) Create a sockaddr_in struct for the proper port and bind() to it.
    struct sockaddr_in addr;
    if (make_server_sockaddr(&addr, port) == -1) {
        return -1;
    }

    // (3b) Bind to the port.
    if (bind(sockfd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1) {
        // perror("Error binding stream socket");
        return -1;
    }

    // (3c) Detect which port was chosen.
    port = get_port_number(sockfd);
    // printf("Server listening on port %d...\n", port);

    // (4) Begin listening for incoming connections.
    if (listen(sockfd, queue_size) == -1) {
        // perror("Error listening");
        return -1;
    }

    return sockfd;
}

void send_response(const string& message, int connectionfd)
{
    ssize_t sent = 0;
    ssize_t response_length = message.length();
    ssize_t n;
    do 
    {
        n = send(connectionfd, message.c_str() + sent, response_length - sent, MSG_NOSIGNAL);
        sent += n;
    } while (sent < response_length);
}

int find_space(const string& req_header)
{
    size_t pos_start = 0, pos_end;
    int cnt = 0;

    while ((pos_end = req_header.find(' ', pos_start)) != string::npos) 
    {
        pos_start = pos_end + 1;
        cnt++;
    }

    return cnt;
}

bool isblock(const std::string &str)
{
    if(str.length()>3)
        return false; // FS_MAXFILEBLOCKS = 124
    for (char c : str)
    {
        if (!isdigit(c))
        {
            return false;
        }
    }
    return true;
}