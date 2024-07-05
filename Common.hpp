#ifndef CLIENSERVERECN_COMMON_HPP
#define CLIENSERVERECN_COMMON_HPP

#include <string>

static short port = 12345;

namespace Requests
{
    static std::string Registration = "Reg";
    static std::string Hello = "Hello";
    static std::string Buy = "Buy";
    static std::string Sell = "Sell";
    static std::string Process = "Process";  
    static std::string Balances = "Balances";
    static std::string Users = "Users";
}

#endif //CLIENSERVERECN_COMMON_HPP
