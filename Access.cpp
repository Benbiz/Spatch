/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "Access.h"

Spatch::Configuration::Access::Access(const std::string &id,const std::shared_ptr<User> user, const std::shared_ptr<Credential> cred, const std::shared_ptr<Server> serv)
: _id(id), _user(user), _cred(cred), _serv(serv)
{
    
}

Spatch::Configuration::Access::Access(const std::string &id,const std::shared_ptr<User> user, const Spatch::Configuration::Access::CredentialType type, const std::shared_ptr<Server> serv)
: _id(id), _user(user), _cred(nullptr), _serv(serv)
{
    
}

Spatch::Configuration::Access::~Access()
{
    
}

const std::string                   &Spatch::Configuration::Access::getId() const
{
    return _id;
}

const std::shared_ptr<Spatch::Configuration::Server>       Spatch::Configuration::Access::getServer() const
{
    return _serv;
}

const std::shared_ptr<Spatch::Configuration::User>         Spatch::Configuration::Access::getUser() const
{
    return _user;
}

const std::shared_ptr<Spatch::Configuration::Credential>   Spatch::Configuration::Access::getCredential() const
{
    return _cred;
}

const Spatch::Configuration::Access::CredentialType         Spatch::Configuration::Access::getCredentialType() const
{
    return _type;
}