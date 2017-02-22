/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <memory>
#include <iostream>
#include <algorithm>
#include "CredentialElement.h"
#include "Config.h"

Spatch::Parsing::CredentialElement::CredentialElement()
{
    
}

Spatch::Parsing::CredentialElement::~CredentialElement()
{
    
}

void    Spatch::Parsing::CredentialElement::setId(std::string &id)
{
    _id = id;
}
bool    Spatch::Parsing::CredentialElement::addProperty(std::string &key, std::string &value)
{
    _prop[key] = value;
}
bool    Spatch::Parsing::CredentialElement::isValid() const
{
    return true;
}
bool    Spatch::Parsing::CredentialElement::finish(Spatch::Configuration::Config &conf)
{
    std::string username;
    std::string password;
    std::shared_ptr<Spatch::Configuration::Server> serv;
    
    std::cout << "Creating credential " << _id << " with :" << std::endl;
    for(auto const  &prop : _prop) {
        std::cout << "\t";
        if (prop.first == "username")
            username = prop.second;
        else if (prop.first == "password")
            password = prop.second;
        else
           std::cout << "(ignored) ";
        std::cout << "[" << prop.first << "] = " << prop.second << std::endl;
    }
    if (username.empty() || password.empty() || _id.empty())
    {
        std::cout << "[Error] Missing argument for credential" << std::endl;
        return false;
    }
    auto it = std::find_if(conf.getCredentials().begin(), conf.getCredentials().end(), [&] (std::shared_ptr<Spatch::Configuration::Credential> c)
    { 
        return (c->getId() == _id);
    });
    if(it != conf.getCredentials().end())
    {
        std::cout << "[Error] Duplicate Credential Id" << std::endl;
        return false;
    }
    
    std::shared_ptr<Spatch::Configuration::Credential> cred = std::make_shared<Spatch::Configuration::Credential>(_id, username, password);
    conf.addCredential(cred);
    std::cout << "[Success] Credential " << _id << " created !" << std::endl;
    return true; 
}