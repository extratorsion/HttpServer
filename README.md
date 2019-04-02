# HttpServer

## 使用说明
	在config.h中配置运行信息在，在source目录的父目录下使用make命令，即可编译成功．
	代码中使用了C++11~C++17的语言标准，请使用支持新标准的编译器编译程序．


## 文件说明
### config.h
	程序的运行目录，及Ｈttp服务运行时的文件文件加载根目录，及其他一些信配置信息

### common_includes.h
	其他文件所共同需要引用的头文件，及部分定义的声明

### threadpool.h
	线程池类的实现

### response.h
	Ｈttp 响应类

### request.h
	Http 请求类

### cgihandler.h
	cgi程序处理类

### mstring.h
	字符串处理类，继承自std::string类