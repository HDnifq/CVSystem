# CVSystem
通用的OpenCV基础系统，目前使用摄像头输入。

## 记录
* poco库里有一个内置的sqlite3的源码，这个sqlite3是没有使用SQLITE_HAS_CODEC的。配合加密使用的时候那么会出连接错误，重定义符号。所以定义poco:enable_data_sqlite=False.
