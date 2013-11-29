#include <cstdio>
#include <iostream>
#include <sys/time.h>

#include "datamessage.h"

#include "controlwebsocketclient.h"

ControlWebsocketClient::ControlWebsocketClient() 
: context_(NULL), wsid_(NULL), fd_(0) {
}

ControlWebsocketClient::~ControlWebsocketClient() {
}

bool ControlWebsocketClient::initialize(const char *uri, const int port, const char *protocol) {
    /* Memo */
    /* If strlen(uri) == 0, websocket server. If strlen(uri) > 0, websocket client. */
    if ((uri == NULL || strlen(uri) == 0) || (port < 1 || port > 65535) || (protocol == NULL || strlen(protocol) == 0)) {
        return false;
    }
    char uribuf[256];
    memset(uribuf, 0, sizeof(uribuf));
    sprintf(uribuf, "%s:%d", uri, port);
    context_ = ico_uws_create_context(uribuf, protocol);
    if (context_ == NULL) {
        return false;
    }

    int ret = ico_uws_set_event_cb(context_, callbackfunc, this);
    if (ret != ICO_UWS_ERR_NONE) {
        terminate();
        return false;
    }
    return true;
}

void ControlWebsocketClient::terminate() {
    if (context_ != NULL) {
        ico_uws_unset_event_cb(context_);
        ico_uws_close(context_);
        fd_ = 0;
    }
}

bool ControlWebsocketClient::send(char *sendbuf, const size_t len) {
    if (context_ == NULL || wsid_ == NULL) {
        return false;
    }
    ico_uws_send(context_, wsid_, reinterpret_cast<unsigned char*>(sendbuf), len);
    return true;
}

bool ControlWebsocketClient::recv(char *recvbuf, const size_t len) {
    std::cout << &recvbuf[0] << " ";
    DataMessage dmsg;
    dmsg.decode(recvbuf, len);
    timeval tv;
    tv = dmsg.getRecordtime();
    std::cout << tv.tv_sec << "." << tv.tv_usec << " ";
    DataOpt dopt = dmsg.getDataOpt();
    std::cout << dopt.common_status << " ";
    int statuslen = getStatussize(len);
    for (int i = 0; i < statuslen; i++) {
        std::cout << static_cast<int>(dopt.status[i]);
    }
    
    std::cout << std::endl;
    return true;
}

int ControlWebsocketClient::getsocketid() const {
    return fd_;
}

void ControlWebsocketClient::setsocketid(int fd) {
    fd_ = fd;
}

void ControlWebsocketClient::setwsid(void *id) {
    wsid_ = id;
}

void ControlWebsocketClient::service() {
    ico_uws_service(context_);
}

void ControlWebsocketClient::callbackfunc(const struct ico_uws_context *context, const ico_uws_evt_e event, const void *id, const ico_uws_detail *detail, void *user_data) {
    ControlWebsocketClient *client = reinterpret_cast<ControlWebsocketClient*>(user_data);
    switch (event) {
    case ICO_UWS_EVT_RECEIVE :
        //std::cout << "ICO_UWS_EVT_RECEIV\n";
        client->recv(reinterpret_cast<char*>(detail->_ico_uws_message.recv_data), detail->_ico_uws_message.recv_len);
        break;
    case ICO_UWS_EVT_ADD_FD :
        //std::cout << "ICO_UWS_EVT_ADD_FD\n";
        if (detail->_ico_uws_fd.fd > 0) {
            client->setsocketid(detail->_ico_uws_fd.fd);
        }
        client->setwsid(const_cast<void*>(id));
        break;
    case ICO_UWS_EVT_DEL_FD :
        //std::cout << "ICO_UWS_EVT_DEL_FD\n";
        client->setsocketid(0);
        client->setwsid(NULL);
        break;
    default :
        break;
    }
    return;
}
