#include <gtest/gtest.h>
#include "../Core.hpp"

class TradingTest : public ::testing::Test {
protected:
    Core core;

    void SetUp() override {
        // Initialization before each test
        core = Core(); // Create a new instance of Core
    }

    void TearDown() override {
        // Cleanup after each test
    }
};

TEST_F(TradingTest, ProcessRequestWithoutTrade) {
    std::string userId = core.RegisterNewUser("TestUser");
    ASSERT_FALSE(userId.empty());  // Ensure userId is valid
    std::string userName = core.GetUserName(userId);
    ASSERT_EQ(userName, "TestUser");  // Ensure the user name is correct

    Request buyRequest;
    buyRequest.userId = userId;
    buyRequest.isBuy = true;
    buyRequest.amount = 100;
    buyRequest.rate = 70.0;
    buyRequest.timestamp = std::time(nullptr);

    core.AddRequest(buyRequest);
    core.ProcessRequests();

    std::string balances = core.GetBalances();
    std::cout << "Actual balances: " << balances << std::endl;
    EXPECT_TRUE(balances.find("TestUser 0 USD 0.000000 RUB") != std::string::npos);

    const auto& requests = core.GetPendingRequests();
    EXPECT_EQ(requests.size(), 0);
};

TEST_F(TradingTest, ProcessMatchingRequests) {
    std::string buyerId = core.RegisterNewUser("Buyer");
    std::string sellerId = core.RegisterNewUser("Seller");
    std::string userName1 = core.GetUserName(buyerId);
    std::string userName2 = core.GetUserName(sellerId);

    Request buyRequest;
    buyRequest.userId = buyerId;
    buyRequest.isBuy = true;
    buyRequest.amount = 100;
    buyRequest.rate = 70.0;
    buyRequest.timestamp = std::time(nullptr);

    Request sellRequest;
    sellRequest.userId = sellerId;
    sellRequest.isBuy = false;
    sellRequest.amount = 100;
    sellRequest.rate = 70.0;
    sellRequest.timestamp = std::time(nullptr) + 1;  // Чуть позже, чем buyRequest

    core.AddRequest(buyRequest);
    core.AddRequest(sellRequest);
    core.ProcessRequests();   
    const auto& requests = core.GetPendingRequests();
    EXPECT_EQ(requests.size(), 0);  // Все заявки должны быть обработаны
};

TEST_F(TradingTest, ProcessPartialMatchingRequests) {
    std::string buyerId = core.RegisterNewUser("Buyer");
    std::string sellerId = core.RegisterNewUser("Seller");
    std::string sellerId2 = core.RegisterNewUser("Seller2");
    std::string userName3 = core.GetUserName(buyerId);
    std::string userName4 = core.GetUserName(sellerId);
    std::string userName5 = core.GetUserName(sellerId2);

    Request buyRequest;
    buyRequest.userId = buyerId;
    buyRequest.isBuy = true;
    buyRequest.amount = 100;
    buyRequest.rate = 70.0;
    buyRequest.timestamp = std::time(nullptr);

    Request sellRequest;
    sellRequest.userId = sellerId;
    sellRequest.isBuy = false;
    sellRequest.amount = 50;  
    sellRequest.rate = 70.0;
    sellRequest.timestamp = std::time(nullptr) + 1;


    Request sellRequest2;
    sellRequest2.userId = sellerId2;
    sellRequest2.isBuy = false;
    sellRequest2.amount = 50;  
    sellRequest2.rate = 70.0;
    sellRequest2.timestamp = std::time(nullptr) + 2;

    core.AddRequest(buyRequest);
    core.AddRequest(sellRequest);
    core.AddRequest(sellRequest2);
    core.ProcessRequests();

    std::string balances = core.GetBalances();
    std::cout << "Actual balances: " << balances << std::endl;
    EXPECT_TRUE(balances.find("Buyer 100 USD -7000.000000 RUB") != std::string::npos);
    EXPECT_TRUE(balances.find("Seller -50 USD 3500.000000 RUB") != std::string::npos);
    EXPECT_TRUE(balances.find("Seller2 -50 USD 3500.000000 RUB") != std::string::npos);

    const auto& requests = core.GetPendingRequests();
    EXPECT_EQ(requests.size(), 0);  // Все заявки должны быть обработаны, даже частично
}
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}