/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   User.h
 * Author: Benjamin
 *
 * Created on 1 février 2017, 15:32
 */

#ifndef USER_H
#define USER_H

#include <string>

namespace Spatch
{
    namespace Configuration
    {
        class User
        {
        public:
            
            enum Privilege
            {
                BasicUser,
                Admin
            };
            
            User(const std::string &id, const std::string &username, const std::string &password, const Privilege right);
            ~User();
            
            const std::string       &getId() const;
            
            const std::string       &getUsername() const;
            void                    setUsername(const std::string &username);
            
            const std::string       &getPassword() const;
            void                    setPassword(const std::string &password);
            
            const Privilege         getPrivilege() const;
            void                    setPrivilege(const Privilege right);
        private:
            std::string _username;
            std::string _id;
            std::string _password;
            Privilege   _priv;
        };
    }
}

#endif /* USER_H */

