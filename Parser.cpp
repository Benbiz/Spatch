/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <iostream>
#include <functional>
#include <cctype>
#include <utility>
#include <algorithm>
#include <locale>
#include "Parser.h"
#include "ServerElement.h"
#include "UserElement.h"
#include "CredentialElement.h"
#include "AccessElement.h"

Spatch::Parsing::Parser::Parser(Spatch::Configuration::Config &conf)
    : _conf(conf)
{
    
}

Spatch::Parsing::Parser::~Parser()
{
    
}

inline std::string          &Spatch::Parsing::Parser::trim(std::string& s)
{
    return ltrim(rtrim(s));
}

inline std::string          &Spatch::Parsing::Parser::rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

inline std::string          &Spatch::Parsing::Parser::ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

void        Spatch::Parsing::Parser::parse(std::istream &is)
{
    std::string buff;
    size_t pos;
    Spatch::Parsing::IElement *tmp = nullptr;
    
    while (std::getline (is, buff))
    {
        if ((pos = buff.find("##")) != std::string::npos)
            buff.erase(pos);
        if (!std::all_of(buff.begin(),buff.end(),isspace))
        {
            if ((pos = buff.find("#")) != std::string::npos)
            {
                if (tmp != nullptr)
                {
                    tmp->finish(_conf);
                    delete tmp;
                    tmp = nullptr;
                }
                if ((tmp = this->getElement(buff)) == nullptr)
                    std::cout << "Parsing error (unknown element) !\n\tIgnoring : \"" << buff << "\"" << std::endl;
            }
            else if (tmp != nullptr)
                this->setKeyValueToElement(tmp, buff);
            else
                std::cout << "Parsing error (no element found)!\n\tIgnoring : \"" << buff << "\"" << std::endl;
        }
    }
    if (tmp != nullptr)
    {
        tmp->finish(_conf);
        delete tmp;
    }
}

Spatch::Parsing::IElement   *Spatch::Parsing::Parser::getElement(std::string &line)
{
    Spatch::Parsing::IElement   *ret = nullptr;
    size_t          pos =  line.find("#");
    
    std::string value = line.substr(pos + 1);
    
    pos =  line.find(" ");
    value = value.substr(0 , pos);
    std::string id = line.substr(pos + 1);
    
    value = trim(value);
    id = trim(id);
    if (value.length() == 0 || id.length() == 0)
        return ret;
    
    return createElement(value, id);
}
Spatch::Parsing::IElement   *Spatch::Parsing::Parser::createElement(std::string &type, std::string &id)
{    
    Spatch::Parsing::IElement   *ret = nullptr;

    if (type == "user")
    {
        ret = new Spatch::Parsing::UserElement();
        ret->setId(id);
    }
    else if (type == "server")
    {
        ret = new Spatch::Parsing::ServerElement();
        ret->setId(id);
    }
    else if (type == "credential")
    {
        ret = new Spatch::Parsing::CredentialElement();
        ret->setId(id);
    }
    else if (type == "access")
    {
        ret = new Spatch::Parsing::AccessElement();
        ret->setId(id);
    }
    return ret;
}
bool                        Spatch::Parsing::Parser::setKeyValueToElement(Spatch::Parsing::IElement *elem, std::string &line)
{
    size_t          pos =  line.find("=");
     
    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);

    value = trim(value);
    key = trim(key);
     if (value.length() == 0 || key.length() == 0)
        return false;
    return (elem->addProperty(key, value));
}