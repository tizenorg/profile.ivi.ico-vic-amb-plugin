#include <appcore-efl.h>
#include <Elementary.h>
#include <stdio.h>

#include <iostream>
#include <vector>

#include "ambconfig.h"
#include "ico_dbus_amb_efl.h"

void disp_notify(char *property, dbus_type type, union dbus_value_variant value, int sequence, struct timeval tv) {
    std::cout << tv.tv_sec << "." << tv.tv_usec << " " << property << " ";
    switch (type) {
    case DBUS_TYPE_BYTE :
        std::cout << value.yval;
        break;
    case DBUS_TYPE_BOOLEAN :
        if (value.bval) {
            std::cout << "true";
        }
        else {
            std::cout << "false";
        }
        break;
    case DBUS_TYPE_INT16 :
        std::cout << value.i16val;
        break;
    case DBUS_TYPE_UINT16 :
        std::cout << value.ui16val;
        break;
    case DBUS_TYPE_INT32 :
        std::cout << value.i32val;
        break;
    case DBUS_TYPE_UINT32 :
        std::cout << value.ui32val;
        break;
    case DBUS_TYPE_DOUBLE : 
        std::cout << value.dval;
        break;
    case DBUS_TYPE_STRING :
        std::cout << value.sval;
        break;
    default :
        std::cout << "Not supported.";
        break;
    }
    std::cout << std::endl;
}

EAPI int elm_main(int argc, char *argv[]) {
    struct appcore_ops ops;

    ops.create = NULL;
    ops.resume = NULL;
    ops.reset = NULL;
    ops.pause = NULL;
    ops.terminate = NULL;
    ops.data = NULL;

    ico_dbus_amb_start();
    std::vector<VehicleInfoDefine> configlist;
    configlist = 
    //ico_dbus_amb_get("VehicleSpeed", 0, DBUS_TYPE_UINT16, getvalue);
    //ico_dbus_amb_get("EngineSpeed", 0, DBUS_TYPE_UINT16, getvalue);

    //ico_dbus_amb_subscribe("EngineSpeed", 0, DBUS_TYPE_UINT16, notifyvalue);
    //ico_dbus_amb_get("FuelAverageEconomy", 0, DBUS_TYPE_UINT16, getvalue);

    appcore_efl_main("org.tizen.ico.testdbus", &argc, &argv, &ops);

    ico_dbus_amb_end();

    elm_shutdown();
}
ELM_MAIN()
