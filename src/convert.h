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
#ifndef CONVERT_H
#define CONVERT
#include <string>

#include "abstractpropertytype.h"

#include "config.h"

struct AMBVehicleInfo;
struct MWVehicleInfo;

/**
 * This class performs the conversion of vehicle information of AMB and vehicle information of MW.
 */
class Converter {
public:
    /**
     * Constructor.
     */
    Converter();
    /**
     * Destructor.
     */
    ~Converter();
    /**
     * Initialization.
     *
     * @param conf Instanse of Config class.
     * @return Success : true Failure : false
     */
    bool
    initialize(Config *conf);
    /**
     * This function converts the vehicle infomation of MW into the vehicle information of AMB.
     *
     * @param mwvehicleinfo Vehicle information of MW.
     * @param ambvehicleinfo Vehicle information of AMB.
     * @return -1 : Conversion failure >0 : The number of pieces of vehicle information of AMB that has been converted.
     */
    int
    convertMWtoAMB(MWVehicleInfo *mwvehicleinfo, AMBVehicleInfo *ambvehicleinfo,
                   int arraysize);
    /**
     * This function converts the vehicle information of AMB into the vehicle information of MW.
     *
     * @param ambvehicleinfo Vehicle information of AMB.
     * @param mwvehicleinfo Vehicle information of MW.
     * @return -1 : Conversion failure >0 : The number of pieces of vehicle information of AMB that has been converted.
     */
    int
    convertAMBtoMW(AMBVehicleInfo *ambvehicleinfo,
                   MWVehicleInfo *mwvehicleinfo);
private:
    std::string
    toString(std::string ambname, char *data, int size,
             VehicleInfoDefine::Status::DataType type);
    int
    toBinary(std::string ambname, AbstractPropertyType *value, int size,
             VehicleInfoDefine::Status::DataType type, char *buf);
    double
    toDouble(timeval time);
    timeval
    toTimeval(double time);
    std::string
    specialConvertAMBtoMW(std::string ambname, AbstractPropertyType *value);
    std::string
    specialConvertMWtoAMB(std::string ambname, char *data, int size,
                          VehicleInfoDefine::Status::DataType type);
    struct ConvertTable {
        std::string mwname;
        struct AmbVehicleInfoData {
            std::string ambname;
            VehicleInfoDefine::Status::DataType type;
            int typesize;
        };
        std::vector<AmbVehicleInfoData> ambdataarray;
    };

    std::vector<ConvertTable> converttablelist;
    std::vector<std::string> specialconvertlist;
};
#endif // CONVERT_H
