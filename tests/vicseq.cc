#include <unistd.h>
#include <sys/time.h>

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
int maxstandarddatacount = 1;
int maxcustomdatacount = 0;
int interspace = 100;
int testdatanum = maxstandarddatacount + maxcustomdatacount;

struct TestData {
    std::string vicproperty;
    std::string dbusproperty;
    DataType type;
    bool isstd;
};

template <typename T>
int SetDataOpt(DataOpt *opt, int idx, T value) {
    memcpy(opt->status + idx, &value, sizeof(value));
    return sizeof(value);
}

int main(int argc, char *argv[]) {
    int ret;
    std::string configpath = "/etc/ambd/AMBformat.conf";
    std::string retstr;
    while ((ret = getopt(argc, argv, "c:i:n:s:")) != -1) {
        switch (ret) {
        case 'c':
            configpath = std::string(optarg);
            break;
        case 'i':
            retstr = std::string(optarg);
            interspace = std::stoi(retstr);
            break;
        case 'n':
            retstr = std::string(optarg);
            maxcustomdatacount = std::stoi(retstr);
            testdatanum = maxstandarddatacount + maxcustomdatacount;
            break;
        case 's':
            retstr = std::string(optarg);
            maxstandarddatacount = std::stoi(retstr);
            testdatanum = maxstandarddatacount + maxcustomdatacount;
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
    struct TestData testdata[testdatanum];
    int datacount = 0;
    int standarddatacount = 0;
    int customdatacount = 0;

    for (int i = 0; i < 10; i++) {
        stddataws.service();
        customdataws.service();
        usleep(50 * 1000);
    }

    DataOpt dopt;
    for (auto itr = vehicleinfolist.begin(); itr != itr_end && datacount < testdatanum; itr++) {
        if ((*itr).status.size() > 1) {
            continue;
        }
        auto itr2_end = (*itr).status.end();
        if (std::find(standardvehicleinfolist.begin(), standardvehicleinfolist.end(), std::string((*itr).KeyEventType)) != standardvehicleinfolist.end()) {
            if (standarddatacount >= maxstandarddatacount) {
                continue;
            }
            testdata[datacount].vicproperty = std::string((*itr).KeyEventType);
            testdata[datacount].isstd = true;
            for (auto itr2 = (*itr).status.begin(); itr2 != itr2_end; itr2++) {
                testdata[datacount].dbusproperty = std::string((*itr2).dbusPropertyName);
                testdata[datacount].type = (*itr2).type;
            }
            datacount++;
            standarddatacount++;
        }
        else {
            if (customdatacount >= maxcustomdatacount) {
                continue;
            }
            testdata[datacount].vicproperty = std::string((*itr).KeyEventType);
            testdata[datacount].isstd = false;
            for (auto itr2 = (*itr).status.begin(); itr2 != itr2_end; itr2++) {
                testdata[datacount].dbusproperty = std::string((*itr2).dbusPropertyName);
                testdata[datacount].type = (*itr2).type;
            }
            datacount++;
            customdatacount++;
        }
    }
    
    struct timeval starttv, endtv;
    for (int i = 0; i < 100; i++) {
        gettimeofday(&starttv, NULL);
        for (int j = 0; j < testdatanum; j++) {
            switch (testdata[j].type) {
            case INT : {
                static int i = 0;
                idx = SetDataOpt(&dopt, 0, i);
                logger.PutOut(cltype, cotype, testdata[j].dbusproperty, i++);
                break;
            }
            case DOUBLE : {
                static double d = 0.0;
                idx = SetDataOpt(&dopt, 0, d);
                logger.PutOut(cltype, cotype, testdata[j].dbusproperty, d);
                d += 0.1;
                break;
            }
            case CHAR : {
                static char c = 0;
                idx = SetDataOpt(&dopt, 0, c);
                logger.PutOut(cltype, cotype, testdata[j].dbusproperty, c++);
                break;
            }
            case INT16 : {
                static int16_t i16 = 0;
                idx = SetDataOpt(&dopt, 0, i16);
                logger.PutOut(cltype, cotype, testdata[j].dbusproperty, i16++);
                break;
            }
            case UINT16 : {
                static uint16_t ui16 = 0;
                idx = SetDataOpt(&dopt, 0, ui16);
                logger.PutOut(cltype, cotype, testdata[j].dbusproperty, ui16++);
                break;
            }
            case UINT32 : {
                static uint32_t ui32 = 0;
                idx = SetDataOpt(&dopt, 0, ui32);
                logger.PutOut(cltype, cotype, testdata[j].dbusproperty, ui32++);
                break;
            }
            case INT64 : {
                static int64_t i64 = 0;
                idx = SetDataOpt(&dopt, 0, i64);
                logger.PutOut(cltype, cotype, testdata[j].dbusproperty, i64++);
                break;
            }
            case UINT64 : {
                static uint64_t ui64 = 0;
                idx = SetDataOpt(&dopt, 0, ui64);
                logger.PutOut(cltype, cotype, testdata[j].dbusproperty, ui64++);
                break;
            }
            case BOOL : {
                static bool b = true;
                idx = SetDataOpt(&dopt, 0, b);
                logger.PutOut(cltype, cotype, testdata[j].dbusproperty, b);
                b = !b;
                break;
            }
            default : {
                break;
            }
            }
            gettimeofday(&tv, NULL);
            if (testdata[j].isstd) {
                stddataws.send(datamsg.encode(const_cast<char*>(testdata[j].vicproperty.c_str()), tv, dopt), StandardMessage::KEYEVENTTYPESIZE + sizeof(timeval) + sizeof(int) + idx);
            }
            else {
                customdataws.send(datamsg.encode(const_cast<char*>(testdata[j].vicproperty.c_str()), tv, dopt), StandardMessage::KEYEVENTTYPESIZE + sizeof(timeval) + sizeof(int) + idx);
            }
        }

        stddataws.service();
        customdataws.service();
        gettimeofday(&endtv, NULL);
        //std::cerr << "End - Start = " << ((endtv.tv_sec - starttv.tv_sec) * 1000 * 1000 + (endtv.tv_usec - starttv.tv_usec)) << std::endl;
        usleep(interspace * 1000 - ((endtv.tv_sec - starttv.tv_sec) * 1000 * 1000 + (endtv.tv_usec - starttv.tv_usec)));
    }

    return 0;
}
