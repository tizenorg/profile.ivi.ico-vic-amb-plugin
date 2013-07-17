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
#include <string.h>

#include <iostream>

#include "debugout.h"

#include "mwinterface.h"
#include "controlwebsocket.h"
#include "viccommunicator.h"

using ::std::string;

MWNotifyInfo::MWNotifyInfo()
{
    memset(nochangemask, 0, sizeof(nochangemask));
    mutex = PTHREAD_MUTEX_INITIALIZER;
}

MWNotifyInfo::~MWNotifyInfo()
{
    for (auto itr = notifyMap.begin(); itr != notifyMap.end(); itr++) {
        (*itr).second.clear();
    }
    notifyMap.clear();
}

bool
MWNotifyInfo::insert(std::string name, int interval, int sense, char *mask)
{
    if (sense == 0xff) {
        eraseAllMask(name);
    }
    pthread_mutex_lock(&mutex);
    if (notifyMap.find(name) == notifyMap.end()) {
        for (auto itr = notifyMap[name].begin(); itr != notifyMap[name].end();
             itr++) {
            if (memcpy((*itr).mask, mask, STATUSSIZE) == 0) {
                (*itr).interval = interval;
                pthread_mutex_unlock(&mutex);
                return true;
            }
        }
    }
    pthread_mutex_unlock(&mutex);
    NotifyOpt opt;
    opt.interval = interval;
    memcpy(opt.mask, mask, STATUSSIZE);
    opt.lastChanged.tv_sec = 0;
    opt.lastChanged.tv_usec = 0;
    pthread_mutex_lock(&mutex);
    notifyMap[name].push_back(opt);
    pthread_mutex_unlock(&mutex);
    return true;
}

bool
MWNotifyInfo::erase(std::string name, char *mask)
{
    pthread_mutex_lock(&mutex);
    if (notifyMap.find(name) == notifyMap.end()) {
        pthread_mutex_unlock(&mutex);
        return false;
    }
    for (auto itr = notifyMap[name].begin(); itr != notifyMap[name].end();
         itr++) {
        if (memcpy((*itr).mask, mask, STATUSSIZE) == 0) {
            itr = notifyMap[name].erase(itr);
            pthread_mutex_unlock(&mutex);
            return true;
        }
    }
    pthread_mutex_unlock(&mutex);
    return false;
}

bool
MWNotifyInfo::eraseAllMask(std::string name)
{
    pthread_mutex_lock(&mutex);
    if (notifyMap.find(name) == notifyMap.end()) {
        pthread_mutex_unlock(&mutex);
        return false;
    }
    notifyMap[name].clear();
    notifyMap.erase(name);
    pthread_mutex_unlock(&mutex);
    return true;
}

bool
MWNotifyInfo::checkNotify(std::string name, char *olddata, char *newdata,
                          timeval newtime)
{
    DebugOut() << "MWNotiryInfo checkNotify(" << name << ")" << std::endl;
    pthread_mutex_lock(&mutex);
    if (notifyMap.find(name) == notifyMap.end()) {
        pthread_mutex_unlock(&mutex);
        return false;
    }
    pthread_mutex_unlock(&mutex);
    DebugOut(10) << "MWNotiryInfo checkStatus(" << name << ")" << std::endl;
    char checkDiff[STATUSSIZE] = {};
    for (int i = 0; i < STATUSSIZE; i++) {
        checkDiff[i] = (*(olddata + i)) ^ (*(newdata + i));
    }
    DebugOut(10) << "MWNotiryInfo Update checkDiff" << std::endl;
    pthread_mutex_lock(&mutex);
    for (auto itr = notifyMap[name].begin(); itr != notifyMap[name].end();
         itr++) {
        for (int i = 0; i < STATUSSIZE; i++) {
            DebugOut() << "checkDiff[" << i << "] = " << checkDiff[i]
                       << ", (*itr).mask[i] = " << (*itr).mask[i]
                       << " , checkDiff[i] & (*itr).mask[i] = "
                       << (int)(checkDiff[i] & (*itr).mask[i]) << std::endl;
            if ((checkDiff[i] & (*itr).mask[i]) != 0x00) {
                pthread_mutex_unlock(&mutex);
                return true;
                /* ToDo */
                /* Check Interval */
            }
        }
    }
    pthread_mutex_unlock(&mutex);
    return false;
}

MWIF::MWIF()
{
    for (int i = 0; i < SERVERNUM; i++) {
        websocketserver[i] = NULL;
    }
}

MWIF::~MWIF()
{
    vehicleinfoArray.clear();
    for (int i = 0; i < SERVERNUM; i++) {
        if (websocketserver[i] != NULL) {
            delete websocketserver[i];
        }
    }
    websocketservermap.clear();
    mwnotifyinfomap.clear();
}

bool
MWIF::initialize(VICCommunicator *com, AMBConfig *conf)
{
    communicator = com;

    vector<VehicleInfoDefine> table;
    table = conf->getVehicleInfoConfig();
    for (auto itr = table.begin(); itr != table.end(); itr++) {
        MWVehicleInfo vi;
        vi.name = string((*itr).KeyEventType);
        memset(&vi.status, 0, STATUSSIZE);
        vi.statussize = 0;
        for (auto itr2 = (*itr).status.begin(); itr2 != (*itr).status.end();
             itr2++) {
            vi.statussize += (*itr2).typesize;
            vi.delimeterposition.push_back((*itr2).typesize);
        }
        vehicleinfoArray.push_back(vi);
        DebugOut() << "MWIF initialize mwvehicleinfo name = " << vi.name
                   << std::endl;
    }
    PortInfo portinfo = conf->getPort();
    DebugOut() << "MWIF initialize portinfo (" << portinfo.standard.dataPort
               << "," << portinfo.standard.controlPort << ","
               << portinfo.custom.dataPort << "," << portinfo.custom.controlPort
               << ")\n";
    createThread(&portinfo);
    return true;
}

void
MWIF::send(MWVehicleInfo *vehicleinfo)
{
    DebugOut(10) << "MWIF " << "send data is " << vehicleinfo->name << "("
                 << vehicleinfo->status[0] << ")" << std::endl;
    if (vehicleinfo == NULL) {
        return;
    }

    MWVehicleInfo *curvehicleinfo = find(vehicleinfo->name);
    if (curvehicleinfo != NULL) {
        vehicleinfo->statussize = curvehicleinfo->statussize;
        vehicleinfo->delimeterposition = curvehicleinfo->delimeterposition;
        DebugOut(10) << "MWIF send : mwnotifyinfomap.size() = "
                << mwnotifyinfomap.size() << "\n";
        for (auto itr = mwnotifyinfomap.begin(); itr != mwnotifyinfomap.end();
                itr++) {
            if ((*itr).second.checkNotify(vehicleinfo->name,
                                          curvehicleinfo->status,
                                          vehicleinfo->status,
                                          vehicleinfo->recordtime)) {
                DebugOut(10) << "MWIF send Notify" << std::endl;
                sendMessage((*itr).first, SUPPORT, vehicleinfo);
            }
        }
        memcpy(curvehicleinfo->status, vehicleinfo->status, STATUSSIZE);
        curvehicleinfo->recordtime = vehicleinfo->recordtime;
    }
}

void
MWIF::recvRawdata(int commid, char *keyeventtype, timeval recordtime,
                  void *data, size_t len)
{
    DebugOut() << "MWIF recvRawdata(" << commid << "," << keyeventtype << ")\n";
    if (find(string(keyeventtype)) != NULL) {
        if (websocketservermap.find(commid) == websocketservermap.end()) {
            return;
        }
        websocketserver[static_cast<int>(websocketservermap[commid])]->receive(
                commid, keyeventtype, recordtime, data, len);
    }
    // Error Message
    else {
        DebugOut() << "MWIF recvRawdata " << "Error Data." << std::endl;
        MWVehicleInfo errorvi;
        errorvi.name = string(keyeventtype);
        errorvi.recordtime = recordtime;
        errorvi.statussize = len - StandardMessage::KEYEVENTTYPESIZE
                - sizeof(timeval) - sizeof(int);
        memset(errorvi.status, 0, STATUSSIZE);
        sendMessage(commid, UNKNOWN, &errorvi);
    }
}

void
MWIF::recvMessage(MessageType type, int commid, char *keyeventtype,
                  timeval recordtime, void *data, size_t len)
{
    DebugOut(10) << "MWIF recvMessage(" << commid << ")\n";
    switch (type) {
    case MessageType::SET:
    {
        DebugOut() << "MWIF recvMessage(" << commid << ",SET) " << keyeventtype
                   << "\n";
        DataOpt *opt = reinterpret_cast<DataOpt*>(data);
        procSetMessage(commid, keyeventtype, recordtime, opt, len);
        break;
    }
    case MessageType::GET:
    {
        DebugOut() << "MWIF recvMessage(" << commid << ",GET) " << keyeventtype
                   << "\n";
        EventOpt *opt = reinterpret_cast<EventOpt*>(data);
        procGetMessage(commid, keyeventtype, recordtime, opt, len);
        break;
    }
    case MessageType::CALLBACK:
    {
        DebugOut() << "MWIF recvMessage(" << commid << ",CALLBACK) "
                   << keyeventtype << "\n";
        EventOpt *opt = reinterpret_cast<EventOpt*>(data);
        procCallbackMessage(commid, keyeventtype, recordtime, opt, len);
        break;
    }
    default:
    {
        break;
    }
    }
}

void
MWIF::registDestination(ControlWebsocket::ServerProtocol type, int commid)
{
    DebugOut() << "MWIF type = " << type << std::endl;
    if (websocketserver[type]->registSocket(commid)) {
        DebugOut() << "MWIF registDestination insert(" << commid << "," << type
                   << ")\n";
        websocketservermap.insert(make_pair(commid, type));
    }
}

void
MWIF::unregistDestination(ControlWebsocket::ServerProtocol type, int commid)
{
    if (websocketservermap.find(commid) != websocketservermap.end()) {
        if (websocketserver[type]->unregistSocket(commid)) {
            DebugOut() << "MWIF unregistDestination erase(" << commid << ","
                       << type << ")\n";
            websocketservermap.erase(commid);
        }
    }
}

void
MWIF::sendMessage(int commid, CommonStatus status, MWVehicleInfo *vehicleinfo)
{
    DataOpt opt;
    opt.common_status = status;
    memcpy(opt.status, vehicleinfo->status, STATUSSIZE);
    size_t len = StandardMessage::KEYEVENTTYPESIZE + sizeof(timeval)
            + sizeof(int) + vehicleinfo->statussize;
    DebugOut(10) << "MWIF sendMessage vehicleinfo->statussize = "
                 << vehicleinfo->statussize << ", len = " << len << std::endl;
    if (websocketservermap.find(commid) == websocketservermap.end()) {
        return;
    }
    DebugOut() << "MWIF sendMessage controlwebsocket->send(" << commid << ","
               << vehicleinfo->name << "),len = " << len << std::endl;
    websocketserver[websocketservermap[commid]]->send(
            commid, const_cast<char*>(vehicleinfo->name.c_str()),
            vehicleinfo->recordtime, (void *)&opt, len);
}

void
MWIF::createThread(PortInfo *portinfo)
{
    for (int i = 0; i < SERVERNUM; i++) {
        websocketserver[i] = new ControlWebsocket();
    }
    websocketserver[ControlWebsocket::DATA_STANDARD]->initialize(
            portinfo->standard.dataPort, ControlWebsocket::DATA_STANDARD, this);
    websocketserver[ControlWebsocket::CONTROL_STANDARD]->initialize(
            portinfo->standard.controlPort, ControlWebsocket::CONTROL_STANDARD,
            this);
    websocketserver[ControlWebsocket::DATA_CUSTOM]->initialize(
            portinfo->custom.dataPort, ControlWebsocket::DATA_CUSTOM, this);
    websocketserver[ControlWebsocket::CONTROL_CUSTOM]->initialize(
            portinfo->custom.controlPort, ControlWebsocket::CONTROL_CUSTOM,
            this);
}

MWVehicleInfo *
MWIF::find(string name)
{
    for (auto itr = vehicleinfoArray.begin(); itr != vehicleinfoArray.end();
            itr++) {
        DebugOut(10) << "MWIF find" << (*itr).name << std::endl;
        if ((*itr).name == name) {
            return &(*itr);
        }
    }
    DebugOut() << "MWIF find can't find property = " << name << std::endl;
    return NULL;
}

void
MWIF::procSetMessage(int commid, char *keyeventtype, timeval recordtime,
                     DataOpt *data, size_t len)
{
    MWVehicleInfo *vehicleinfo = find(string(keyeventtype));
    MWVehicleInfo updatevehicleinfo;
    updatevehicleinfo.name = vehicleinfo->name;
    updatevehicleinfo.recordtime = recordtime;
    updatevehicleinfo.statussize = vehicleinfo->statussize;
    memcpy(updatevehicleinfo.status, data->status, STATUSSIZE);
    // Update AMB Data
    communicator->setAMBVehicleInfo(&updatevehicleinfo);
    // Update MW Data
    vehicleinfo->recordtime = recordtime;
    memcpy(vehicleinfo->status, updatevehicleinfo.status, STATUSSIZE);
}

void
MWIF::procGetMessage(int commid, char *keyeventtype, timeval recordtime,
                     EventOpt *data, size_t len)
{
    MWVehicleInfo vehicleinfo;
    vehicleinfo.name = string(keyeventtype);
    vehicleinfo.statussize = find(vehicleinfo.name)->statussize;
    communicator->getAMBVehicleInfo(&vehicleinfo);
    sendMessage(commid, SUPPORT, &vehicleinfo);
}

void
MWIF::procCallbackMessage(int commid, char *keyeventtype, timeval recordtime,
                          EventOpt *data, size_t len)
{
    MWVehicleInfo *vehicleinfo = find(string(keyeventtype));
    //Set Mask bit.
    int index = 0;
    char mask[STATUSSIZE];
    memset(mask, 0, STATUSSIZE);
    for (int i = 0; i < static_cast<int>(vehicleinfo->delimeterposition.size());
         i++) {
        char flg = 0x00;
        if (data->sense == 0xff || (i + 1) == data->sense) {
            flg = 0xff;
        }
        for (int j = 0; j < vehicleinfo->delimeterposition[i]; j++) {
            mask[index++] = flg;
        }
    }

    MWNotifyInfo notifyinfo;
    if (mwnotifyinfomap.find(commid) == mwnotifyinfomap.end()) {
        mwnotifyinfomap.insert(make_pair(commid, notifyinfo));
    }
    else {
        notifyinfo = mwnotifyinfomap[commid];
    }
    if (notifyinfo.insert(string(keyeventtype), data->common, data->sense,
                          mask)) {
        sendMessage(commid, SUPPORT, vehicleinfo);
    }
    else {
        sendMessage(commid, NOTSUPPORT, vehicleinfo);
    }
    mwnotifyinfomap[commid] = notifyinfo;
}
