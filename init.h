#ifndef _SPIDER_INIT_H
#define _SPIDER_INIT_H

#include"main.h"

//************�궨�岿��***************

#define BLOCK_SIZE 512					//���С
#define DISK_SIZE 4612					//���̴�С
#define NAME "SPIDEREXT2"				//����


#define DISK_START 0					//���̿�ʼ��ַ 0
#define SUPER_BLOCK_SIZE 32				//�������С 32B			



#define GDT_START (0+512)				//������������ʼ��ַ
#define GDT_SIZE 32						//������������С 32B	


#define BLOCK_BITMAP_START (512+512)	//��λͼ��ʼ��ַ
#define INODE_BITMAP_START (1024+512)	//INODE λͼ��ʼ��ַ


#define INODE_TABLE_START (1536+512)	//INODE ����ʼ��ַ
#define INODE_SIZE 64					//INODE ��С 64B
#define INODE_TABLE_COUNTS 4096			//INODE ��


#define DATA_BLOCK_START (263680 + 512) //���ݿ���ʼ��ַ 512*4+4096*64
#define DATA_BLOCK_COUNTS 4096			//���ݿ���


#define BLOCKS_GROUP_COUNTS 4612		//ÿ���еĿ���


//**************���ݽṹ����************

struct super_block                          // ������ 32 B 
{
    char sb_volume_name[16];                //�ļ�ϵͳ��
    unsigned short sb_disk_size;            //�����ܴ�С
    unsigned short sb_blocks_per_group;     // ÿ���еĿ���
    unsigned short sb_size_per_block;	    // ���С
    char sb_filled[10];                     //���
};


struct group_description                    //���������� 32B
{
    char gd_volume_name[16];                //�ļ�ϵͳ��
    unsigned short gd_block_bitmap;         //��λͼ����ʼ���
    unsigned short gd_inode_bitmap;         //�������λͼ����ʼ���
    unsigned short gd_inode_table;          //�����������ʼ���
    unsigned short gd_free_blocks_count;    //������п�ĸ���
    unsigned short gd_free_inodes_count;    //��������������ĸ���
    unsigned short gd_used_dirs_count;      //���з����Ŀ¼�Ľ����
    char gd_filled[4];                         //���
};


struct inode                        //�����ڵ� 64B
{
    unsigned short i_mode;          //�ļ����ͼ�����Ȩ��
    unsigned short i_blocks;        //�ļ���ռ�����ݿ����(0--7), ���Ϊ7
    unsigned short i_uid;           //�ļ�ӵ���߱�ʶ��
    unsigned short i_gid;           //�ļ����û����ʶ��
    unsigned short i_links_count;   //�ļ���Ӳ���Ӽ���
    unsigned short i_flags;         //���ļ��ķ�ʽ
    unsigned long i_size;           //�ļ���Ŀ¼��С(��λ byte)
    unsigned long i_atime;          //����ʱ��
    unsigned long i_ctime;          //����ʱ��
    unsigned long i_mtime;          //�޸�ʱ��
    unsigned long i_dtime;          //ɾ��ʱ��
    unsigned short i_block[8];      //ֱ��������ʽ ָ�����ݿ��
    char i_filled[14];              //���(0xff)

};

struct dir_entry //16B
{
    unsigned short inode;           //�����ڵ��
    unsigned short rec_len;         //Ŀ¼���
    unsigned short name_len;        //�ļ�������
    char file_type;                 //�ļ�����(1 ��ͨ�ļ� 2 Ŀ¼ )
    char name[9];                   //�ļ���
};

//****************ȫ�ֱ�������*************************

static unsigned short last_alloc_inode;                 //�������������ڵ��
static unsigned short last_alloc_block;                 //�����������ݿ��
static unsigned short current_dir;                      //��ǰĿ¼�Ľڵ��


static unsigned short current_dirlen;                   //��ǰ·������


static short fopen_table[16];                           // �ļ��򿪱�


static struct super_block sb_block[1];	                //�����黺����
static struct group_description gdt[1];	                //��������������
static struct inode inode_area[1];                      //inode������
static unsigned char bitbuf[512] = { 0 };               //λͼ������
static unsigned char ibuf[512] = { 0 };
static struct dir_entry dir[32];                        //Ŀ¼����� 32*16=512
static char Buffer[512];                                //������ݿ�Ļ�����
static char tempbuf[4096];	                            //�ļ�д�뻺����
static FILE* fp;


char current_path[256];                                 //��ǰ·��                                        


//*************ϵͳ������������********************

static void update_super_block(void);                   //���³�����
static void reload_super_block(void);                   //���س���������


static void update_group_description(void);             //����������������
static void reload_group_description(void);             //����������������
static void update_inode_entry(unsigned short i);       //����indoe��
static void reload_inode_entry(unsigned short i);       //����inode��
static void update_block_bitmap(void);                  //���¿�λͼ
static void reload_block_bitmap(void);                  //���ؿ�λͼ
static void update_inode_bitmap(void);                  //����inodeλͼ
static void reload_inode_bitmap(void);                  //����inodeλͼ
static void update_dir(unsigned short i);               //����Ŀ¼
static void reload_dir(unsigned short i);               //����Ŀ¼
static void update_block(unsigned short i);             //�������ݿ�
static void reload_block(unsigned short i);             //�������ݿ�
static int alloc_block(void);                           //�������ݿ�
static int get_inode(void);                             //�õ�inode�ڵ�
static unsigned short reserch_file(char tmp[9], int file_type, unsigned short* inode_num, unsigned short* block_num, unsigned short* dir_num);//�����ļ�
static void dir_prepare(unsigned short tmp, unsigned short len, int type);
static void remove_block(unsigned short del_num);       //ɾ�����ݿ�
static void remove_inode(unsigned short del_num);       //ɾ��inode�ڵ�
static unsigned short search_file(unsigned short Ino);  //�ڴ��ļ����в����Ƿ��Ѵ��ļ�
static void init_disk(void);                            //��ʼ������



#endif // !_SPIDER_INIT_H
