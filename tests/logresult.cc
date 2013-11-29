#include <sstream>

#include "logresult.h"

LogResult::LogResult() {
}

LogResult::~LogResult() {
}

std::string LogResult::MakeString(enum ClientType cltype, enum CommandType cotype, std::string property) {
    CurrentTime();
    std::stringstream sstr;
    sstr << tv.tv_sec << "." << std::setfill('0') << std::setw(6) << std::right << tv.tv_usec << " ";
    switch (cltype) {
    case CLIENT_VIC :
        sstr << "VIC ";
        break;
    case CLIENT_APP :
        sstr << "APP ";
        break;
    default :
        sstr << "Unknown ";
        break;
    }
    switch (cotype) {
    case CMDTYPE_SET:
        sstr << "SET ";
        break;
    case CMDTYPE_GET:
        sstr << "GET ";
        break;
    case CMDTYPE_NOTIFY:
        sstr << "NOTIFY ";
        break;
    case CMDTYPE_CALLBACK:
        sstr << "CALLBACK ";
        break;
    default :
        sstr << "NONE ";
        break;
    }

    sstr << property << " ";
    return sstr.str();
}

void LogResult::CurrentTime() {
    gettimeofday(&tv, NULL);
}
