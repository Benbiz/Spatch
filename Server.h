/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Server.h
 * Author: Benjamin
 *
 * Created on 1 février 2017, 15:50
 */

#ifndef SPATCH_SERVER_H
#define SPATCH_SERVER_H

#include <string>

namespace Spatch
{
    namespace Configuration
    {
        class Server
        {
        public:
            Server();
            Server(const std::string id, const std::string &name, const std::string &ip, const int port);
            Server(const std::string id, const std::string &ip, const int port);
            ~Server();
            
            const std::string       &getId() const;
            
            const std::string       &getName() const;
            void                    setName(const std::string &name);
            
            const std::string       &getIp() const;
            void                    setIp(const std::string &ip);
            
            const int               getPort() const;
            void                    setPort(const int port);
            
        private:
            std::string             _name;
            int                     _port;
            std::string             _ip;
            std::string             _id;
        };
    }
}

#endif /* SPATCH_SERVER_H */

