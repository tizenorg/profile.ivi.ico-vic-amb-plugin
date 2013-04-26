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
#include <pthread.h>
#include <string.h>

#include "debugout.h"

#include "controlwebsocket.h"
#include "mwinterface.h"
#include "standardmessage.h"

/**
 * Socket ID generated class.
 */
template<class T>
    class GenerateCommID {
    public:
        /**
         * This function retrieves an instance of the class.
         *
         * @return Instance of GenerateCommID
         */
        static GenerateCommID *
        getInstance()
        {
            static GenerateCommID<T> instance;
            return &instance;
        }

        /**
         * This function returns the ID that is registered.
         * If the ID is not registerd, it register the ID.
         *
         * @param value Value of socket.
         * @return ID
         */
        int
        getID(T value)
        {
            pthread_mutex_lock(&mutex);
            for (auto itr = commidmap.begin(); itr != commidmap.end(); itr++) {
                if ((*itr).second == value) {
                    pthread_mutex_unlock(&mutex);
                    return (*itr).first;
                }
            }
            int newid = generateID();
            commidmap.insert(make_pair(newid, value));
            pthread_mutex_unlock(&mutex);
            return newid;
        }
        /**
         * This function returns the socket information registered.
         *
         * @param id Socket ID
         * @return Value of socket
         */
        T
        getValue(int id)
        {
            pthread_mutex_lock(&mutex);
            if (commidmap.find(id) == commidmap.end()) {
                pthread_mutex_unlock(&mutex);
                return NULL;
            }
            pthread_mutex_unlock(&mutex);
            return commidmap[id];
        }

        /**
         * This function removes the socket information registered.
         *
         * @param id Socket ID
         * @return Success : true Failure : false
         */
        bool
        unsetID(int id)
        {
            pthread_mutex_lock(&mutex);
            bool ret = false;
            auto itr = commidmap.find(id);
            if (itr != commidmap.end()) {
                itr = commidmap.erase(itr);
                ret = true;
            }
            pthread_mutex_unlock(&mutex);
            return ret;
        }
    private:
        GenerateCommID() : mutex(PTHREAD_MUTEX_INITIALIZER)
        {
        }

        ~GenerateCommID()
        {
            commidmap.clear();
        }

        GenerateCommID(const GenerateCommID &)
        {
        }

        const GenerateCommID&
        operator =(const GenerateCommID &)
        {
        }

        int
        generateID()
        {
            if (commidmap.empty()) {
                return 1;
            }
            int count = 1;
            for (auto itr = commidmap.begin(); itr != commidmap.end(); itr++) {
                if (itr->first != count) {
                    return count;
                }
                count++;
            }
            return count + 1;
        }

        std::map<int, T> commidmap;
        pthread_mutex_t mutex;
    };

MWIF *ControlWebsocket::mwif = NULL;

ControlWebsocket::ControlWebsocket()
{
    mutex = PTHREAD_MUTEX_INITIALIZER;
    protocollist[1] = {NULL, NULL, 0};
}

ControlWebsocket::~ControlWebsocket()
{
    socketmap.clear();
}

bool
ControlWebsocket::initialize(int port, enum ServerProtocol stype)
{
    DebugOut() << "ControlWebsocket[" << type << "]" << " initialize.(" << port
               << ")\n";
    type = stype;
    switch (type) {
    case DATA_STANDARD:
    {
        protocollist[0] = {"standarddatamessage-only", ControlWebsocket::callback_receive, 0};
        break;
    }
    case CONTROL_STANDARD : {
        protocollist[0] = {"standardcontrolmessage-only", ControlWebsocket::callback_receive, 0};
        break;
    }
    case DATA_CUSTOM : {
        protocollist[0] = {"customdatamessage-only", ControlWebsocket::callback_receive, 0};
        break;
    }
    case CONTROL_CUSTOM : {
        protocollist[0] = {"customcontrolmessage-only", ControlWebsocket::callback_receive, 0};
        break;
    }
    default : {
        return false;
    }
}
    context = libwebsocket_create_context(port, "lo", protocollist,
                                          libwebsocket_internal_extensions,
                                          NULL, NULL, -1, -1, 0);
    if (context == NULL) {
        DebugOut() << "ControlWebsocket[" << type << "]" 
                   << " couldn't create libwebsockets_context." << std::endl;
        return false;
    }
    if (pthread_create(&threadid, NULL, ControlWebsocket::run, (void*)this)
        == -1) {
        libwebsocket_context_destroy(context);
        DebugOut() << "ControlWebsocket[" << type << "]" 
                   << " couldn't create thread." << std::endl;
        return false;
    }
    return true;
}

bool
ControlWebsocket::send(int commid, char *keyeventtype, timeval time, void *data,
                       size_t len)
{
    DebugOut() << "ControlWebsocket[" << type << "]" << " send data(" << commid
               << "," << keyeventtype << ") len = " << len << std::endl;
    libwebsocket *wsi = NULL;
    if (socketmap.find(commid) == socketmap.end()) {
        if (!registSocket(commid)) {
            DebugOut() << "ControlWebsocket[" << type << "]" 
                       << " can't regist socket(" << commid << ")" 
                       << std::endl;
            return false;
        }
    }
    wsi = socketmap[commid];

    if (wsi == NULL) {
        DebugOut() << "ControlWebsocket Get websocket object is NULL."
                   << std::endl;
        return false;
    }
    pthread_mutex_lock(&mutex);
    memset(buf, 0, sizeof(buf));
    memcpy(buf + LWS_SEND_BUFFER_PRE_PADDING,
           datamsg.encode(keyeventtype, time,
                          *(reinterpret_cast<DataOpt*>(data))),
           len);
    DebugOut() << "ControlWebsocket Send Data[" << keyeventtype << "]"
               << std::endl;
    libwebsocket_write(
            wsi,
            reinterpret_cast<unsigned char*>(buf + LWS_SEND_BUFFER_PRE_PADDING),
            len, LWS_WRITE_BINARY);
    pthread_mutex_unlock(&mutex);
    return true;
}

bool
ControlWebsocket::receive(int commid, char *keyeventtype, timeval recordtime,
                          void *data, size_t len)
{
    DebugOut() << "ControlWebsocket[" << type << "]" << " receive message("
               << commid << "," << keyeventtype << ")\n";
    switch (type) {
    case DATA_STANDARD:
    case DATA_CUSTOM:
    {
        datamsg.decodeOpt(keyeventtype, recordtime, data);
        DataOpt dop = datamsg.getDataOpt();
        mwif->recvMessage(SET, commid, keyeventtype, recordtime, (void*)&dop,
                          len);
        break;
    }
    case CONTROL_STANDARD:
    case CONTROL_CUSTOM:
    {
        eventmsg.decodeOpt(keyeventtype, recordtime, data);
        EventOpt opt = eventmsg.getEventOpt();
        if (opt.common == -1 && opt.sense == -1) {
            mwif->recvMessage(GET, commid, keyeventtype, recordtime,
                              (void*)&opt, len);
        }
        else {
            mwif->recvMessage(CALLBACK, commid, keyeventtype, recordtime,
                              (void*)&opt, len);
        }
        break;
    }
    default:
    {
        return false;
    }
    }
    return true;
}

void
ControlWebsocket::observation()
{
    int ret = 0;
    while (ret >= 0) {
        ret = libwebsocket_service(context, 100);
        if (ret != 0) {
            break;
        }
    }
}

bool
ControlWebsocket::registSocket(int commid)
{
    if (socketmap.find(commid) != socketmap.end()) {
        return true;
    }
    GenerateCommID<libwebsocket*> *idserver =
            GenerateCommID<libwebsocket*>::getInstance();
    libwebsocket *wsi = idserver->getValue(commid);
    if (wsi == NULL) {
        return false;
    }
    libwebsocket_protocols *protocol =
            const_cast<libwebsocket_protocols*>(libwebsockets_get_protocol(wsi));
    if (protocol != NULL) {
        if (context == protocol[0].owning_server) {
            socketmap[commid] = wsi;
            return true;
        }
    }
    return false;
}

bool
ControlWebsocket::unregistSocket(int commid)
{
    if (socketmap.find(commid) == socketmap.end()) {
        return false;
    }
    socketmap.erase(commid);
    GenerateCommID<libwebsocket*> *idserver =
            GenerateCommID<libwebsocket*>::getInstance();
    return idserver->unsetID(commid);
}

int
ControlWebsocket::callback_receive(libwebsocket_context *context,
                                   libwebsocket *wsi,
                                   libwebsocket_callback_reasons reason,
                                   void *user, void *in, size_t len)
{
    DebugOut(10) << "Reason(" << reason << ")\n";
    switch (reason) {
    case LWS_CALLBACK_ESTABLISHED:
    {
        GenerateCommID<libwebsocket*> *idserver =
                GenerateCommID<libwebsocket*>::getInstance();
        int id = idserver->getID(wsi);
        DebugOut() << "ControlWebsocket callback_receive Insert ID is " << id
                   << "\n";
        break;
    }
    case LWS_CALLBACK_CLOSED:
    {
        GenerateCommID<libwebsocket*> *idserver =
                GenerateCommID<libwebsocket*>::getInstance();
        int id = idserver->getID(wsi);
        idserver->unsetID(id);
        DebugOut() << "ControlWebsocket callback_receive Delete ID is " << id
                   << "\n";
        ControlWebsocket::mwif->unregistDestination(id);
        break;
    }
    case LWS_CALLBACK_RECEIVE:
    {
        GenerateCommID<libwebsocket*> *idserver =
                GenerateCommID<libwebsocket*>::getInstance();
        int id = idserver->getID(wsi);
        StandardMessage msg;
        char *buf = reinterpret_cast<char*>(in);
        msg.decode(buf, len);
        DebugOut() << "ControlWebsocket callback_receive Receive message : "
                   << msg.getKeyEventType() << "," << msg.getRecordtime().tv_sec
                   << "\n";
        ControlWebsocket::mwif->recvRawdata(
                id, msg.getKeyEventType(), msg.getRecordtime(),
                (buf + StandardMessage::KEYEVENTTYPESIZE + sizeof(timeval)),
                len);
        break;
    }
    default:
    {
        break;
    }
    }
    return 0;
}

void *
ControlWebsocket::run(void *arg)
{
    ControlWebsocket *control = reinterpret_cast<ControlWebsocket*>(arg);
    control->observation();
    return NULL;
}
