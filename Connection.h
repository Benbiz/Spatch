/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   connection.h
 * Author: Benjamin
 *
 * Created on 19 février 2017, 19:25
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <libssh/libssh.h>

namespace Spatch
{
    namespace Ssh
    {
        class Connection
        {
        public:
            Connection(const std::string &host, const int port);
            ~Connection();
            
            bool    connect(const std::string &username, const std::string &password);
            int     read(void *buff, int len);
            int     write(void *buff, int len);
            bool    close();
        private:
            ssh_session _session;
            ssh_channel _channel;
        };
    }
}

#endif /* CONNECTION_H */

