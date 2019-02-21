#include "../../src/fpSingleton/fpInfo.h"

using namespace fp;

TEST(fpInfoClass, setParamMtry)
{
    fpInfo info;

    for (int i = 0; i < 100; ++i)
    {
        info.setParameter("mtry", i);
        EXPECT_EQ(info.returnMtry(), i);
    }
}

TEST(fpInfoClass, setParamNumFeatures)
{
    fpInfo info;

    for (int i = 0; i < 100; ++i)
    {
        info.setNumFeatures(i);
        EXPECT_EQ(info.returnNumFeatures(), i);
    }
}

TEST(fpInfoClass, setMtry)
{
    fpInfo info;

    int features = 100;
    int mtry = 20;

    info.setNumFeatures(features);
    info.setParameter("mtry", mtry);
    info.setMTRY(); // this shouldn't do anything to mtry since we set it
    EXPECT_EQ(info.returnMtry(), mtry);

    fpInfo info2;
    info2.setNumFeatures(features);
    info2.setMTRY(); // this should set it to sqrt(features)
    EXPECT_EQ(info2.returnMtry(), 10);

    fpInfo info3;
    info3.setNumFeatures(features);
    info3.setParameter("fractionOfFeaturesToTest", .4);
    info3.setMTRY(); // this should set it to .4*features
    EXPECT_EQ(info3.returnMtry(), 40);
}