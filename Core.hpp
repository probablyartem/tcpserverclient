#pragma once

#include <string>
#include <map>
#include <vector>
#include <utility>

struct Request {
    std::string userId;
    bool isBuy;
    int amount;
    double rate;
    unsigned long timestamp;
};

class Core
{
public:
    std::string RegisterNewUser(const std::string& aUserName);
    std::string GetUserName(const std::string& aUserId) const;
    void AddRequest(const Request& request);
    void ProcessRequests();
    std::string GetBalances() const;
    std::vector<std::string> GetAllUsers() const;
    const std::vector<Request>& GetPendingRequests() const;

private:
    std::map<size_t, std::string> mUsers;
    std::map<size_t, std::pair<int, double>> mBalances; // <UserId, {USD, RUB}>
    std::vector<Request> mRequests;

    bool isNumeric(const std::string& str) const;
};

Core& GetCore();