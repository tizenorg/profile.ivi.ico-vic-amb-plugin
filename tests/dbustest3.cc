#include <appcore-efl.h>
#include <Elementary.h>
#include <stdio.h>

#include <cassert>
#include <ico_dbus_amb_efl.h>

#include "ambconfig.h"
#include "logresult.h"

void getvalue(const char *objectname, const char *property, dbus_type type, union dbus_value_variant value, void *user_data) {
    enum ClientType cltype = CLIENT_APP;
    enum CommandType cotype = CMDTYPE_GET;
    LogResult *logger = reinterpret_cast<LogResult*>(user_data);
    switch (type) {
    case DBUS_TYPE_BYTE :
        logger->PutOut(cltype, cotype, std::string(property), value.yval);
        break;
    case DBUS_TYPE_BOOLEAN :
        if (value.bval) {
            logger->PutOut(cltype, cotype, std::string(property), true);
        }
        else {
            logger->PutOut(cltype, cotype, std::string(property), false);
        }
        break;
    case DBUS_TYPE_INT16 :
        logger->PutOut(cltype, cotype, std::string(property), value.i16val);
        break;
    case DBUS_TYPE_UINT16 :
        logger->PutOut(cltype, cotype, std::string(property), value.ui16val);
        break;
    case DBUS_TYPE_INT32 :
        logger->PutOut(cltype, cotype, std::string(property), value.i32val);
        break;
    case DBUS_TYPE_UINT32 :
        logger->PutOut(cltype, cotype, std::string(property), value.ui32val);
        break;
    case DBUS_TYPE_DOUBLE :
        logger->PutOut(cltype, cotype, std::string(property), value.dval);
        break;
    case DBUS_TYPE_STRING :
        logger->PutOut(cltype, cotype, std::string(property), value.sval);
        break;
    default :
        break;
    }
    return;
}

EAPI int elm_main(int argc, char *argv[]) {
    int ret;
    std::string configpath = "/etc/ambd/AMBformat.conf";
    while ((ret = getopt(argc, argv, "c:")) != -1) {
        switch (ret) {
        case 'c':
            configpath = std::string(optarg);
            break;
        default :
            break;
        }
    }

    struct appcore_ops ops;
    ops.create = NULL;
    ops.resume = NULL;
    ops.reset = NULL;
    ops.pause = NULL;
    ops.terminate = NULL;
    ops.data = NULL;

    assert(ico_dbus_amb_start() == 0);

    AMBConfig config;
    if (!config.readConfig(configpath)) {
        std::cerr << "Can't read VIC-Plugin config file." << std::endl;
        return -1;
    }

    std::vector<VehicleInfoDefine> vehicleinfolist =config.getVehicleInfoConfig();
    LogResult logger;

    auto itr_end = vehicleinfolist.end();
    for (auto itr = vehicleinfolist.begin(); itr != itr_end; itr++) {
        auto itr2_end = (*itr).status.end();
        for (auto itr2 = (*itr).status.begin(); itr2 != itr2_end; itr2++) {
            switch ((*itr2).type) {
            case INT : {
                ico_dbus_amb_get((*itr2).dbusObjectName.c_str(), (*itr2).dbusPropertyName.c_str(), 0, DBUS_TYPE_INT32, getvalue, &logger);
                break;
            }
            case DOUBLE : {
                ico_dbus_amb_get((*itr2).dbusObjectName.c_str(), (*itr2).dbusPropertyName.c_str(), 0, DBUS_TYPE_DOUBLE, getvalue, &logger);
                break;
            }
            case CHAR : {
                ico_dbus_amb_get((*itr2).dbusObjectName.c_str(), (*itr2).dbusPropertyName.c_str(), 0, DBUS_TYPE_BYTE, getvalue, &logger);
                break;
            }
            case INT16 : {
                ico_dbus_amb_get((*itr2).dbusObjectName.c_str(), (*itr2).dbusPropertyName.c_str(), 0, DBUS_TYPE_INT16, getvalue, &logger);
                break;
            }
            case UINT16 : {
                ico_dbus_amb_get((*itr2).dbusObjectName.c_str(), (*itr2).dbusPropertyName.c_str(), 0, DBUS_TYPE_UINT16, getvalue, &logger);
                break;
            }
            case UINT32 : {
                ico_dbus_amb_get((*itr2).dbusObjectName.c_str(), (*itr2).dbusPropertyName.c_str(), 0, DBUS_TYPE_UINT32, getvalue, &logger);
                break;
            }
            case INT64 : {
                ico_dbus_amb_get((*itr2).dbusObjectName.c_str(), (*itr2).dbusPropertyName.c_str(), 0, DBUS_TYPE_INT64, getvalue, &logger);
                break;
            }
            case UINT64 : {
                ico_dbus_amb_get((*itr2).dbusObjectName.c_str(), (*itr2).dbusPropertyName.c_str(), 0, DBUS_TYPE_UINT64, getvalue, &logger);
                break;
            }
            case BOOL : {
                ico_dbus_amb_get((*itr2).dbusObjectName.c_str(), (*itr2).dbusPropertyName.c_str(), 0, DBUS_TYPE_BOOLEAN, getvalue, &logger);
                break;
            }
            default : {
                break;
            }
            }
        }
    }

    appcore_efl_main("org.tizen.ico.testdbus", &argc, &argv, &ops);

    assert(ico_dbus_amb_end() == 0);

    elm_shutdown();

    return 0;
}
ELM_MAIN()
