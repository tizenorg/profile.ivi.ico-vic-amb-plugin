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

#include "debugout.h"

#include "ambinterface.h"
#include "convert.h"
#include "messageformat.h"
#include "mwinterface.h"
#include "viccommunicator.h"

VICCommunicator::VICCommunicator()
{
}

VICCommunicator::~VICCommunicator()
{
}

bool
VICCommunicator::initialize(AMBIF *amb, MWIF *mw, Converter *conv)
{
    converter = conv;
    ambif = amb;
    mwif = mw;
    return true;
}

void
VICCommunicator::setAMBVehicleInfo(MWVehicleInfo *vehicleinfo)
{
    AMBVehicleInfo ambvehicleinfo[maxambdatasize];
    DebugOut() << "VICCOMM" << " in setAMBVehicleInfo(" << vehicleinfo->name
               << ")" << " status[0] = " << vehicleinfo->status[0] << "\n";
    resetAmbBuf(&ambvehicleinfo[0], maxambdatasize);
    int ret = converter->convertMWtoAMB(vehicleinfo, &ambvehicleinfo[0],
                                        maxambdatasize);
    for (int i = 0; i < ret; i++) {
        ambif->requestUpdate(&ambvehicleinfo[i]);
        DebugOut() << "VICCOMM" << " out setAMBVehicleInfo(AMBname = "
                   << ambvehicleinfo[i].name << ","
                   << ambvehicleinfo[i].value->toString() << ")\n";
    }
    for (int i = 0; i < maxambdatasize; i++) {
        if (ambvehicleinfo[i].value != NULL) {
            delete ambvehicleinfo[i].value;
        }
    }
    DebugOut(10) << "VICCOMM" << " out setAMBVehicleInfo(MWname = "
                 << vehicleinfo->name << ")";
}

void
VICCommunicator::getAMBVehicleInfo(MWVehicleInfo *vehicleinfo)
{
    AMBVehicleInfo *ambvehicleinfo[maxambdatasize];
    AMBVehicleInfo tempambvehicleinfo[maxambdatasize];

    DebugOut() << "VICCOMM" << " in getAMBVehicleInfo(" << vehicleinfo->name
               << "," << vehicleinfo->status[0] << ")\n";
    resetAmbBuf(&tempambvehicleinfo[0], maxambdatasize);
    int ret = converter->convertMWtoAMB(vehicleinfo, &tempambvehicleinfo[0],
                                        maxambdatasize);
    for (int i = 0; i < ret; i++) {
        delete tempambvehicleinfo[i].value;
        ambvehicleinfo[i] = ambif->getPropertyRequest(
                tempambvehicleinfo[i].name);
        DebugOut() << "VICCOMM" << " getAMBVehicleInfo(AMBname = "
                   << ambvehicleinfo[i]->name << ","
                   << ambvehicleinfo[i]->value->toString() << ")\n";
        converter->convertAMBtoMW(ambvehicleinfo[i], vehicleinfo);
    }
    DebugOut() << "VICCOMM" << " out getAMBVehicleInfo(MWname = "
               << vehicleinfo->name << "," << vehicleinfo->status[0] << ")\n";
}

void
VICCommunicator::setMWVehicleInfo(AMBVehicleInfo *vehicleinfo)
{
    MWVehicleInfo mwvehicleinfo;
    DebugOut() << "VICCOMM" << " in setMWVehicleInfo(AMBname = "
               << vehicleinfo->name << "," << vehicleinfo->value->toString()
               << ")\n";
    resetMwBuf(&mwvehicleinfo);
    converter->convertAMBtoMW(vehicleinfo, &mwvehicleinfo);
    mwif->send(&mwvehicleinfo);
    DebugOut() << "VICCOMM" << " out setMWVehicleInfo(MWname = "
               << mwvehicleinfo.name << "," << mwvehicleinfo.status[0] << ")\n";
}

void
VICCommunicator::getMWVehicleInfo(AMBVehicleInfo *vehicleinfo)
{
    /* don't use */
}

void
VICCommunicator::resetAmbBuf(AMBVehicleInfo *vehicleinfo, size_t len)
{
    for (int i = 0; i < static_cast<int>(len); i++) {
        vehicleinfo[i].name = "";
        vehicleinfo[i].value = NULL;
        vehicleinfo[i].isCustom = false;
    }
}

void
VICCommunicator::resetMwBuf(MWVehicleInfo *vehicleinfo)
{
    vehicleinfo->name = "";
    vehicleinfo->recordtime.tv_sec = 0;
    vehicleinfo->recordtime.tv_usec = 0;
    memset(vehicleinfo->status, 0, STATUSSIZE);
}
