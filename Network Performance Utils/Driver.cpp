/*
 * Copyright (c) 2018 Andrew Johnson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
 * Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "NetworkClient.h"
#include "NetworkServer.h"

#include <csignal>
#include <iostream>
#include <string>
#include <sstream>

namespace
{
    int currentSigIntCount = 0;
    NetworkServer * serverRef = nullptr;
    NetworkClient * clientRef = nullptr;
}

void HandleSigInt(int s)
{
    currentSigIntCount++;
    if (currentSigIntCount < 1)
    {
        std::cout << "SIGINT caught. Trying to shutdown cleanly" << std::endl;
        if (serverRef != nullptr)
        {
            serverRef->StopListening();
        }
        else if (clientRef != nullptr)
        {
            clientRef->DisconnectFromServer();
        }
        else
        {
            std::cout << "Neither server nor client set. Exiting immediately." << std::endl;
            exit(1);
        }
    }
    else
    {
        std::cout << "SIGINT caught again. Shutting down immediately" << std::endl;
        exit(1);
    }
}

int main(int argc, char ** argv)
{
    if (argc < 3)
    {
        std::cout << "Usage: <this program> (server|client) (client: <server host name>) (server: <port number>)";
        return 1;
    }

    // Set up signal handling first
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = HandleSigInt;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, nullptr);

    const std::string role(argv[1]);
    if (role == "server")
    {
        const std::string portNumberArg(argv[2]);
        std::stringstream portNumberStringStream(portNumberArg);

        int portNumber = 0;
        portNumberStringStream >> portNumber;

        NetworkServer server(portNumber);
        serverRef = &server;
        server.StartListening();
    }
    else if (role == "client")
    {
        const std::string serverHost(argv[2]);
        NetworkClient client(serverHost);
        clientRef = &client;
        client.ConnectToServer();
    }
    else
    {
        std::cout << "Unknown role specified. You must specify either server or client." << std::endl;
        return 1;
    }

    return 0;
}