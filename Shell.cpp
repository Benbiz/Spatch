/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <sstream>
#include <fstream>
#include <iostream>
#include "Parser.h"
#include "Shell.h"
#include "Client.h"

std::map<std::string, Spatch::Ssh::Shell::parsingCallback>   Spatch::Ssh::Shell::_commands = std::map<std::string, Spatch::Ssh::Shell::parsingCallback>();
std::map<std::string, Spatch::Ssh::Shell::parsingCallback>   Spatch::Ssh::Shell::_admincommands = std::map<std::string, Spatch::Ssh::Shell::parsingCallback>();

Spatch::Ssh::Shell::Shell(Spatch::Configuration::Config& conf, Spatch::Ssh::Client& client)
: _conf(conf), _client(client)
{
    if (_commands.empty())
    {
        _commands["list"] = &Spatch::Ssh::Shell::list;
        _commands["connect"] = &Spatch::Ssh::Shell::connect;
        _commands["help"] = &Spatch::Ssh::Shell::help;
        _commands["top"] = &Spatch::Ssh::Shell::top;
    }
    if (_admincommands.empty())
    {
        _admincommands["list"] = &Spatch::Ssh::Shell::adminList;
        _admincommands["reload"] = &Spatch::Ssh::Shell::reloadConfig;
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
    ss << "\ttop [ACCESS ID]" << std::endl;
    ss << "\thelp" << std::endl;
    if (_client.getUser()->getPrivilege() == Spatch::Configuration::User::Privilege::Admin)
    {
        ss << "\t--- ADMIN commands ---" << std::endl;
        ss << "\tadmin list users" << std::endl;
        ss << "\tadmin list servers" << std::endl;
        ss << "\tadmin list accesses [USER ID]" << std::endl;
        ss << "\tadmin reload" << std::endl;
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
    
    ss << "The list of your available accesses:" << std::endl;
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
                ss << "with your own credentials" << std::endl;
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

int    Spatch::Ssh::Shell::top(std::istringstream &iss)
{
    std::string id;
    iss >> id;
    
    for (auto a : _conf.getAccesses())
    {
        if (a->getUser() == _client.getUser())
        {
            if (a->getId() == id)
            {
                _client.proxify(a, "top");
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
    std::string token;
    iss >> token;
    if (token == "servers")
        adminListServers();
    else if (token == "users")
        adminListUsers();
    else if (token == "accesses")
        adminListAccesses(iss);
    else
        writeString("Error: Command not found.\n");
    printPrompt();
    return 0;
}

int    Spatch::Ssh::Shell::adminListServers()
{
    std::stringstream ss;
    ss << "Here is the list of available servers: " << std::endl;
    for (std::shared_ptr<Spatch::Configuration::Server> s : _conf.getServers())
    {
        ss << "\t[" << s->getId() << "] [";
        if (s->getName().length())
            ss << s->getName();
        else
            ss << "unnamed";
        ss << "] [" << s->getIp() << ":" << s->getPort() << "]" << std::endl;
    }
    writeString(ss.str());
    return 0;
}

int    Spatch::Ssh::Shell::adminListUsers()
{
    std::stringstream ss;
    ss << "Here is the list of registered users: " << std::endl;
    ss << "\t--- Basic users ---" << std::endl;
    for (std::shared_ptr<Spatch::Configuration::User> u : _conf.getUsers())
    {
        if (u->getPrivilege() == Spatch::Configuration::User::Privilege::BasicUser)
        {
            ss << "\t[" << u->getId() << "] ";
            ss << "[" << u->getUsername() << ":" << u->getPassword() << "]" << std::endl;
        }
    }
    ss << "\t--- Admins ---" << std::endl;
    for (std::shared_ptr<Spatch::Configuration::User> u : _conf.getUsers())
    {
        if (u->getPrivilege() == Spatch::Configuration::User::Privilege::Admin)
        {
            ss << "\t[" << u->getId() << "] ";
            ss << "[" << u->getUsername() << ":" << u->getPassword() << "]" << std::endl;
        }
    }
    writeString(ss.str());
    return 0;
}

int    Spatch::Ssh::Shell::adminListAccesses(std::istringstream &iss)
{
    std::stringstream ss;
    std::string token;
    std::shared_ptr<Spatch::Configuration::User>    user;
    
    iss >> token;
    for (std::shared_ptr<Spatch::Configuration::User> u : _conf.getUsers())
    {
        if (u->getId() == token)
        {
            user = u;
            break;
        }
    }
    if (!user)
    {
        writeString("Error: User Id not found.");
        return 0;
    }
    ss << "The list of " <<  user->getUsername() << "'s accesses:" << std::endl;
    for (auto a : _conf.getAccesses())
    {
        if (a->getUser() == user)
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
                ss << "with its own credentials" << std::endl;
        }
    }
    writeString(ss.str());
    return 0;
}

int         Spatch::Ssh::Shell::reloadConfig(std::istringstream &)
{
    std::filebuf    fb;
    std::istream    is(&fb);
    Spatch::Parsing::Parser         parser(_conf);
    
    _conf.clearAll();
    if (fb.open("spatch.conf",std::ios::in))
    {
        parser.parse(is);
        fb.close();
    }
    writeString("Config reloaded !\n");
    printPrompt();
    return 0;
}