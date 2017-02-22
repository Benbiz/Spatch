/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ServerElement.h
 * Author: Benjamin
 *
 * Created on 13 février 2017, 14:56
 */

#ifndef SERVERELEMENT_H
#define SERVERELEMENT_H

#include <map>
#include "IElement.h"

namespace Spatch
{
    namespace Parsing
    {
        class ServerElement : public IElement
        {
        public:
            ServerElement();
            virtual ~ServerElement();
            
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

#endif /* SERVERELEMENT_H */

