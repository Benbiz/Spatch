/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <algorithm>
#include <iostream>
#include "UserElement.h"

Spatch::Parsing::UserElement::UserElement()
{
    
}
Spatch::Parsing::UserElement::~UserElement()
{
    
}

void    Spatch::Parsing::UserElement::setId(std::string &id)
{
    _id = id;
    return ;
}

bool    Spatch::Parsing::UserElement::addProperty(std::string &key, std::string &value)
{
    _prop[key] = value;
    return true;
}

bool    Spatch::Parsing::UserElement::isValid() const
{
    return true;
}

bool    Spatch::Parsing::UserElement::finish(Spatch::Configuration::Config &conf)
{
    Spatch::Configuration::User::Privilege  priv;
    std::string password;
    std::string username;
    std::string privilege;
    
    std::cout << "Creating user " << _id << " with :" << std::endl;
    for(auto const  &prop : _prop) {
        std::cout << "\t";
        if (prop.first == "password")
            password = prop.second;
        else if (prop.first == "username")
            username = prop.second;
        else if (prop.first == "privilege")
            privilege = prop.second;
        else
           std::cout << "(ignored) ";
        std::cout << "[" << prop.first << "] = " << prop.second << std::endl;
    }
    if (privilege.empty() || username.empty() || password.empty() || _id.empty())
    {
        std::cout << "[Error] Missing argument for user" << std::endl;
        return false;
    }
    if (privilege == "USER")
        priv = Spatch::Configuration::User::Privilege::BasicUser;
    else if (privilege == "ADMIN")
        priv = Spatch::Configuration::User::Privilege::Admin;
    else
    {
        std::cout << "[Error] Invalid privilege for user " << _id << std::endl;
        return false;
    }
    auto it = std::find_if(conf.getUsers().begin(), conf.getUsers().end(), [&] (std::shared_ptr<Spatch::Configuration::User> u)
    { 
        return (u->getId() == _id);
    });
    if(it != conf.getUsers().end())
    {
        std::cout << "[Error] Duplicate User Id" << std::endl;
        return false;
    }
    std::shared_ptr<Spatch::Configuration::User> user = std::make_shared<Spatch::Configuration::User>(_id, username, password, priv);
    conf.addUser(user);
    std::cout << "[Success] User " << _id << " created !" << std::endl;
    return true; 
}