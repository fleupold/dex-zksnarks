#include <cassert>
#include <gtest/gtest.h>
#include "declarations.h"

struct Private {};
#define ORDERS 2
#define TOKENS 4
#define ACCOUNTS 4

#include <apps/parsing.h>
#include "util.h"

TEST(ParsingTest, ParseAccount) {
    bool bits[BITS_PER_ACCOUNT] = { 0 };
    auto account = parseAccount(bits);
    ASSERT_EQ(account, 0);

    bits[BITS_PER_ACCOUNT-1] = 1;
    account = parseAccount(bits);
    ASSERT_EQ(account, 1);

    for (size_t i = 0; i < BITS_PER_ACCOUNT; i++) {
        bits[i] = 1;
    }
    account = parseAccount(bits);
    ASSERT_EQ(account, ACCOUNTS - 1);
}

TEST(ParsingTest, ParseToken) {
    bool bits[BITS_PER_TOKEN] = { 0 };
    auto token = parseToken(bits);
    ASSERT_EQ(token, 0);

    bits[BITS_PER_TOKEN-1] = 1;
    token = parseToken(bits);
    ASSERT_EQ(token, 1);

    for (size_t i = 0; i < BITS_PER_TOKEN; i++) {
        bits[i] = 1;
    }
    token = parseToken(bits);
    ASSERT_EQ(token, ACCOUNTS - 1);
}

TEST(ParsingTest, ParseDecimal) {
    bool bits[BITS_PER_DECIMAL] = { 0 };
    auto decimal = parseDecimal(bits);
    ASSERT_EQ(decimal, 0);

    bits[BITS_PER_DECIMAL-1] = 1;
    decimal = parseDecimal(bits);
    ASSERT_EQ(decimal, 1);
}

TEST(ParsingTest, ParseOrders) {
    bool bits[BITS_PER_ORDER*ORDERS] = { 0 };
    
    /**
     * First order: {
     *   account: 1,
     *   sellToken: 1
     *   buyToken: 2
     *   limitPrice: 4
     *   sellAmount: 8
     * }
     */
    bits[BITS_PER_ACCOUNT-1] = 1;
    bits[BITS_PER_ACCOUNT + BITS_PER_TOKEN - 1] = 1;
    bits[BITS_PER_ACCOUNT + (2*BITS_PER_TOKEN) - 2] = 1;
    bits[BITS_PER_ACCOUNT + (2*BITS_PER_TOKEN) + BITS_PER_DECIMAL - 3] = 1;
    bits[BITS_PER_ACCOUNT + (2*BITS_PER_TOKEN) + (2*BITS_PER_DECIMAL) - 4] = 1;

    /**
     * Second: {
     *   account: 2,
     *   sellToken: 2
     *   buyToken: 1
     *   limitPrice: 16
     *   sellAmount: 32
     * }
     */
    bits[BITS_PER_ORDER + BITS_PER_ACCOUNT - 2] = 1;
    bits[BITS_PER_ORDER + BITS_PER_ACCOUNT + BITS_PER_TOKEN - 2] = 1;
    bits[BITS_PER_ORDER + BITS_PER_ACCOUNT + (2*BITS_PER_TOKEN) - 1] = 1;
    bits[BITS_PER_ORDER + BITS_PER_ACCOUNT + (2*BITS_PER_TOKEN) + BITS_PER_DECIMAL - 5] = 1;
    bits[BITS_PER_ORDER + BITS_PER_ACCOUNT + (2*BITS_PER_TOKEN) + (2*BITS_PER_DECIMAL) - 6] = 1;

    struct Order result[ORDERS];
    parseOrders(bits, result);

    ASSERT_EQ(result[1].account, 2);
    ASSERT_EQ(result[1].sellToken, 2);
    ASSERT_EQ(result[1].buyToken, 1);
    ASSERT_EQ(result[1].limitPrice, 16);
    ASSERT_EQ(result[1].sellAmount, 32);
}

TEST(ParsingTest, ParseBalances) {
    bool bits[ACCOUNTS*TOKENS*BITS_PER_DECIMAL] = { 0 };
    
    // Each account will have balance in a single token 
    // (same ID as their account)
    bits[BITS_PER_DECIMAL - 1] = 1; //acc1
    bits[(TOKENS*BITS_PER_DECIMAL) + (2*BITS_PER_DECIMAL) - 1] = 1; //acc2
    bits[(2*TOKENS*BITS_PER_DECIMAL) + (3*BITS_PER_DECIMAL) - 1] = 1; //acc3
    bits[(3*TOKENS*BITS_PER_DECIMAL) + (4*BITS_PER_DECIMAL) - 1] = 1; //acc4

    struct Balance balances[ACCOUNTS] = { 0 };
    parseBalances(bits, balances);

    ASSERT_EQ(balances[0].token[0], 1);
    ASSERT_EQ(balances[0].token[1], 0);
    ASSERT_EQ(balances[0].token[2], 0);
    ASSERT_EQ(balances[0].token[3], 0);

    ASSERT_EQ(balances[1].token[0], 0);
    ASSERT_EQ(balances[1].token[1], 1);
    ASSERT_EQ(balances[1].token[2], 0);
    ASSERT_EQ(balances[1].token[3], 0);

    ASSERT_EQ(balances[2].token[0], 0);
    ASSERT_EQ(balances[2].token[1], 0);
    ASSERT_EQ(balances[2].token[2], 1);
    ASSERT_EQ(balances[2].token[3], 0);

    ASSERT_EQ(balances[3].token[0], 0);
    ASSERT_EQ(balances[3].token[1], 0);
    ASSERT_EQ(balances[3].token[2], 0);
    ASSERT_EQ(balances[3].token[3], 1);
}

TEST(ParsingTest, SerializeBalances) {
    bool original[ACCOUNTS*TOKENS*BITS_PER_DECIMAL] = { 0 };
    original[BITS_PER_DECIMAL - 1] = 1;
    original[(TOKENS*BITS_PER_DECIMAL) + (2*BITS_PER_DECIMAL) - 1] = 1;
    original[(2*TOKENS*BITS_PER_DECIMAL) + (3*BITS_PER_DECIMAL) - 1] = 1;
    original[(3*TOKENS*BITS_PER_DECIMAL) + (4*BITS_PER_DECIMAL) - 1] = 1;

    struct Balance balances[ACCOUNTS] = { 0 };
    parseBalances(original, balances);

    // serialize back and expect to match original
    bool serialized[ACCOUNTS*TOKENS*BITS_PER_DECIMAL] = { 0 };
    serializeBalances(balances, serialized);

    ASSERT_EQ(
        std::vector<bool>(original, original+(ACCOUNTS*TOKENS*BITS_PER_DECIMAL)),
        std::vector<bool>(serialized, serialized+(ACCOUNTS*TOKENS*BITS_PER_DECIMAL))
    );
}

int main(int argc, char **argv) {
    testing::FLAGS_gtest_death_test_style = "threadsafe";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}