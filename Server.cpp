#include <cstdlib>
#include <iostream>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include <vector>
#include "json.hpp"
#include "Common.hpp"
#include "Core.hpp"
using boost::asio::ip::tcp;

// struct Request {
//     std::string userId;
//     bool isBuy; // true для покупки, false для продажи
//     int amount;
//     double rate;
//     unsigned long timestamp;
// };


// class Core
// {
// public:
//     // "Регистрирует" нового пользователя и возвращает его ID.
//     std::string RegisterNewUser(const std::string& aUserName)
//     {
//         size_t newUserId = mUsers.size();
//         mUsers[newUserId] = aUserName;
//         mBalances[newUserId] = {0, 0}; // {USD, RUB}
//         return std::to_string(newUserId);
//     }

//     // Запрос имени клиента по ID
//     std::string GetUserName(const std::string& aUserId)
//     {
//         const auto userIt = mUsers.find(std::stoi(aUserId));
//         if (userIt == mUsers.cend())
//         {
//             return "Error! Unknown User";
//         }
//         else
//         {
//             return userIt->second;
//         }
//     }

// void AddRequest(const Request& request)
//     {
//         mRequests.push_back(request);
//     }

//     void ProcessRequests()
// {
//     std::vector<Request> buyRequests;
//     std::vector<Request> sellRequests;

//     // Отделим заявки на покупку и продажу
//     for (const auto& req : mRequests)
//     {
//         if (req.isBuy)
//         {
//             buyRequests.push_back(req);
//         }
//         else
//         {
//             sellRequests.push_back(req);
//         }
//     }

   
//     std::sort(buyRequests.begin(), buyRequests.end(), [](const Request& a, const Request& b) {
//         return a.rate > b.rate;
//     });

//     std::sort(sellRequests.begin(), sellRequests.end(), [](const Request& a, const Request& b) {
//         return a.rate < b.rate;
//     });


//     for (auto& buyReq : buyRequests)
//     {
//         for (auto& sellReq : sellRequests)
//         {
//             if (buyReq.rate >= sellReq.rate && buyReq.amount > 0 && sellReq.amount > 0)
//             {
//                 int tradedAmount = std::min(buyReq.amount, sellReq.amount);
//                 double tradeRate = (buyReq.timestamp < sellReq.timestamp) ? buyReq.rate : sellReq.rate;
//                 if (buyReq.userId.empty() || !isNumeric(buyReq.userId))
//                 {
//                     std::cerr << "Invalid userId for buyReq: " << buyReq.userId << std::endl;
//                     continue; 
//                 }

//                 // Validate sellReq.userId
//                 if (sellReq.userId.empty() || !isNumeric(sellReq.userId))
//                 {
//                     std::cerr << "Invalid userId for sellReq: " << sellReq.userId << std::endl;
//                     continue; 
//                 }

//                 try {
//                     int buyUserId = std::stoi(buyReq.userId);
//                     int sellUserId = std::stoi(sellReq.userId);

//                     // Update balances
//                     mBalances[buyUserId].first += tradedAmount; // Add USD
//                     mBalances[buyUserId].second -= tradedAmount * tradeRate; // Subtract RUB
//                     mBalances[sellUserId].first -= tradedAmount; // Subtract USD
//                     mBalances[sellUserId].second += tradedAmount * tradeRate; // Add RUB
//                 } catch (const std::invalid_argument& e) {
//                     std::cerr << "Invalid user ID: " << e.what() << std::endl;
//                 } catch (const std::out_of_range& e) {
//                     std::cerr << "User ID out of range: " << e.what() << std::endl;
//                 }

//                 // Reduce buy and sell amounts
//                 buyReq.amount -= tradedAmount;
//                 sellReq.amount -= tradedAmount;
//             }
//         }
//     }

//     mRequests.clear();
// }

// // Function to check if a string consists only of numeric characters
// bool isNumeric(const std::string& str)
// {
//     return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
// }
   
//     std::string GetBalances()
//     {
//         std::string result;
//         for (const auto& user : mUsers)
//         {
//             result += user.second + " " + std::to_string(mBalances[user.first].first) + " USD " +
//                       std::to_string(mBalances[user.first].second) + " RUB\n";
//         }
//         return result;
//     }

//     std::vector<std::string> GetAllUsers()
//     {
//         std::vector<std::string> users;
//         for (const auto& user : mUsers)
//         {
//             users.push_back(user.second);
//         }
//         return users;
//     }

// private:
//     std::map<size_t, std::string> mUsers;
//     std::map<size_t, std::pair<int, double>> mBalances; // <UserId, {USD, RUB}>
//     std::vector<Request> mRequests;
// };

// Core& GetCore()
// {
//     static Core core;
//     return core;
// }

class session
{
public:
    session(boost::asio::io_service& io_service)
        : socket_(io_service)
    {
    }

    tcp::socket& socket()
    {
        return socket_;
    }

    void start()
    {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            boost::bind(&session::handle_read, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

    // Обработка полученного сообщения.
    void handle_read(const boost::system::error_code& error,
        size_t bytes_transferred)
    {
        if (!error)
        {
            data_[bytes_transferred] = '\0';
            
            // Парсим json, который пришёл нам в сообщении.
            auto j = nlohmann::json::parse(data_);
            auto reqType = j["ReqType"];
            std::cout << j << std::endl;
            std::string reply = "Error! Unknown request type";
            if (reqType == Requests::Registration)
            {
                // Это реквест на регистрацию пользователя.
                // Добавляем нового пользователя и возвращаем его ID.
                reply = GetCore().RegisterNewUser(j["Message"]);
            }
            else if (reqType == Requests::Hello)
            {
                // Это реквест на приветствие.
                // Находим имя пользователя по ID и приветствуем его по имени.
                reply = "Hello, " + GetCore().GetUserName(j["UserId"]) + "!\n";
            }
            else if (reqType == Requests::Buy || reqType == Requests::Sell)
            {
                Request req;
                auto message = j["Message"];
                std::cout << "Type of Amount: " << message["Amount"].type_name() << std::endl;
                std::cout << "Type of Rate: " << message["Rate"].type_name() << std::endl;
                
                req.isBuy = (reqType == "Buy");
                req.userId = j["UserId"];
                req.amount = message["Amount"].get<int>();
                req.rate = message["Rate"].get<double>();

                GetCore().AddRequest(req);
                reply = "Request received\n";
            }
            else if (reqType == Requests::Process)
            {
                GetCore().ProcessRequests();
                reply = "Requests processed\n";
            }
            else if (reqType == Requests::Balances)
            {
                reply = GetCore().GetBalances();
            }
            else if (reqType == Requests::Users)
            {
                auto users = GetCore().GetAllUsers();
                reply = "Connected users:\n";
                for (const auto& user : users)
                {
                    reply += user + "\n";
                }
            }

            boost::asio::async_write(socket_,
                boost::asio::buffer(reply, reply.size()),
                boost::bind(&session::handle_write, this,
                    boost::asio::placeholders::error));
        }
        else
        {
            delete this;
        }
    }

    void handle_write(const boost::system::error_code& error)
    {
        if (!error)
        {
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                boost::bind(&session::handle_read, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            delete this;
        }
    }

private:
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class server
{
public:
    server(boost::asio::io_service& io_service)
        : io_service_(io_service),
        acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
    {
        std::cout << "Server started! Listen " << port << " port" << std::endl;
        
        start_accept();
    }

    void start_accept() {
        session* new_session = new session(io_service_);
        acceptor_.async_accept(new_session->socket(),
            boost::bind(&server::handle_accept, this, new_session,
                boost::asio::placeholders::error));
    }
        

    void handle_accept(session* new_session,
        const boost::system::error_code& error)
    {
        if (!error)
        {
            new_session->start();
            new_session = new session(io_service_);
            acceptor_.async_accept(new_session->socket(),
                boost::bind(&server::handle_accept, this, new_session,
                    boost::asio::placeholders::error));
        }
        else
        {
            delete new_session;
        }
    }

private:
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
    static const int port = 12345; 
};

int main()
{
    try
    {
        boost::asio::io_service io_service;
        static Core core;

        server s(io_service);

        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}