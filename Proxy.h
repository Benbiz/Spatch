/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Proxy.h
 * Author: Benjamin
 *
 * Created on 19 février 2017, 19:32
 */

#ifndef PROXY_H
#define PROXY_H

#include <vector>
#include <memory>
#include <libssh/libssh.h>
#include <libssh/server.h>
#include <libssh/callbacks.h>
#include <poll.h>
#include "Client.h"
#include "Config.h"

namespace Spatch
{
    namespace Ssh
    {
        class Proxy
        {
        public:
            Proxy(const Spatch::Configuration::Config &conf, const int port);
            ~Proxy();
            
            void    setRSAFile(const std::string &RSAFile);
            void    setDSAFile(const std::string &DSAFile);
            
            bool    handleClients();
            
        private:
            std::shared_ptr<Client>                         getClient();
            std::shared_ptr<Spatch::Configuration::User>    getUser(const std::string &username, const std::string &password);
            
            std::shared_ptr<Spatch::Configuration::User>    authClient(ssh_session session);
            
            static int                                      incomingConnectionCallback(socket_t fd, int revents, void *userdata);
            
            ssh_bind                                _bind;
            ssh_event                               _event;
            std::string                             _rsafile;
            std::string                             _dsafile;
            const Spatch::Configuration::Config     &_conf;
            int                                     _port;
            std::vector<std::shared_ptr<Client>>    _clients;

        };
    }
}

#endif /* PROXY_H */

