#include <iostream>
#include <fstream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

void updateProgress(int current, int total)
{
    float progress = (static_cast<float>(current) / total) * 100.0f;
    std::cout << "Progress: " << progress << "%\r" << std::flush;
}


int main()
{
    // Initialize Winsock
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Create a socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Connect to server
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr));

    std::cout << "Connected to server!\n";

    int totalFileSize;
    recv(clientSocket, (char *)&totalFileSize, sizeof(totalFileSize), 0);
    std::cout << "Total file size: " << totalFileSize << " bytes\n";

    // Open file to receive
    std::ofstream file("FileName", std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to create file!\n";
        return 1;
    }

    // Receive file contents
    char buffer[1024];
    int bytesReceived;
    int receivedBytesTotal = 0;
    while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0)
    {
        file.write(buffer, bytesReceived);
        receivedBytesTotal += bytesReceived;
        updateProgress(receivedBytesTotal, totalFileSize);

    }

    std::cout << "\nFile received successfully!\n";

    // Close file, socket, and cleanup
    file.close();
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
