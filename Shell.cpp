/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <sstream>
#include "Shell.h"
#include "Client.h"

Spatch::Ssh::Shell::Shell(const Spatch::Configuration::Config& conf, Spatch::Ssh::Client& client)
: _conf(conf), _client(client)
{
    
}

Spatch::Ssh::Shell::~Shell()
{
    
}

void    Spatch::Ssh::Shell::onWelcome()
{
    writeString("Welcome on Spatch Server.\n");
    writeString("Spatch is an SSH Proxy.\n\n");
    printHelp();
    printPrompt();
}

void    Spatch::Ssh::Shell::printPrompt()
{
    writeString("Spatch > ");
}

void    Spatch::Ssh::Shell::printHelp()
{
    std::stringstream ss;
    
    ss << "Here is a list on available commands:" << std::endl;
    ss << "\t list accesses" << std::endl;
    ss << "\t connect [ACCESS ID]" << std::endl;
    ss << "\t help" << std::endl;
    if (_client.getUser()->getPrivilege() == Spatch::Configuration::User::Privilege::Admin)
    {
        ss << "\t --- ADMIN commands ---" << std::endl;
        ss << "\t admin list users" << std::endl;
        ss << "\t admin list servers" << std::endl;
        ss << "\t admin list accesses [USER ID]" << std::endl;
    }
    writeString(ss.str());
}

int     Spatch::Ssh::Shell::onRead()
{
    char buf[2048];
    int sz = 0;
    
    sz = read(_client.getSlaveFd(), buf, 2048);
    if (sz <= 0)
        return sz;
    buf[sz - 1] = '\0';
    writeString("You just said: \"");
    sz = write(_client.getSlaveFd(), buf, sz);
    writeString("\"\n");
    printPrompt();
    return sz;
}

int    Spatch::Ssh::Shell::writeString(const std::string &str)
{
    return (write(_client.getSlaveFd(), str.c_str(), str.length()));
}