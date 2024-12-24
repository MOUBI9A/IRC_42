#include <iostream>
#include <memory>
#include <stdexcept>
#include <array>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

std::string getPublicIPAddress() {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("curl -s ifconfig.me", "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int findAvailablePort() {
    int sockfd;
    struct sockaddr_in addr;
    int port = 0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = 0; // Let the OS choose the port

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(sockfd);
        throw std::runtime_error("Failed to bind socket");
    }

    socklen_t addr_len = sizeof(addr);
    if (getsockname(sockfd, (struct sockaddr *)&addr, &addr_len) == -1) {
        close(sockfd);
        throw std::runtime_error("Failed to get socket name");
    }

    port = ntohs(addr.sin_port);
    close(sockfd);
    return port;
}

int main() {
    try {
        std::string ipAddress = getPublicIPAddress();
        int port = findAvailablePort();
        std::cout << "Public IP Address: " << ipAddress << std::endl;
        std::cout << "Available Port: " << port << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}