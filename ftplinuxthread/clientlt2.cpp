#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>

constexpr int MAX_BUFFER_SIZE = 10240;

void updateProgress(int current, int total)
{
    float progress = (static_cast<float>(current) / total) * 100.0f;
    std::cout << "Progress: " << progress << "%\r" << std::flush;
}

void receiveFile(int clientSocket, int totalFileSize)
{
    // Open file to receive
    std::ofstream file("movie2.mkv", std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to create file!\n";
        close(clientSocket);
        return;
    }

    // Receive file contents
    char buffer[MAX_BUFFER_SIZE];
    int receivedBytesTotal = 0;
    int bytesReceived;
    while ((bytesReceived = recv(clientSocket, buffer, MAX_BUFFER_SIZE, 0)) > 0)
    {
        file.write(buffer, bytesReceived);
        receivedBytesTotal += bytesReceived;
        updateProgress(receivedBytesTotal, totalFileSize);
    }

    if (bytesReceived == -1)
    {
        std::cerr << "Error receiving file.\n";
    }
    else
    {
        std::cout << "\nFile received successfully!\n";
    }

    // Close file, socket
    file.close();
    close(clientSocket);
}

int main()
{
    // Get port number from user
    int portNumber;
    std::cout << "Enter port number: ";
    std::cin >> portNumber;

    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        std::cerr << "Error creating socket.\n";
        return 1;
    }

    // Connect to server
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portNumber);
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) != 1)
    {
        std::cerr << "Invalid address. Conversion failed.\n";
        close(clientSocket);
        return 1;
    }

    if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1)
    {
        std::cerr << "Connect failed.\n";
        close(clientSocket);
        return 1;
    }

    std::cout << "Connected to server!\n";

    int totalFileSize;
    if (recv(clientSocket, reinterpret_cast<char*>(&totalFileSize), sizeof(totalFileSize), 0) == -1)
    {
        std::cerr << "Error receiving file size.\n";
        close(clientSocket);
        return 1;
    }

    std::cout << "Total file size: " << totalFileSize << " bytes\n";

    // Create a new thread to receive the file
    std::thread receiveThread(receiveFile, clientSocket, totalFileSize);
    receiveThread.join(); // Wait for the thread to finish

    return 0;
}
