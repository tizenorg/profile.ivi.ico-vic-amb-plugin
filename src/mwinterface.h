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
#ifndef MWINTERFACE_H
#define MWINTERFACE_H

#include <pthread.h>
#include <sys/time.h>

#include <map>
#include <string>
#include <vector>

#include <libwebsockets.h>

#include "ambconfig.h"
#include "controlwebsocket.h"
#include "messageformat.h"

class VICCommunicator;

/**
 * Vehicle information of MW
 */
struct MWVehicleInfo {
    std::string name;
    timeval recordtime;
    char status[STATUSSIZE];
    int statussize;
    vector<int> delimeterposition;
};

/**
 * This class manages the vehicle information to be notified to MW.
 */
class MWNotifyInfo {
public:
    /**
     * Conditions of vehicle information changes notification.
     */
    struct NotifyOpt {
        std::string name;
        int interval;
        char mask[STATUSSIZE];
        timeval lastChanged;
    };
    /**
     * Constructor.
     */
    MWNotifyInfo();
    /**
     * Destructor.
     */
    ~MWNotifyInfo();
    /**
     * This function registers the change notifiction contitions.
     *
     * @param name The name of vehicle information of MW
     * @param interval Transmission interval change notification.
     * @param sense Information to detect.
     * @param mask For change detection mask.
     * @return Success : true Failure : false
     */
    bool
    insert(std::string name, int interval, int sense, char *mask);
    /**
     * This function removes the notification conditions.
     *
     * @param name The name of vehicle information of MW.
     * @param mask For change detection mask.
     * @return Success : true Failure : false
     */
    bool
    erase(std::string name, char *mask);
    /**
     * This function removes the all notification conditions with a matching name.
     *
     * @param name The name of vehicle information of MW.
     * @return Success : true Failure : false
     */
    bool
    eraseAllMask(std::string name);
    /**
     * This function will check whether the vehicle information is applicable to the notification conditions.
     * If applicable, update the internal table.
     *
     * @param name The name of vehicle information of MW.
     * @param olddata Value before the change.
     * @param newdata Value after the change.
     * @param newtime Time after the change.
     * @return Success : true Failure : false
     */
    bool
    checkNotify(std::string name, char *olddata, char *newdata,
                timeval newtime);
private:
    enum GetType {
        NORMAL, ALLNAME, ALLCOMMID, ALLDATA
    };
    std::map<std::string, std::vector<NotifyOpt> > notifyMap;
    char nochangemask[STATUSSIZE];
    pthread_mutex_t mutex;
};

/**
 * Interface of MW and Plugin.
 */
class MWIF {
public:
    /**
     * Constructor.
     */
    MWIF();
    /**
     * Destructor.
     */
    ~MWIF();
    /**
     * Initialization.
     *
     * @param comm Instance of VICCommunicator.
     * @param conf Instance of Config.
     * @return Success : true Failure : false
     */
    bool
    initialize(VICCommunicator *com, AMBConfig *conf);
    /**
     * This function issues a request to send vehicle information to the MW.
     *
     * @param vehicleinfo Vehicle information of MW.
     */
    void
    send(MWVehicleInfo *vehicleinfo);
    /**
     * This function distributes the incoming mesage from the MW.
     *
     * @param commid socket id.
     * @param keyeventtype The name of vehicle information of MW.
     * @param recordtime Time of the message.
     * @param data Binary data other than intersection.
     * @param len Length of the message.
     */
    void
    recvRawdata(int commid, char *keyeventtype, timeval recordtime, void *data,
                size_t len);
    /*
     * This function is a function of the received vehicle information message.
     *
     * @param type Type of the received message.
     * @param commid socket id.
     * @param keyeventtype The name of vehicle information of MW.
     * @param recordtime Time of the message.
     * @param data Binary data other than intersection.
     * @param len Length of the message.
     */
    void
    recvMessage(MessageType type, int commid, char *keyeventtype,
                timeval recordtime, void *data, size_t len);
    /**
     * Mapping the instance and destination socket ID.
     *
     * @param commid Socket ID.
     */

    void
    registDestination(ControlWebsocket::ServerProtocol type, int commid);
    /**
     * Unmapped instances and destination socket ID.
     * 
     * @param commid Socket ID.
     */
    void
    unregistDestination(ControlWebsocket::ServerProtocol type, int commid);
private:

    void
    sendMessage(int commid, CommonStatus status, MWVehicleInfo *vehicleinfo);
    void
    createThread(PortInfo *portinfo);
    MWVehicleInfo *
    find(std::string name);
    void
    procSetMessage(int commid, char *keyeventtype, timeval recordtime,
                   DataOpt *data, size_t len);
    void
    procGetMessage(int commid, char *keyeventtype, timeval recordtime,
                   EventOpt *data, size_t len);
    void
    procCallbackMessage(int commid, char *keyeventtype, timeval recordtime,
                        EventOpt *data, size_t len);

    static const int SERVERNUM = 4;
    std::vector<MWVehicleInfo> vehicleinfoArray;
    ControlWebsocket *websocketserver[SERVERNUM];
    VICCommunicator *communicator;
    std::map<int, ControlWebsocket::ServerProtocol> websocketservermap;
    std::map<int, MWNotifyInfo> mwnotifyinfomap;
};
#endif // MWINTERFACE_H
