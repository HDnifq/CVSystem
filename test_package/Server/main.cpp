#include "CVSystem/web/HttpServer.h"

#include "dlog/dlog.h"
#include <thread>

#include "xuexuejson/JsonMapper.hpp"
#include "xuexue/csharp/csharp.h"

using namespace dxlib;
using namespace std;

int main(int argc, char* argv[])
{
    using xuexue::csharp::File;
    using xuexue::csharp::Path;

    dlog_init("log", "Server", dlog_init_relative::MODULE);

    dxlib::CVSystemHttpServer app;
    int res = app.run(argc, argv);
    dlog_close();
    return res;
}