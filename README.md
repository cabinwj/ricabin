ricabin
=======

ricabin: tcp network
ricabin: 简单的tcp网络低层库，在io操作方面可以少一层copy哦，个人经验总结。。 
         具体的应用例子可以看看应用代码的实现。希望可以帮助更多的人搭建网络框架，快速开发。。

         当然很多人会有疑问：为什么不用libevent, boost, stl　等等问题。。
        　　参看博文：windows client c/c++ 经常会问 linux server c/c++ 的几个问题
        　　　　　　　http://blog.sina.com.cn/s/blog_8cf0057a0101b7sj.html

         本源码都是可以在windows和linux下运行，但本库中有关协程方面，不建议在windows上运行。。         
         源码中包括，socket 库，服务器的内部结构，服务器集群组的架构（运维和布署）。。
         欢迎大家批评指点。有好的方案，就采取好的方法。。

         邮箱：  x.m.wang7075@gmail.com

ricabin/utils: base lib tools  open source.
ricabin/utils: 是一个基础工具库(网络上开源的代码)，提供服务器开发常用的工具类。。

ricabin/baselibc/hc_coroutine: coroutine lib tools open source.
ricabin/baselibc/hc_coroutine: 是一个开源的协程库，这里只是为本业务做了简单的封装。。
                               该协程库在windows可以运行，但是测试下来感觉不太稳定，
                               查了相关资料，是windows上，不建议用LPVOID m_fiber_;
                               so.不建议在windows上运行，服务器必须稳定啊。。
                               这里用windows只是为了要让整个工程在windows上运行时，
                               确保逻辑和功能没有问题。。

　　有关协程库可参考：https://github.com/cloudwu/coroutine
　　                  https://github.com/maxburke/coroutine
　　                  https://github.com/lijie/libcoro


ricabin/baseframe:  基本框架。可作为底层库(或协程)的应用例子。
ricabin/net_proxy/coro_webagent:  web反向代理，可作为基本框架的应用例子。



