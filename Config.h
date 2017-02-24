/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Config.h
 * Author: Benjamin
 *
 * Created on 1 février 2017, 15:40
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <memory>
#include "User.h"
#include "Server.h"
#include "Access.h"
#include "Credential.h"



namespace Spatch
{
    namespace Configuration
    {
        class Config
        {
        public:
            Config();
            ~Config();
            
            void                                                clearAll();
            
            bool                                                addUser(std::shared_ptr<User> u);
            const std::vector<std::shared_ptr<User>>            &getUsers() const;
            bool                                                addServer(std::shared_ptr<Server> s);
            const std::vector<std::shared_ptr<Server>>          &getServers() const;
            bool                                                addCredential(std::shared_ptr<Credential> c);
            const std::vector<std::shared_ptr<Credential>>      &getCredentials() const;
            bool                                                addAccess(std::shared_ptr<Access> a);
            const std::vector<std::shared_ptr<Access>>          &getAccesses() const;
        
        private:
            std::vector<std::shared_ptr<User>>          _users;
            std::vector<std::shared_ptr<Server>>        _servers;
            std::vector<std::shared_ptr<Access>>        _accesses;
            std::vector<std::shared_ptr<Credential>>    _credentials;
            
        };
    }
}

#endif /* CONFIG_H */

