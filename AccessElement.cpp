/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <algorithm>
#include <iostream>
#include "AccessElement.h"

Spatch::Parsing::AccessElement::AccessElement()
{
    
}

Spatch::Parsing::AccessElement::~AccessElement()
{
    
}

void    Spatch::Parsing::AccessElement::setId(std::string &id)
{
    _id = id;
}
bool    Spatch::Parsing::AccessElement::addProperty(std::string &key, std::string &value)
{
    _prop[key] = value;
}
bool    Spatch::Parsing::AccessElement::isValid() const
{
    return true;
}
bool    Spatch::Parsing::AccessElement::finish(Spatch::Configuration::Config &conf)
{
    std::string                                         server;
    std::shared_ptr<Spatch::Configuration::Server>      serv;
    std::string                                         credential;
    Spatch::Configuration::Access::CredentialType       type;
    std::shared_ptr<Spatch::Configuration::Credential>  cred;
    std::string                                         user;
    std::shared_ptr<Spatch::Configuration::User>        u;
    
    std::cout << "Creating access " << _id << " with :" << std::endl;
    for(auto const  &prop : _prop) {
        std::cout << "\t";
        if (prop.first == "server")
            server = prop.second;
        else if (prop.first == "credential")
            credential = prop.second;
        else if (prop.first == "user")
            user = prop.second;
        else
           std::cout << "(ignored) ";
        std::cout << "[" << prop.first << "] = " << prop.second << std::endl;
    }
    if (user.empty() || server.empty() || credential.empty() || _id.empty())
    {
        std::cout << "[Error] Missing argument for access" << std::endl;
        return false;
    }
    auto it = std::find_if(conf.getAccesses().begin(), conf.getAccesses().end(), [&] (std::shared_ptr<Spatch::Configuration::Access> a)
    { 
        return (a->getId() == _id);
    });
    if(it != conf.getAccesses().end())
    {
        std::cout << "[Error] Duplicate access Id" << std::endl;
        return false;
    }
    if (credential == "USER")
        type = Spatch::Configuration::Access::CredentialType::UserCred;
    else if (credential == "ASKED")
    {
        type = Spatch::Configuration::Access::CredentialType::Asked;
    }
    else if ((cred = getCredentialRef(conf, credential)) == nullptr)
    {
        std::cout << "[Error] Credential reference not found" << std::endl;
        return false;
    }
    else
        type = Spatch::Configuration::Access::CredentialType::Provided;
    if ((serv = getServerRef(conf, server)) == nullptr)
    {
        std::cout << "[Error] Server reference not found" << std::endl;
        return false;
    }
    if ((u = getUserRef(conf, user)) == nullptr)
    {
        std::cout << "[Error] User reference not found" << std::endl;
        return false;
    }
    std::shared_ptr<Spatch::Configuration::Access> access;
    if (type == Spatch::Configuration::Access::CredentialType::Provided)
         access = std::make_shared<Spatch::Configuration::Access>(_id, u, cred, serv);
    else
        access = std::make_shared<Spatch::Configuration::Access>(_id, u, type, serv);
    conf.addAccess(access);
    std::cout << "[Success] Access " << _id << " created !" << std::endl;
    return true;
}

std::shared_ptr<Spatch::Configuration::Server>  Spatch::Parsing::AccessElement::getServerRef(Spatch::Configuration::Config &conf, std::string &ref)
{
    size_t          pos;
    
    if ((pos = ref.find("$")) == std::string::npos)
        return nullptr;
    std::string id = ref.substr(pos + 1);
    
    if (id.empty())
        return nullptr;
    auto it = std::find_if(conf.getServers().begin(), conf.getServers().end(), [&] (std::shared_ptr<Spatch::Configuration::Server> s)
    { 
        return (s->getId() == id);
    });
    if(it == conf.getServers().end())
        return nullptr;
    
    return *it;
}

std::shared_ptr<Spatch::Configuration::Credential>  Spatch::Parsing::AccessElement::getCredentialRef(Spatch::Configuration::Config &conf, std::string &ref)
{
    size_t          pos;
    
    if ((pos = ref.find("$")) == std::string::npos)
        return nullptr;
    std::string id = ref.substr(pos + 1);
    
    if (id.empty())
        return nullptr;
    auto it = std::find_if(conf.getCredentials().begin(), conf.getCredentials().end(), [&] (std::shared_ptr<Spatch::Configuration::Credential> c)
    { 
        return (c->getId() == id);
    });
    if(it == conf.getCredentials().end())
        return nullptr;
    
    return *it;
}

std::shared_ptr<Spatch::Configuration::User>  Spatch::Parsing::AccessElement::getUserRef(Spatch::Configuration::Config &conf, std::string &ref)
{
    size_t          pos;
    
    if ((pos = ref.find("$")) == std::string::npos)
        return nullptr;
    std::string id = ref.substr(pos + 1);
    
    if (id.empty())
        return nullptr;
    auto it = std::find_if(conf.getUsers().begin(), conf.getUsers().end(), [&] (std::shared_ptr<Spatch::Configuration::User> u)
    { 
        return (u->getId() == id);
    });
    if(it == conf.getUsers().end())
        return nullptr;
    
    return *it;
}