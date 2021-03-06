#pragma once

#include "messages.h"
#include <libnet/node_member.h>
using namespace coind::p2p::messages;

namespace coind::p2p
{
    class CoindProtocol;
}

#include <memory>
#include <boost/asio.hpp>
#include <boost/function.hpp>
namespace ip = boost::asio::ip;

namespace coind::p2p
{
    class P2PSocket : public c2pool::libnet::INodeMember, public std::enable_shared_from_this<P2PSocket>
    {
    public:
        //for receive
        P2PSocket(ip::tcp::socket socket, c2pool::libnet::INodeMember node_member) : _socket(std::move(socket)), c2pool::libnet::INodeMember(node_member)
        {
        }

        //for connect
        void init(const boost::asio::ip::tcp::resolver::results_type endpoints, shared_ptr<coind::p2p::CoindProtocol> proto);
        void connectionMade(boost::asio::ip::tcp::endpoint ep);

        bool isConnected() const { return _socket.is_open(); }
        ip::tcp::socket &get() { return _socket; }
        void disconnect() { _socket.close(); }

        ip::tcp::endpoint endpoint()
        {
            boost::system::error_code ec;
            return _socket.remote_endpoint(ec);
        }

        void write(std::shared_ptr<base_message> msg);

    private:
        void start_read();
        void read_prefix(std::shared_ptr<raw_message> tempRawMessage);
        void read_command(std::shared_ptr<raw_message> tempRawMessage);
        void read_length(std::shared_ptr<raw_message> tempRawMessage);
        void read_checksum(shared_ptr<raw_message> tempRawMessage);
        void read_payload(std::shared_ptr<raw_message> tempRawMessage);

        void write_prefix(std::shared_ptr<base_message> msg);
        void write_message_data(std::shared_ptr<base_message> msg);

    private:
        // std::shared_ptr<coind::ParentNetwork> _net; //Parent network
        ip::tcp::socket _socket;

        std::weak_ptr<coind::p2p::CoindProtocol> _protocol;
    };
} // namespace c2pool::p2p