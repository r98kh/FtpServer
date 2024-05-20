#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

constexpr int DEFAULT_PORT = 8080;
constexpr int MAX_BUFFER_SIZE = 10240;

void updateProgress(int current, int total)
{
    float progress = (static_cast<float>(current) / total) * 100.0f;
    std::cout << "Progress: " << progress << "%\r" << std::flush;
}

int main()
{
    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Error creating socket.\n";
        return 1;
    }

    // Bind the socket
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(DEFAULT_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1)
    {
        std::cerr << "Bind failed.\n";
        close(serverSocket);
        return 1;
    }

    // Listen for connections
    if (listen(serverSocket, SOMAXCONN) == -1)
    {
        std::cerr << "Listen failed.\n";
        close(serverSocket);
        return 1;
    }

    std::cout << "Server is listening on port " << DEFAULT_PORT << "...\n";

    // Accept a client socket
    sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrSize);
    if (clientSocket == -1)
    {
        std::cerr << "Accept failed.\n";
        close(serverSocket);
        return 1;
    }

    std::cout << "Client connected!\n";

    // Open file to send
    std::ifstream file("movie.mkv", std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file!\n";
        close(clientSocket);
        close(serverSocket);
        return 1;
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
        close(serverSocket);
        return 1;
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
            close(serverSocket);
            return 1;
        }
    }

    std::cout << "File sent successfully!\n";

    // Close file, sockets
    file.close();
    close(clientSocket);
    close(serverSocket);

    return 0;
}
