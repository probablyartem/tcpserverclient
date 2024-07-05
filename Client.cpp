#include <iostream>
#include <boost/asio.hpp>

#include "Common.hpp"
#include "json.hpp"

using boost::asio::ip::tcp;

// Отправка сообщения на сервер по шаблону.
void SendMessage(
    tcp::socket& aSocket,
    const std::string& aId,
    const std::string& aRequestType,
    const nlohmann::json& aMessage)
{
    nlohmann::json req;
    req["UserId"] = aId;
    req["ReqType"] = aRequestType;
    req["Message"] = aMessage; 


    std::string request = req.dump();
    boost::asio::write(aSocket, boost::asio::buffer(request, request.size()));
}

// Возвращает строку с ответом сервера на последний запрос.
std::string ReadMessage(tcp::socket& aSocket)
{
    boost::asio::streambuf b;
    boost::asio::read_until(aSocket, b, "\0");
    std::istream is(&b);
    std::string line(std::istreambuf_iterator<char>(is), {});
    return line;
}

// "Создаём" пользователя, получаем его ID.
std::string ProcessRegistration(tcp::socket& aSocket)
{
    std::string name;
    std::cout << "Hello! Enter your name: ";
    std::cin >> name;

    // Для регистрации Id не нужен, заполним его нулём
    SendMessage(aSocket, "0", Requests::Registration, name);
    return ReadMessage(aSocket);
}

void SendBuyRequest(tcp::socket& aSocket, const std::string& userId)
{
    int amount;
    double rate;
    std::cout << "Enter amount of USD to buy: ";
    std::cin >> amount;
    std::cout << "Enter rate (RUB per 1 USD): ";
    std::cin >> rate;

    nlohmann::json message;
    message["Amount"] = amount;
    message["Rate"] = rate;

    SendMessage(aSocket, userId, "Buy", message);
    std::cout << ReadMessage(aSocket);
}

void SendSellRequest(tcp::socket& aSocket, const std::string& userId)
{
    int amount;
    double rate;
    std::cout << "Enter amount of USD to sell: ";
    std::cin >> amount;
    std::cout << "Enter rate (RUB per 1 USD): ";
    std::cin >> rate;

    nlohmann::json message;
    message["Amount"] = amount;
    message["Rate"] = rate;

    SendMessage(aSocket, userId, "Sell", message);
    std::cout << ReadMessage(aSocket);
}

void ProcessRequests(tcp::socket& aSocket)
{
    SendMessage(aSocket, "0", "Process", "");
    std::cout << ReadMessage(aSocket);
}

void ShowBalances(tcp::socket& aSocket)
{
    SendMessage(aSocket, "0", "Balances", "");
    std::cout << ReadMessage(aSocket);
}

void ShowUsers(tcp::socket& aSocket)
{
    SendMessage(aSocket, "0", "Users", "");
    std::cout << ReadMessage(aSocket);
}



int main()
{
    const int port = 12345; 
    try
    {
        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(tcp::v4(), "127.0.0.1", std::to_string(port));
        tcp::resolver::iterator iterator = resolver.resolve(query);

        tcp::socket s(io_service);
        s.connect(*iterator);

        // Мы предполагаем, что для идентификации пользователя будет использоваться ID.
        // Тут мы "регистрируем" пользователя - отправляем на сервер имя, а сервер возвращает нам ID.
        // Этот ID далее используется при отправке запросов.
        std::string my_id = ProcessRegistration(s);

        while (true)
        {
            // Тут реализовано "бесконечное" меню.
            std::cout << "Menu:\n"
                         "1) Hello Request\n"
                         "2) Buy USD\n"
                         "3) Sell USD\n"
                         "4) Process Requests\n"
                         "5) Show Balances\n"
                         "6) Show Users\n"
                         "7) Exit\n"
                         << std::endl;

            short menu_option_num;
            std::cin >> menu_option_num;
            switch (menu_option_num)
            {
                case 1:
                {
                    // Для примера того, как может выглядить взаимодействие с сервером
                    // реализован один единственный метод - Hello.
                    // Этот метод получает от сервера приветствие с именем клиента,
                    // отправляя серверу id, полученный при регистрации.
                    SendMessage(s, my_id, Requests::Hello, "");
                    std::cout << ReadMessage(s);
                    break;
                }
                case 2:
                {
                    SendBuyRequest(s, my_id);
                    break;
                }
                case 3:
                {
                    SendSellRequest(s, my_id);
                    break;
                }
                case 4:
                {
                    ProcessRequests(s);
                    break;
                }
                case 5:
                {
                    ShowBalances(s);
                    break;
                }
                case 6:
                {
                    ShowUsers(s);
                    break;
                }
                case 7:
                {
                    exit(0);
                    break;
                }
                default:
                {
                    std::cout << "Unknown menu option\n" << std::endl;
                }
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}   