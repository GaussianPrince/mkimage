# 内核签名工具
​	该工具在可执行镜像前加入一个uboot引导所需的64字节头。
```c
struct image_header {
    unsigned int magic; //魔术字 固定为0x27051956
    unsigned int hcrc; //头部CRC校验码
    unsigned int time; //打包时间
    unsigned int size; //可执行镜像大小
    unsigned int load; //镜像加载位置
    unsigned int ep; //镜像启动位置
    unsigned int dcrc; //镜像CRC校验码
    unsigned char os; //镜像所含系统类型
    unsigned char arch;//镜像目标CPU
    unsigned char type;//镜像类型
    unsigned char comp;//镜像压缩类型
    unsigned char name[IH_NMLEN];//镜像名字
};
```
## 编译
```
gcc -o mkimage mkimage.c
```
## 用例
```
./mkimage -A arm64 -O linux -T kernel -C none -a 0x40200000 -e 0x40200000 -n "gaussianprince" -d src_file target_file 
```

