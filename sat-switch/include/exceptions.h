//
// Created by 玥辰 on 2021/10/2.
//

#ifndef INV_SEARCH_EXCEPTIONS_H
#define INV_SEARCH_EXCEPTIONS_H

#include <exception>
#include <string>

class SatException : public std::exception {
protected:
    std::string msg;
public:
    explicit SatException(std::string message) : msg(std::move(message)) {};
};

#endif //INV_SEARCH_EXCEPTIONS_H
