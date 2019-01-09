#include "pch.h"

#include "../CVSystem/Common/FileHelper.h"

using namespace dxlib;

TEST(FileHelper, getImagePairs)
{
    ImagePairsResult ipr = FileHelper::getImagePairs("1", "2");

    EXPECT_TRUE(ipr.imagelist.size() == 0);
}