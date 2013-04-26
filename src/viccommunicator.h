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
#ifndef VICCOMMUNICATOR_H
#define VICCOMMUNICATOR_H

#include "ambinterface.h"

class Converter;
class MWIF;

/**
 * This class is an interface AMBIF class and MWIF class.
 *
 */
class VICCommunicator {
public:
    /**
     * Constructor.
     */
    VICCommunicator();
    /**
     * Destructor.
     */
    ~VICCommunicator();
    /**
     * Initialization.
     * 
     * @param ambif Instance of AMBIF
     * @param mwif Instance of MWIF
     * @param conf Instance of Converter
     * @return Success : true Failure : false
     */
    bool
    initialize(AMBIF *ambif, MWIF *mwif, Converter *conv);
    /**
     * This function changes the vehicle information of AMBIF.
     *
     * @param vehicleinfo Vehicle information of MW.
     */
    void
    setAMBVehicleInfo(MWVehicleInfo *vehicleinfo);
    /**
     * This function retrieves vehicle information about the AMBIF.
     *
     * @param vehicleinfo Vehicle information of MW.
     */
    void
    getAMBVehicleInfo(MWVehicleInfo *vehicleinfo);
    /**
     * This function changes the vehicle information of MWIF.
     *
     * @param vehicleinfo Vehicle information of AMB.
     */
    void
    setMWVehicleInfo(AMBVehicleInfo *vehicleinfo);
    /**
     * Unused class
     */
    void
    getMWVehicleInfo(AMBVehicleInfo *vehicleinfo);
    /**
     * Buffer size of vehicle information of AMB.
     */
    static const int maxambdatasize = 16;
private:
    void
    resetAmbBuf(AMBVehicleInfo *vehicleinfo, size_t len);
    void
    resetMwBuf(MWVehicleInfo *vehicleinfo);

    Converter *converter;
    AMBIF *ambif;
    MWIF *mwif;
    AMBVehicleInfo ambvehicleinfo[maxambdatasize];
    MWVehicleInfo mwvehicleinfo;
};
#endif // #ifndef VICCOMMUNICATOR_H
