﻿#include "CameraImage.h"
#include "dlog/dlog.h"

namespace dxlib {

float CameraImage::costTime()
{
    return (float)(grabEndTime - grabStartTime) / CLOCKS_PER_SEC * 1000;
}
} // namespace dxlib