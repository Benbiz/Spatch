/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "Server.h"

Spatch::Configuration::Server::Server(const std::string id, const std::string& name, const std::string& ip, const int port)
: _id(id), _name(name), _ip(ip), _port(port)
{
    
}

Spatch::Configuration::Server::Server(const std::string id, const std::string& ip, const int port)
: _id(id), _name(""), _ip(ip), _port(port)
{
    
}

Spatch::Configuration::Server::~Server()
{
    
}

const std::string       &Spatch::Configuration::Server::getId() const
{
    return (_id);
}

const std::string       &Spatch::Configuration::Server::getName() const
{
    return _name;
}
void                    Spatch::Configuration::Server::setName(const std::string &name)
{
    _name = name;
}

const std::string       &Spatch::Configuration::Server::getIp() const
{
    return _ip;
}

void                    Spatch::Configuration::Server::setIp(const std::string &ip)
{
    _ip = ip;
}

const int               Spatch::Configuration::Server::getPort() const
{
    return _port;
}

void                    Spatch::Configuration::Server::setPort(const int port)
{
    _port = port;
}