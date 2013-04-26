/**
 * Copyright (C) 2012  TOYOTA MOTOR CORPORATION.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */
#ifndef CONTROLWEBSOCKETCLIENTAPP_H_
#define CONTROLWEBSOCKETCLIENTAPP_H_

#include <pthread.h>
#include <sys/time.h>

#include <map>
#include <vector>

#include <libwebsockets.h>

#include "controlwebsocket.h"
#include "controlwebsocketclient.h"
#include "standardjsonmessage.h"

class ControlWebsocketClientApp : public ControlWebsocketClient {
public:
    ControlWebsocketClientApp();
    ~ControlWebsocketClientApp();
    bool
    initialize(int port);
    bool
    send(std::string type, std::string name, std::string propertyname,
         std::string value, double timestamp);
    bool
    receive(char *keyeventtype, timeval recordtime, void *data, size_t len);
    void
    observation();
    static int
    callback_receive(libwebsocket_context *context, libwebsocket *wsi,
                     libwebsocket_callback_reasons reason, void *user, void *in,
                     size_t len);
    static void *
    run(void *arg);

    static pthread_mutex_t mutex_scenario;
    static pthread_cond_t cond_scenario;
    static std::string vehiclename_scenario;

private:
    libwebsocket_context *context;
    libwebsocket* socket;
    libwebsocket_protocols protocollist[2];
    JsonMessage jsonmsg;
    pthread_t threadid;
    pthread_mutex_t mutex;
    char buf[LWS_SEND_BUFFER_PRE_PADDING + JsonMessage::BUFSIZE
             + LWS_SEND_BUFFER_POST_PADDING];
};
#endif // #ifndef CONTROLWEBSOCKETCLIENTAPP_H_
