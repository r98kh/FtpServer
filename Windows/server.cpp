#include <iostream>
#include <fstream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE 10240

int main()
{
    // Initialize Winsock
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Create a socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    int PORT;
    std::cout << "Enter Server PORT: ";
    std::cin >> PORT;

    // Bind the socket
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);
    bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr));

    // Listen for connections
    listen(serverSocket, SOMAXCONN);

    std::cout << "Server is listening on port " << PORT << "...\n";

    // Accept a client socket
    SOCKET clientSocket;
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientAddrSize);
    std::cout << "Client connected!\n";

    // Open file to send
    std::ifstream file("fileName", std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file!\n";
        return 1;
    }
    // Get total file size
    file.seekg(0, std::ios::end);
    int totalFileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    std::cout << totalFileSize;

    // Send total file size to client

    send(clientSocket, (char *)&totalFileSize, sizeof(totalFileSize), 0);
    // Send file contents

    char buffer[BUFFER_SIZE];
    int bytesRead;
    while ((bytesRead = file.readsome(buffer, BUFFER_SIZE)) > 0)
    {
        int bytesSent = send(clientSocket, buffer, bytesRead, 0);
        if (bytesSent < 0)
        {
            std::cerr << "Error sending file!\n";
            break;
        }
    }

    // Close file, sockets, and cleanup
    file.close();
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
