#ifndef _SPIDER_MAIN_H
#define _SPIDER_MAIN_H


//*****************主函数的声明*********************
//用于保存当前路径
extern char current_path[256];
//输出可用函数指令
extern void help(void);
//初始化内存
extern void init_memory(void);
//格式化
extern void format(void);
//检查磁盘状态
extern void check_disk(void);
//进入目录（更改当前路径的值）
extern void cd(char tmp[9]);
//创建一个目录
extern void mkdir(char tmp[9], int type);
//创建一个文件
extern void touch(char tmp[9], int type);
//删除一个空目录
extern void rmdir(char tmp[9]);
//删除文件
extern void del(char tmp[9]);
//打开文件
extern void open_file(char tmp[9]);
//关闭文件
extern void close_file(char tmp[9]);
//读文件
extern void read_file(char tmp[9]);
//覆盖方式写入文件
extern void write_file(char tmp[9]);
//查看目录下内容
extern void ls(void);

//查找文件并打印路径，转跳到文件的父级目录
extern void find(char tmp[9]);




#endif // !_SPIDER_MAIN_H
