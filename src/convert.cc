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

#include <algorithm>
#include <sstream>

#include "debugout.h"

#include "ambinterface.h"
#include "convert.h"
#include "mwinterface.h"

Converter::Converter()
{
    specialconvertlist.push_back(VehicleProperty::TransmissionShiftPosition);
    specialconvertlist.push_back(VehicleProperty::TransmissionGearPosition);
    specialconvertlist.push_back(VehicleProperty::TransmissionMode);
    specialconvertlist.push_back(VehicleProperty::TurnSignal);
    specialconvertlist.push_back(VehicleProperty::ButtonEvent);
}

Converter::~Converter()
{
    converttablelist.clear();
}

bool
Converter::initialize(AMBConfig *conf)
{
    vector<VehicleInfoDefine> dtableArray;
    dtableArray = conf->getVehicleInfoConfig();
    for (auto itr = dtableArray.begin(); itr != dtableArray.end(); itr++) {
        ConvertTable ctable;
        ctable.mwname = string((*itr).KeyEventType);
        DebugOut() << "Converter initialize mwname = " << ctable.mwname << "\n";
        for (auto itr2 = (*itr).status.begin(); itr2 != (*itr).status.end();
             itr2++) {
            ConvertTable::AmbVehicleInfoData ambdata;
            ambdata.ambname = (*itr2).ambPropertyName;
            DebugOut() << "Converter initialize ambname = " << ambdata.ambname
                       << "\n";
            ambdata.type = (*itr2).type;
            ambdata.typesize = (*itr2).typesize;
            ctable.ambdataarray.push_back(ambdata);
        }
        converttablelist.push_back(ctable);
    }
    return true;
}

int
Converter::convertMWtoAMB(MWVehicleInfo *mwvehicleinfo,
                          AMBVehicleInfo *ambvehicleinfo, int arraysize)
{
    int ret = 0;
    for (auto itr = converttablelist.begin(); itr != converttablelist.end();
         itr++) {
        DebugOut(10) << "Converter convertMWtoAMB mwname = " << (*itr).mwname
                     << "\n";
        if ((*itr).mwname == mwvehicleinfo->name) {
            int arrayidx = 0;
            int statusidx = 0;
            for (auto itr2 = (*itr).ambdataarray.begin();
                 itr2 != (*itr).ambdataarray.end(); itr2++) {
                ambvehicleinfo[arrayidx].name = (*itr2).ambname;
                ambvehicleinfo[arrayidx].value =
                        VehicleProperty::getPropertyTypeForPropertyNameValue(
                                ambvehicleinfo[arrayidx].name, "0");
                if (ambvehicleinfo[arrayidx].value == NULL) {
                    continue;
                }
                ambvehicleinfo[arrayidx].value->timestamp = toDouble(
                        mwvehicleinfo->recordtime);
                char statusbuf[(*itr2).typesize];
                memcpy(statusbuf, mwvehicleinfo->status + statusidx,
                       sizeof(statusbuf));
                ambvehicleinfo[arrayidx].value->fromString(
                        toString((*itr2).ambname, statusbuf, sizeof(statusbuf),
                                 (*itr2).type));
                statusidx += sizeof(statusbuf);
                DebugOut() << "Converter convertMWtoAMB ambname = "
                           << (*itr2).ambname << "("
                           << ambvehicleinfo[arrayidx].value->toString() << ")\n";
                arrayidx++;
                if (arrayidx == arraysize
                        && arraysize
                                < static_cast<int>((*itr).ambdataarray.size())) {
                    ret = -1;
                    break;
                }
                ret = arrayidx;
            }
            break;
        }
    }
    return ret;
}

int
Converter::convertAMBtoMW(AMBVehicleInfo *ambvehicleinfo,
                          MWVehicleInfo *mwvehicleinfo)
{
    int ret = 0;
    int statusidx = 0;
    for (auto itr = converttablelist.begin(); itr != converttablelist.end();
            itr++) {
        statusidx = 0;
        DebugOut(10) << "Converter convertAMBtoMW mwname = " << (*itr).mwname
                     << "\n";
        for (auto itr2 = (*itr).ambdataarray.begin();
             itr2 != (*itr).ambdataarray.end(); itr2++) {
            DebugOut(10) << "Converter convertAMBtoMW ambname = "
                         << (*itr2).ambname << "\n";
            if ((*itr2).ambname == ambvehicleinfo->name) {
                DebugOut() << "Converter convertAMBtoMW ambname = "
                           << (*itr2).ambname << "\n";
                mwvehicleinfo->name = (*itr).mwname;
                mwvehicleinfo->recordtime = toTimeval(
                        ambvehicleinfo->value->timestamp);
                if (ambvehicleinfo->value == NULL) {
                    DebugOut() << "Converter convertAMBtoMW "
                               << "ambvehicleinfo->value is NULL\n";
                    ret = -1;
                }
                else {
                    DebugOut() << "Converter check data "
                               << ambvehicleinfo->value->toString() << std::endl;
                    toBinary(ambvehicleinfo->name, ambvehicleinfo->value,
                             (*itr2).typesize, (*itr2).type,
                             mwvehicleinfo->status + statusidx);
                    ret = 1;
                }
                return ret;
            }
            statusidx += (*itr2).typesize;
        }
    }
    return ret;
}

string
Converter::toString(string ambname, char *data, int size, DataType type)
{
    if (find(specialconvertlist.begin(), specialconvertlist.end(), ambname)
            != specialconvertlist.end()) {
        return specialConvertMWtoAMB(ambname, data, size, type);
    }
    stringstream sstr;
    sstr.str("");
    sstr.precision(10);
    switch (type) {
    case INT:
    {
        int val;
        memcpy(&val, data, size);
        sstr << val;
        break;
    }
    case DOUBLE:
    {
        double val;
        memcpy(&val, data, size);
        sstr << val;
        break;
    }
    case CHAR:
    {
        char val;
        memcpy(&val, data, size);
        sstr << val;
        break;
    }
    case INT16:
    {
        int16_t val;
        memcpy(&val, data, size);
        sstr << val;
        break;
    }
    case UINT16:
    {
        uint16_t val;
        memcpy(&val, data, size);
        sstr << val;
        break;
    }
    case UINT32:
    {
        uint32_t val;
        memcpy(&val, data, size);
        sstr << val;
        break;
    }
    case INT64:
    {
        int64_t val;
        memcpy(&val, data, size);
        sstr << val;
        break;
    }
    case UINT64:
    {
        uint64_t val;
        memcpy(&val, data, size);
        sstr << val;
        break;
    }
    case BOOL:
    {
        bool val;
        memcpy(&val, data, size);
        sstr << val;
        break;
    }
    default:
    {
        break;
    }
    }
    return sstr.str();
}

int
Converter::toBinary(string ambname, AbstractPropertyType *value, int size,
                    DataType type, char *buf)
{
    stringstream sstr;
    sstr.str("");
    if (find(specialconvertlist.begin(), specialconvertlist.end(), ambname)
            != specialconvertlist.end()) {
        sstr << specialConvertAMBtoMW(ambname, value);
    }
    else {
        sstr << value->toString();
    }
    DebugOut(10) << "Converter toBinary " << sstr.str() << "\n";
    switch (type) {
    case INT:
    {
        int val;
        sstr >> val;
        memcpy(buf, &val, size);
        DebugOut() << "Converter toBinary<int> " << val << "\n";
        break;
    }
    case DOUBLE:
    {
        double val;
        sstr >> val;
        memcpy(buf, &val, size);
        DebugOut() << "Converter toBinary<double> " << val << "\n";
        break;
    }
    case CHAR:
    {
        char val;
        sstr >> val;
        memcpy(buf, &val, size);
        DebugOut() << "Converter toBinary<char> " << val << "\n";
        break;
    }
    case INT16:
    {
        int16_t val;
        sstr >> val;
        memcpy(buf, &val, size);
        DebugOut() << "Converter toBinary<int16_t> " << val << "\n";
        break;
    }
    case UINT16:
    {
        uint16_t val;
        sstr >> val;
        memcpy(buf, &val, size);
        DebugOut() << "Converter toBinary<uint16_t> " << val << "\n";
        break;
    }
    case UINT32:
    {
        uint32_t val;
        sstr >> val;
        memcpy(buf, &val, size);
        DebugOut() << "Converter toBinary<uint32_t> " << val << "\n";
        break;
    }
    case INT64:
    {
        int64_t val;
        sstr >> val;
        memcpy(buf, &val, size);
        DebugOut() << "Converter toBinary<int64_t> " << val << "\n";
        break;
    }
    case UINT64:
    {
        uint64_t val;
        sstr >> val;
        memcpy(buf, &val, size);
        DebugOut() << "Converter toBinary<uint64_t> " << val << "\n";
        break;
    }
    case BOOL:
    {
        int tmpval;
        bool val;
        sstr >> tmpval;
        if (tmpval > 0) {
            val = true;
        }
        else {
            val = false;
        }
        memcpy(buf, &val, sizeof(bool));
        DebugOut() << "Converter toBinary<bool> " << val << "\n";
        break;
    }
    default:
    {
        size = 0;
        break;
    }
    }
    return size;
}

double
Converter::toDouble(timeval time)
{
    return static_cast<double>(time.tv_sec + (time.tv_usec / 1000000));
}

timeval
Converter::toTimeval(double time)
{
    timeval tv;
    tv.tv_sec = time;
    double utime = time - tv.tv_sec;
    tv.tv_usec = utime * 1000000;
    return tv;
}

std::string
Converter::specialConvertAMBtoMW(std::string ambname,
                                 AbstractPropertyType *value)
{
    stringstream retstr;
    retstr.str("");
    if (ambname == VehicleProperty::TransmissionShiftPosition) {
        switch (boost::any_cast < Transmission::TransmissionPositions
                > (value->anyValue())) {
        case Transmission::Neutral:
        {
            retstr << 2;
            break;
        }
        case Transmission::First:
        {
            retstr << 5;
            break;
        }
        case Transmission::Second:
        {
            retstr << 6;
            break;
        }
        case Transmission::Third:
        {
            retstr << 7;
            break;
        }
        case Transmission::Forth:
        {
            retstr << 8;
            break;
        }
        case Transmission::Fifth:
        {
            retstr << 9;
            break;
        }
        case Transmission::Sixth:
        {
            retstr << 10;
            break;
        }
        case Transmission::Seventh:
        {
            retstr << 11;
            break;
        }
        case Transmission::Eighth:
        {
            retstr << 12;
            break;
        }
        case Transmission::Ninth:
        {
            retstr << 13;
            break;
        }
        case Transmission::Tenth:
        {
            retstr << 14;
            break;
        }
        case Transmission::CVT:
        {
            retstr << 15;
            break;
        }
        case Transmission::Drive:
        {
            retstr << 4;
            break;
        }
        case Transmission::Reverse:
        {
            retstr << 1;
            break;
        }
        case Transmission::Park:
        {
            retstr << 0;
            break;
        }
        }
        DebugOut() << "Converter specialConvertAMBtoMW"
                   << "(TransmissionShiftPosition): " << value->toString() << "->"
                   << retstr.str() << std::endl;
    }
    else if (ambname == VehicleProperty::TransmissionGearPosition) {
        switch (boost::any_cast < Transmission::TransmissionPositions
                > (value->anyValue())) {
        case Transmission::Neutral:
        {
            retstr << 64;
            break;
        }
        case Transmission::First:
        case Transmission::Second:
        case Transmission::Third:
        case Transmission::Forth:
        case Transmission::Fifth:
        case Transmission::Sixth:
        case Transmission::Seventh:
        case Transmission::Eighth:
        case Transmission::Ninth:
        case Transmission::Tenth:
        {
            retstr << value->toString();
            break;
        }
        case Transmission::CVT:
        {
            retstr << 256;
            break;
        }
        case Transmission::Drive:
        {
            retstr << 0;
            break;
        }
        case Transmission::Reverse:
        {
            retstr << 128;
            break;
        }
        case Transmission::Park:
        {
            retstr << 0;
            break;
        }
        default:
        {
            retstr << 172;
            break;
        }
        }
        DebugOut() << "Converter specialConvertAMBtoMW"
                   << "(TransmissionGearPosition): " << value->toString() << "->"
                   << retstr.str() << std::endl;
    }
    else if (ambname == VehicleProperty::TransmissionMode) {
        retstr << value->toString();
    }
    else if (ambname == VehicleProperty::TurnSignal) {
        retstr << value->toString();
    }
    else if (ambname == VehicleProperty::ButtonEvent) {
        /* ToDo */
    }
    return retstr.str();
}

std::string
Converter::specialConvertMWtoAMB(std::string ambname, char *data, int size,
                                 DataType type)
{
    stringstream retstr;
    retstr.str("");
    if (ambname == VehicleProperty::TransmissionShiftPosition) {
        int val;
        memcpy(&val, data, size);
        switch (val) {
        case 0:
        {
            retstr << Transmission::Park;
            break;
        }
        case 1:
        {
            retstr << Transmission::Reverse;
            break;
        }
        case 2:
        {
            retstr << Transmission::Neutral;
            break;
        }
        case 4:
        {
            retstr << Transmission::Drive;
            break;
        }
        case 5:
        {
            retstr << Transmission::First;
            break;
        }
        case 6:
        {
            retstr << Transmission::Second;
            break;
        }
        case 7:
        {
            retstr << Transmission::Third;
            break;
        }
        case 8:
        {
            retstr << Transmission::Forth;
            break;
        }
        case 9:
        {
            retstr << Transmission::Fifth;
            break;
        }
        case 10:
        {
            retstr << Transmission::Sixth;
            break;
        }
        case 11:
        {
            retstr << Transmission::Seventh;
            break;
        }
        case 12:
        {
            retstr << Transmission::Eighth;
            break;
        }
        case 13:
        {
            retstr << Transmission::Ninth;
            break;
        }
        case 14:
        {
            retstr << Transmission::Tenth;
            break;
        }
        case 15:
        {
            retstr << Transmission::CVT;
            break;
        }
        default:
        {
            break;
        }
        }
        DebugOut() << "Converter specialConvertMWtoAMB"
                   << "(TransmissionShiftPosition): " << val << "->"
                   << retstr.str() << std::endl;
    }
    else if (ambname == VehicleProperty::TransmissionGearPosition) {
        int val;
        memcpy(&val, data, size);
        switch (val) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        {
            retstr << val;
            break;
        }
        case 64:
        {
            retstr << Transmission::Neutral;
            break;
        }
        case 128:
        {
            retstr << Transmission::Reverse;
            break;
        }
        case 256:
        {
            retstr << Transmission::CVT;
            break;
        }
        default:
        {
            retstr << 0;
            break;
        }
        }
        DebugOut() << "Converter specialConvertMWtoAMB"
                   << "(TransmissionGearPosition): " << val << "->" << retstr.str()
                   << std::endl;
    }
    else if (ambname == VehicleProperty::TransmissionMode) {
        int val;
        memcpy(&val, data, size);
        retstr << val;
    }
    else if (ambname == VehicleProperty::TurnSignal) {
        int val;
        memcpy(&val, data, size);
        retstr << val;
    }
    else if (ambname == VehicleProperty::ButtonEvent) {
        /* ToDo */
    }
    return retstr.str();
}
