ricabin
=======

ricabin: tcp network
ricabin: 简单的tcp网络低层库，在io操作方面可以少一层copy哦，个人经验总结。
　　　具体的应用例子可以看看应用代码的实现。希望可以帮助更多的人搭建网络框架，快速开发。

　　　当然很多人会有疑问：为什么不用libevent　等等问题。
        　　参看博文：windows client c/c++ 经常会问 linux server c/c++ 的几个问题
        　　　　　　　http://blog.sina.com.cn/s/blog_8cf0057a0101b7sj.html

　　　欢迎大家批评指点。有好的方案，就采取好的方法。

ricabin/utils:  base lib tools  open source.
ricabin/utils: 是一个基础工具库(网络上开源的代码)，提供服务器开发常用的工具类。

ricabin/baselibc/hc_coroutine: coroutine lib tools open source.
ricabin/baselibc/hc_coroutine: 是一个开源的协程库，这里只是为本业务做了简单的封装
　　有关协程库可参考：https://github.com/cloudwu/coroutine
                                    https://github.com/maxburke/coroutine
                                    https://github.com/lijie/libcoro
                                    
