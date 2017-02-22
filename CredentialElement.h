/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CredentialElement.h
 * Author: Benjamin
 *
 * Created on 13 février 2017, 15:43
 */

#ifndef CREDENTIALELEMENT_H
#define CREDENTIALELEMENT_H

#include <memory>
#include <map>
#include "IElement.h"
#include "Server.h"

namespace Spatch
{
    namespace Parsing
    {
        class CredentialElement : public IElement
        {
        public:
            CredentialElement();
            virtual ~CredentialElement();
            
            virtual void    setId(std::string &id);
            virtual bool    addProperty(std::string &key, std::string &value);
            virtual bool    isValid() const;
            virtual bool    finish(Spatch::Configuration::Config &conf);
            
        private:
            
            std::string                         _id;
            std::map<std::string, std::string>  _prop;
        };
    }
}

#endif /* CREDENTIALELEMENT_H */

