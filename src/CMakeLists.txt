#aux_source_directory(. SRC_LIST)
set(SRC_LIST mprpcapplication.cc mprpcconfig.cc rpcheader.pb.cc rpcprovider.cc mprpcchannel.cc mprpccontroller.cc logger.cc )
#add_library(mprpc SHARED ${SRC_LIST})   #生成动态库：SHARED

#因为muduo是静态库 所以需要生成静态
add_library(mprpc ${SRC_LIST})
#依赖的网络库 muduo_net 是核心部分，继而base库，base库依赖net库
target_link_libraries(mprpc muduo_net muduo_base)