/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IElement.h
 * Author: Benjamin
 *
 * Created on 4 février 2017, 17:45
 */

#ifndef IELEMENT_H
#define IELEMENT_H

#include "Config.h"

namespace Spatch
{
    namespace Parsing
    {
        class IElement
        {
        public:
            virtual ~IElement() {}
            
            virtual void    setId(std::string &id) = 0;
            virtual bool    addProperty(std::string &key, std::string &value) =  0;
            virtual bool    isValid() const = 0;
            virtual bool    finish(Spatch::Configuration::Config &conf) = 0;
        };
    }
}

#endif /* IELEMENT_H */

