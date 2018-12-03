程序分为2部分:压缩/解压
码书的设计使用课上讲到的LBG算法.

1.编译
环境:Linux gcc
使用命令: source compile.sh
编译成功后生成可执行文件:LBG_VQ(压缩) 与 Decode_VQ(解压)
2.运行
使用命令: ./LBG_VQ,对lena512x512.raw进行压缩
压缩后生成codebook.raw文件

使用命令./Decode_VQ , 对codebook.raw进行解压,
解压后生成lena_decode.raw

注:该程序使用.raw格式的图像文件进行处理,
可以使用支持16进制的编辑器查看.raw的内容
codebook.raw中各字节含义及内容在程序注释中有详细说明.
