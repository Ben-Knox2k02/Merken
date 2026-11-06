#ifndef LOOPBACK_RECEIVER_H
#define LOOPBACK_RECEIVER_H

#include <string>

//listen on 127.0.0.1 for the redirect from Google
class ILoopbackReceiver{
    public:
        virtual ~ILoopbackReceiver() = default;

        //start listening on a free port and return the port or default to 0
        virtual int Start() = 0;
        
        virtual std::string WaitForRedirect(int timeoutSeconds) = 0;
        virtual void Stop() = 0;

};

#endif