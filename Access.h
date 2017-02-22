/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Access.h
 * Author: Benjamin
 *
 * Created on 1 février 2017, 15:59
 */

#ifndef ACCESS_H
#define ACCESS_H

#include <memory>
#include "Credential.h"
#include "Server.h"
#include "User.h"

namespace Spatch
{
    namespace Configuration
    {
        class Access
        {
        public:
            enum CredentialType
            {
                Provided,
                UserCred,
                Asked
            };
            
            Access(const std::string &id, const std::shared_ptr<User> user, const std::shared_ptr<Credential> cred, const std::shared_ptr<Server> serv);
            Access(const std::string &id, const std::shared_ptr<User> user, const CredentialType type, const std::shared_ptr<Server> serv);
            ~Access();
            
            const std::string                   &getId() const;
            
            const std::shared_ptr<Server>       getServer() const;
            const std::shared_ptr<User>         getUser() const;
            const std::shared_ptr<Credential>   getCredential() const;
            const CredentialType                getCredentialType() const;
            
        private:
            std::string                         _id;
            std::shared_ptr<Credential>         _cred;
            std::shared_ptr<User>               _user;
            std::shared_ptr<Server>             _serv;
            CredentialType                      _type;
        };
    }
}

#endif /* ACCESS_H */

