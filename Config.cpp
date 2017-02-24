/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <algorithm>
#include "Config.h"

Spatch::Configuration::Config::Config()
{
    
}

Spatch::Configuration::Config::~Config()
{
    
}


 bool                                                Spatch::Configuration::Config::addUser(std::shared_ptr<User> user)
 {
    auto it = std::find_if(_users.begin(), _users.end(), [&] (std::shared_ptr<Spatch::Configuration::User> u)
    { 
        return (u->getId() == user->getId());
    });
    if(it != _users.end())
        return false;
    _users.push_back(user);
 }

const std::vector<std::shared_ptr<Spatch::Configuration::User>>          &Spatch::Configuration::Config::getUsers() const
{
    return this->_users;
}

 bool                                                Spatch::Configuration::Config::addServer(std::shared_ptr<Server> serv)
 {
    auto it = std::find_if(_servers.begin(), _servers.end(), [&] (std::shared_ptr<Spatch::Configuration::Server> s)
    { 
        return (s->getId() == serv->getId());
    });
    if(it != _servers.end())
        return false;
    _servers.push_back(serv);
 }

const std::vector<std::shared_ptr<Spatch::Configuration::Server>>        &Spatch::Configuration::Config::getServers() const
{
    return this->_servers;
}

 bool                                                Spatch::Configuration::Config::addCredential(std::shared_ptr<Credential> cred)
 {
    auto it = std::find_if(_credentials.begin(), _credentials.end(), [&] (std::shared_ptr<Spatch::Configuration::Credential> c)
    { 
        return (c->getId() == cred->getId());
    });
    if(it != _credentials.end())
        return false;
    _credentials.push_back(cred);
 }

const std::vector<std::shared_ptr<Spatch::Configuration::Credential>>    &Spatch::Configuration::Config::getCredentials() const
{
    return this->_credentials;
}

bool                                                Spatch::Configuration::Config::addAccess(std::shared_ptr<Access> access)
 {
    auto it = std::find_if(_accesses.begin(), _accesses.end(), [&] (std::shared_ptr<Spatch::Configuration::Access> a)
    { 
        return (a->getId() == access->getId());
    });
    if(it != _accesses.end())
        return false;
    _accesses.push_back(access);
 }

const std::vector<std::shared_ptr<Spatch::Configuration::Access>>        &Spatch::Configuration::Config::getAccesses() const
{
    return this->_accesses;
}

void        Spatch::Configuration::Config::clearAll()
{
    _users.clear();
    _servers.clear();
    _accesses.clear();
    _credentials.clear();
}