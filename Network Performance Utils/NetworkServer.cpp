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

#include "NetworkServer.h"

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <algorithm>
#include <random>

namespace
{
    constexpr static int kBufferSize = 4194304; // 4 mebibytes
}

using RandomBytesEngine = std::independent_bits_engine<std::default_random_engine, CHAR_BIT, unsigned char>;

NetworkServer::NetworkServer(const int portNumber) :
    _portNumber(portNumber), _shouldStayConnected(true), _shouldStayConnectedMutex()
{
}

NetworkServer::~NetworkServer()
{
}

void NetworkServer::StartListening()
{
    boost::asio::io_context ioContext;
    boost::asio::ip::tcp::acceptor acceptor(
        ioContext,
        boost::asio::ip::tcp::endpoint(
            boost::asio::ip::tcp::v4(),
            this->_portNumber
        )
    );
    boost::asio::ip::tcp::socket socket(ioContext);
    acceptor.accept(socket);
    
    RandomBytesEngine randomBytesEngine;
    boost::array<char, kBufferSize> buffer;
    
    // Generate random bytes ahead of time
    std::generate(buffer.begin(), buffer.end(), randomBytesEngine);
    while (true && socket.is_open())
    {
        // Scope this check
        {
            std::lock_guard<std::mutex> scopedLockCheck(this->_shouldStayConnectedMutex);
            if (this->_shouldStayConnected == false)
            {
                socket.close();
                acceptor.close();
                return;
            }
        }
        
        // Send the data down
        boost::system::error_code error;
        boost::asio::write(socket, boost::asio::buffer(buffer), error);
        if (error)
        {
            throw boost::system::system_error(error);
        }
    }
}

void NetworkServer::StopListening()
{
    std::lock_guard<std::mutex> lock(this->_shouldStayConnectedMutex);
    this->_shouldStayConnected = false;
}