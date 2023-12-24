#include <sys/socket.h> 
#include <stdlib.h>     // atoi()
#include <unistd.h>     // close()
#include <arpa/inet.h>  // htons()
#include <netinet/in.h> // struct sockaddr_in
#include <string.h>     // memcpy()
#include <string>
#include <cctype>

using std::string;

int get_port_number(int sockfd);
int make_server_sockaddr(struct sockaddr_in *addr, int port);
int setup_socket(int & port, int queue_size);
void send_response(const string& message, int connectionfd);
int find_space(const string& req_header);
bool isblock(const std::string &str);