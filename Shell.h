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
        private:

            int     writeString(const std::string &str);
            void    printPrompt();
            void    printHelp();
            
            const Spatch::Configuration::Config             &_conf;
            Spatch::Ssh::Client                             &_client;
        };
    }
}

#endif /* SHELL_H */

