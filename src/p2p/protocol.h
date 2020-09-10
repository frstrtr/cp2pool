#ifndef CPOOL_PROTOCOL_H
#define CPOOL_PROTOCOL_H

#include "boost/asio.hpp"
#include "types.h"

// #include "factory.h"
// #include "node.h"
// //#include <stdio>
// #include <string>
// #include "pystruct.h"
// #include <sstream>
// #include "config.cpp"
// #include <map>
// #include <iostream>
// #include <boost/algorithm/string.hpp>
// #include "log.cpp"
// #include "converter.cpp"
// #include "other.h"
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include <deque>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include "messages.h"
//#include "node.h"
using boost::asio::ip::tcp;

//-----------------------------------------------------------

namespace c2pool::p2p
{
    class Node;
    class NodesManager;
    class Factory;
}
namespace c2pool::messages
{
    class message;
}
//-----------------------------------------------------------
namespace c2pool::p2p
{
    class Protocol : public std::enable_shared_from_this<Protocol>
    {
    public:
        Protocol(boost::asio::ip::tcp::socket _socket, c2pool::p2p::Factory *_factory);

        virtual void send(c2pool::messages::message *msg);

        //OLD: fromStr
        virtual void handle(std::stringstream ss);

        unsigned long long nonce() const{
            return _nonce;
        }

    protected:
        //used for write message in protocol
        //virtual void write(unique_ptr<c2pool::messages::message> msg);

        void read_prefix();

        void read_command();

        void read_length();

        void read_checksum();

        void read_payload();
        
        //py: dataReceived(self, data)
        //virtual void handlePacket() = 0;
        //virtual void sendPacket(c2pool::messages::message *payload) = 0;
        //virtual void connectionMade() = 0;
        virtual void disconnect();

        template <class MsgType>
        MsgType *GenerateMsg(std::stringstream &ss);

        virtual void handle(c2pool::messages::message_version *msg);

        virtual void handle(c2pool::messages::message_addrs *msg);

        virtual void handle(c2pool::messages::message_addrme *msg);

        virtual void handle(c2pool::messages::message_ping *msg);

        virtual void handle(c2pool::messages::message_getaddrs *msg);

        virtual void handle(c2pool::messages::message_error *msg);

        
        void update_addr();
        //TODO: Friend class: Message for handle_<command>
    protected:
        const int version;

        unsigned int other_version = -1;
        std::string other_sub_version;
        int other_services; //TODO: int64? IntType(64)
        unsigned long long _nonce;

        //peer address
        std::tuple<std::string, std::string> addr;

        boost::asio::ip::tcp::socket socket;
        std::shared_ptr<c2pool::p2p::NodesManager> nodes;
        c2pool::p2p::Factory *factory; //todo: shared_ptr

        unique_ptr<c2pool::messages::IMessage> tempMessage; 
    };

    class ClientProtocol : public Protocol
    {
    public:
        ClientProtocol(boost::asio::ip::tcp::socket _socket, c2pool::p2p::Factory *_factory, const boost::asio::ip::tcp::resolver::results_type endpoints);

        void do_connect(const boost::asio::ip::tcp::resolver::results_type endpoint);
    };

    class ServerProtocol : public Protocol
    {
    public:
        ServerProtocol(boost::asio::ip::tcp::socket _socket, c2pool::p2p::Factory *_factory);

        void start();
    };
} // namespace c2pool::p2p

#endif //CPOOL_PROTOCOL_H
