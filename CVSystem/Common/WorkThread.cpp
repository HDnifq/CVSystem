#include "WorkThread.h"

namespace dxlib {

 




    //bool WorkThread::Start()
    //{
    //    if (isRun.load() == true) {
    //        LogI("WorkThread.Start():已经在工作了,函数直接返回.");
    //        return false;
    //    }

    //    //如果当前的工作线程不为null,那么就是刚好掐在一个中间时刻了，那么先小等一会(但是这样的处理可能带来了风险)
    //    while (this->_thread.load() != nullptr) {
    //        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    //    }
    //    isRun = true;
    //    this->_thread = new std::thread(&WorkThread::doWork, this);//成员函数可能都有一个默认对象函数指针
    //    //确保当前原子赋值都已经生效了
    //    while (!isRun || this->_thread.load() == nullptr) {
    //        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    //    }
    //    LogI("WorkThread.Start():工作线程启动id=%d!", _thread.load()->get_id());
    //    return true;
    //}

    ///// <summary> 停止线程. </summary>
    //bool WorkThread::Stop()
    //{
    //    if (isRun.load() == false) {
    //        LogI("WorkThread.Stop():已经停止了,函数直接返回.");
    //        return false;
    //    } else {
    //        isRun = false;

    //        //如果当前还存在一个线程
    //        auto thread = _thread.load();
    //        if (thread != nullptr) {
    //            //如果是其他线程调用的停止函数
    //            if (std::this_thread::get_id() != thread->get_id()) {

    //                if (thread->joinable()) {
    //                    LogI("WorkThread.Stop():当前线程进入join()");
    //                    thread->join();//等待综合分析线程退出
    //                }
    //                this->_thread = nullptr;
    //                init = nullptr;
    //                workOnce = nullptr;
    //                LogI("WorkThread.Stop():工作线程已经关闭！");
    //            } else { //如果是自己线程调用的自己停止

    //                if(_)
    //                        = thread; //添加记录当前的工作线程
    //                this->_thread = nullptr;
    //                init = nullptr;
    //                workOnce = nullptr;
    //                LogI("WorkThread.Stop():工作线程自身调用自己关闭，后续可能等待清理！");
    //            }

    //        }
    //        return true;
    //    }
    //}



}