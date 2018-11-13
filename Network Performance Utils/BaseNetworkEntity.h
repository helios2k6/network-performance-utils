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

#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

/**
 * The base class for all network-capable entities
 */
class BaseNetworkEntity
{
public:
    virtual void Start() const = 0;
    virtual void Stop() const = 0;

protected:
    explicit BaseNetworkEntity(const int);
    virtual ~BaseNetworkEntity();

    const boost::asio::io_context& getIOContext() const;
    const boost::asio::ip::tcp::socket& getSocket() const;

    const int _portNumber;
private:
    // Note: The order in which these members are declared is important. Reordering this will
    // cause a runtime error due to the way C++ initializes them
    boost::asio::io_context _ioContext;
    boost::asio::ip::tcp::socket _socket;
};