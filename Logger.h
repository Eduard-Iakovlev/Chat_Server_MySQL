# pragma once
#include <string>

class Logger{
    public:
    Logger(std::string mess);

    ~Logger();
    private:
    std::string _mess;
};