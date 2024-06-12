#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <string.h>
#include "main.h"
#include "init.h"



int main(int argc, char** argv)
{
	char command[10], temp[9];
	init_memory();//初始化内存

	while (1)
	{
		printf("%s]$", current_path);
		scanf("%s", command);
		if (!strcmp(command, "cd")) //进入当前目录下
		{
			scanf("%s", temp);
			cd(temp);
		}

		else if (!strcmp(command, "initm"))  //创建目录
		{
			init_memory();
		}

		else if (!strcmp(command, "mkdir"))  //创建目录
		{
			scanf("%s", temp);
			mkdir(temp, 2);
		}

		else if (!strcmp(command, "ls"))      //显示当前目录内文件或目录
		{
			ls();
		}

		else if (!strcmp(command, "rmdir"))  //删除空目录
		{
			scanf("%s", temp);
			rmdir(temp);
		}
		else if (!strcmp(command, "touch"))    //创建文件
		{
			scanf("%s", temp);
			touch(temp, 1);
		}

		else if (!strcmp(command, "rm"))     //删除文件
		{
			scanf("%s", temp);
			del(temp);
		}
		else if (!strcmp(command, "open"))    //打开一个文件
		{
			scanf("%s", temp);
			open_file(temp);
		}
		else if (!strcmp(command, "close"))   //关闭一个文件
		{
			scanf("%s", temp);
			close_file(temp);
		}
		else if (!strcmp(command, "read"))    //读一个文件
		{
			scanf("%s", temp);
			read_file(temp);
		}
		else if (!strcmp(command, "write"))   //写一个文件
		{
			scanf("%s\n", temp);
			write_file(temp);
		}
		else if (!strcmp(command, "help"))      //显示命令
		{
			help();
		}
		else if (!strcmp(command, "format"))  //格式化硬盘
		{
			char tempch;
			printf("格式化将清空磁盘中所有数据QAQ\n");
			printf("确定要执行格式化吗？y/n:\n");
			fflush(stdin);
			scanf(" %c", &tempch);
			if (tempch == 'Y' || tempch == 'y')
			{
				format();
			}
			else
			{
				printf("磁盘格式化取消\n");
			}
		}
		else if (!strcmp(command, "ckdisk"))  //检查硬盘
		{
			check_disk();
		}
		else if (!strcmp(command, "quit")|| !strcmp(command, "q")|| !strcmp(command, "exit"))    //退出系统
		{
			break;
		}
		else if (!strcmp(command, "find")) //查找文件并进入文件的父级目录
		{
			scanf("%s", temp);
			find(temp);
		}
		else printf("并未找到该命令\n");
		getchar();


	}




	return 0;
}