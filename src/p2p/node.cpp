#include "node.h"

#include <map>
#include <set>
#include <boost/exception/all.hpp> //TODO: all reason = boost::exception???
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <memory>
#include "config.h"
#include "other.h"
#include <iostream>
#include "protocol.h"
#include "factory.h"

#include <iterator>

//c2pool::p2p::NodesManager
namespace c2pool::p2p
{
    NodesManager::NodesManager(boost::asio::io_context &_io, c2pool::config::Network *_networkConfig) : _io_context(_io)
    {
        std::cout << "Initialization NodesManager..." << std::endl; //TODO: DEBUG_LOGGER
        _net = _networkConfig; //TODO
    }

} // namespace c2pool::p2p

//c2pool::p2p::Node
namespace c2pool::p2p
{
    Node::Node(std::shared_ptr<c2pool::p2p::NodesManager> _nodes, std::string _port) : INode(_nodes), _think_timer(_nodes->io_context(), boost::posix_time::seconds(0))
    {
        std::cout << "Start Node initialization..." << std::endl; //TODO: DEBUG_LOGGER
        nonce = c2pool::random::RandomNonce();
        std::cout << "Node nonce generated: " << nonce << std::endl; //TODO: DEBUG_LOGGER
        port = _port;

        //boost::asio::io_context &io_context_, shared_ptr<c2pool::p2p::NodesManager> _nodes, int _desired_conns, int _max_attempts
        client = std::make_unique<c2pool::p2p::Client>(_nodes->io_context(), _nodes, 10, 30); // client.start()

        //boost::asio::io_context &io_context_, shared_ptr<c2pool::p2p::NodesManager> _nodes, const tcp::endpoint &endpoint, int _max_conns
        tcp::endpoint the_endpoint(tcp::v4(), atoi(_port.c_str()));                                     //ipv4, port; atoi -> str to int
        server = std::make_unique<c2pool::p2p::Server>(_nodes->io_context(), _nodes, the_endpoint, 50); // server.start()

        //todo? self.singleclientconnectors = [reactor.connectTCP(addr, port, SingleClientFactory(self)) for addr, port in self.connect_addrs]

        _think_timer.async_wait(boost::bind(&Node::_think, this, boost::asio::placeholders::error));
        std::cout << "Node created." << std::endl; //TODO: DEBUG_LOGGER
    }

    void Node::got_conn(shared_ptr<c2pool::p2p::Protocol> protocol)
    {
        if (peers.count(protocol->nonce()) != 0)
        {
            std::cout << "Already have peer!" << std::endl; //TODO: raise ValueError('already have peer')
        }
        peers.insert(std::make_pair<int, shared_ptr<c2pool::p2p::Protocol>>(protocol->nonce(), protocol->shared_from_this()));
    }

    void Node::lost_conn(shared_ptr<c2pool::p2p::Protocol> protocol, boost::exception *reason)
    {
        if (peers.count(protocol->nonce()) == 0)
        {
            std::cout << "Don't have peer!" << std::endl; //TODO: raise ValueError('''don't have peer''')
            return;
        }

        if (protocol != peers.at(protocol->nonce()))
        {
            std::cout << "Wrong conn!" << std::endl; //TODO: raise ValueError('wrong conn')
            return;
        }

        protocol.reset(); //delete for smart pointer

        //todo: print 'Lost peer %s:%i - %s' % (conn.addr[0], conn.addr[1], reason.getErrorMessage())
    }

    void Node::_think(const boost::system::error_code &error)
    { //TODO: rename method&
        if (peers.size() > 0)
        {
            int pos = c2pool::random::RandomInt(0, peers.size());
            auto item = peers.begin();
            std::advance(item, pos);
            auto proto = item->second;
            proto->send(std::make_unique<c2pool::messages::message_getaddrs>(8));
            //c2pool::random::RandomChoice<unsigned long long, std::shared_ptr<c2pool::p2p::Protocol>>(peers)->send(std::make_unique<c2pool::messages::message_getaddrs>(8)); //TODO: add send_getaddrs to c2pool::p2p::Protocol
        }
        boost::posix_time::milliseconds interval(static_cast<int>(c2pool::random::Expovariate(1.0 / 20) * 1000));
        _think_timer.expires_at(_think_timer.expires_at() + interval);
        _think_timer.async_wait(boost::bind(&Node::_think, this, boost::asio::placeholders::error));
    }
} // namespace c2pool::p2p