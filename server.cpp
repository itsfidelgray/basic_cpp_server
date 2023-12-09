#include <iostream>
#include <thread>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

void handleRequest(SOCKET clientSocket)
{
    char buffer[BUFFER_SIZE] = {0};

    // Receive data from the client
    recv(clientSocket, buffer, BUFFER_SIZE, 0);

    std::cout << "Received request:\n"
              << buffer << std::endl;

    // Simple GET request handling
    const char *response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
    send(clientSocket, response, strlen(response), 0);

    // Close the client socket
    closesocket(clientSocket);
}

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Error: WSAStartup failed\n";
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Error: Could not create socket\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::cerr << "Error: Could not bind to port\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, 10) == SOCKET_ERROR)
    {
        std::cerr << "Error: Could not listen on socket\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server is listening on port " << PORT << "...\n";

    while (true)
    {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Error: Could not accept client connection\n";
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        // Handle each incoming connection in a separate thread
        std::thread(handleRequest, clientSocket).detach();
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
