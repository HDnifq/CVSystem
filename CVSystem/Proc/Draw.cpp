#include "Draw.h"
#include "../Common/FileHelper.h"

namespace dxlib {

#pragma region 私有字段

struct Draw::Fields
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
    return _m->filePath;
}

cv::Mat& Draw::diagram()
{
    return _m->diagram;
}

std::vector<cv::Rect>& Draw::vImageROI()
{
    return _m->vImageROI;
}

#pragma endregion

Draw* Draw::m_pInstance = NULL;

Draw::Draw(int width, int height, float k)
{
    //构造成员
    _m = new Fields();

    _m->diagram = cv::Mat(height, width, CV_8UC3, cv::Scalar(0, 0, 0)); //黑色背景
    this->k = k;
    this->width = width;
    this->height = height;
    _m->winName.clear();

    _m->filePath = FileHelper::getModuleDir().append("\\images");
    FileHelper::isExistsAndCreat(_m->filePath);
    _m->filePath = _m->filePath.append("\\debug");
    FileHelper::isExistsAndCreat(_m->filePath);
    _m->filePath = _m->filePath.append("\\dImg");

    nameCount = 0;

    setImageROI(cv::Size(640, 360));
    clear();
}

Draw::~Draw()
{
    delete _m;
}

Draw* Draw::GetInst()
{
    if (m_pInstance == NULL) { //判断是否第一次调用
        //这里注意当前相机的分辨率
        m_pInstance = new Draw(1920, 1080, 2.95f); //当摄像机画面是640x360的时候,合适的k值是2.95
    }
    return m_pInstance;
}

float Draw::get_k()
{
    return k;
}

void Draw::set_k(cv::Size cameraSize)
{
    k = 2.95f * 360 / cameraSize.height; //当摄像机画面是640x360的时候,合适的k值是2.95
}

void Draw::setImageROI(cv::Size size, int count)
{
    _m->vImageROI.clear();
    _m->vImageROI.push_back(cv::Rect(0, 1080 - 150 - size.height * 2, size.width, size.height)); //1号相机画在左上角
    _m->vImageROI.push_back(cv::Rect(0, 1080 - 150 - size.height, size.width, size.height));     //2号相机画在左下角

    _m->vImageROI.push_back(cv::Rect(size.width, 1080 - 150 - size.height, size.width, size.height));     //3号相机画在右下角
    _m->vImageROI.push_back(cv::Rect(size.width, 1080 - 150 - size.height * 2, size.width, size.height)); //4号相机画在右上角
}

cv::Mat Draw::drawRectangle(cv::Rect2f rect, cv::Scalar color)
{
    if (!isEnableDraw) {
        return _m->diagram;
    }

    cv::Rect2f rect_k = cv::Rect2f(rect.x * k, rect.y * k, rect.width * k, rect.height * k);
    cv::rectangle(_m->diagram, rect_k, color, 1); //画方框
    return _m->diagram;
}

cv::Mat Draw::drawRectangle(cv::RotatedRect rect, cv::Scalar color)
{
    if (!isEnableDraw) {
        return _m->diagram;
    }

    cv::Point2f vertices[4];
    rect.points(vertices);
    for (int i = 0; i < 4; i++)
        drawLine(vertices[i], vertices[(i + 1) % 4], color);
    return _m->diagram;
}

cv::Mat Draw::drawLine(cv::Point2f point1, cv::Point2f point2, cv::Scalar color)
{
    if (!isEnableDraw) {
        return _m->diagram;
    }

    line(_m->diagram, point1 * k, point2 * k, color); //画线
    return _m->diagram;
}

cv::Mat Draw::drawPoint(cv::Point2f point1, cv::Scalar color)
{
    if (!isEnableDraw) {
        return _m->diagram;
    }

    line(_m->diagram, point1 * k, point1 * k, color, 2); //画线
    return _m->diagram;
}

cv::Mat Draw::drawCross(cv::Point2f point1, cv::Scalar color, float size)
{
    if (!isEnableDraw) {
        return _m->diagram;
    }

    drawLine(point1 - cv::Point2f(5 * size, 0), point1 + cv::Point2f(5 * size, 0), color);
    drawLine(point1 - cv::Point2f(0, 5 * size), point1 + cv::Point2f(0, 5 * size), color);
    return _m->diagram;
}

cv::Mat Draw::drawText(const std::string text, const cv::Point& org, cv::Scalar color)
{
    if (!isEnableDraw) {
        return _m->diagram;
    }

    if (text.size() < 230) { //自动换行的限制
        putText(_m->diagram, text, org * k, cv::FONT_HERSHEY_SIMPLEX, 0.5, color, 1, 8);
        return _m->diagram;
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
        putText(_m->diagram, subStr[i], org * k + cv::Point(0, 29 * i), cv::FONT_HERSHEY_SIMPLEX, 0.5, color, 1, 8);
#pragma warning(pop)
    }
    return _m->diagram;
}

cv::Mat Draw::drawTextLine(const std::string text, cv::Scalar color)
{
    if (!isEnableDraw) {
        return _m->diagram;
    }

    Draw::drawText(text, _m->point_line, color);
    _m->point_line.y += (int)(5 / k * 3); //递增起始点记录
    _m->point_line.x = 0;
    return _m->diagram;
}

cv::Mat Draw::drawMat(cv::Mat& src, cv::Rect roi)
{
    if (!isEnableDraw) {
        return _m->diagram;
    }

    src.copyTo(_m->diagram(roi));
    return _m->diagram;
}

cv::Mat Draw::drawMat(cv::Mat& src, cv::Rect roi_src, cv::Rect roi_dst)
{
    if (!isEnableDraw) {
        return _m->diagram;
    }

    src(roi_src).copyTo(_m->diagram(roi_dst));
    return _m->diagram;
}

cv::Mat Draw::drawMat(cv::Mat& src, int index)
{
    if (!isEnableDraw) {
        return _m->diagram;
    }

    src.copyTo(_m->diagram(_m->vImageROI[index]));
    return _m->diagram;
}

void Draw::showWin(std::string winName, int x, int y) //2736
{
    if (_m->winName != winName) {
        cv::namedWindow(winName, cv::WINDOW_AUTOSIZE); // Create a window for display.
        cv::moveWindow(winName, x, y);
        _m->winName = winName;
    }
    cv::imshow(winName, _m->diagram); // Show our image inside it.
}

void Draw::closeWin()
{
    if (!_m->winName.empty()) {
        cv::destroyWindow(_m->winName);
        _m->winName.clear();
    }
}

void Draw::saveToMemory()
{
    //如果没有使能绘图那就不保存到内存了
    if (!isEnableDraw) {
        return;
    }
    _m->dqDiagram.push_back(_m->diagram.clone()); //队列末尾添加这一帧结果,也就是末尾永远是当前帧
    while (_m->dqDiagram.size() > memSavelimit) { //只保存1200帧 / 512
        cv::Mat img = _m->dqDiagram.front();
        _m->dqDiagram.pop_front();
        img.release(); //释放这个图片
    }
}

void Draw::saveMemImgToFile()
{
    while (_m->dqDiagram.size() > 0) {
        cv::Mat img = _m->dqDiagram.front();
        _m->dqDiagram.pop_front();
        char fileName[128];
        sprintf_s(fileName, 128, "%s%02d.png", _m->filePath.c_str(), nameCount);
        cv::imwrite(fileName, img);
        nameCount++;
    }
}

void Draw::writeMemImgToMem(void* buff)
{
    memcpy(buff, _m->diagram.data, _m->diagram.cols * _m->diagram.rows * _m->diagram.channels());
}

void Draw::clear()
{
    _m->diagram.setTo(cv::Scalar(0, 0, 0));          //黑色背景
    _m->point_line = cv::Point(0, (int)(5 / k * 3)); //重置文本位置
}

void Draw::clearMemory()
{
    _m->dqDiagram.clear();
}
} // namespace dxlib