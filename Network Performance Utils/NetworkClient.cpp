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

#include <boost/array.hpp>
#include <boost/asio.hpp>

namespace
{
    const int kBytesToReadFromServer = 4096; // 4 kibibytes
} // hidden namespace

NetworkClient::NetworkClient(
    const std::string serverIPAddress,
    const int portNumber
) : _serverIPAddress(serverIPAddress), _portNumber(portNumber), _shouldStayConnected(true), _shouldStayConnectedMutex()
{
}

NetworkClient::~NetworkClient()
{
}

void NetworkClient::ConnectToServer()
{
    boost::asio::io_context ioContext;
    boost::asio::ip::tcp::resolver resolver(ioContext);
    boost::asio::ip::tcp::resolver::results_type endpoints =
        resolver.resolve(this->_serverIPAddress, "Network Performance Utils - Client");
    
    boost::asio::ip::tcp::socket socket(ioContext);
    boost::asio::connect(socket, endpoints);

    while (true)
    {
        boost::array<char, kBytesToReadFromServer> buffer;
        // Scope this check
        {
            std::lock_guard<std::mutex> scopedLockCheck(this->_shouldStayConnectedMutex);
            if (this->_shouldStayConnected == false)
            {
                // TODO: Disconnect from the server
                return;
            }
        }

        // Proceed with connection to server
    }
}

void NetworkClient::DisconnectFromServer()
{
    std::lock_guard<std::mutex> lock(this->_shouldStayConnectedMutex);
    this->_shouldStayConnected = false;
}