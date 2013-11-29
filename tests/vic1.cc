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
char NOTREGISTERED[] = "NotRegistered";

class ControlWebsocketClientForControl : public ControlWebsocketClient {
    public:
        bool recv(char *recvbuf, const size_t len);
    private:
        LogResult logger;
};

bool ControlWebsocketClientForControl::recv(char *recvbuf, const size_t len) {
    DataMessage datamsg;
    datamsg.decode(recvbuf, len);
    DataOpt dopt = datamsg.getDataOpt();

    auto itr_end = vehicleinfolist.end();
    ClientType cltype = CLIENT_VIC;
    CommandType cotype = CMDTYPE_GET;
    for (auto itr = vehicleinfolist.begin(); itr != itr_end; itr++) {
        if (strcmp((*itr).KeyEventType, datamsg.getKeyEventType()) == 0) {
            auto itr2_end = (*itr).status.end();
            int idx = 0;
            for (auto itr2 = (*itr).status.begin(); itr2 != itr2_end; itr2++) {
                switch ((*itr2).type) {
                case INT:
                    int i;
                    memcpy(&i, dopt.status + idx, sizeof(i));
                    logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, i, datamsg.getRecordtime());
                    idx += sizeof(i);
                    break;
                case DOUBLE:
                    double d;
                    memcpy(&d, dopt.status + idx, sizeof(d));
                    logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, d, datamsg.getRecordtime());
                    idx += sizeof(d);
                    break;
                case CHAR:
                    char c;
                    memcpy(&c, dopt.status + idx, sizeof(c));
                    logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, c, datamsg.getRecordtime());
                    idx += sizeof(c);
                    break;
                case INT16:
                    int16_t i16;
                    memcpy(&i16, dopt.status + idx, sizeof(i16));
                    logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, i16, datamsg.getRecordtime());
                    idx += sizeof(i16);
                    break;
                case UINT16:
                    uint16_t ui16;
                    memcpy(&ui16, dopt.status + idx, sizeof(ui16));
                    logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, ui16, datamsg.getRecordtime());
                    idx += sizeof(ui16);
                    break;
                case UINT32:
                    uint32_t ui32;
                    memcpy(&ui32, dopt.status + idx, sizeof(ui32));
                    logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, ui32, datamsg.getRecordtime());
                    idx += sizeof(ui32);
                    break;
                case INT64:
                    int64_t i64;
                    memcpy(&i64, dopt.status + idx, sizeof(i64));
                    logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, i64, datamsg.getRecordtime());
                    idx += sizeof(i64);
                    break;
                case UINT64:
                    uint64_t ui64;
                    memcpy(&ui64, dopt.status + idx, sizeof(ui64));
                    logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, ui64, datamsg.getRecordtime());
                    idx += sizeof(ui64);
                    break;
                case BOOL:
                    bool b;
                    memcpy(&b, dopt.status + idx, sizeof(b));
                    logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, b, datamsg.getRecordtime());
                    idx += sizeof(b);
                    break;
                default:
                    break;
                }
            }
        }
    }
    return true;
}

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
    ControlWebsocketClientForControl stdctrlws, customctrlws;
    if (!stddataws.initialize("ws://127.0.0.1", portinfo.standard.dataPort, "standarddatamessage-only")) {
        std::cerr << "Can't connect standarddatamessage-only." << std::endl;
        return -1;
    }
    if (!stdctrlws.initialize("ws://127.0.0.1", portinfo.standard.controlPort, "standardcontrolmessage-only")) {
        std::cerr << "Can't connect standarddatamessage-only." << std::endl;
        return -1;
    }
    if (!customdataws.initialize("ws://127.0.0.1", portinfo.custom.dataPort, "customdatamessage-only")) {
        std::cerr << "Can't connect customdatamessage-only." << std::endl;
        return -1;
    }
    if (!customctrlws.initialize("ws://127.0.0.1", portinfo.custom.controlPort, "customcontrolmessage-only")) {
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
        stdctrlws.service();
        customctrlws.service();
        usleep(50 * 1000);
    }

    for (auto itr = vehicleinfolist.begin(); itr != itr_end; itr++) {
        stddataws.service();
        customdataws.service();
        stdctrlws.service();
        customctrlws.service();
        usleep(50 * 1000);

        DataOpt dopt;
        dopt.common_status = SUPPORT;
        idx = 0;
        auto itr2_end = (*itr).status.end();
        gettimeofday(&tv, NULL);
        for (auto itr2 = (*itr).status.begin(); itr2 != itr2_end; itr2++) {
            switch ((*itr2).type) {
            case INT : {
                int i = 5;
                idx += SetDataOpt(&dopt, idx, i);
                logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, i);
                break;
            }
            case DOUBLE : {
                double d = 5.0;
                idx += SetDataOpt(&dopt, idx, d);
                logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, d);
                break;
            }
            case CHAR : {
                char c = 5;
                idx += SetDataOpt(&dopt, idx, c);
                logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, c);
                break;
            }
            case INT16 : {
                int16_t i16 = 5;
                idx += SetDataOpt(&dopt, idx, i16);
                logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, i16);
                break;
            }
            case UINT16 : {
                uint16_t ui16 = 5;
                idx += SetDataOpt(&dopt, idx, ui16);
                logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, ui16);
                break;
            }
            case UINT32 : {
                uint32_t ui32 = 5;
                idx += SetDataOpt(&dopt, idx, ui32);
                logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, ui32);
                break;
            }
            case INT64 : {
                int64_t i64 = 5;
                idx += SetDataOpt(&dopt, idx, i64);
                logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, i64);
                break;
            }
            case UINT64 : {
                uint64_t ui64 = 5;
                idx += SetDataOpt(&dopt, idx, ui64);
                logger.PutOut(cltype, cotype, (*itr2).ambPropertyName, ui64);
                break;
            }
            case BOOL : {
                bool b = true;
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
        stdctrlws.service();
        customctrlws.service();
        usleep(50 * 1000);
    }
    DataOpt dopt;
    dopt.common_status = SUPPORT;
    SetDataOpt(&dopt, 0, (int)5);
    stddataws.send(datamsg.encode(NOTREGISTERED, tv, dopt), StandardMessage::KEYEVENTTYPESIZE + sizeof(timeval) + 2 * sizeof(int));
    stddataws.service();
    customdataws.service();
    stdctrlws.service();
    customctrlws.service();
    usleep(50 * 1000);
    customdataws.send(datamsg.encode(NOTREGISTERED, tv, dopt), StandardMessage::KEYEVENTTYPESIZE + sizeof(timeval) + 2 * sizeof(int));
    stddataws.service();
    customdataws.service();
    stdctrlws.service();
    customctrlws.service();
    usleep(50 * 1000);
    sleep(1);
    EventMessage eventmsg;
    EventOpt eopt;
    eopt.common = -1;
    eopt.sense = -1;
    eopt.event_mask = -1;

    for (auto itr = vehicleinfolist.begin(); itr != itr_end; itr++) {
        stddataws.service();
        customdataws.service();
        stdctrlws.service();
        customctrlws.service();
        usleep(50 * 1000);

        gettimeofday(&tv, NULL);
        if (std::find(standardvehicleinfolist.begin(), standardvehicleinfolist.end(), std::string((*itr).KeyEventType)) != standardvehicleinfolist.end()) {
            stdctrlws.send(eventmsg.encode((*itr).KeyEventType, tv, eopt), StandardMessage::KEYEVENTTYPESIZE + sizeof(timeval) + sizeof(EventOpt));
            usleep(100 * 1000);
        }
        else {
            customctrlws.send(eventmsg.encode((*itr).KeyEventType, tv, eopt), StandardMessage::KEYEVENTTYPESIZE + sizeof(timeval) + sizeof(EventOpt));
            usleep(100 * 1000);
        }
        stddataws.service();
        customdataws.service();
        stdctrlws.service();
        customctrlws.service();
        usleep(50 * 1000);
    }
    stdctrlws.send(eventmsg.encode(NOTREGISTERED, tv, eopt), StandardMessage::KEYEVENTTYPESIZE + sizeof(timeval) + sizeof(EventOpt));
    stddataws.service();
    customdataws.service();
    stdctrlws.service();
    customctrlws.service();
    usleep(50 * 1000);
    customctrlws.send(eventmsg.encode(NOTREGISTERED, tv, eopt), StandardMessage::KEYEVENTTYPESIZE + sizeof(timeval) + sizeof(EventOpt));
    stddataws.service();
    customdataws.service();
    stdctrlws.service();
    customctrlws.service();
    usleep(50 * 1000);

    for (int i = 0; i < 10; i++) {
        stddataws.service();
        customdataws.service();
        stdctrlws.service();
        customctrlws.service();
        usleep(50 * 1000);
    }
    
    return 0;
}
