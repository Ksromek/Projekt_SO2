#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

constexpr int PORT = 12345;
constexpr int BUFFER_SIZE = 1024;
const char* SERVER_IP = "127.0.0.1";

void receiveMessages(SOCKET sock) {
    char buffer[BUFFER_SIZE];

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived <= 0) {
            std::cout << "Server disconnected.\n";
            break;
        }

        std::cout << "\n[CHAT] " << buffer << std::endl;
        std::cout << "> ";
    }
}

int main() {
    WSADATA wsaData;
    SOCKET sock;
    sockaddr_in serverAddr{};

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::string username;
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);
    send(sock, username.c_str(), username.length(), 0);

    std::cout << "Connected to server. Type messages below.\n";
    std::cout << "(Type 'exit' to quit)\n";

    std::thread receiveThread(receiveMessages, sock);
    receiveThread.detach();

    std::string input;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "exit") {
            break;
        }

        send(sock, input.c_str(), input.length(), 0);
    }

    closesocket(sock);
    WSACleanup();
    std::cout << "Disconnected.\n";
    return 0;
}
