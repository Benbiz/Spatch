/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Credential.h
 * Author: Benjamin
 *
 * Created on 1 février 2017, 16:08
 */

#ifndef CREDENTIAL_H
#define CREDENTIAL_H

#include <memory>
#include <string>

namespace Spatch
{
    namespace Configuration
    {
        class Credential
        {
        public:
            Credential(const std::string &id, const std::string &username, const std::string &password);
            ~Credential();
            
            const std::string               &getId() const;
            
            
            const std::string               &getUsername() const;
            void                            setUsername(const std::string &username);
            
            const std::string               &getPassword() const;
            void                            setPassword(const std::string &password);
            
        private:
            std::string                     _id;
            std::string                     _username;
            std::string                     _password;
        };
    }
}

#endif /* CREDENTIAL_H */

