/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <pty.h>
#include <sstream>
#include <iostream>
#include <errno.h>
#include "Client.h"
#include "Connection.h"

Spatch::Ssh::Connection::Connection(Spatch::Ssh::Client& client, const std::string& host, const int port, ssh_event event)
: _client(client), _host(host), _port(port), _event(event), _masterpty(-1), _slavepty(-1)
{
    _session = ssh_new();
    ssh_options_set(_session, SSH_OPTIONS_HOST, _host.c_str());
    ssh_options_set(_session, SSH_OPTIONS_PORT, &_port);
    
    _callbacks.channel_data_function = channelCallback;
    _callbacks.channel_eof_function = NULL;
    _callbacks.channel_close_function = chan_close;
    _callbacks.userdata = this;
     
    ssh_callbacks_init(&_callbacks);
}

Spatch::Ssh::Connection::~Connection()
{
    if (_channel != nullptr)
    {
        ssh_event_remove_session(_event, ssh_channel_get_session(_channel));
        if (!ssh_channel_is_eof(_channel))
            ssh_channel_send_eof(_channel);
        if (!ssh_channel_is_closed(_channel))
            ssh_channel_close(_channel);
        ssh_channel_free(_channel);
    }
    if (_masterpty != -1)
    {
        ssh_event_remove_fd(_event, _masterpty);
        ssh_event_remove_fd(_event, _slavepty);
        close(_masterpty);
        _masterpty = -1;
        _slavepty = -1;
    }
}

void        Spatch::Ssh::Connection::onClose()
{
    _client.proxify(nullptr);
}

void Spatch::Ssh::Connection::chan_close(ssh_session session, ssh_channel channel, void *userdata) {
    Spatch::Ssh::Connection *connection = static_cast<Spatch::Ssh::Connection *>(userdata);
    return(connection->onClose());
}

bool    Spatch::Ssh::Connection::connect(const std::string &username, const std::string &password, const std::string &command)
{
    _username = username;
    _password = password;
    _command = command;
    connect();
}

bool    Spatch::Ssh::Connection::connect(const std::string &command)
{
    if (!command.empty())
        _command = command;
    if (_username.empty() || _password.empty())
    {
        askAuth();
        return true;
    }
    else if (_masterpty != -1)
    {
        ssh_event_remove_fd(_event, _masterpty);
        ssh_event_remove_fd(_event, _slavepty);
        close(_masterpty);
        _masterpty = -1;
        _slavepty = -1;
    }
    int rc;
    rc = ssh_connect(_session);
    if (rc != SSH_OK)
        return false;
    rc = ssh_userauth_password(_session, _username.c_str(), _password.c_str());
    if (rc != SSH_AUTH_SUCCESS)
        return false;
    _channel = ssh_channel_new(_session);
    if (_channel == NULL)
        return false;
    ssh_set_channel_callbacks(_channel, &_callbacks);
    rc = ssh_channel_open_session(_channel);
    if (rc != SSH_OK)
        return false;
    
    rc = ssh_channel_request_pty(_channel);
    if (rc != SSH_OK)
        return false;
    rc = ssh_channel_change_pty_size(_channel, 80, 24);
    if (rc != SSH_OK)
        return false;
    ssh_event_add_session(_event, ssh_channel_get_session(_channel));
    if (_command.empty())
    {
        rc = ssh_channel_request_shell(_channel);
        if (rc != SSH_OK)
            return false;
    }
    else
    {
        rc = ssh_channel_request_exec(_channel, _command.c_str());
        if (rc != SSH_OK)
            return false;
    }

    return true;
}

int     Spatch::Ssh::Connection::writeToChannel(void *data, uint32_t len)
{
    if (_masterpty == -1)
        return(ssh_channel_write(_channel, data, len));
    else
    {
        if (len == 1 && ((unsigned char *)data)[0] == 0x04)
        {
            _client.writeToChannel("\n\r", 2);
            onClose();
            return (1);
        }
        return (write(_masterpty, data, len));
    }
}

bool    Spatch::Ssh::Connection::closeChannel()
{
    ssh_channel_close(_channel);
    return true;
}

int     Spatch::Ssh::Connection::channelToClient(void *data, uint32_t len)
{
    return(_client.writeToChannel(data, len));
}

int Spatch::Ssh::Connection::channelCallback(ssh_session session,
                                         ssh_channel channel,
                                         void *data,
                                         uint32_t len,
                                         int is_stderr,
                                         void *userdata)
{
    Spatch::Ssh::Connection *connection = static_cast<Spatch::Ssh::Connection *>(userdata);
    return(connection->channelToClient(data, len));
}

bool                                        Spatch::Ssh::Connection::askAuth()
{
    int size;
    int rc;
    
    if (_masterpty == -1)
    {
        rc = openpty(&_masterpty, &_slavepty, NULL, NULL, NULL);
        if (rc != 0)
            return false;
        if(ssh_event_add_fd(_event, _masterpty, POLLIN | POLLPRI | POLLERR | POLLHUP | POLLNVAL, Spatch::Ssh::Connection::fdMasterCallback, this) != SSH_OK)
            return false;
        if(ssh_event_add_fd(_event, _slavepty, POLLIN | POLLPRI | POLLERR | POLLHUP | POLLNVAL, Spatch::Ssh::Connection::slaveEventCB, this) != SSH_OK)
            return false;
    }
    if (_username.empty())
    {
        size = write(_slavepty, "login as: ", strlen("login as: "));
        if (size <= 0)
            return false;
    }
    else if (_password.empty())
    {
        struct termios  tios;
        tcgetattr(_masterpty, &tios);        
        tios.c_lflag &= ~ECHO;
        tcsetattr(_masterpty, TCSANOW, &tios);
        std::stringstream   ss;
        ss << _username << "'s password: ";
        size = write(_slavepty, ss.str().c_str(), ss.str().length());
        if (size <= 0)
            return false;
    }
    return true;
}

int                                        Spatch::Ssh::Connection::masterPtyToChannel(int revents)
{
    char buf[2048];
    int sz = 0;

    if(revents & POLLIN)
    {
        sz = read(_masterpty, buf, 2048);
        return _client.writeToChannel(buf, sz);
    }
    return 0;
}

int                                 Spatch::Ssh::Connection::fdMasterCallback(socket_t fd, int revents, void *userdata)
{
    Spatch::Ssh::Connection *connection = static_cast<Spatch::Ssh::Connection *>(userdata);
    return(connection->masterPtyToChannel(revents));
}

int                                        Spatch::Ssh::Connection::slaveEvent(int revents)
{
    char buf[2048];
    int sz = 0;

    if (revents & POLLHUP)
    {
        onClose();
        return -1;
    }
    if(revents & POLLIN)
    {
        sz = read(_slavepty, buf, 2048);
        if (sz <= 0)
        {
            onClose();
            return -1;
        }
        buf[sz - 1] = 0;
        if (_username.empty())
        {
            _username = buf;
            if (!connect())
            {
                onClose();
                return -1;
            }
        }
        else
        {
            _password = buf;
           _client.writeToChannel("\n\r", 2);
            if (!connect())
            {
                onClose();
                return -1;
            }
        }
    }
    return 0;
}

int                                 Spatch::Ssh::Connection::slaveEventCB(socket_t fd, int revents, void *userdata)
{
    Spatch::Ssh::Connection *connection = static_cast<Spatch::Ssh::Connection *>(userdata);
    return(connection->slaveEvent(revents));
}

