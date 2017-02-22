/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AccessElement.h
 * Author: Benjamin
 *
 * Created on 13 février 2017, 16:28
 */

#ifndef ACCESSELEMENT_H
#define ACCESSELEMENT_H

#include <memory>
#include <map>
#include "IElement.h"
#include "User.h"
#include "Credential.h"
#include "Server.h"

namespace Spatch
{
    namespace Parsing
    {
        class AccessElement : public IElement
        {
        public:
            AccessElement();
            virtual ~AccessElement();
            
            virtual void    setId(std::string &id);
            virtual bool    addProperty(std::string &key, std::string &value);
            virtual bool    isValid() const;
            virtual bool    finish(Spatch::Configuration::Config &conf);
            
        private:
            std::shared_ptr<Spatch::Configuration::Server>      getServerRef(Spatch::Configuration::Config &conf, std::string &ref);
            std::shared_ptr<Spatch::Configuration::Credential>  getCredentialRef(Spatch::Configuration::Config &conf, std::string &ref);
            std::shared_ptr<Spatch::Configuration::User>        getUserRef(Spatch::Configuration::Config &conf, std::string &ref);
            
            std::string                         _id;
            std::map<std::string, std::string>  _prop;
        };
    }
}

#endif /* ACCESSELEMENT_H */

