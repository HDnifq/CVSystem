#include "pch.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

TEST(Log, spdLog)
{
    try {
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e %t][%l] %v");

        auto my_logger = spdlog::basic_logger_mt("basic_logger", "log/basic-log.txt");
        spdlog::set_default_logger(my_logger);
        for (size_t i = 0; i < 50000; i++) {
            my_logger->info("第一条日志!!!!!");
        }
    }
    catch (const spdlog::spdlog_ex &ex) {
        std::cout << "Log init failed: " << ex.what() << std::endl;
    }
}