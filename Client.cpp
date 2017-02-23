/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <sys/ioctl.h>
#include <pty.h>
#include <iostream>
#include "Client.h"

Spatch::Ssh::Client::Client(const Spatch::Configuration::Config &conf, const std::shared_ptr<Spatch::Configuration::User> u, ssh_channel channel, ssh_event event)
    : _conf(conf), _user(u), _connection(nullptr), _shell(conf, *this), _channel(channel), _session(ssh_channel_get_session(channel)), _event(event)
{
    openpty(&_masterpty, &_slavepty, NULL, NULL, NULL);
    
    _callbacks.channel_data_function = channelCallback;
    _callbacks.userdata = this;
    ssh_callbacks_init(&_callbacks);
    ssh_set_channel_callbacks(_channel, &_callbacks);
    
    if(ssh_event_add_session(_event, _session) != SSH_OK)
        return ;
    if(ssh_event_add_fd(_event, _masterpty, POLLIN | POLLPRI | POLLERR | POLLHUP | POLLNVAL, fdMasterCallback, this) != SSH_OK)
        return ;
    if(ssh_event_add_fd(_event, _slavepty, POLLIN | POLLPRI | POLLERR | POLLHUP | POLLNVAL, fdSlaveCallback, this) != SSH_OK)
        return ;
    
    _shell.onWelcome();
    
}

Spatch::Ssh::Client::~Client()
{
    ssh_event_remove_fd(_event, _masterpty);
    ssh_event_remove_fd(_event, _slavepty);
    ssh_event_remove_session(_event, _session);
    ssh_channel_free(_channel);
    ssh_disconnect(_session);
    ssh_free(_session);
}

bool                                        Spatch::Ssh::Client::isEOF() const
{
    return ssh_channel_is_eof(_channel);
}

bool                                        Spatch::Ssh::Client::isClose() const
{
    return ssh_channel_is_closed(_channel);
}

void                                        Spatch::Ssh::Client::proxify(std::shared_ptr<Spatch::Configuration::Access> access)
{
    if (access == nullptr)
    {
        _connection = nullptr;
        _shell.printPrompt();
    }
    else
    {
        _connection = std::make_shared<Spatch::Ssh::Connection>(*this, access->getServer()->getIp(), access->getServer()->getPort(), _event);
        if (access->getCredentialType() == Spatch::Configuration::Access::CredentialType::Provided)
            _connection->connect(access->getCredential()->getUsername(), access->getCredential()->getPassword());
        else if (access->getCredentialType() == Spatch::Configuration::Access::CredentialType::UserCred)
            _connection->connect(_user->getUsername(), _user->getPassword());
        else
            _connection->connect();
    }
}

const std::shared_ptr<Spatch::Ssh::Connection>           Spatch::Ssh::Client::getProxifiyConnection() const
{
    return nullptr;
}

int                                        Spatch::Ssh::Client::masterPtyToChannel(int revents)
{
    char buf[2048];
    int sz = 0;

    if(!_channel)
    {
        close(_masterpty);
        return 0;
    }
    if(revents & POLLIN)
    {
        sz = read(_masterpty, buf, 2048);
        if(sz > 0)
            return (ssh_channel_write(_channel, buf, sz));
    }
    if(revents & POLLHUP)
        return (ssh_channel_close(_channel));
    return 0;
}

int                                        Spatch::Ssh::Client::onSlaveEvent(int revents)
{
    if(!_channel)
    {
        close(_masterpty);
        return 0;
    }
    if(revents & POLLIN)
    {
        if (!_connection)
            return (_shell.onRead());
    }
    if(revents & POLLHUP)
        return (ssh_channel_close(_channel));
    return 0;
    
}

int                                        Spatch::Ssh::Client::writeToChannel(const void *data, uint32_t len)
{
    return (ssh_channel_write(_channel, data, len));
}

int                                        Spatch::Ssh::Client::channelToMasterPty(void *data, uint32_t len)
{
    if (!_connection)
        return(write(_masterpty, data, len));
    else
        return (_connection->writeToChannel(data, len));
}

int                                             Spatch::Ssh::Client::getSlaveFd() const
{
    return _slavepty;
}

const std::shared_ptr<Spatch::Configuration::User>  Spatch::Ssh::Client::getUser() const
{
    return _user;
}

/*
 * C Type Callbacks private static member function
 */

int Spatch::Ssh::Client::fdMasterCallback(socket_t fd, int revents, void *userdata)
{
    Spatch::Ssh::Client *client = static_cast<Spatch::Ssh::Client *>(userdata);
    return(client->masterPtyToChannel(revents));
}

int Spatch::Ssh::Client::fdSlaveCallback(socket_t fd, int revents, void *userdata)
{
    Spatch::Ssh::Client *client = static_cast<Spatch::Ssh::Client *>(userdata);
    return (client->onSlaveEvent(revents));
}

int Spatch::Ssh::Client::channelCallback(ssh_session session,
                                         ssh_channel channel,
                                         void *data,
                                         uint32_t len,
                                         int is_stderr,
                                         void *userdata)
{
    Spatch::Ssh::Client *client = static_cast<Spatch::Ssh::Client *>(userdata);
    return(client->channelToMasterPty(data, len));
}