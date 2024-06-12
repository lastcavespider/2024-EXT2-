#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <string.h>
#include "main.h"
#include "init.h"



int main(int argc, char** argv)
{
	char command[10], temp[9];
	init_memory();//��ʼ���ڴ�

	while (1)
	{
		printf("%s]$", current_path);
		scanf("%s", command);
		if (!strcmp(command, "cd")) //���뵱ǰĿ¼��
		{
			scanf("%s", temp);
			cd(temp);
		}

		else if (!strcmp(command, "initm"))  //����Ŀ¼
		{
			init_memory();
		}

		else if (!strcmp(command, "mkdir"))  //����Ŀ¼
		{
			scanf("%s", temp);
			mkdir(temp, 2);
		}

		else if (!strcmp(command, "ls"))      //��ʾ��ǰĿ¼���ļ���Ŀ¼
		{
			ls();
		}

		else if (!strcmp(command, "rmdir"))  //ɾ����Ŀ¼
		{
			scanf("%s", temp);
			rmdir(temp);
		}
		else if (!strcmp(command, "touch"))    //�����ļ�
		{
			scanf("%s", temp);
			touch(temp, 1);
		}

		else if (!strcmp(command, "rm"))     //ɾ���ļ�
		{
			scanf("%s", temp);
			del(temp);
		}
		else if (!strcmp(command, "open"))    //��һ���ļ�
		{
			scanf("%s", temp);
			open_file(temp);
		}
		else if (!strcmp(command, "close"))   //�ر�һ���ļ�
		{
			scanf("%s", temp);
			close_file(temp);
		}
		else if (!strcmp(command, "read"))    //��һ���ļ�
		{
			scanf("%s", temp);
			read_file(temp);
		}
		else if (!strcmp(command, "write"))   //дһ���ļ�
		{
			scanf("%s\n", temp);
			write_file(temp);
		}
		else if (!strcmp(command, "help"))      //��ʾ����
		{
			help();
		}
		else if (!strcmp(command, "format"))  //��ʽ��Ӳ��
		{
			char tempch;
			printf("��ʽ������մ�������������QAQ\n");
			printf("ȷ��Ҫִ�и�ʽ����y/n:\n");
			fflush(stdin);
			scanf(" %c", &tempch);
			if (tempch == 'Y' || tempch == 'y')
			{
				format();
			}
			else
			{
				printf("���̸�ʽ��ȡ��\n");
			}
		}
		else if (!strcmp(command, "ckdisk"))  //���Ӳ��
		{
			check_disk();
		}
		else if (!strcmp(command, "quit")|| !strcmp(command, "q")|| !strcmp(command, "exit"))    //�˳�ϵͳ
		{
			break;
		}
		else if (!strcmp(command, "find")) //�����ļ��������ļ��ĸ���Ŀ¼
		{
			scanf("%s", temp);
			find(temp);
		}
		else printf("��δ�ҵ�������\n");
		getchar();


	}




	return 0;
}