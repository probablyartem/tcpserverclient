#include "Core.hpp"
#include <algorithm>
#include <stdexcept>
#include <iostream>


std::string Core::RegisterNewUser(const std::string& aUserName)
{
    size_t newUserId = mUsers.size();
    mUsers[newUserId] = aUserName;
    mBalances[newUserId] = {0, 0}; // {USD, RUB}
    return std::to_string(newUserId);
}

std::string Core::GetUserName(const std::string& aUserId) const
{
    const auto userIt = mUsers.find(std::stoi(aUserId));
    if (userIt == mUsers.cend())
    {
        return "Error! Unknown User";
    }
    else
    {
        return userIt->second;
    }
}

void Core::AddRequest(const Request& request)
{
    mRequests.push_back(request);
}

void Core::ProcessRequests()
{
    std::vector<Request> buyRequests;
    std::vector<Request> sellRequests;

    for (const auto& req : mRequests)
    {
        if (req.isBuy)
        {
            buyRequests.push_back(req);
        }
        else
        {
            sellRequests.push_back(req);
        }
    }

    std::sort(buyRequests.begin(), buyRequests.end(), [](const Request& a, const Request& b) {
        return a.rate > b.rate;
    });

    std::sort(sellRequests.begin(), sellRequests.end(), [](const Request& a, const Request& b) {
        return a.rate < b.rate;
    });

    for (auto& buyReq : buyRequests)
    {
        for (auto& sellReq : sellRequests)
        {
            if (buyReq.rate >= sellReq.rate && buyReq.amount > 0 && sellReq.amount > 0)
            {
                int tradedAmount = std::min(buyReq.amount, sellReq.amount);
                double tradeRate = (buyReq.timestamp < sellReq.timestamp) ? buyReq.rate : sellReq.rate;
                
                if (buyReq.userId.empty() || !isNumeric(buyReq.userId))
                {
                    std::cerr << "Invalid userId for buyReq: " << buyReq.userId << std::endl;
                    continue; 
                }

                if (sellReq.userId.empty() || !isNumeric(sellReq.userId))
                {
                    std::cerr << "Invalid userId for sellReq: " << sellReq.userId << std::endl;
                    continue;
                     }

                try {
                    int buyUserId = std::stoi(buyReq.userId);
                    int sellUserId = std::stoi(sellReq.userId);

                    // Update balances
                    mBalances[buyUserId].first += tradedAmount; // Add USD
                    mBalances[buyUserId].second -= tradedAmount * tradeRate; // Subtract RUB
                    mBalances[sellUserId].first -= tradedAmount; // Subtract USD
                    mBalances[sellUserId].second += tradedAmount * tradeRate; // Add RUB
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Invalid user ID: " << e.what() << std::endl;
                } catch (const std::out_of_range& e) {
                    std::cerr << "User ID out of range: " << e.what() << std::endl;
                }

                // Reduce buy and sell amounts
                buyReq.amount -= tradedAmount;
                sellReq.amount -= tradedAmount;
            }
        }
    }

    mRequests.clear();
}

std::string Core::GetBalances() const
{
    std::string result;
    for (const auto& user : mUsers)
    {
        result += user.second + " " + std::to_string(mBalances.at(user.first).first) + " USD " +
                  std::to_string(mBalances.at(user.first).second) + " RUB\n";
    }
    return result;
}

std::vector<std::string> Core::GetAllUsers() const
{
    std::vector<std::string> users;
    for (const auto& user : mUsers)
    {
        users.push_back(user.second);
    }
    return users;
}

const std::vector<Request>& Core::GetPendingRequests() const
{
    return mRequests;
}

bool Core::isNumeric(const std::string& str) const
{
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

Core& GetCore()
{
    static Core core;
    return core;
}