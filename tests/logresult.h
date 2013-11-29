#include <sys/time.h>

#include <iomanip>
#include <iostream>
#include <string>
#include <typeinfo>

enum CommandType {
    CMDTYPE_SET,
    CMDTYPE_GET,
    CMDTYPE_NOTIFY,
    CMDTYPE_CALLBACK,
    CMDTYPE_NONE,
};

enum ClientType {
    CLIENT_VIC,
    CLIENT_APP,
    CLIENT_NONE,
};

class LogResult {
    public:
        LogResult();
        ~LogResult();

        template <typename T>
        void PutOut(enum ClientType cltype, enum CommandType cotype, std::string property, T value);
        template <typename T>
        void PutOut(enum ClientType cltype, enum CommandType cotype, std::string property, T value, struct timeval rcvtv);
    private:
        std::string MakeString(enum ClientType cltype, enum CommandType cotype, std::string property);
        void CurrentTime();

        struct timeval tv;
};

template <typename T>
void LogResult::PutOut(enum ClientType cltype, enum CommandType cotype, std::string property, T value) {
    std::cout << MakeString(cltype, cotype, property);
    if (typeid(T) == typeid(bool)) {
        if (value) {
            std::cout << "true";
        }
        else {
            std::cout << "false";
        }
    }
    else {
        std::cout << value;
    }
    std::cout << std::endl;
}

template <typename T>
void LogResult::PutOut(enum ClientType cltype, enum CommandType cotype, std::string property, T value, struct timeval rcvtv) {
    std::cout << MakeString(cltype, cotype, property);
    if (typeid(T) == typeid(bool)) {
        if (value) {
            std::cout << "true ";
        }
        else {
            std::cout << "false ";
        }
    }
    else {
        std::cout << value << " ";
    }
    std::cout << rcvtv.tv_sec << "." << std::setfill('0') << std::setw(6) << std::right << tv.tv_usec << std::endl;
}

