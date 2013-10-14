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
#ifndef AMBINTERFACE_H
#define AMBINTERFACE_H

#include <pthread.h>

#include <map>
#include <string>
#include <vector>

#include "abstractsource.h"
#include "ambconfig.h"

/**
 * Vehicle information of AMB.
 */
struct AMBVehicleInfo {
    std::string name;
    AbstractPropertyType *value;
    bool isCustom;
};

class AMBConfig;
class VICCommunicator;

/**
 * Interface of AMB-Core and Plugin.
 */
class AMBIF : public AbstractSource {
public:
    /**
     * Constructor.
     * 
     * @param engine Instance of AMB-Core.
     * @param config Information in a configuration file of AMB.
     */
    AMBIF(AbstractRoutingEngine *engine,
          std::map<std::string, std::string> config);
    /**
     * Destructor.
     */
    ~AMBIF();

    /**
     * AMBIF class receives a request for acquisition of information from AMB-Core.
     * Vehicle Information stored in the hold, plugin will return the data.
     * 
     * @param reply Variable for storing the vehiclle information to be returnd to AMB-Core.
     */
    void
    getPropertyAsync(AsyncPropertyReply *reply);
    /**
     * AMBIF class receives a request for a change in the vehicle information from AMB-Core.
     * Changes to vehicle information received from its own information.
     *
     * @param request Vehicle information that has been requested change.
     * @return Vehicle information after the change.
     */
    AsyncPropertyReply *
    setProperty(AsyncSetPropertyRequest request);
    /**
     * AMBIF class receives a request for acquisition of vehicle information list from AMB-Core.
     *
     * @return Vehicle information list held by the plugin.
     */
    PropertyList
    supported();
    /**
     * AMBIF class receives from AMB-Core a request for obtaining the operation list.
     * AMBIF class supports the operations(Get/Set)
     *
     * @return Operation
     */
    int
    supportedOperations();

    /**
     * AMBIF class will issue a UUID.
     *
     * @return UUID
     */
    const string
    uuid();
    /**
     * AMBIF class is received the vehicle information that other plugin updated.
     *
     * @param property the name of Vehicle information of AMB.
     * @param value the value of Vehicle information of AMB.
     * @param uuid UUID of the source plugin.
     */
    void
    propertyChanged(VehicleProperty::Property property,
                    AbstractPropertyType *value, std::string uuid);
    /**
     * AMBIF class reads the configuration file AMB.
     *
     * @param config Information in a configuration file of AMB.
     */
    void
    setConfiguratin(std::map<std::string, std::string> config);

    /**
     * Initialization.
     *
     * @param comm Instance of VICCommunicator.
     * @param conf Instance of Config.
     * @return Success : true Failure : false
     */
    bool
    initialize(VICCommunicator *comm, AMBConfig *conf);
    /**
     * AMBIF class is required to get the vehicle information to AMB-Core.
     *
     * @param propertyname The name of vehicle information of AMB.
     * @return The value of vehicle information of AMB.
     */
    AMBVehicleInfo *
    getPropertyRequest(std::string propertyname);
    /**
     * AMBIF class ask AMB-Core to change the vehicle information.
     * This request is used for vehicle information that is managed by other plugin.
     *
     * @param vehicleinfo Vehicle information of AMB.(name and value)
     */
    void
    setPropertyRequest(AMBVehicleInfo *vehicleinfo);
    /**
     * AMBIF class notify a change in the vehicle information to the AMB-Core.
     * This instruction is subject to only the vehicle information that is managed by the plugin.
     *
     * @param vehicleinfo Vehicle information of AMB.(name and value)
     */
    void
    updateProperty(AMBVehicleInfo *vehicleinfo);
    /**
     * AMBIF class locks the vehicle information changes.
     */
    void
    lock();
    /**
     * AMBIF class releases the lock status of the vehicle information changes.
     */
    void
    unLock();
    /**
     * This function is an interface with other objects in the Plugin.
     * This interface issues a request to change the information.
     * 
     * @param vehicleinfo Vehicle information of AMB.
     */
    void
    requestUpdate(AMBVehicleInfo *vehicleinfo);

    /**
     * Unused virtual function.
     */
    void
    getRangePropertyAsync(AsyncRangePropertyReply *reply)
    {
    }
    /**
     * Unused virtual function.
     */
    void
    subscribeToPropertyChanges(VehicleProperty::Property property)
    {
    }
    /**
     * Unused virtual function.
     */
    void
    unsubscribeToPropertyChanges(VehicleProperty::Property property)
    {
    }
    /**
     * Unused virtual function.
     */
    void
    supportedChanged(PropertyList)
    {
    }
private:
    bool
    registVehicleInfo(std::string propertyName, DataType type,
                      std::string value);
    AMBVehicleInfo *
    find(std::string propertyName);

    PropertyList propertylist;
    vector<AMBVehicleInfo> vehicleinfoArray;
    VICCommunicator *communicator;
    pthread_mutex_t mutex;
};
#endif // #ifndef AMBINTERFACE_H
