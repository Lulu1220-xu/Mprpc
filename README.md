1. 项目介绍：
在分布式环境中，通过RPC（Remote Procedure Call Protocol）远程过程调用协议可以实现同一服务器不同进程间的函数调用以及不同服务器间函数的调用结果的返回；

黄色部分：rpc方法参数的打包和解析（序列化和反序列化），使用protobuf.
绿色部分：网络部分，zookeeper服务配置中心寻找rpc服务主机，使用muduo网络库发起rpc调用请求和响应rpc调用结果
红色部分：方法请求端和方法响应端
图解过程：
1.左图的User作为远程调用者，需要调用不同进程、不同服务器的另一函数方法；
2.local call 将具体的RPC调用的细节信息，方法，标识，参数进行pack argument打包，也就是序列化；
3.我们调用muduo网络库时和zookeeper服务配置中心查找需要调用的方法位于哪一台服务器上，将调用请求通过网络传输给目的端服务器；
4.远端从网络接收到RPC调用请求(对应上图的receive)；
5.要把从网络接收的打包的参数解包，对应上图的unpack argument，我们称作反序列化；
6.然后就到server2上了，对应上图的callee—call，通过解包后获取出具体的信息执行调用操作—work，将函数调用的返回值return ，不管是正常运行还是出错，return 返回相应的结果；
7.这时进行pack result，打包结果，即序列化成字节流；
8.通过网络muduo库发到caller端；caller端接收—receive；
9.这个网络就会把字节流上报到User-stub上，把字节流反序列化—解包unpack result；
10.调用端最终得到具体的应用程序调用方法的结果的数据—local return。

在步骤6：可能在执行的时候函数执行出现问题，我们可以在返回的东西中还可以加一些响应：
响应码是0：表示这个方法在远端执行是正常的，
执行错误：就返回一些信息（响应码，错误码，错误信息，具体的返回值）；
步骤10：根据返回的信息如果没错误码，就可以读返回值了，如果有错误码，意味着RPC方法在调用时出错了，就直接读错误消息描述。

在分布式通信框架中，都有桩，stub就是“桩“，我们可以认为就是一个”代理“，因为RPC调用的发起方只关心业务，不用关心底层的网络上的RPC方法，通信的细节，细节都包装在stub，做数据的序列化和反序列化，网络数据的收发。执行RPC请求的这一端也是如此。执行网络的收发数据，执行数据的序列化和反序列化。


2. 序列化、反序列化方法选择：
在打包解包中我们使用了PB进行序列化、反序列化，为什么选择PB？
1.是因为PB基于二进制存储，存储节省了内存空间、降低了机器负载！
2.在远程调用中高效传输节省了数据交换成本，降低了网络延迟，提升用户使用感受！
3.更适用传输数据量大、网络不稳定情况下，保障服务的可靠性！

3. 如何实现一个本地方法实现远程方法的调用
1.在文件中写入 func.proto 文件，完善方法message:request、response；service：funcRpc
2.在当前文件下生成C++文件 ：protoc func.proto --cpp_out=./;
3.在.cc 文件中重写由 service继承的funcRpc的 func 方法：
request 提取参数；   执行；   response 写入响应；   回调 done->Run;
4.框架简介： 实现框架的初始化  ，创建发布rpc 的网络对象（可以实现发布rpc 方法）


找不到zk动态库：export LD_LIBRARY_PATH=/home/stu/ku/zookeeper-3.4.10/src/c/.libs:$LD_LIBRARY_PATH  