#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>

#pragma comment(lib, "Ws2_32.lib")

constexpr int PORT = 12345;
constexpr int BUFFER_SIZE = 1024;

std::vector<SOCKET> clients;
std::map<SOCKET, std::string> usernames;
std::mutex clientsMutex;
std::mutex fileMutex;

void logMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(fileMutex);
    std::ofstream logfile("chat_log.txt", std::ios::app);
    logfile << message << "\n";
}

void broadcastMessage(const std::string& message, SOCKET sender) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    std::string msgWithNewline = message + "\n";
    for (SOCKET client : clients) {
        if (client != sender) {
            send(client, msgWithNewline.c_str(), msgWithNewline.size(), 0);
        }
    }
    logMessage(message);
}

void handleClient(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE];

    memset(buffer, 0, BUFFER_SIZE);
    int nameLen = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
    std::string username = (nameLen > 0) ? std::string(buffer, nameLen) : "Unknown";

    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        usernames[clientSocket] = username;
    }

    std::cout << "New client connected: " << username << "\n";

    // Wyślij historię czatu nowemu klientowi
    {
        std::lock_guard<std::mutex> lock(fileMutex);
        std::ifstream logfile("chat_log.txt");
        if (logfile.is_open()) {
            std::string line;
            std::string historyHeader = "--- Chat history ---\n";
            send(clientSocket, historyHeader.c_str(), historyHeader.size(), 0);

            while (std::getline(logfile, line)) {
                std::string msgWithNewline = line + "\n";
                send(clientSocket, msgWithNewline.c_str(), msgWithNewline.size(), 0);
            }

            std::string historyEnd = "--- End of history ---\n";
            send(clientSocket, historyEnd.c_str(), historyEnd.size(), 0);
        }
    }

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);

        if (bytesReceived <= 0) {
            std::cout << "Client disconnected: " << username << "\n";
            break;
        }

        std::string message(buffer, bytesReceived);

        if (message == "exit") {
            break;
        }

        if (message.rfind("/list", 0) == 0) {
            std::string userList = "Connected users:\n";
            {
                std::lock_guard<std::mutex> lock(clientsMutex);
                for (const auto& pair : usernames) {
                    userList += "- " + pair.second + "\n";
                }
            }
            send(clientSocket, userList.c_str(), userList.length(), 0);
            continue;
        }

        if (message.rfind("/private", 0) == 0) {
            std::istringstream iss(message);
            std::string cmd, targetUser;
            iss >> cmd >> targetUser;
            std::string privateMsg = message.substr(cmd.length() + targetUser.length() + 2);

            SOCKET targetSocket = INVALID_SOCKET;
            {
                std::lock_guard<std::mutex> lock(clientsMutex);
                for (const auto& pair : usernames) {
                    if (pair.second == targetUser) {
                        targetSocket = pair.first;
                        break;
                    }
                }
            }

            if (targetSocket != INVALID_SOCKET) {
                std::string formatted = "[Private from " + username + "]: " + privateMsg + "\n";
                send(targetSocket, formatted.c_str(), formatted.length(), 0);
                logMessage(formatted);
            } else {
                std::string error = "User not found.\n";
                send(clientSocket, error.c_str(), error.length(), 0);
            }
            continue;
        }

        std::string fullMessage = "[" + username + "]: " + message;
        std::cout << fullMessage << std::endl;
        broadcastMessage(fullMessage, clientSocket);
    }

    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
        usernames.erase(clientSocket);
    }

    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    SOCKET serverSocket;
    sockaddr_in serverAddr{}, clientAddr{};
    int clientSize = sizeof(clientAddr);

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed\n";
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients.push_back(clientSocket);
        }

        std::thread clientThread(handleClient, clientSocket);
        clientThread.detach();
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
