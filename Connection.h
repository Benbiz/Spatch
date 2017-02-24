/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   connection.h
 * Author: Benjamin
 *
 * Created on 19 février 2017, 19:25
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <libssh/libssh.h>
#include <libssh/callbacks.h>

namespace Spatch
{
    namespace Ssh
    {
        class Client;
        class Connection
        {
        public:
            Connection(Spatch::Ssh::Client &client, const std::string &host, const int port, ssh_event event);
            ~Connection();
            
            bool    connect(const std::string &username, const std::string &password, const std::string &command = "");
            bool    connect(const std::string &command = "");
            int     writeToChannel(void *buf, uint32_t len);
            bool    closeChannel();
        private:
            
            int                                         channelToClient(void *data, uint32_t len);
            
            static int                                  channelCallback(ssh_session session, ssh_channel channel,
                                                                        void *data, uint32_t len,
                                                                        int is_stderr,
                                                                        void *userdata);
            
            void                                        onClose();
            static void                                 chan_close(ssh_session session, ssh_channel channel, void *userdata);
            
            bool                                        askAuth();
            
            int                                         masterPtyToChannel(int revents);
            static int                                  fdMasterCallback(socket_t fd, int revents, void *userdata);
            
            int                                         slaveEvent(int revents);
            static int                                  slaveEventCB(socket_t fd, int revents, void *userdata);
            
            
            ssh_session                     _session;
            ssh_channel                     _channel;
            ssh_channel_callbacks_struct    _callbacks;
            ssh_event                       _event;
            
            std::string                     _host;
            int                             _port;
            Spatch::Ssh::Client             &_client;
            
            std::string                     _username;
            std::string                     _password;
            std::string                     _command;
            
            pthread_t                       _thread;
            int                             _masterpty;
            int                             _slavepty;
        };
    }
}

#endif /* CONNECTION_H */

