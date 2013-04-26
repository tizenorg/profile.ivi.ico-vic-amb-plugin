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
#ifndef CONTROLWEBSOCKET_H_
#define CONTROLWEBSOCKET_H_

#include <pthread.h>
#include <sys/time.h>

#include <map>
#include <vector>

#include <libwebsockets.h>

#include "eventmessage.h"
#include "datamessage.h"

class MWIF;

/**
 * Class that manages the Websocket.
 */
class ControlWebsocket {
public:
    /**
     * The protocol used to Websocket communicate.
     */
    enum ServerProtocol {
        DATA_STANDARD, CONTROL_STANDARD, DATA_CUSTOM, CONTROL_CUSTOM
    };
    /**
     * Constructor.
     */
    ControlWebsocket();
    /**
     * Destructor.
     */
    ~ControlWebsocket();
    /**
     * Initialization.
     *
     * @param port The port number used by websocket server.
     * @param type Kind of the protocol.
     * @return Success : true Failure : false
     */
    bool
    initialize(int port, enum ServerProtocol stype);
    /**
     * This function sends a message to the MW.
     * 
     * @param commid socket id.
     * @param keyeventtype The name of vehicle information of MW.
     * @param time Time of the message.
     * @param data Data of the message.
     * @return Success : true Failure : false
     */
    bool
    send(int commid, char *keyeventtype, timeval time, void *data, size_t len);
    /**
     * This function analyzes the received message.
     *
     * @param commid socket id.
     * @param keyeventtype The name of vehicle information of MW.
     * @param time Time of the message.
     * @param data Data of the message.
     * @param len Length of the message.
     * @return Success : true Failure : false
     */
    bool
    receive(int commid, char *keyeventtype, timeval recordtime, void *data,
            size_t len);
    /**
     * This function monitors the file descriptor for Websocket Server.
     */
    void
    observation();
    /**
     * Registration of the socket. This function retrieves the socket information from the socket ID.
     * 
     * @param commid socket id.
     * @return Success : true Failure : false
     */
    bool
    registSocket(int commid);
    /**
     * Delete the socket. This function removes the socket information from the socket ID.
     *
     * @param commid socket id.
     * @return Success : true Failure : false
     */
    bool
    unregistSocket(int commid);
    /**
     * Callback function for libwebsockets
     *
     * @param context Websockets context
     * @param wsi Opaque websocket instance pointer
     * @param reason The reason for the call
     * @param user Pointer to per-session user data allocated by library.
     * @param in Pointer used for some callback reasons
     * @param len Length set for some callback reasons.
     */
    static int
    callback_receive(libwebsocket_context *context, libwebsocket *wsi,
                     libwebsocket_callback_reasons reason, void *user, void *in,
                     size_t len);
    /**
     * Function for multi-threaded execution.
     */
    static void *
    run(void *arg);

    /**
     * Instance of MWIF.
     */
    static MWIF *mwif;
private:
    libwebsocket_context *context;
    libwebsocket_protocols protocollist[2];
    enum ServerProtocol type;
    std::map<int, libwebsocket*> socketmap;
    EventMessage eventmsg;
    DataMessage datamsg;
    pthread_t threadid;
    pthread_mutex_t mutex;
    char buf[LWS_SEND_BUFFER_PRE_PADDING + StandardMessage::BUFSIZE
            + LWS_SEND_BUFFER_POST_PADDING];
};
#endif // #ifndef CONTROLWEBSOCKET_H_
