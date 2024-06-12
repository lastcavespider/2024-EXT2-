#ifndef _SPIDER_MAIN_H
#define _SPIDER_MAIN_H


//*****************������������*********************
//���ڱ��浱ǰ·��
extern char current_path[256];
//������ú���ָ��
extern void help(void);
//��ʼ���ڴ�
extern void init_memory(void);
//��ʽ��
extern void format(void);
//������״̬
extern void check_disk(void);
//����Ŀ¼�����ĵ�ǰ·����ֵ��
extern void cd(char tmp[9]);
//����һ��Ŀ¼
extern void mkdir(char tmp[9], int type);
//����һ���ļ�
extern void touch(char tmp[9], int type);
//ɾ��һ����Ŀ¼
extern void rmdir(char tmp[9]);
//ɾ���ļ�
extern void del(char tmp[9]);
//���ļ�
extern void open_file(char tmp[9]);
//�ر��ļ�
extern void close_file(char tmp[9]);
//���ļ�
extern void read_file(char tmp[9]);
//���Ƿ�ʽд���ļ�
extern void write_file(char tmp[9]);
//�鿴Ŀ¼������
extern void ls(void);

//�����ļ�����ӡ·����ת�����ļ��ĸ���Ŀ¼
extern void find(char tmp[9]);




#endif // !_SPIDER_MAIN_H
