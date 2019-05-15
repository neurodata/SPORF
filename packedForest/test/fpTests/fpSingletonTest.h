#include "../../src/fpSingleton/fpSingleton.h"

using namespace fp;

TEST(fpSingleton, checkDataDependantParameters_errorPatchHeightMax)
{
	std::string p1 = "CSVFileName";
	std::string p2 = "../res/mnist.csv";
	fpSingleton::getSingleton().setParameter(p1, p2);
	fpSingleton::getSingleton().setParameter("mtry", 10);
	fpSingleton::getSingleton().setParameter("methodToUse", 2);
	fpSingleton::getSingleton().setParameter("imageHeight", 28);
	fpSingleton::getSingleton().setParameter("imageWidth", 28);
	fpSingleton::getSingleton().setParameter("patchHeightMax", 45);
	fpSingleton::getSingleton().setParameter("patchHeightMin", 1);
	fpSingleton::getSingleton().setParameter("patchWidthMax", 28);
	fpSingleton::getSingleton().setParameter("patchWidthMin", 1);
	fpSingleton::getSingleton().loadData();

	ASSERT_THROW(fpSingleton::getSingleton().checkDataDependentParameters(), std::runtime_error);
}


TEST(fpSingleton, checkDataDependantParameters_errorPatchHeightMin)
{
	fpSingleton::getSingleton().setParameter("patchHeightMax", 28);
	fpSingleton::getSingleton().setParameter("patchHeightMin", -10);
	fpSingleton::getSingleton().setParameter("patchWidthMax", 28);
	fpSingleton::getSingleton().setParameter("patchWidthMin", 1);

	ASSERT_THROW(fpSingleton::getSingleton().checkDataDependentParameters(), std::runtime_error);
}


TEST(fpSingleton, checkDataDependantParameters_errorPatchWidthMax)
{
	fpSingleton::getSingleton().setParameter("patchHeightMax", 28);
	fpSingleton::getSingleton().setParameter("patchHeightMin", 1);
	fpSingleton::getSingleton().setParameter("patchWidthMax", 45);
	fpSingleton::getSingleton().setParameter("patchWidthMin", 1);

	ASSERT_THROW(fpSingleton::getSingleton().checkDataDependentParameters(), std::runtime_error);
}


TEST(fpSingleton, checkDataDependantParameters_errorPatchWidthMin)
{
	fpSingleton::getSingleton().setParameter("patchHeightMax", 28);
	fpSingleton::getSingleton().setParameter("patchHeightMin", 1);
	fpSingleton::getSingleton().setParameter("patchWidthMax", 28);
	fpSingleton::getSingleton().setParameter("patchWidthMin", -10);

	ASSERT_THROW(fpSingleton::getSingleton().checkDataDependentParameters(), std::runtime_error);
}
