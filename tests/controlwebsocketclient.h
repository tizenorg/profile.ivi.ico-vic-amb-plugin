#ifndef CONTROLWEBSOCKETCLIENT_H_
#define CONTROLWEBSOCKETCLIENT_H_
#include <cstring>
#include <ctime>

#include <ico_uws.h>

class ControlWebsocketClient {
    public:
        ControlWebsocketClient();
        ~ControlWebsocketClient();
        bool initialize(const char *uri, const int port, const char *protocol);
        void terminate();
        virtual bool send(char *sendbuf, const size_t len);
        virtual bool recv(char *recvbuf, const size_t len);
        int getsocketid() const;
        void setsocketid(int fd);
        void setwsid(void *id);
        void service();

        static void callbackfunc(const struct ico_uws_context *context, const ico_uws_evt_e event, const void *id, const ico_uws_detail *detail, void *user_data);
    protected:
        struct ico_uws_context *context_;
        void* wsid_;
        int fd_;
};
#endif // CONTROLWEBSOCKETCLIENT_H_
