﻿#include "pch.h"

#include "../CVSystem/Common/Draw.h"

using namespace dxlib;

TEST(Draw, filePath)
{
    Draw::GetInst()->filePath() = "123";

    EXPECT_TRUE(Draw::GetInst()->filePath() == "123");
}