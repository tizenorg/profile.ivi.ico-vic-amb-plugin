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

#include <memory>
#include <sstream>

#include "debugout.h"

#include "ambinterface.h"
#include "ambconfig.h"
#include "convert.h"
#include "mwinterface.h"
#include "viccommunicator.h"

extern "C" AbstractSource *
create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
    AMBIF *ambif = new AMBIF(routingengine, config);
    AMBConfig *conf = new AMBConfig();
    if (!conf->readConfig(config["configfile"])) {
        DebugOut(DebugOut::Error) << "Failed to Load Configfile for VIC-Plugin.\n";
        delete ambif;
        delete conf;
        return NULL;
    }
    VICCommunicator *communicator = new VICCommunicator();
    MWIF *mwif = new MWIF();
    Converter *converter = new Converter();
    if (!ambif->initialize(communicator, conf)) {
        DebugOut(DebugOut::Error) << "Failed to initialize AMBIF\n";
        delete ambif;
        delete conf;
        delete communicator;
        delete converter;
        return NULL;
    }
    if (!communicator->initialize(ambif, mwif, converter)) {
        DebugOut(DebugOut::Error) << "Failed to initialize VICCommunicator\n";
        delete ambif;
        delete conf;
        delete communicator;
        delete converter;
        return NULL;
    }
    if (!mwif->initialize(communicator, conf)) {
        DebugOut(DebugOut::Error) << "Failed to initialize MWIF\n";
        delete ambif;
        delete conf;
        delete communicator;
        delete converter;
        return NULL;
    }
    if (!converter->initialize(conf)) {
        DebugOut(DebugOut::Error) << "Failed to initialize Converter\n";
        delete ambif;
        delete conf;
        delete communicator;
        delete converter;
        return NULL;
    }
    return ambif;
}

AMBIF::AMBIF(AbstractRoutingEngine *engine,
             std::map<std::string, std::string> config) :
        AbstractSource(engine, config)
{
}

AMBIF::~AMBIF()
{
    propertylist.clear();
    vehicleinfoArray.clear();
    pthread_mutex_destroy(&mutex);
}

void
AMBIF::getPropertyAsync(AsyncPropertyReply *reply)
{
    reply->success = false;
    DebugOut(10) << "AMBIF " << "Get Request property : " << reply->property
            << std::endl;
    lock();
    AMBVehicleInfo *vehicleinfo = find(reply->property);
    DebugOut(50) << "AMBIF " << "Find Data : " << reply->property << std::endl;
    if (vehicleinfo != NULL) {
        reply->value = vehicleinfo->value;
        reply->success = true;
        reply->completed(reply);
    }
    unLock();
}

AsyncPropertyReply *
AMBIF::setProperty(AsyncSetPropertyRequest request)
{
    DebugOut(10) << "AMBIF" << "Set Request propety : " << request.property
               << std::endl;
    lock();
    AMBVehicleInfo *vehicleinfo = find(request.property);
    if (vehicleinfo == NULL) {
        unLock();
        return NULL;
    }
    AsyncPropertyReply *reply = new AsyncPropertyReply(request);
    reply->success = true;
    DebugOut(50) << "AMBIF" << "Update Value!" << std::endl;
    delete vehicleinfo->value;
    vehicleinfo->value = request.value->copy();
    reply->value = vehicleinfo->value;
    communicator->setMWVehicleInfo(vehicleinfo);
    DebugOut(50) << "AMBIF setProperty " << "Set Value(" << request.property
               << "," << reply->value->toString() << ")" << std::endl;
    reply->completed(reply);
    #if LATER1024
    routingEngine->updateProperty(vehicleinfo->value, uuid());
    #else
    routingEngine->updateProperty(vehicleinfo->name, vehicleinfo->value, uuid());
    #endif
    unLock();
    return reply;
}

PropertyList
AMBIF::supported()
{
    return propertylist;
}

int
AMBIF::supportedOperations()
{
    return Get | Set;
}

#if LATER1024
const string
#else
string
#endif
AMBIF::uuid()
{
    return "f68f8b9a-fafb-4284-8ced-b45b5d720185";
}

void
AMBIF::propertyChanged(VehicleProperty::Property property,
                       AbstractPropertyType *value, std::string uuid)
{
    DebugOut(1) << "INFO CHG_VIC_INF Receive notification from Core. Property is " << property << ".\n";
    AMBVehicleInfo *vehicleinfo = find(property);
    lock();
    if (vehicleinfo != NULL) {
        delete vehicleinfo->value;
        vehicleinfo->value = value->copy();
    }
    unLock();
}

void
AMBIF::setConfiguratin(std::map<std::string, std::string> config)
{
}

bool
AMBIF::initialize(VICCommunicator *comm, AMBConfig *conf)
{
    DebugOut(50) << "AMBIF Initialize\n";
    communicator = comm;
    mutex = PTHREAD_MUTEX_INITIALIZER;

    vector<VehicleInfoDefine> table;
    table = conf->getVehicleInfoConfig();
    for (auto itr = table.begin(); itr != table.end(); itr++) {
        for (auto itr2 = (*itr).status.begin(); itr2 != (*itr).status.end();
             itr2++) {
            AMBVehicleInfo vi;
            vi.isCustom = false;
            vi.name = (*itr2).ambPropertyName;
            vi.value = VehicleProperty::getPropertyTypeForPropertyNameValue(
                    vi.name, (*itr2).defaultvalue);
            if (vi.value == nullptr) {
                if (!registVehicleInfo(vi.name, (*itr2).type,
                                       (*itr2).defaultvalue)) {
                    DebugOut(50) << "AMBIF Initialize Couldn't regist property["
                               << vi.name << "]\n";
                    continue;
                }
                vi.value = VehicleProperty::getPropertyTypeForPropertyNameValue(
                        vi.name, (*itr2).defaultvalue);
                vi.isCustom = true;
            }
            vi.zone = (*itr2).zone;
            vehicleinfoArray.push_back(vi);
            propertylist.push_back(vi.name);
            DebugOut(50) << "AMBIF Initialize regist propertyname = " << vi.name
                       << "\n";
        }
    }
    routingEngine->setSupported(supported(), this);
    DebugOut(1) << "INFO CHG_VIC_INF The number of AMB vehicle info is " << vehicleinfoArray.size() << ".\n";
    return true;
}

AMBVehicleInfo *
AMBIF::getPropertyRequest(std::string propertyname)
{
    DebugOut(50) << "AMBIF getPropertyRequest(" << propertyname << ")\n";
    AsyncPropertyRequest request;
    request.property = propertyname;
    request.completed = [](AsyncPropertyReply *reply) {
        if (reply->success) {
            DebugOut(50) << "AMBIF getPropertyRequest completed success!!.\n";
        }
        else {
            DebugOut(50) << "AMBIF getPropertyRequest completed false!!.\n";
        }
    };

    AsyncPropertyReply *reply = routingEngine->getPropertyAsync(request);
    lock();
    AMBVehicleInfo *vehicleinfo = find(propertyname);
    if (vehicleinfo == NULL) {
        delete reply;
        unLock();
        return NULL;
    }
    if (vehicleinfo->value != reply->value && reply->success) {
        delete vehicleinfo->value;
        vehicleinfo->value = reply->value->copy();
    }
    delete reply;
    unLock();
    DebugOut(50) << "AMBIF getPropertyRequest after call "
               << vehicleinfo->value->toString() << std::endl;
    return vehicleinfo;
}

void
AMBIF::setPropertyRequest(AMBVehicleInfo *vehicleinfo)
{
    AsyncSetPropertyRequest request;
    request.property = vehicleinfo->name;
    request.value = vehicleinfo->value;
    request.completed =
            [](AsyncPropertyReply *reply) {
                if (reply->success) {
                    DebugOut(50)<<"AMBIF" << reply->property << ":" << reply->value->toString() << std::endl;
                }
                else {
                    DebugOut(50)<<"AMBIF" << reply->property << " isn't registered." << std::endl;
                }
            };
    AsyncPropertyReply *reply = routingEngine->setProperty(request);
    if (reply != NULL) {
        delete reply;
    }
}

void
AMBIF::updateProperty(AMBVehicleInfo *vehicleinfo)
{
    if (vehicleinfo->name == VehicleProperty::VehicleSpeed) {
        static uint16_t prevspd = -1;
        static const uint16_t unusablespd = -1;
        uint16_t spd = vehicleinfo->value->value<uint16_t>();
        if ((prevspd == unusablespd && spd > 0) || (prevspd == 0 && spd > 0)) {
            DebugOut(3) << "PERF CHG_VIC_INF VIC-Plugin notify Code of update " 
                        << vehicleinfo->name 
                        << ". VehicleSpeed is 1km/h or more.\n";
        }
        else if ((prevspd == unusablespd && spd == 0) || 
                 (prevspd > 0 && spd == 0)) {
            DebugOut(3) << "PERF CHG_VIC_INF VIC-Plugin notify Code of update " 
                        << vehicleinfo->name << ". VehicleSpeed is 0km/h.\n";
        }
        prevspd = spd;
    }
    #if LATER1024
    routingEngine->updateProperty(vehicleinfo->value, uuid());
    #else
    routingEngine->updateProperty(vehicleinfo->name, vehicleinfo->value, uuid());
    #endif
    AMBVehicleInfo *ambvehicleinfo = find(vehicleinfo->name);
    lock();
    if (ambvehicleinfo != NULL) {
        delete ambvehicleinfo->value;
        ambvehicleinfo->value = vehicleinfo->value->copy();
    }
    unLock();
}

void
AMBIF::lock()
{
    pthread_mutex_lock(&mutex);
}

void
AMBIF::unLock()
{
    pthread_mutex_unlock(&mutex);
}

void
AMBIF::requestUpdate(AMBVehicleInfo *vehicleinfo)
{
    DebugOut(50) << "AMBIF requestUpdate request property name is "
               << vehicleinfo->name << "\n";
    if (find(vehicleinfo->name) != NULL) {
        vehicleinfo->value->zone = vehicleinfo->zone;
        DebugOut(50) << "AMBIF requestUpdate request property name is "
                   << vehicleinfo->name << ", zone is " << vehicleinfo->zone << "\n";
        updateProperty(vehicleinfo);
    }
    else {
        setPropertyRequest(vehicleinfo);
    }
}

bool
AMBIF::registVehicleInfo(std::string propertyName, DataType type, string value)
{
    DebugOut(50) << "AMBIF registVehicleInfo(" << propertyName << ")\n";
    VehicleProperty::PropertyTypeFactoryCallback factory;
    switch (type) {
    case INT:
    {
        factory = [value, propertyName]() {
            return new BasicPropertyType<int>(propertyName, value);
        };
        break;
    }
    case DOUBLE:
    {
        factory = [value, propertyName]() {
            return new BasicPropertyType<double>(propertyName, value);
        };
        break;
    }
    case CHAR:
    {
        factory = [value, propertyName]() {
            return new BasicPropertyType<char>(propertyName, value);
        };
        break;
    }
    case INT16:
    {
        factory = [value, propertyName]() {
            return new BasicPropertyType<int16_t>(propertyName, value);
        };
        break;
    }
    case UINT16:
    {
        factory = [value, propertyName]() {
            return new BasicPropertyType<uint16_t>(propertyName, value);
        };
        break;
    }
    case UINT32:
    {
        factory = [value, propertyName]() {
            return new BasicPropertyType<uint32_t>(propertyName, value);
        };
        break;
    }
    case INT64:
    {
        factory = [value, propertyName]() {
            return new BasicPropertyType<int64_t>(propertyName, value);
        };
        break;
    }
    case UINT64:
    {
        factory = [value, propertyName]() {
            return new BasicPropertyType<uint64_t>(propertyName, value);
        };
        break;
    }
    case BOOL:
    {
        factory = [value, propertyName]() {
            return new BasicPropertyType<bool>(propertyName, value);
        };
        break;
    }
    default:
    {
        return false;
    }
    }
    return VehicleProperty::registerProperty(propertyName, factory);
}

AMBVehicleInfo *
AMBIF::find(std::string propertyName)
{
    AMBVehicleInfo vi;
    vi.name = propertyName;
    std::vector<AMBVehicleInfo>::iterator itr;
    if ((itr = std::find(vehicleinfoArray.begin(), vehicleinfoArray.end(), vi)) 
        != vehicleinfoArray.end()) {
        return &(*itr);
    }
    return NULL;
}

#if LATER1024
PropertyInfo AMBIF::getPropertyInfo(VehicleProperty::Property property) {
    if (propertyInfoMap.find(property) != propertyInfoMap.end()) {
        return propertyInfoMap[property];
    }
    std::list<Zone::Type> zones;
    AMBVehicleInfo vi;
    vi.name = property;
    std::vector<AMBVehicleInfo>::iterator itr, itr_idx, itr_end;
    itr_idx = vehicleinfoArray.begin();
    itr_end = vehicleinfoArray.end();
    while ((itr = std::find(itr_idx, itr_end, vi)) != itr_end) {
        zones.push_back((*itr).zone);
        itr_idx = (++itr);
    }
    if (zones.empty()) {
        return PropertyInfo::invalid();
    }
    else {
        PropertyInfo info(0, zones);
        propertyInfoMap[vi.name] = info;
        return propertyInfoMap[vi.name];
    }
}
#endif
