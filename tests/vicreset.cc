#include <unistd.h>

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <vector>

#include "ambconfig.h"
#include "controlwebsocketclient.h"
#include "datamessage.h"
#include "eventmessage.h"
#include "logresult.h"

std::vector<VehicleInfoDefine> vehicleinfolist;

template <typename T>
int SetDataOpt(DataOpt *opt, int idx, T value) {
    memcpy(opt->status + idx, &value, sizeof(value));
    return sizeof(value);
}

int main(int argc, char *argv[]) {
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

    ifstream fin("standardvehicleinfo.txt");
    std::string str;
    std::vector<std::string> standardvehicleinfolist;
    while (!fin.eof()) {
        fin >> str;
        standardvehicleinfolist.push_back(str);
    }

    AMBConfig config;
    if (!config.readConfig(configpath)) {
        std::cerr << "Can't read VIC-Plugin config file." << std::endl;
        return -1;
    }
    PortInfo portinfo = config.getPort();

    ControlWebsocketClient stddataws, customdataws;
    if (!stddataws.initialize("ws://127.0.0.1", portinfo.standard.dataPort, "standarddatamessage-only")) {
        std::cerr << "Can't connect standarddatamessage-only." << std::endl;
        return -1;
    }
    if (!customdataws.initialize("ws://127.0.0.1", portinfo.custom.dataPort, "customdatamessage-only")) {
        std::cerr << "Can't connect customdatamessage-only." << std::endl;
        return -1;
    }

    vehicleinfolist =config.getVehicleInfoConfig();
    auto itr_end = vehicleinfolist.end();
    int idx = 0;
    struct timeval tv;
    LogResult logger;
    enum ClientType cltype = CLIENT_VIC;
    enum CommandType cotype = CMDTYPE_SET;
    DataMessage datamsg;

    for (int i = 0; i < 10; i++) {
        stddataws.service();
        customdataws.service();
        usleep(50 * 1000);
    }

    for (auto itr = vehicleinfolist.begin(); itr != itr_end; itr++) {
        stddataws.service();
        customdataws.service();
        usleep(50 * 1000);

        DataOpt dopt;
        dopt.common_status = SUPPORT;
        idx = 0;
        auto itr2_end = (*itr).status.end();
        gettimeofday(&tv, NULL);
        for (auto itr2 = (*itr).status.begin(); itr2 != itr2_end; itr2++) {
            switch ((*itr2).type) {
            case INT : {
                int i = 0;
                idx += SetDataOpt(&dopt, idx, i);
                logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, i);
                break;
            }
            case DOUBLE : {
                double d = 0.0;
                idx += SetDataOpt(&dopt, idx, d);
                logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, d);
                break;
            }
            case CHAR : {
                char c = 0;
                idx += SetDataOpt(&dopt, idx, c);
                logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, c);
                break;
            }
            case INT16 : {
                int16_t i16 = 0;
                idx += SetDataOpt(&dopt, idx, i16);
                logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, i16);
                break;
            }
            case UINT16 : {
                uint16_t ui16 = 0;
                idx += SetDataOpt(&dopt, idx, ui16);
                logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, ui16);
                break;
            }
            case UINT32 : {
                uint32_t ui32 = 0;
                idx += SetDataOpt(&dopt, idx, ui32);
                logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, ui32);
                break;
            }
            case INT64 : {
                int64_t i64 = 0;
                idx += SetDataOpt(&dopt, idx, i64);
                logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, i64);
                break;
            }
            case UINT64 : {
                uint64_t ui64 = 0;
                idx += SetDataOpt(&dopt, idx, ui64);
                logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, ui64);
                break;
            }
            case BOOL : {
                bool b = false;
                idx += SetDataOpt(&dopt, idx, b);
                logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, b);
                break;
            }
            default : {
                break;
            }
            }
        }
        if (std::find(standardvehicleinfolist.begin(), standardvehicleinfolist.end(), std::string((*itr).KeyEventType)) != standardvehicleinfolist.end()) {
            stddataws.send(datamsg.encode((*itr).KeyEventType, tv, dopt), StandardMessage::KEYEVENTTYPESIZE + sizeof(timeval) + sizeof(int) + idx);
            usleep(100 * 1000);
        }
        else {
            customdataws.send(datamsg.encode((*itr).KeyEventType, tv, dopt), StandardMessage::KEYEVENTTYPESIZE + sizeof(timeval) + sizeof(int) + idx);
            usleep(100 * 1000);
        }
        stddataws.service();
        customdataws.service();
        usleep(50 * 1000);
    }
    
    for (int i = 0; i < 10; i++) {
        stddataws.service();
        customdataws.service();
        usleep(50 * 1000);
    }

    return 0;
}
