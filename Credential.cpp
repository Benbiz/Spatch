/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "Credential.h"

Spatch::Configuration::Credential::Credential(const std::string &id, const std::string& username, const std::string& password)
: _id(id), _username(username), _password(password)
{
    
}

Spatch::Configuration::Credential::~Credential()
{
    
}

 const std::string               &Spatch::Configuration::Credential::getId() const
 {
     return _id;
 }

const std::string               &Spatch::Configuration::Credential::getUsername() const
{
    return _username;
}

void                            Spatch::Configuration::Credential::setUsername(const std::string &username)
{
     _username = username;
}

const std::string               &Spatch::Configuration::Credential::getPassword() const
 {
    return _password;
 }

void                            Spatch::Configuration::Credential::setPassword(const std::string &password)
{
    _password = password;
}