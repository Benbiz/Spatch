/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <sstream>
#include <iostream>
#include "Shell.h"
#include "Client.h"

std::map<std::string, Spatch::Ssh::Shell::parsingCallback>   Spatch::Ssh::Shell::_commands = std::map<std::string, Spatch::Ssh::Shell::parsingCallback>();
std::map<std::string, Spatch::Ssh::Shell::parsingCallback>   Spatch::Ssh::Shell::_admincommands = std::map<std::string, Spatch::Ssh::Shell::parsingCallback>();

Spatch::Ssh::Shell::Shell(const Spatch::Configuration::Config& conf, Spatch::Ssh::Client& client)
: _conf(conf), _client(client)
{
    if (_commands.empty())
    {
        _commands["list"] = &Spatch::Ssh::Shell::list;
        _commands["connect"] = &Spatch::Ssh::Shell::connect;
        _commands["help"] = &Spatch::Ssh::Shell::help;
    }
    if (_admincommands.empty())
    {
        _admincommands["list"] = &Spatch::Ssh::Shell::adminList;
    }
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
    
    ss << "Here is a list of available commands:" << std::endl;
    ss << "\tlist accesses" << std::endl;
    ss << "\tconnect [ACCESS ID]" << std::endl;
    ss << "\thelp" << std::endl;
    if (_client.getUser()->getPrivilege() == Spatch::Configuration::User::Privilege::Admin)
    {
        ss << "\t--- ADMIN commands ---" << std::endl;
        ss << "\tadmin list users" << std::endl;
        ss << "\tadmin list servers" << std::endl;
        ss << "\tadmin list accesses [USER ID]" << std::endl;
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
    std::string input(buf);
    std::istringstream iss(input);
    std::string token;
    iss >> token;
    if (token.length() == 0)
    {
        printPrompt();
        return 0;
    }
    for (auto p : _commands)
    {
        if (token == p.first)
            return ((this->*p.second)(iss));
    }
    if (_client.getUser()->getPrivilege() == Spatch::Configuration::User::Privilege::Admin)
    {
        if (token == "admin")
        {
            iss >> token;
            for (auto p : _admincommands)
            {
                if (token == p.first)
                    return ((this->*p.second)(iss));
            }
        }
    }
    writeString("Error: Command not found.\n");
    printPrompt();
    return sz;
}

int    Spatch::Ssh::Shell::writeString(const std::string &str)
{
    return (write(_client.getSlaveFd(), str.c_str(), str.length()));
}

/*
* User parsing function
*/

int    Spatch::Ssh::Shell::list(std::istringstream &iss)
{
    std::string token;
    iss >> token;
    if (token == "accesses")
        listAccesses();
    else
        writeString("Error: Command not found.\n");
    printPrompt();
    return 0;
    
}
int    Spatch::Ssh::Shell::listAccesses()
{
    std::stringstream ss;
    
    ss << "The list of your available connection:" << std::endl;
    for (auto a : _conf.getAccesses())
    {
        if (a->getUser() == _client.getUser())
        {
            ss << "\t[" << a->getId() << "] ";
            if (a->getServer()->getName() != "")
                ss << "[" << a->getServer()->getName() << "] ";
            else
                ss << "[" << a->getServer()->getIp() << "] ";
            if (a->getCredentialType() == Spatch::Configuration::Access::Provided)
                ss << "as [" << a->getCredential()->getUsername() << "]" << std::endl;
            else if (a->getCredentialType() == Spatch::Configuration::Access::Asked)
                ss << "with asked credentials" << std::endl;
            else
                ss << "with your credentials" << std::endl;
        }
    }
    writeString(ss.str());
}

int    Spatch::Ssh::Shell::connect(std::istringstream &iss)
{
    std::string id;
    iss >> id;
    
    for (auto a : _conf.getAccesses())
    {
        if (a->getUser() == _client.getUser())
        {
            if (a->getId() == id)
            {
                _client.proxify(a);
                return 0;
            }
        }
    }
    printPrompt();
    return 0;
}

int    Spatch::Ssh::Shell::help(std::istringstream &iss)
{
    printHelp();
    printPrompt();
    return 0;
}

/*
* Admin parsing function
*/

int    Spatch::Ssh::Shell::adminList(std::istringstream &iss)
{
    writeString("Did you ask for admin list ?\n");
    printPrompt();
    return 0;
}

int    Spatch::Ssh::Shell::adminListServers()
{
    
}

int    Spatch::Ssh::Shell::adminListUsers()
{
    
}

int    Spatch::Ssh::Shell::adminListAccesses(std::istringstream &iss)
{
    
}