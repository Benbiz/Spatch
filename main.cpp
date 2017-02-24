/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: Benjamin
 *
 * Created on 1 février 2017, 15:21
 */

#include <fstream>
#include <iostream>
#include "Config.h"
#include "Parser.h"
#include "Proxy.h"
#include "Client.h"

int main(int argc, char** argv)
{
    std::filebuf    fb;
    std::istream    is(&fb);
    Spatch::Configuration::Config   conf;
    Spatch::Parsing::Parser         parser(conf);
    
    if (fb.open("spatch.conf",std::ios::in))
    {
        parser.parse(is);
        fb.close();
    }

    Spatch::Ssh::Proxy  proxy(conf, 42);
    
    proxy.setDSAFile("dsa/id_dsa");
    proxy.setRSAFile("rsa/id_rsa");
    if (!proxy.handleClients())
        std::cout << "Error while handling clients" << std::endl; 
    return EXIT_SUCCESS;
}

