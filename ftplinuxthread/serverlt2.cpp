#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <thread>
#include <vector>

constexpr int MAX_BUFFER_SIZE = 10240;

void updateProgress(int current, int total)
{
    float progress = (static_cast<float>(current) / total) * 100.0f;
    std::cout << "Progress: " << progress << "%\r" << std::flush;
}

void handleClient(int clientSocket)
{
    // Open file to send
    std::ifstream file("movie.mkv", std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file!\n";
        close(clientSocket);
        return;
    }

    // Get total file size
    file.seekg(0, std::ios::end);
    int totalFileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Send total file size to client
    if (send(clientSocket, reinterpret_cast<char*>(&totalFileSize), sizeof(totalFileSize), 0) == -1)
    {
        std::cerr << "Error sending file size.\n";
        file.close();
        close(clientSocket);
        return;
    }

    // Send file contents
    char buffer[MAX_BUFFER_SIZE];
    int bytesRead;
    while ((bytesRead = file.readsome(buffer, MAX_BUFFER_SIZE)) > 0)
    {
        int bytesSent = send(clientSocket, buffer, bytesRead, 0);
        if (bytesSent == -1)
        {
            std::cerr << "Error sending file.\n";
            file.close();
            close(clientSocket);
            return;
        }
        usleep(10000); // Sleep for 0.01 second
    }

    std::cout << "File sent successfully!\n";

    // Close file, client socket
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
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
        return 1;
    }

    // Bind the socket
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portNumber);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1)
    {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        close(serverSocket);
        return 1;
    }

    // Listen for connections
    if (listen(serverSocket, SOMAXCONN) == -1)
    {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Server is listening on port " << portNumber << "...\n";

    std::vector<std::thread> clientThreads;

    while (true)
    {
        // Accept a client socket
        sockaddr_in clientAddr;
        socklen_t clientAddrSize = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrSize);
        if (clientSocket == -1)
        {
            std::cerr << "Accept failed: " << strerror(errno) << std::endl;
            close(serverSocket);
            return 1;
        }

        std::cout << "Client connected!\n";

        // Create a new thread to handle the client
        clientThreads.emplace_back(handleClient, clientSocket);
    }

    // Join all client threads (this part is unreachable in this example)

    // Close server socket
    close(serverSocket);

    return 0;
}
