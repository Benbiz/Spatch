/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Client.h
 * Author: Benjamin
 *
 * Created on 19 février 2017, 19:37
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <libssh/callbacks.h>
#include <memory>
#include <poll.h>
#include "Config.h"
#include "Connection.h"
#include "Shell.h"

namespace Spatch
{
    namespace Ssh
    {
        class Client
        {
        public:
            Client(const Spatch::Configuration::Config &conf, const std::shared_ptr<Spatch::Configuration::User> u, ssh_channel channel, ssh_event event);
            ~Client();
            
            bool                                                isEOF() const;
            bool                                                isClose() const;
            
            
            
            void                                                proxify(std::shared_ptr<Connection> connection);
            const std::shared_ptr<Connection>                   getProxifiyConnection() const;
            
            int                                                 getSlaveFd() const;
            const std::shared_ptr<Spatch::Configuration::User>  getUser() const;
        private:
            
            /*
             * Master PTY Synchronization with channel
             */
            
            int                                         masterPtyToChannel(int revents);
            int                                         channelToMasterPty(void *data, uint32_t len);
            
            /*
             * Slave PTY Callback
             */
            
            int                                         onSlaveEvent(int revents);
            
            /*
             *  C Type callbacks
             */
            
            static int                                  fdMasterCallback(socket_t fd, int revents, void *userdata);
            static int                                  channelCallback(ssh_session session, ssh_channel channel,
                                                                        void *data, uint32_t len,
                                                                        int is_stderr,
                                                                        void *userdata);
            
            static int                                  fdSlaveCallback(socket_t fd, int revents, void *userdata);
            
            /*
             * LibSSH Properties
             */
            ssh_session                                         _session;
            ssh_channel                                         _channel;
            ssh_event                                           _event;
            ssh_channel_callbacks_struct                        _callbacks;
            
            /*
             * PTY Properties
             */
            
            int                                                 _masterpty;
            int                                                 _slavepty;
            
            /*
             * Spatch related properties 
             */
            
            const Spatch::Configuration::Config                 &_conf;
            const std::shared_ptr<Spatch::Configuration::User>  _user;
            Spatch::Ssh::Shell                                  _shell;
            std::shared_ptr<Connection>                         _connection;
        };
    }
}

#endif /* CLIENT_H */

