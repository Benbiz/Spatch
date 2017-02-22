/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   UserElement.h
 * Author: Benjamin
 *
 * Created on 4 février 2017, 17:48
 */

#ifndef USERELEMENT_H
#define USERELEMENT_H

#include <string>
#include <map>
#include "IElement.h"

namespace Spatch
{
    namespace Parsing
    {
        class UserElement : public IElement
        {
        public:
            UserElement();
            virtual ~UserElement();
            
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

#endif /* USERELEMENT_H */

