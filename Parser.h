/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Parser.h
 * Author: Benjamin
 *
 * Created on 4 février 2017, 17:52
 */

#ifndef PARSER_H
#define PARSER_H

#include <istream>
#include "Config.h"
#include "IElement.h"

namespace Spatch
{
    namespace Parsing
    {
        class Parser
        {
        public:
            Parser(Spatch::Configuration::Config &conf);
            ~Parser();
            
            void    parse(std::istream &is);
            
        private:
            inline std::string &trim(std::string &s);
            inline std::string &rtrim(std::string &s);
            inline std::string &ltrim(std::string &s);
            
            Spatch::Parsing::IElement   *getElement(std::string &line);
            Spatch::Parsing::IElement   *createElement(std::string &type, std::string &id);
            bool                        setKeyValueToElement(Spatch::Parsing::IElement *elem, std::string &line);
            
            Spatch::Configuration::Config &_conf;
        };
    }
}

#endif /* PARSER_H */

