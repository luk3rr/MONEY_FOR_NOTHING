/*
 * Filename: credit_card_manager_test.cc
 * Created on: June 12, 2024
 * Author: Lucas Araújo <araujolucas@dcc.ufmg.br>
 */

#include <gtest/gtest.h>
#include <stdatomic.h>

#include "credit_card_manager.h"

class CreditCardManagerTest : public ::testing::Test
{
    protected:
        CreditCardManager* m_creditCardManager;
        DBManager&         m_dbManager;

        void SetUp() override
        {
            m_dbManager.ResetDatabase();
            m_creditCardManager = new CreditCardManager();
        }

        void TearDown() override
        {
            delete m_creditCardManager;
        }

        CreditCardManagerTest()
            : m_dbManager(DBManager::GetInstance())
        { }

        ~CreditCardManagerTest() { }
};

TEST_F(CreditCardManagerTest, AddCreditCardValid)
{
    CreditCardManager creditCardManager;
    bool              succAddCreditCard =
        creditCardManager.AddCreditCard("1234567890123456", 1, "John Doe", 1000);

    EXPECT_TRUE(succAddCreditCard);
}

TEST_F(CreditCardManagerTest, AddCreditCardInvalidNumber)
{
    CreditCardManager creditCardManager;
    bool              succAddCreditCard =
        creditCardManager.AddCreditCard("1234567890123456", 1, "John Doe", 1000);

    EXPECT_TRUE(succAddCreditCard);

    // Try to add the same credit card again
    bool succAddCreditCardAgain =
        creditCardManager.AddCreditCard("1234567890123456", 1, "John", 1001);

    EXPECT_FALSE(succAddCreditCardAgain);
}

TEST_F(CreditCardManagerTest, AddCreditCardInvalidDueDay)
{
    CreditCardManager creditCardManager;
    bool              succAddCreditCard =
        creditCardManager.AddCreditCard("1234567890123456", 0, "John Doe", 1000);

    EXPECT_FALSE(succAddCreditCard);

    bool succAddCreditCardAgain =
        creditCardManager.AddCreditCard("00031123213123", 29, "John Does", 1001);

    EXPECT_FALSE(succAddCreditCardAgain);
}

TEST_F(CreditCardManagerTest, AddCreditCardInvalidMaxDebt)
{
    CreditCardManager creditCardManager;
    bool              succAddCreditCard =
        creditCardManager.AddCreditCard("1234567890123456", 10, "John Doe", 0);

    EXPECT_FALSE(succAddCreditCard);

    bool succAddCreditCardAgain =
        creditCardManager.AddCreditCard("00031123213123", 10, "John Does", -1);

    EXPECT_FALSE(succAddCreditCardAgain);
}

TEST_F(CreditCardManagerTest, GetCreditCardInfo)
{
    // Add a credit
    bool succAddCreditCard =
        m_creditCardManager->AddCreditCard("1234567890123456", 1, "John Doe", 1000);

    ASSERT_TRUE(succAddCreditCard);

    // Check if the credit card info can be retrieved correctly
    std::string cardName;
    double_t    maxDebt;
    uint16_t    billingDueDay;
    bool        succGetCreditCardInfo =
        m_creditCardManager->GetCreditCardInfo("1234567890123456",
                                               cardName,
                                               maxDebt,
                                               billingDueDay);

    ASSERT_TRUE(succGetCreditCardInfo);

    EXPECT_EQ(cardName, "John Doe");
    EXPECT_EQ(maxDebt, 1000);
    EXPECT_EQ(billingDueDay, 1);
}

TEST_F(CreditCardManagerTest, GetCreditCardInfoInvalid)
{
    // Try to retrieve the info of a non-existent credit card
    std::string cardName;
    double_t    maxDebt;
    uint16_t    billingDueDay;
    bool        succGetCreditCardInfo =
        m_creditCardManager->GetCreditCardInfo("1234567890123456",
                                               cardName,
                                               maxDebt,
                                               billingDueDay);

    EXPECT_FALSE(succGetCreditCardInfo);
}

TEST_F(CreditCardManagerTest, GetCreditCards)
{
    // Add some credit cards
    m_creditCardManager->AddCreditCard("1111222233334444", 1, "Card 1", 500);
    m_creditCardManager->AddCreditCard("2222333344445555", 15, "Card 2", 1000);
    m_creditCardManager->AddCreditCard("3333444455556666", 5, "Card 3", 1500);

    // Retrieve the credit cards
    std::vector<std::string> creditCards;
    m_creditCardManager->GetCreditCards(creditCards);

    // Check if all credit cards are retrieved correctly
    ASSERT_EQ(creditCards.size(), 3);
    EXPECT_TRUE(std::find(creditCards.begin(), creditCards.end(), "1111222233334444") !=
                creditCards.end());
    EXPECT_TRUE(std::find(creditCards.begin(), creditCards.end(), "2222333344445555") !=
                creditCards.end());
    EXPECT_TRUE(std::find(creditCards.begin(), creditCards.end(), "3333444455556666") !=
                creditCards.end());
}

TEST_F(CreditCardManagerTest, AddDebt)
{
    // Add a credit card first
    bool succAddCreditCard =
        m_creditCardManager->AddCreditCard("1234567890123456", 1, "John Doe", 1000);

    ASSERT_TRUE(succAddCreditCard);

    // Add a debt to the credit card
    bool succAddDebt = m_creditCardManager->AddDebt("1234567890123456",
                                                    "Groceries",
                                                    "2024/06/12",
                                                    150,
                                                    "Groceries shopping",
                                                    1);

    // TODO: Criar função para verificar se a dívida foi adicionada corretamente
    EXPECT_TRUE(succAddDebt);
}

TEST_F(CreditCardManagerTest, AddDebtWithNoCreditCard)
{
    // Try to add a debt without a credit card
    bool succAddDebt = m_creditCardManager->AddDebt("1234567890123456",
                                                    "Groceries",
                                                    "2024/06/12",
                                                    150,
                                                    "Groceries shopping",
                                                    1);

    EXPECT_FALSE(succAddDebt);
}

TEST_F(CreditCardManagerTest, AddDebtInvalidAmount)
{
    // Add a credit card first
    bool succAddCreditCard =
        m_creditCardManager->AddCreditCard("1234567890123456", 1, "John Doe", 1000);

    ASSERT_TRUE(succAddCreditCard);

    // Try to add a debt with an invalid amount
    bool succAddDebt = m_creditCardManager->AddDebt("1234567890123456",
                                                    "Groceries",
                                                    "2024/06/12",
                                                    0,
                                                    "Groceries shopping",
                                                    1);

    EXPECT_FALSE(succAddDebt);

    bool succAddDebtAgain = m_creditCardManager->AddDebt("1234567890123456",
                                                         "Groceries",
                                                         "2024/06/12",
                                                         -1,
                                                         "Groceries shopping",
                                                         1);

    EXPECT_FALSE(succAddDebtAgain);
}

TEST_F(CreditCardManagerTest, AddDebtWithInstallments)
{
    // Add a credit card first
    bool succAddCreditCard =
        m_creditCardManager->AddCreditCard("1234567890123456", 1, "John Doe", 1000);

    ASSERT_TRUE(succAddCreditCard);

    // Add a debt to the credit card
    bool succAddDebt = m_creditCardManager->AddDebt("1234567890123456",
                                                    "Groceries",
                                                    "2024/06/12",
                                                    150,
                                                    "Groceries shopping",
                                                    7);

    ASSERT_TRUE(succAddDebt);

    // TODO: Criar função para verificar se a dívida foi adicionada corretamente
}

TEST_F(CreditCardManagerTest, AddDebtWithInstallmentsInvalidInstallments)
{
    // Add a credit card first
    bool succAddCreditCard =
        m_creditCardManager->AddCreditCard("1234567890123456", 1, "John Doe", 1000);

    ASSERT_TRUE(succAddCreditCard);

    // Try to add a debt with an invalid amount
    bool succAddDebt = m_creditCardManager->AddDebt("1234567890123456",
                                                    "Groceries",
                                                    "2024/06/12",
                                                    150,
                                                    "Groceries shopping",
                                                    0);

    EXPECT_FALSE(succAddDebt);
}

TEST_F(CreditCardManagerTest, AddDebtWithInstallmentsInvalidAmount)
{
    // Add a credit card first
    bool succAddCreditCard =
        m_creditCardManager->AddCreditCard("1234567890123456", 1, "John Doe", 1000);

    ASSERT_TRUE(succAddCreditCard);

    // Try to add a debt with an invalid amount
    bool succAddDebt = m_creditCardManager->AddDebt("1234567890123456",
                                                    "Groceries",
                                                    "2024/06/12",
                                                    0,
                                                    "Groceries shopping",
                                                    7);

    EXPECT_FALSE(succAddDebt);

    bool succAddDebtAgain = m_creditCardManager->AddDebt("1234567890123456",
                                                         "Groceries",
                                                         "2024/06/12",
                                                         -1,
                                                         "Groceries shopping",
                                                         7);

    EXPECT_FALSE(succAddDebtAgain);
}
