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

#include <algorithm>
#include <sstream>

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
        getID(T value, int key)
        {
            pthread_mutex_lock(&mutex);
            for (auto itr = commidmap.begin(); itr != commidmap.end(); itr++) {
                if ((void*)(*itr).second == (void*)value) {
                    pthread_mutex_unlock(&mutex);
                    return (*itr).first;
                }
            }
            int newid = key;
            if (newid == -1) {
                newid = generateID();
            }
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
        GenerateCommID() :
                mutex(PTHREAD_MUTEX_INITIALIZER)
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
            while (commidmap.find(count) == commidmap.end()) {
                count++;
            }
            return count;
        }

        std::map<int, T> commidmap;
        pthread_mutex_t mutex;
    };

ControlWebsocket::ControlWebsocket()
{
    mutex = PTHREAD_MUTEX_INITIALIZER;
}

ControlWebsocket::~ControlWebsocket()
{
    socketmap.clear();
    pollfds.clear();
}

bool
ControlWebsocket::initialize(int port, enum ServerProtocol stype, MWIF *mwif_)
{
    type = stype;
    DebugOut() << "ControlWebsocket[" << type << "]" << " initialize.(" << port
               << ")\n";
    stringstream address, protocol;
    address.str("");
    protocol.str("");
    address << ":" << port;
    switch (type) {
    case DATA_STANDARD:
    {
        protocol << "standarddatamessage-only";
        break;
    }
    case CONTROL_STANDARD:
    {
        protocol << "standardcontrolmessage-only";
        break;
    }
    case DATA_CUSTOM:
    {
        protocol << "customdatamessage-only";
        break;
    }
    case CONTROL_CUSTOM:
    {
        protocol << "customcontrolmessage-only";
        break;
    }
    default:
    {
        return false;
    }
    }

    context = ico_uws_create_context(address.str().c_str(),
                                     protocol.str().c_str());
    if (context == NULL) {
        DebugOut() << "ControlWebsocket[" << type << "]"
                   << " couldn't create libwebsockets_context." << std::endl;
        return false;
    }
    mwif = mwif_;
    container.mwif = mwif;
    container.type = type;
    if (ico_uws_set_event_cb(context, ControlWebsocket::callback_receive,
                             (void*)&container) != 0) {
        DebugOut() << "ControlWebsocket[" << type << "]"
                   << " couldn't set callback function." << std::endl;
        return false;
    }
    if (pthread_create(&threadid, NULL, ControlWebsocket::run, (void*)this)
            == -1) {
        ico_uws_close(context);
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
    void *wsi = NULL;
    if (socketmap.find(commid) == socketmap.end()) {
        if (!registSocket(commid)) {
            DebugOut() << "ControlWebsocket[" << type << "]"
                       << " can't regist socket(" << commid << ")" << std::endl;
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
    memcpy(buf,
           datamsg.encode(keyeventtype, time,
                          *(reinterpret_cast<DataOpt*>(data))),
                          len);
    DebugOut() << "ControlWebsocket Send Data[" << keyeventtype << "]"
               << std::endl;
    ico_uws_send(context, wsi, reinterpret_cast<unsigned char*>(buf), len);
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
    int ret;
    while (true) {
        ret = poll(&pollfds[0], pollfds.size(), -1);
        if (ret < 0) {
            DebugOut() << "Error: poll(" << strerror(errno) << ")\n";
            continue;
        }
        ico_uws_service(context);
    }
}

bool
ControlWebsocket::registSocket(int commid)
{
    DebugOut() << "socketmap[" << type << "]!!\n" << std::flush;
    GenerateCommID<void*> *idserver = GenerateCommID<void*>::getInstance();
    if (idserver->getValue(commid) != NULL) {
        socketmap[commid] = idserver->getValue(commid);
        pollfd fds;
        fds.fd = commid;
        fds.events = POLLIN | POLLERR;
        pollfds.push_back(fds);
        return true;
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
    for (auto itr = pollfds.begin(); itr != pollfds.end(); itr++) {
        if ((*itr).fd == commid) {
            itr = pollfds.erase(itr);
            break;
        }
    }
    GenerateCommID<void*> *idserver = GenerateCommID<void*>::getInstance();
    return idserver->unsetID(commid);
}

void
ControlWebsocket::callback_receive(const struct ico_uws_context *context,
                                   const ico_uws_evt_e event, const void *id,
                                   const ico_uws_detail *detail,
                                   void *user_data)
{
    user_datacontainer *container = static_cast<user_datacontainer*>(user_data);
    switch (event) {
    case ICO_UWS_EVT_OPEN:
        break;
    case ICO_UWS_EVT_ERROR:
        break;
    case ICO_UWS_EVT_CLOSE:
        break;
    case ICO_UWS_EVT_RECEIVE:
    {
        GenerateCommID<void*> *idserver = GenerateCommID<void*>::getInstance();
        int commid = idserver->getID(const_cast<void*>(id), -1);
        StandardMessage msg;
        char *buf = reinterpret_cast<char*>(detail->_ico_uws_message.recv_data);
        msg.decode(buf, detail->_ico_uws_message.recv_len);
        DebugOut() << "ControlWebsocket callback_receive Receive message["
                   << detail->_ico_uws_message.recv_len << "] : "
                   << msg.getKeyEventType() << "," << msg.getRecordtime().tv_sec
                   << "\n";
        container->mwif->recvRawdata(
                commid, msg.getKeyEventType(), msg.getRecordtime(),
                (buf + StandardMessage::KEYEVENTTYPESIZE + sizeof(timeval)),
                detail->_ico_uws_message.recv_len);
        break;
    }
    case ICO_UWS_EVT_ADD_FD:
    {
        GenerateCommID<void*> *idserver = GenerateCommID<void*>::getInstance();
        int commid = idserver->getID(const_cast<void*>(id),
                                     detail->_ico_uws_fd.fd);
        DebugOut() << "ControlWebsocket callback_receive Insert ID is "
                   << commid << "\n";
        container->mwif->registDestination(container->type,
                                           detail->_ico_uws_fd.fd);
        break;
    }
    case ICO_UWS_EVT_DEL_FD:
    {
        container->mwif->unregistDestination(container->type,
                                             detail->_ico_uws_fd.fd);
        break;
    }
    default:
        break;
    }
}

void *
ControlWebsocket::run(void *arg)
{
    ControlWebsocket *control = reinterpret_cast<ControlWebsocket*>(arg);
    control->observation();
    return NULL;
}
