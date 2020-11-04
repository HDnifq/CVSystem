#include "CVSystem/CVSystem.h"

#include "dlog/dlog.h"
#include <thread>

using namespace dxlib;
using namespace std;

class TestProc : public FrameProc
{
public:
    TestProc() {}
    ~TestProc() {}

    int count = 0;

    void process(pCameraImage camImage, int& key) override
    {
        for (size_t i = 0; i < camImage->vImage.size(); i++)
        {
            if (camImage->vImage[i].isSuccess == false)
            {
                LogE("TestProc.process():有个相机采图失败!");
                continue;
            }
            cv::Mat image = camImage->vImage[i].image;
            cv::Mat gray; cv::Mat thr40; cv::Mat  thr110; cv::Mat thr180;
            thresholdOnce(image, gray, thr40, thr110, thr180);
        }

        count++;                     //采图的计数加1

        //try {
        //    key = cv::waitKey(1); //一定要加waitKey无法显示图片
        //}
        //catch (const std::exception& e) {
        //}
    }

    /**
     * 为了节省开销,一步生成灰色图和门限图.
     *
     * @author daixian
     * @date 2020/3/30
     *
     * @param [in,out] image  The image.
     * @param [in,out] gray   The gray.
     * @param [in,out] thr40  The thr 40.
     * @param [in,out] thr110 The thr 110.
     * @param [in,out] thr180 The thr 180.
     */
    void thresholdOnce(cv::Mat& image, cv::Mat& gray, cv::Mat& thr40, cv::Mat& thr110, cv::Mat& thr180)
    {
        gray = cv::Mat(image.rows, image.cols, CV_8UC1);
        thr40 = cv::Mat(image.rows, image.cols, CV_8UC1, cv::Scalar(0, 0, 0));
        thr110 = cv::Mat(image.rows, image.cols, CV_8UC1, cv::Scalar(0, 0, 0));
        thr180 = cv::Mat(image.rows, image.cols, CV_8UC1, cv::Scalar(0, 0, 0));

        int nrow = image.rows;
        int ncol = image.cols;
        int channels = image.channels();
        for (int i = 0; i < nrow; i++) {
            uchar* dataIn = image.ptr<uchar>(i); //get the address of row i;
            uchar* dataGray = gray.ptr<uchar>(i);
            uchar* dataThr40 = thr40.ptr<uchar>(i);
            uchar* dataThr110 = thr110.ptr<uchar>(i);
            uchar* dataThr180 = thr180.ptr<uchar>(i);

            for (int j = 0; j < ncol; j++) {
                dataGray[j] = dataIn[j * channels]; //灰色图

                if (dataIn[j * channels] >= 40) {
                    dataThr40[j] = 255;
                    if (dataIn[j * channels] >= 80) {
                        dataThr110[j] = 255;
                        if (dataIn[j * channels] >= 160) {
                            dataThr180[j] = 255;
                        }
                    }
                }
            }
        }
    }

    void onEnable() override
    {
    }

    void onDisable() override
    {
    }
};



int main(int argc, char* argv[])
{
    dlog_init("log", "CamCapture", dlog_init_relative::MODULE);
    CameraManger::GetInst()->clear();
    //得到第一个相机名
    DevicesHelper::GetInst()->listDevices();
    if (DevicesHelper::GetInst()->devList.size() == 0) {
        LogE("没有发现相机!");
        dlog_close();
        return 1;
    }
    LogI("找到了%zu个相机", DevicesHelper::GetInst()->devList.size());

    for (auto& kvp : DevicesHelper::GetInst()->devList)
    {
        string camName = kvp.second;
        CameraManger::GetInst()->add(pCamera(new Camera(camName, cv::Size(1280, 400))));

    }

    std::map<int, pCamera>& camMap = CameraManger::GetInst()->camMap;
    for (auto& kvp : camMap) {
        kvp.second->isVirtualCamera = false; //不为逻辑相机
        kvp.second->size = cv::Size(1280, 400);
        kvp.second->paramSize = cv::Size(1280, 400);

        kvp.second->setProp(cv::CAP_PROP_FPS, 60);
        //sc->setProp(CV_CAP_PROP_AUTO_EXPOSURE, 0);

#if defined(_WIN32) || defined(_WIN64)
        kvp.second->setProp(cv::CAP_PROP_EXPOSURE, -11);
#else
        kvp.second->setProp(cv::CAP_PROP_EXPOSURE, 1);
#endif

        //输出一下
        kvp.second->outputProp();
    }

    MultiCamera::GetInst()->addProc(new TestProc());
    MultiCamera::GetInst()->openCamera(); //打开相机

    //启动计算线程
    MultiCamera::GetInst()->start();

    dlog_set_console_thr(dlog_level::info);
    dlog_set_file_thr(dlog_level::info);
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        LogI("当前fps=%f", MultiCamera::GetInst()->fps());

    }
    dlog_close();
    return 0;
}