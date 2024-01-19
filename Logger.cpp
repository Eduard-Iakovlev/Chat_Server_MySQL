#include "Logger.h"
#include <iostream>
#include <thread>

Logger::Logger(std::string mess) : _mess(mess), _filename("log.txt"){

    _filestream.open(_filename, std::ios::app);
    if (!_filestream.is_open()) {
        std::ofstream newFileStream(_filename);
        newFileStream.close();
        _filestream.open(_filename, std::ios::app);
    }
}
Logger::~Logger(){
    _filestream.close();
}

void Logger::read_last_line(){
    std::cout << " запись в журнале: \n" << get_last_line() << std::endl;
}

void Logger::apped_to_log(){
    shared_mutex.lock();
    _filestream << _mess << std::endl;
    shared_mutex.unlock();
}

std::string Logger::get_last_line() {
    std::ifstream file(_filename);
    std::string last_line;

    if (file.is_open()) {
        std::string line;
        shared_mutex.lock_shared();
        while (std::getline(file, line)) {
            last_line = line;
        }
        shared_mutex.unlock_shared();
    }

    return last_line;
}
