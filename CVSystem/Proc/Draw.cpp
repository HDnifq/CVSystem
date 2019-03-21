﻿#include "Draw.h"
#include "../Common/FileHelper.h"

namespace dxlib {

#pragma region 私有字段

struct Draw::Draw_impl
{
    /// <summary>要开启的窗口名字. </summary>
    std::string winName;

    /// <summary> 按行写文本时起始位置记录. </summary>
    cv::Point point_line = cv::Point(0, 10);

    /// <summary> 一个图片队列. </summary>
    std::deque<cv::Mat> dqDiagram;

    /// <summary> 保存文件路径. </summary>
    std::string filePath;

    /// <summary> 当前图片. </summary>
    cv::Mat diagram;

    /// <summary> 预设的用来画相机的图像的roi. </summary>
    std::vector<cv::Rect> vImageROI;
};

std::string& Draw::filePath()
{
    return _impl->filePath;
}

cv::Mat& Draw::diagram()
{
    return _impl->diagram;
}

std::vector<cv::Rect>& Draw::vImageROI()
{
    return _impl->vImageROI;
}

cv::Rect Draw::imageROI(int index)
{
    if (index < _impl->vImageROI.size())
        return _impl->vImageROI[index];
    return cv::Rect();
}

#pragma endregion

Draw* Draw::m_pInstance = NULL;

Draw::Draw(int width, int height, float k)
{
    //构造成员
    _impl = new Draw_impl();

    _impl->diagram = cv::Mat(height, width, CV_8UC3, cv::Scalar(0, 0, 0)); //黑色背景
    this->k = k;
    this->width = width;
    this->height = height;
    _impl->winName.clear();

    _impl->filePath = FileHelper::getModuleDir().append("\\images");
    FileHelper::isExistsAndCreat(_impl->filePath);
    _impl->filePath = _impl->filePath.append("\\debug");
    FileHelper::isExistsAndCreat(_impl->filePath);
    _impl->filePath = _impl->filePath.append("\\dImg");

    nameCount = 0;

    setImageROI(cv::Size(640, 360));
    clear();
}

Draw::~Draw()
{
    delete _impl;
}

float Draw::get_k()
{
    return k;
}

void Draw::set_k(const cv::Size& cameraSize)
{
    k = 2.95f * 360 / cameraSize.height; //当摄像机画面是640x360的时候,合适的k值是2.95
}

void Draw::setImageROI(const cv::Size& size, int count)
{
    _impl->vImageROI.clear();
    _impl->vImageROI.push_back(cv::Rect(0, 1080 - 150 - size.height * 2, size.width, size.height)); //1号相机画在左上角
    _impl->vImageROI.push_back(cv::Rect(0, 1080 - 150 - size.height, size.width, size.height));     //2号相机画在左下角

    _impl->vImageROI.push_back(cv::Rect(size.width, 1080 - 150 - size.height, size.width, size.height));     //3号相机画在右下角
    _impl->vImageROI.push_back(cv::Rect(size.width, 1080 - 150 - size.height * 2, size.width, size.height)); //4号相机画在右上角
}

cv::Mat& Draw::drawRectangle(const cv::Rect2f& rect, const cv::Scalar& color)
{
    if (!isEnableDraw) {
        return _impl->diagram;
    }

    cv::Rect2f rect_k = cv::Rect2f(rect.x * k, rect.y * k, rect.width * k, rect.height * k);
    cv::rectangle(_impl->diagram, rect_k, color, 1); //画方框
    return _impl->diagram;
}

cv::Mat& Draw::drawRectangle(const cv::RotatedRect& rect, const cv::Scalar& color)
{
    if (!isEnableDraw) {
        return _impl->diagram;
    }

    cv::Point2f vertices[4];
    rect.points(vertices);
    for (int i = 0; i < 4; i++)
        drawLine(vertices[i], vertices[(i + 1) % 4], color);
    return _impl->diagram;
}

cv::Mat& Draw::drawLine(const cv::Point2f& point1, const cv::Point2f& point2, const cv::Scalar& color)
{
    if (!isEnableDraw) {
        return _impl->diagram;
    }

    line(_impl->diagram, point1 * k, point2 * k, color); //画线
    return _impl->diagram;
}

cv::Mat& Draw::drawPoint(const cv::Point2f& point, const cv::Scalar& color)
{
    if (!isEnableDraw) {
        return _impl->diagram;
    }

    line(_impl->diagram, point * k, point * k, color, 2); //画线
    return _impl->diagram;
}

cv::Mat& Draw::drawCross(const cv::Point2f& point, const cv::Scalar& color, float size)
{
    if (!isEnableDraw) {
        return _impl->diagram;
    }

    drawLine(point - cv::Point2f(5 * size, 0), point + cv::Point2f(5 * size, 0), color);
    drawLine(point - cv::Point2f(0, 5 * size), point + cv::Point2f(0, 5 * size), color);
    return _impl->diagram;
}

cv::Mat& Draw::drawText(const std::string& text, const cv::Point& org, const cv::Scalar& color, double fontScale)
{
    if (!isEnableDraw) {
        return _impl->diagram;
    }

    if (text.size() < 230) { //自动换行的限制
        putText(_impl->diagram, text, org * k, cv::FONT_HERSHEY_SIMPLEX, fontScale, color, 1, 8);
        return _impl->diagram;
    }
    std::vector<std::string> subStr;
    size_t index = 0;
    while (index < text.length()) {
        size_t len = 230;
        if (index + len > text.length()) {
            len = text.length() - index;
        }
        std::string sub = text.substr(index, len);
        subStr.push_back(sub);
        index += len;
    }
    for (size_t i = 0; i < subStr.size(); i++) {
#pragma warning(push)
#pragma warning(suppress : 4267)
        putText(_impl->diagram, subStr[i], org * k + cv::Point(0, 18 * fontScale * i) * k, cv::FONT_HERSHEY_SIMPLEX, fontScale, color, 1, 8);
#pragma warning(pop)
    }
    return _impl->diagram;
}

cv::Mat& Draw::drawTextLine(const std::string& text, const cv::Scalar& color, double fontScale)
{
    if (!isEnableDraw) {
        return _impl->diagram;
    }

    Draw::drawText(text, _impl->point_line, color, fontScale);
    _impl->point_line.y += (int)(5 / k * 3 * fontScale / 0.5); //递增起始点记录
    _impl->point_line.x = 0;
    return _impl->diagram;
}

cv::Mat& Draw::drawMat(const cv::Mat& src, const cv::Rect& roi)
{
    if (!isEnableDraw) {
        return _impl->diagram;
    }

    src.copyTo(_impl->diagram(roi));
    return _impl->diagram;
}

cv::Mat& Draw::drawMat(const cv::Mat& src, const cv::Rect& roi_src, const cv::Rect& roi_dst)
{
    if (!isEnableDraw) {
        return _impl->diagram;
    }

    src(roi_src).copyTo(_impl->diagram(roi_dst));
    return _impl->diagram;
}

cv::Mat& Draw::drawMat(const cv::Mat& src, int index)
{
    if (!isEnableDraw) {
        return _impl->diagram;
    }

    src.copyTo(_impl->diagram(_impl->vImageROI[index]));
    return _impl->diagram;
}

cv::Mat& Draw::drawLineROI(int index, const cv::Point2f& point1, const cv::Point2f& point2, const cv::Scalar& color)
{
    if (!isEnableDraw) {
        return _impl->diagram;
    }

    line(_impl->diagram(_impl->vImageROI[index]), point1, point2, color); //画线
    return _impl->diagram;
}

cv::Mat& Draw::drawPolygonROI(int index, const std::vector<cv::Point>& polygon, const cv::Scalar color)
{
    if (!isEnableDraw) {
        return _impl->diagram;
    }
    size_t size = polygon.size();
    for (size_t i = 0; i < size - 1; i++) {
        line(_impl->diagram(_impl->vImageROI[index]), polygon[i], polygon[i + 1], color); //画线
    }
    if (size >= 2)
        line(_impl->diagram(_impl->vImageROI[index]), polygon[size - 1], polygon[0], color); //画线
    return _impl->diagram;
}

cv::Mat& Draw::drawTextROI(int roiIndex, const std::string& text, const cv::Point& org, const cv::Scalar& color, double fontScale)
{
    if (!isEnableDraw) {
        return _impl->diagram;
    }

    if (text.size() < 230) { //自动换行的限制
        putText(_impl->diagram(_impl->vImageROI[roiIndex]), text, org, cv::FONT_HERSHEY_SIMPLEX, fontScale, color, 1, 8);
        return _impl->diagram;
    }
    std::vector<std::string> subStr;
    size_t index = 0;
    while (index < text.length()) {
        size_t len = 230;
        if (index + len > text.length()) {
            len = text.length() - index;
        }
        std::string sub = text.substr(index, len);
        subStr.push_back(sub);
        index += len;
    }
    for (size_t i = 0; i < subStr.size(); i++) {
#pragma warning(push)
#pragma warning(suppress : 4267)
        putText(_impl->diagram(_impl->vImageROI[roiIndex]), subStr[i], org + cv::Point(0, 20 * i), cv::FONT_HERSHEY_SIMPLEX, fontScale, color, 1, 8);
#pragma warning(pop)
    }
    return _impl->diagram;
}

void Draw::showWin(const std::string& winName, int x, int y) //2736
{
    if (_impl->winName != winName) {
        cv::namedWindow(winName, cv::WINDOW_AUTOSIZE); // Create a window for display.
        cv::moveWindow(winName, x, y);
        _impl->winName = winName;
    }
    cv::imshow(winName, _impl->diagram); // Show our image inside it.
}

void Draw::closeWin()
{
    if (!_impl->winName.empty()) {
        cv::destroyWindow(_impl->winName);
        _impl->winName.clear();
    }
}

void Draw::saveToMemory()
{
    //如果没有使能绘图那就不保存到内存了
    if (!isEnableDraw) {
        return;
    }

    //如果设置不保存到队列那么也直接返回,不需要克隆了
    if (memSavelimit == 0) {
        return;
    }
    _impl->dqDiagram.push_back(_impl->diagram.clone()); //队列末尾添加这一帧结果,也就是末尾永远是当前帧
    while (_impl->dqDiagram.size() > memSavelimit) { //只保存1200帧 / 512
        cv::Mat img = _impl->dqDiagram.front();
        _impl->dqDiagram.pop_front();
        img.release(); //释放这个图片
    }
}

void Draw::saveMemImgToFile()
{
    while (_impl->dqDiagram.size() > 0) {
        cv::Mat img = _impl->dqDiagram.front();
        _impl->dqDiagram.pop_front();
        char fileName[128];
        sprintf_s(fileName, 128, "%s%02d.png", _impl->filePath.c_str(), nameCount);
        cv::imwrite(fileName, img);
        nameCount++;
    }
}

void Draw::writeMemImgToMem(void* buff)
{
    memcpy(buff, _impl->diagram.data, _impl->diagram.cols * _impl->diagram.rows * _impl->diagram.channels());
}

void Draw::clear()
{
    _impl->diagram.setTo(cv::Scalar(0, 0, 0));          //黑色背景
    _impl->point_line = cv::Point(0, (int)(5 / k * 3)); //重置文本位置
}

void Draw::clearMemory()
{
    _impl->dqDiagram.clear();
}
} // namespace dxlib