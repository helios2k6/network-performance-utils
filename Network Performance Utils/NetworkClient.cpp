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

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/chrono.hpp>

namespace
{
    constexpr static int kBytesToReadFromServer = 4194304; // 4 mebibytes
} // hidden namespace

NetworkClient::NetworkClient(
    const std::string& serverHost,
    const std::string& serverPort
) : _serverHost(serverHost),
    _serverPort(serverPort),
    _shouldStayConnected(true),
    _shouldStayConnectedMutex()
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
        resolver.resolve(this->_serverHost, this->_serverPort);
    
    boost::asio::ip::tcp::socket socket(ioContext);
    boost::asio::connect(socket, endpoints);

    // Keep track of number of bytes and how much time has passed so we can get
    // a running speed average
    size_t bytesDownloaded = 0;
    boost::array<char, kBytesToReadFromServer> buffer;
    const boost::chrono::time_point<boost::chrono::steady_clock> startTime(boost::chrono::steady_clock::now());
    while (true)
    {
        // Scope this check
        { 
            std::lock_guard<std::mutex> scopedLockCheck(this->_shouldStayConnectedMutex);
            if (this->_shouldStayConnected == false)
            {
                // Close socket
                socket.close();
                return;
            }
        }

        // Proceed with connection to server
        boost::system::error_code error;
        bytesDownloaded += socket.read_some(boost::asio::buffer(buffer), error);
        boost::chrono::time_point<boost::chrono::steady_clock> finishedDownloadTime(boost::chrono::steady_clock::now());
        if (error == boost::asio::error::eof)
        {
            // Clean break from server
            break;
        }
        else if (error)
        {
            // Some other error
            throw boost::system::system_error(error);
        }

        const boost::chrono::seconds downloadDuration(
            boost::chrono::duration_cast<boost::chrono::seconds>(finishedDownloadTime - startTime)
        );

        std::cout << bytesDownloaded << " bytes " << " / " << downloadDuration << std::endl;
    }
}

void NetworkClient::DisconnectFromServer()
{
    std::lock_guard<std::mutex> lock(this->_shouldStayConnectedMutex);
    this->_shouldStayConnected = false;
}