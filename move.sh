#!/bin/sh

# 检查 bin 目录是否存在
if [ ! -d bin ]
then
    mkdir bin
fi

# 检查 bin/module 目录是否存在
if [ ! -d bin/module ]
then
    mkdir bin/module
else
    # unlink 用于删除指定目录下的文件或符号链接
    unlink bin/radar_system
    unlink bin/module/libradar_system.so
fi

# 将 sylar 生成的可执行文件移过去
cp WebServer_sylar/bin/sylar bin/radar_system
# 将 sylar 生成的的动态库.so移过去
cp lib/libradar_system.so bin/module/

# 检查 /tmp/test_radar.sock 目录是否存在，若存在则先删除
if [ ! -d /tmp/test_radar.sock ]
then
    rm -rf /tmp/test_radar.sock
fi