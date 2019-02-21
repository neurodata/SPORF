// This tests whether the seed works in single/parallel execution

#include "../../src/baseFunctions/MWC.h"

TEST(setSeed, seed_low)
{
    int seed = 123;
    int curRandNum;
    int prevRandNum;
    randomNumberRerFMWC randNum;
    for (int i = 0; i < 1000; ++i)
    {
        randNum.initialize(seed);
        if (i > 0)
        {
            EXPECT_EQ(curRandNum, prevRandNum);
        }
        curRandNum = randNum.gen(1000);
        prevRandNum = curRandNum;
    }
}

TEST(setSeed, random_vector)
{
    int seed = 123;
    std::vector<int> seq1;

    randomNumberRerFMWC randNum1;
    randNum1.initialize(seed);
    for (int i = 0; i < 1000; ++i)
    {
        seq1.push_back(randNum1.gen());
    }

    std::vector<int> seq2;
    randomNumberRerFMWC randNum2;
    randNum2.initialize(seed);
    for (int i = 0; i < 1000; ++i)
    {
        seq2.push_back(randNum2.gen());
    }

    EXPECT_EQ(seq1, seq2);

    //use a diff. seed
    std::vector<int> seq3;
    randomNumberRerFMWC randNum3;
    randNum3.initialize(seed + 1);
    for (int i = 0; i < 1000; ++i)
    {
        seq3.push_back(randNum3.gen());
    }

    EXPECT_NE(seq1, seq3);
}

TEST(setSeed, seed_high)
{
    long seed = 2351351351351351;
    int curRandNum;
    int prevRandNum;
    randomNumberRerFMWC randNum;
    for (int i = 0; i < 1000; ++i)
    {
        randNum.initialize(seed);
        if (i > 0)
        {
            EXPECT_EQ(curRandNum, prevRandNum);
        }
        curRandNum = randNum.gen(1000);
        prevRandNum = curRandNum;
    }
}

TEST(setSeed, seed_neg)
{
    long seed = -1001;
    int curRandNum;
    int prevRandNum;
    randomNumberRerFMWC randNum;
    for (int i = 0; i < 1000; ++i)
    {
        randNum.initialize(seed);
        if (i > 0)
        {
            EXPECT_EQ(curRandNum, prevRandNum);
        }
        curRandNum = randNum.gen(1000);
        prevRandNum = curRandNum;
    }
}

//expect two seq values random generated from large range to be NE
TEST(setSeed, randomGenNE)
{
    int range = 10000;
    long seed = -1001;
    int prevRandNum;
    randomNumberRerFMWC randNum;
    randNum.initialize(seed);
    prevRandNum = randNum.gen(range);
    for (int i = 0; i < 1000; ++i)
    {
        EXPECT_NE(randNum.gen(range), prevRandNum);
    }
}