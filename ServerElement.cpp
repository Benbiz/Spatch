/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <algorithm>
#include <iostream>
#include "ServerElement.h"

Spatch::Parsing::ServerElement::ServerElement(){
    
}

Spatch::Parsing::ServerElement::~ServerElement(){
    
}

void    Spatch::Parsing::ServerElement::setId(std::string &id)
{
    _id = id;
}
bool    Spatch::Parsing::ServerElement::addProperty(std::string &key, std::string &value)
{
    _prop[key] = value;
}
bool    Spatch::Parsing::ServerElement::isValid() const
{
    return true;
}
bool    Spatch::Parsing::ServerElement::finish(Spatch::Configuration::Config &conf)
{
    std::string ip;
    std::string port;
    std::string label;
    
    std::cout << "Creating server " << _id << " with :" << std::endl;
    for(auto const  &prop : _prop) {
        std::cout << "\t";
        if (prop.first == "host")
            ip = prop.second;
        else if (prop.first == "port")
            port = prop.second;
        else if (prop.first == "label")
            label = prop.second;
        else
           std::cout << "(ignored) ";
        std::cout << "[" << prop.first << "] = " << prop.second << std::endl;
    }
    if (ip.empty() || port.empty() || _id.empty())
    {
        std::cout << "[Error] Missing argument for server" << std::endl;
        return false;
    }
    auto it = std::find_if(conf.getServers().begin(), conf.getServers().end(), [&] (std::shared_ptr<Spatch::Configuration::Server> s)
    { 
        return (s->getId() == _id);
    });
    if(it != conf.getServers().end())
    {
        std::cout << "[Error] Duplicate Server Id" << std::endl;
        return false;
    }
    std::shared_ptr<Spatch::Configuration::Server> serv = std::make_shared<Spatch::Configuration::Server>(_id, label, ip, std::stoi(port));
    conf.addServer(serv);
    std::cout << "[Success] Server " << _id << " created !" << std::endl;
    return true; 
}