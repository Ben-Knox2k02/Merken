#ifndef CLOCK_SERVICE_H
#define CLOCK_SERVICE_H


class IClockService{
    public:
        virtual ~IClockService() = default;

        //current time in Unix seconds
        virtual long long NowSeconds() = 0;
};

#endif