0. 前置需求
0.1 对外依赖库: sqlite3
0.2 需要的权限: 创建文件夹/data/runxun, 并在其中创建数据库文件

1. 对外提供的文件:
1.1 动态库 libnet_tool.so
1.2 头文件 net_tool.hpp message.hpp addr_id.hpp

2. 编译方法
2.1 创建build文件夹
2.2 进入build, 执行 cmake ..
2.3.1 编译动态库: make net_tool
2.3.2 编译测试可执行文件: make test
PS: 执行不加参数的make 会同时编译出动态库和测试可执行文件