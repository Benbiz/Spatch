/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "User.h"

Spatch::Configuration::User::User(const std::string& id, const std::string& username, const std::string& password, const Privilege right)
    : _username(username), _id(id), _password(password), _priv(right)
{
    
}

Spatch::Configuration::User::~User()
{
    
}

const std::string       &Spatch::Configuration::User::getId() const
{
    return _id;
}

const std::string       &Spatch::Configuration::User::getUsername() const
{
    return _username;
}
void                    Spatch::Configuration::User::setUsername(const std::string &username)
{
    _username = username;
}

const std::string       &Spatch::Configuration::User::getPassword() const
{
    return _password;
}
void                    Spatch::Configuration::User::setPassword(const std::string &password)
{
    _password = password;
}

const Spatch::Configuration::User::Privilege         Spatch::Configuration::User::getPrivilege() const
{
    return _priv;
}

void                    Spatch::Configuration::User::setPrivilege(const Spatch::Configuration::User::Privilege right)
{
    _priv = right;
}