# FTP Server

## Overview
This is a simple FTP server implemented in C++. It supports basic file operations such as upload, download, and directory browsing with multi-user functionality.

## Features
- File upload/download
- Directory navigation
- Multi-user support
- Basic authentication

## Installation
1. Clone the repository:
    ```bash
    git clone https://github.com/r98kh/FtpServer.git
    cd FtpServer/windows
    ```
2. Compile the code:
    ```bash
    g++ -o server server.cpp
    g++ -o client client.cpp
    ```
3. Run the server:
   run this file on server system
    ```bash
    ./server 
    ```
    run this file on client system
    ```bash
    ./client 
    ```
