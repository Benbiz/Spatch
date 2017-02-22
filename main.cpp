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

#include <libssh/libssh.h>
#include <libssh/server.h>
#include <poll.h>
#include <libssh/callbacks.h>
#include <fstream>
#include <iostream>
#include <pty.h>
#include "Config.h"
#include "Parser.h"

#define SSHD_USER "libssh"
#define SSHD_PASSWORD "libssh"

static ssh_event event = ssh_event_new();

ssh_channel connect()
{
    ssh_session my_ssh_session;
    ssh_channel channel;
    int rc;
    // Open session and set options
    my_ssh_session = ssh_new();
    if (my_ssh_session == NULL)
      exit(-1);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, "192.168.0.22");
    // Connect to server
    rc = ssh_connect(my_ssh_session);
    if (rc != SSH_OK)
    {
      fprintf(stderr, "Error connecting to localhost: %s\n",
              ssh_get_error(my_ssh_session));
      ssh_free(my_ssh_session);
      exit(-1);
    }

    rc = ssh_userauth_password(my_ssh_session, "benjamin", "*inj=7KE");
    if (rc != SSH_AUTH_SUCCESS)
    {
      fprintf(stderr, "Error authenticating with password: %s\n",
              ssh_get_error(my_ssh_session));
      ssh_disconnect(my_ssh_session);
      ssh_free(my_ssh_session);
      exit(-1);
    }
    channel = ssh_channel_new(my_ssh_session);
    if (channel == NULL)
    {
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        return NULL;
    }
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
      ssh_channel_free(channel);
      ssh_disconnect(my_ssh_session);
      ssh_free(my_ssh_session);
      return NULL;
    }
    
    rc = ssh_channel_request_pty(channel);
    if (rc != SSH_OK) return NULL;
    rc = ssh_channel_change_pty_size(channel, 80, 24);
    if (rc != SSH_OK) return NULL;
    rc = ssh_channel_request_shell(channel);
    if (rc != SSH_OK) return NULL;
    return channel;
}

static int copy_chan_to_fd(ssh_session session,
                                           ssh_channel channel,
                                           void *data,
                                           uint32_t len,
                                           int is_stderr,
                                           void *userdata) {
    //ssh_channel chan = (ssh_channel)userdata;
    (void)session;
    (void)channel;
    (void)is_stderr;
    int *fd = (int*)userdata;
    std::cout << (char*)data << std::endl;
    return write(*fd, data, len);
}

static void chan_close(ssh_session session, ssh_channel channel, void *userdata) {
    (void)session;
    (void)channel;
    std::cout << "close" << std::endl;
}

static void chan_eof(ssh_session session, ssh_channel channel, void *userdata) {
    (void)session;
    (void)channel;
    std::cout << "eof" << std::endl;
}

static int copy_fd_to_chan(socket_t fd, int revents, void *userdata) {
    ssh_channel chan = (ssh_channel)userdata;
    char buf[2048];
    int sz = 0;

    if(!chan) {
        close(fd);
        return -1;
    }
    if(revents & POLLIN) {
        sz = read(fd, buf, 2048);
        if(sz > 0) {
            ssh_channel_write(chan, buf, sz);
        }
    }
    if(revents & POLLHUP) {
        ssh_channel_close(chan);
        sz = -1;
    }
    return sz;
}

static int main_loop(ssh_channel chan) {
    
    int *fd = new int;
    int slave;
    openpty(fd, &slave, NULL, NULL, NULL);
    if(ssh_event_add_session(event, ssh_channel_get_session(chan)) != SSH_OK) {
        printf("Couldn't add the session to the event\n");
        return -1;
    }
    //ssh_channel con = connect();
    ssh_channel_callbacks cb = new ssh_channel_callbacks_struct;
    cb->channel_data_function = copy_chan_to_fd;
    cb->channel_eof_function = chan_eof;
    cb->channel_close_function = chan_close;
    cb->userdata = fd;
    
    /*struct ssh_channel_callbacks_struct cb2;
    cb2.channel_data_function = copy_chan_to_fd;
    cb2.channel_eof_function = chan_eof;
    cb2.channel_close_function = chan_close;
    cb2.userdata = chan;*/
    
    //ssh_session session = ssh_channel_get_session(chan);
    //ssh_event event;
    //short events;

    ssh_callbacks_init(cb);
    //ssh_callbacks_init(&cb2);
    ssh_set_channel_callbacks(chan, cb);

    //ssh_set_channel_callbacks(con, &cb2);
    if(ssh_event_add_fd(event, *fd, POLLIN | POLLPRI | POLLERR | POLLHUP | POLLNVAL, copy_fd_to_chan, chan) != SSH_OK) {
        printf("Couldn't add an fd to the event\n");
        return -1;
    }

   /* events = POLLIN | POLLPRI | POLLERR | POLLHUP | POLLNVAL;

    event = ssh_event_new();
    if(event == NULL) {
        printf("Couldn't get a event\n");
        return -1;
    }
    if(ssh_event_add_session(event, session) != SSH_OK) {
        printf("Couldn't add the session to the event\n");
        return -1;
    }
    
    if(ssh_event_add_session(event, ssh_channel_get_session(con)) != SSH_OK) {
        printf("Couldn't add the session to the event\n");
        return -1;
    }

    do {
        ssh_event_dopoll(event, 1000);
        std::cout << "event loop" << std::endl;
    } while(!ssh_channel_is_closed(chan));

    ssh_event_remove_session(event, session);
    ssh_event_remove_session(event, ssh_channel_get_session(con));
    ssh_event_free(event);
    ssh_channel_free(con);
    ssh_disconnect(ssh_channel_get_session(con));
    ssh_free(ssh_channel_get_session(con));
    std::cout << "endind loop" << std::endl;*/
    return 0;
}


static int auth_password(const char *user, const char *password){
    if(strcmp(user, SSHD_USER))
        return 0;
    if(strcmp(password, SSHD_PASSWORD))
        return 0;
    return 1; // authenticated
}

static int authenticate(ssh_session session) {
    ssh_message message;

    do {
        message=ssh_message_get(session);
        if(!message)
            break;
        switch(ssh_message_type(message)){
            case SSH_REQUEST_AUTH:
                switch(ssh_message_subtype(message)){
                    case SSH_AUTH_METHOD_PASSWORD:
                        printf("User %s wants to auth with pass %s\n",
                               ssh_message_auth_user(message),
                               ssh_message_auth_password(message));
                        if(auth_password(ssh_message_auth_user(message),
                           ssh_message_auth_password(message))){
                               ssh_message_auth_reply_success(message,0);
                               ssh_message_free(message);
                               return 1;
                           }
                        ssh_message_auth_set_methods(message,
                                                SSH_AUTH_METHOD_PASSWORD |
                                                SSH_AUTH_METHOD_INTERACTIVE);
                        // not authenticated, send default message
                        ssh_message_reply_default(message);
                        break;

                    case SSH_AUTH_METHOD_NONE:
                    default:
                        printf("User %s wants to auth with unknown auth %d\n",
                               ssh_message_auth_user(message),
                               ssh_message_subtype(message));
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
    } while (1);
    return 0;
}

int  incomingConnectionCallback(socket_t fd, int revents, void
*userdata)
{
    ssh_session session;
    ssh_message message;
    ssh_channel chan=0;
    ssh_bind sshbind = (ssh_bind)userdata;
    int auth=0;
    int shell=0;
    int r;
    
    session=ssh_new();
    r = ssh_bind_accept(sshbind, session);
    if(r==SSH_ERROR){
      printf("Error accepting a connection: %s\n", ssh_get_error(sshbind));
      return 1;
    }
    if (ssh_handle_key_exchange(session)) {
        printf("ssh_handle_key_exchange: %s\n", ssh_get_error(session));
        return 1;
    }

    /* proceed to authentication */
    auth = authenticate(session);
    if(!auth){
        printf("Authentication error: %s\n", ssh_get_error(session));
        ssh_disconnect(session);
        return 1;
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
        printf("Error: cleint did not ask for a channel session (%s)\n",
                                                    ssh_get_error(session));
        ssh_finalize();
        return 1;
    }


    /* wait for a shell */
    do {
        message = ssh_message_get(session);
        if(message != NULL) {
            if(ssh_message_type(message) == SSH_REQUEST_CHANNEL) {
                if(ssh_message_subtype(message) == SSH_CHANNEL_REQUEST_SHELL) {
                    shell = 1;
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
        printf("Error: No shell requested (%s)\n", ssh_get_error(session));
        return 1;
    }

    printf("it works !\n");

    main_loop(chan);
    return 0;
}


int test_sshd()
{
    ssh_bind sshbind;

    sshbind=ssh_bind_new();

    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_DSAKEY,
                                            "dsa/id_dsa");
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_RSAKEY,
                                            "rsa/id_rsa");
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDPORT_STR, "42");

    if(ssh_bind_listen(sshbind)<0){
        printf("Error listening to socket: %s\n", ssh_get_error(sshbind));
        return 1;
    }
    printf("Started sample libssh sshd on port %d\n", 22);
    printf("You can login as the user %s with the password %s\n", SSHD_USER,
                                                            SSHD_PASSWORD);
    
    int events = POLLIN | POLLPRI | POLLERR | POLLHUP | POLLNVAL;

    if(event == NULL) {
        printf("Couldn't get a event\n");
        return -1;
    }
    ssh_event_add_fd(event, ssh_bind_get_fd(sshbind), events, incomingConnectionCallback, sshbind);

    do {
        ssh_event_dopoll(event, 1000);
        std::cout << "event loop" << std::endl;
    } while(true);

    ssh_bind_free(sshbind);
    ssh_finalize();
    return 0;
}

#include "Proxy.h"
#include "Client.h"

int main(int argc, char** argv)
{
    std::filebuf    fb;
    std::istream    is(&fb);
    Spatch::Configuration::Config   conf;
    Spatch::Parsing::Parser         parser(conf);
    
    if (fb.open("test.conf",std::ios::in))
    {
        parser.parse(is);
        fb.close();
    }
    
    //test_sshd();
    Spatch::Ssh::Proxy  proxy(conf, 42);
    
    proxy.setDSAFile("dsa/id_dsa");
    proxy.setRSAFile("rsa/id_rsa");
    if (!proxy.handleClients())
        std::cout << "Error while handling clients" << std::endl; 
    return EXIT_SUCCESS;
}

