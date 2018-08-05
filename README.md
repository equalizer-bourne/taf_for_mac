# taf_for_mac

1.在原来的epoll的taf框架下增加可以夸平台的select IO复用网络库,之所以增加这个库，只是为了在mac系统或者windows系统下调试服务器程序方便。原来的taf库不支持在mac系统下调试。

2.删除了原来库里java相关的东西。只保留C++部分.

3.删除了库里里的32位相关内容，只保留64位内容。

说明,如果需要对库作压力测试请在linux下使用。

原来的taf库地址：https://github.com/fengmm521/taf
