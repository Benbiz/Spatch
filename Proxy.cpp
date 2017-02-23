/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <iostream>
#include <algorithm>
#include "Proxy.h"

Spatch::Ssh::Proxy::Proxy(const Spatch::Configuration::Config &conf, const int port)
    : _conf(conf), _port(port)
{
    ssh_threads_set_callbacks(ssh_threads_get_pthread());
    ssh_init();
    _event = ssh_event_new();
    _bind = ssh_bind_new();
    ssh_bind_options_set(_bind, SSH_BIND_OPTIONS_BINDPORT, &port);

}

Spatch::Ssh::Proxy::~Proxy()
{
    _clients.clear();
    ssh_event_remove_fd(_event, ssh_bind_get_fd(_bind));
    ssh_event_free(_event);
    ssh_bind_free(_bind);
    ssh_finalize();
}

void    Spatch::Ssh::Proxy::setRSAFile(const std::string &RSAFile)
{
    ssh_bind_options_set(_bind, SSH_BIND_OPTIONS_RSAKEY, RSAFile.c_str());
}

void    Spatch::Ssh::Proxy::setDSAFile(const std::string &DSAFile)
{
    ssh_bind_options_set(_bind, SSH_BIND_OPTIONS_DSAKEY, DSAFile.c_str());
}

std::shared_ptr<Spatch::Configuration::User>    Spatch::Ssh::Proxy::authClient(ssh_session session)
{
    ssh_message message;
    std::shared_ptr<Spatch::Configuration::User> u;

    do {
        message=ssh_message_get(session);
        if(!message)
            break;
        switch(ssh_message_type(message)){
            case SSH_REQUEST_AUTH:
                switch(ssh_message_subtype(message)){
                    case SSH_AUTH_METHOD_PASSWORD:
                        std::cout   << "User " << ssh_message_auth_user(message)
                                << " wants to auth with pass "
                               << ssh_message_auth_password(message) << std::endl;
                        if(u = getUser(ssh_message_auth_user(message),
                           ssh_message_auth_password(message))){
                               ssh_message_auth_reply_success(message,0);
                               ssh_message_free(message);
                               return u;
                           }
                        ssh_message_auth_set_methods(message,
                                                SSH_AUTH_METHOD_PASSWORD |
                                                SSH_AUTH_METHOD_INTERACTIVE);
                        // not authenticated, send default message
                        ssh_message_reply_default(message);
                        break;

                    case SSH_AUTH_METHOD_NONE:
                    default:
                        std::cout   << "User " << ssh_message_auth_user(message)
                                    << " wants to auth with unknown auth "
                                    << ssh_message_subtype(message) << std::endl;
                        ssh_message_auth_set_methods(message,
                                                SSH_AUTH_METHOD_PASSWORD |
                                                SSH_AUTH_METHOD_INTERACTIVE);
                        ssh_message_reply_default(message);
                        break;
                }
                break;
            default:
                ssh_message_auth_set_methods(message,
                                                SSH_AUTH_METHOD_PASSWORD |
                                                SSH_AUTH_METHOD_INTERACTIVE);
                ssh_message_reply_default(message);
        }
        ssh_message_free(message);
    } while (true);
    return nullptr;
}

bool    Spatch::Ssh::Proxy::handleClients()
{
    int events = POLLIN | POLLPRI | POLLERR | POLLHUP | POLLNVAL;
    
    if(ssh_bind_listen(_bind)<0){
        std::cout << "Error listening to socket: " <<  ssh_get_error(_bind) << std::endl;
        return false;
    }
    std::cout << "Started sshd on port " << _port << std::endl;;
    
    ssh_event_add_fd(_event, ssh_bind_get_fd(_bind), events, incomingConnectionCallback, this);

    do {
        ssh_event_dopoll(_event, 1000);
        _clients.erase(std::remove_if(_clients.begin(), _clients.end(), [] (std::shared_ptr<Client> c)
        {
            if (c->isClose())
                return true;
            return false;
        }), _clients.end());
    } while(true);
    return 0;
}

std::shared_ptr<Spatch::Ssh::Client>     Spatch::Ssh::Proxy::getClient()
{
    ssh_session session;
    ssh_message message;
    std::shared_ptr<Spatch::Configuration::User> user;
    ssh_channel chan = 0;
    bool shell = 0;
    int r;
    
    session=ssh_new();
    r = ssh_bind_accept(_bind, session);
    if(r == SSH_ERROR){
      std::cout << "Error accepting a connection: " << ssh_get_error(_bind) << std::endl;;
      return nullptr;
    }
    if (ssh_handle_key_exchange(session)) {
        std::cout << "ssh_handle_key_exchange: " << ssh_get_error(session) << std::endl;
        return nullptr;
    }
    
    if((user = authClient(session)) == nullptr){
        std::cout << "Authentication error: " << ssh_get_error(session) << std::endl;
        ssh_disconnect(session);
        return nullptr;
    }

    /* wait for a channel session */
    do {
        message = ssh_message_get(session);
        if(message){
            if(ssh_message_type(message) == SSH_REQUEST_CHANNEL_OPEN &&
                    ssh_message_subtype(message) == SSH_CHANNEL_SESSION) {
                chan = ssh_message_channel_request_open_reply_accept(message);
                ssh_message_free(message);
                break;
            } else {
                ssh_message_reply_default(message);
                ssh_message_free(message);
            }
        } else {
            break;
        }
    } while(!chan);

    if(!chan) {
        std::cout << "Error: cleint did not ask for a channel session " << ssh_get_error(session) << std::endl;
        ssh_finalize();
        return nullptr;
    }


    /* wait for a shell */
    do {
        message = ssh_message_get(session);
        if(message != NULL) {
            if(ssh_message_type(message) == SSH_REQUEST_CHANNEL) {
                if(ssh_message_subtype(message) == SSH_CHANNEL_REQUEST_SHELL) {
                    shell = true;
                    ssh_message_channel_request_reply_success(message);
                    ssh_message_free(message);
                    break;
                } else if(ssh_message_subtype(message) == SSH_CHANNEL_REQUEST_PTY) {
                    ssh_message_channel_request_reply_success(message);
                    ssh_message_free(message);
                    continue;
                }
            }
            ssh_message_reply_default(message);
            ssh_message_free(message);
        } else {
            break;
        }
    } while(!shell);

    if(!shell) {
        std::cout << "Error: No shell requested " << ssh_get_error(session) << std::endl;
        return nullptr;
    }
    
    std::shared_ptr<Spatch::Ssh::Client> client = std::make_shared<Spatch::Ssh::Client>(_conf, user, chan, _event);
    return client;
}

std::shared_ptr<Spatch::Configuration::User>    Spatch::Ssh::Proxy::getUser(const std::string &username, const std::string &password)
{
    for (std::shared_ptr<Spatch::Configuration::User> u : _conf.getUsers())
    {
        if (u->getUsername() == username && u->getPassword() == password)
            return u;
    }
    return nullptr;
}

int  Spatch::Ssh::Proxy::incomingConnectionCallback(socket_t fd, int revents, void *userdata)
{
    Spatch::Ssh::Proxy  *proxy = static_cast<Spatch::Ssh::Proxy *>(userdata);
    
    std::shared_ptr<Spatch::Ssh::Client>    client;
    client = proxy->getClient();
    if (client != nullptr)
    {
        proxy->_clients.push_back(client);
        return 0;
    }
    return 1;
}