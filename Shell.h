/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Shell.h
 * Author: Benjamin
 *
 * Created on 22 février 2017, 10:38
 */

#ifndef SHELL_H
#define SHELL_H

#include <map>
#include <memory>
#include "Config.h"

namespace Spatch
{
    namespace Ssh
    {
        class Client;
        class Shell
        {
        public:
            Shell(const Spatch::Configuration::Config &conf, Spatch::Ssh::Client &client);
            ~Shell();
            
            void    onWelcome();
            int     onRead();
            void    printPrompt();
        private:

            typedef int(Spatch::Ssh::Shell::*parsingCallback)(std::istringstream &);
            
            int     writeString(const std::string &str);
            
            void    printHelp();
            
            /*
             * User parsing function
             */
            
            int    list(std::istringstream &);
            int    listAccesses();
            
            int    connect(std::istringstream &);
            
            int    help(std::istringstream &);
            
            /*
             * Admin parsing function
             */
            
            int    adminList(std::istringstream &);
            int    adminListServers();
            int    adminListUsers();
            int    adminListAccesses(std::istringstream &);
            
            const Spatch::Configuration::Config             &_conf;
            Spatch::Ssh::Client                             &_client;
            static std::map<std::string, parsingCallback>   _commands;
            static std::map<std::string, parsingCallback>   _admincommands;
        };
    }
}

#endif /* SHELL_H */

