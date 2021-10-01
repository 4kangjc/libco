# libco
## Introduction
学习libco  
简单libco, 不包含co_epoll和共享栈以及协程调度器  
仅仅实现 `co_create`, `co_resume`, `co_yield`, `co_free`等接口   
仅实现64位的`coctx_swap`  
代码量少，一天学会  

## 编译
* 编译环境  
archLinux 64位
gcc verson 11.1.0  
cmake version 3.21.1  

* 编译  
``` shell
git clone https://github.com/4kangjc/libco.git
cd libco
mkdir build && cd build && cmake ..
make
```
## 技术要点
* 协程切换