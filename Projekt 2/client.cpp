#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

constexpr int PORT = 12345;
const char* SERVER_IP = "127.0.0.1";

int main() {
    WSADATA wsaData;
    SOCKET sock;
    sockaddr_in serverAddr{};

    // Init Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    // Set server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    // Connect to server
    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server. Type your message below.\n";
    std::cout << "(Type 'exit' to quit)\n";

    // Message loop
    std::string input;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "exit") {
            break;
        }

        int sent = send(sock, input.c_str(), input.length(), 0);
        if (sent == SOCKET_ERROR) {
            std::cerr << "Send failed\n";
            break;
        }
    }

    closesocket(sock);
    WSACleanup();
    std::cout << "Disconnected.\n";
    return 0;
}
