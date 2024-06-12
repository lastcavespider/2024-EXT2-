#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <string.h>
#include "init.h"


static void update_super_block(void)			//д������
{
	fp = fopen("./SpiderExt2", "r+");			//��r+ģʽ��
	fseek(fp, DISK_START, SEEK_SET);			//��ָ���Ƶ����̿�ʼ��ַ
	fwrite(sb_block, SUPER_BLOCK_SIZE, 1, fp);	//д�볬�����С������
	fflush(fp);									//���������е�����д�أ���֤����һ����

}


static void reload_super_block(void)//��������
{
	fseek(fp, DISK_START, SEEK_SET);//
	fread(sb_block, SUPER_BLOCK_SIZE, 1, fp);//�����ݶ�ȡ�������黺������
}


static void update_group_description(void)//д��������
{
	fp = fopen("./SpiderExt2", "r+");
	fseek(fp, GDT_START, SEEK_SET);//�ƶ���������������ʼλ��
	fwrite(gdt, GDT_SIZE, 1, fp);
	fflush(fp);
}


static void reload_group_description(void) // ����������
{
	fseek(fp, GDT_START, SEEK_SET);
	fread(gdt, GDT_SIZE, 1, fp);

}


static void update_inode_entry(unsigned short i) // д��i��inode
{
	fp = fopen("./SpiderExt2", "r+");
	fseek(fp, INODE_TABLE_START + (i - 1) * INODE_SIZE, SEEK_SET);//�ƶ�����i��inode
	fwrite(inode_area, INODE_SIZE, 1, fp);
	fflush(fp);
}


static void reload_inode_entry(unsigned short i) // ����i��inode
{
	fseek(fp, INODE_TABLE_START + (i - 1) * INODE_SIZE, SEEK_SET);
	fread(inode_area, INODE_SIZE, 1, fp);
}



static void update_dir(unsigned short i) //   д��i�� ���ݿ飨Ŀ¼
{
	fp = fopen("./SpiderExt2", "r+");
	fseek(fp, DATA_BLOCK_START + i * BLOCK_SIZE, SEEK_SET);
	fwrite(dir, BLOCK_SIZE, 1, fp);
	fflush(fp);
}


static void reload_dir(unsigned short i) // ����i�� ���ݿ飨Ŀ¼
{
	fseek(fp, DATA_BLOCK_START + i * BLOCK_SIZE, SEEK_SET);
	fread(dir, BLOCK_SIZE, 1, fp);
	//fclose(fp);
}


static void update_block_bitmap(void) //дblockλͼ
{
	fp = fopen("./SpiderExt2", "r+");
	fseek(fp, BLOCK_BITMAP_START, SEEK_SET);
	fwrite(bitbuf, BLOCK_SIZE, 1, fp);
	fflush(fp);
}


static void reload_block_bitmap(void) //��blockλͼ
{
	fseek(fp, BLOCK_BITMAP_START, SEEK_SET);
	fread(bitbuf, BLOCK_SIZE, 1, fp);
}



static void update_inode_bitmap(void) //дinodeλͼ
{
	fp = fopen("./SpiderExt2", "r+");
	fseek(fp, INODE_BITMAP_START, SEEK_SET);
	fwrite(ibuf, BLOCK_SIZE, 1, fp);
	fflush(fp);
}


static void reload_inode_bitmap(void) // ��inodeλͼ
{
	fseek(fp, INODE_BITMAP_START, SEEK_SET);
	fread(ibuf, BLOCK_SIZE, 1, fp);
}



static void update_block(unsigned short i) // д��i�����ݿ�
{
	fp = fopen("./SpiderExt2", "r+");
	fseek(fp, DATA_BLOCK_START + i * BLOCK_SIZE, SEEK_SET);
	//fseek(fp,0,SEEK_SET);
	fwrite(Buffer, BLOCK_SIZE, 1, fp);
	fflush(fp);
}


static void reload_block(unsigned short i) // ����i�����ݿ�
{
	fseek(fp, DATA_BLOCK_START + i * BLOCK_SIZE, SEEK_SET);
	fread(Buffer, BLOCK_SIZE, 1, fp);
}



static int alloc_block(void) // ����һ�����ݿ�,�������ݿ��
{

	//bitbuf����512���ֽڣ���ʾ4096�����ݿ顣����last_alloc_block/8��������bitbuf����һ���ֽ�

	unsigned short cur = last_alloc_block;
	printf("cur: %d\n",cur);
	unsigned char con = 128; // 1000 0000b
	int flag = 0;
	if (gdt[0].gd_free_blocks_count == 0)//����������ĿΪ0������û�пտ������
	{
		printf("û�п��п�ɹ�������qwq!\n");
		return(0);
	}
	reload_block_bitmap();//��ȡblockλͼ
	cur /= 8;//���Ҫת����bitbuf���±�
	while (bitbuf[cur] == 255)//���ֽڵ�8��bit���������� 11111111
	{
		if (cur == 511)cur = 0; //���һ���ֽ�Ҳ�Ѿ�������ͷ��ʼѰ��    bitbuf[512]
		else cur++;
	}
	while (bitbuf[cur] & con) //��һ���ֽ����Ҿ����ĳһ��bit����λ�� 1000 0000
	{
		con = con / 2;//λ����
		flag++;
	}
	bitbuf[cur] = bitbuf[cur] + con;//ע�⣬�����Ƹ�λ����λͼ��������
	last_alloc_block = cur * 8 + flag;//׼ȷ��λ���п�

	update_block_bitmap();
	gdt[0].gd_free_blocks_count--;//�����������ڲ�ѯ���п飬�ҵ�����п���Ŀ--
	update_group_description();
	return last_alloc_block;
}


static int get_inode(void) // ����һ��inode
{
	unsigned short cur = last_alloc_inode;
	unsigned char con = 128;
	int flag = 0;
	if (gdt[0].gd_free_inodes_count == 0)//�Ȳ鿴�Ƿ��п����inode�ɹ�����
	{
		printf("û�ÿ����inode�ɹ�������qwq!\n");
		return 0;
	}
	reload_inode_bitmap();

	cur = (cur - 1) / 8;   //��һ�������1�����Ǵ洢�Ǵ�0��ʼ��
	//printf("%s",)
	while (ibuf[cur] == 255) //�ȿ����ֽڵ�8��λ�Ƿ��Ѿ�����
	{
		if (cur == 511)cur = 0;//���һ���ֽ�Ҳ�Ѿ�������ͷ��ʼѰ��    ibuf[512]
		else cur++;
	}
	while (ibuf[cur] & con)  //�ٿ�ĳ���ֽڵľ�����һλû�б�ռ��
	{
		con = con / 2;
		flag++;
	}
	ibuf[cur] = ibuf[cur] + con;
	last_alloc_inode = cur * 8 + flag + 1;
	update_inode_bitmap();
	gdt[0].gd_free_inodes_count--;//�տ����--
	update_group_description();
	printf("last_alloc_inode:%d\n",last_alloc_inode);
	return last_alloc_inode;
}



//��ǰĿ¼�в����ļ���Ŀ¼Ϊtmp�����õ����ļ��� inode �ţ������ϼ�Ŀ¼�е����ݿ���Լ����ݿ���Ŀ¼�����

static unsigned short reserch_file(char tmp[9], int file_type, unsigned short* inode_num,unsigned short* block_num, unsigned short* dir_num)
{
	unsigned short j, k;
	reload_inode_entry(current_dir); //���뵱ǰĿ¼����Ŀ¼��inode���뻺������
	j = 0;
	while (j < inode_area[0].i_blocks)//��ѯ���ݿ飨0-7��
	{
		reload_dir(inode_area[0].i_block[j]);//����j�����ݿ�
		k = 0;
		while (k < 32)
		{
			if (!dir[k].inode || dir[k].file_type != file_type || strcmp(dir[k].name, tmp))//�ж��ļ����ͣ��ļ�����Ŀ¼���Ƿ���ͬ
			{
				k++;
			}
			else
			{
				//��inode�ţ����ݿ�ŵȸ�ֵ�������ָ��
				*inode_num = dir[k].inode;
				*block_num = j;
				*dir_num = k;
				return 1;
			}
		}
		j++;
	}
	return 0;
}




//Ϊ����Ŀ¼���ļ����� dir_entry
// 
//���������ļ���ֻ�����һ��inode��
//��������Ŀ¼������inode���⣬����Ҫ�����������洢.��..����Ŀ¼��

static void dir_prepare(unsigned short tmp, unsigned short len, int type)
{
	reload_inode_entry(tmp);

	if (type == 2) // Ŀ¼
	{
		inode_area[0].i_size = 32;
		inode_area[0].i_blocks = 1;
		inode_area[0].i_block[0] = alloc_block();
		//printf("alloc %d\n",inode_area[0].i_block[0]);
		dir[0].inode = tmp;
		dir[1].inode = current_dir;
		dir[0].name_len = len;
		dir[1].name_len = current_dirlen;
		dir[0].file_type = dir[1].file_type = 2;

		for (type = 2; type < 32; type++)
		{
			dir[type].inode = 0;
		}
		strcpy(dir[0].name, ".");//����. ��..
		strcpy(dir[1].name, "..");
		update_dir(inode_area[0].i_block[0]);

		inode_area[0].i_mode = 01006;
	}
	else
	{
		inode_area[0].i_size = 0;
		inode_area[0].i_blocks = 0;
		inode_area[0].i_mode = 0407;
	}
	update_inode_entry(tmp);
}



//ɾ��һ�����

static void remove_block(unsigned short del_num)
{
	unsigned short tmp;
	tmp = del_num / 8;//��ԭʼ�Ŀ�Ŷ�λ��bitbuf[512]�ľ����±�
	reload_block_bitmap();
	switch (del_num % 8) // ����blockλͼ �������λ��Ϊ0
	{
	//ע��λͼ��˳�����������������0��>0xxxxxxx(x��ʾδ֪����)
	case 0:bitbuf[tmp] = bitbuf[tmp] & 127; break; // bitbuf[tmp] & 0111 1111b
	case 1:bitbuf[tmp] = bitbuf[tmp] & 191; break; //bitbuf[tmp]  & 1011 1111b
	case 2:bitbuf[tmp] = bitbuf[tmp] & 223; break; //bitbuf[tmp]  & 1101 1111b
	case 3:bitbuf[tmp] = bitbuf[tmp] & 239; break; //bitbbuf[tmp] & 1110 1111b
	case 4:bitbuf[tmp] = bitbuf[tmp] & 247; break; //bitbuf[tmp]  & 1111 0111b
	case 5:bitbuf[tmp] = bitbuf[tmp] & 251; break; //bitbuf[tmp]  & 1111 1011b
	case 6:bitbuf[tmp] = bitbuf[tmp] & 253; break; //bitbuf[tmp]  & 1111 1101b
	case 7:bitbuf[tmp] = bitbuf[tmp] & 254; break; // bitbuf[tmp] & 1111 1110b
	}
	update_block_bitmap();
	gdt[0].gd_free_blocks_count++;//ɾ������ź���п���++
	update_group_description();
}



//ɾ��һ��inode ��

static void remove_inode(unsigned short del_num)
{
	unsigned short tmp;
	tmp = (del_num - 1) / 8;
	reload_inode_bitmap();
	switch ((del_num - 1) % 8)//����blockλͼ
	{
	case 0:bitbuf[tmp] = bitbuf[tmp] & 127; break;//�߼���ɾ���������
	case 1:bitbuf[tmp] = bitbuf[tmp] & 191; break;
	case 2:bitbuf[tmp] = bitbuf[tmp] & 223; break;
	case 3:bitbuf[tmp] = bitbuf[tmp] & 239; break;
	case 4:bitbuf[tmp] = bitbuf[tmp] & 247; break;
	case 5:bitbuf[tmp] = bitbuf[tmp] & 251; break;
	case 6:bitbuf[tmp] = bitbuf[tmp] & 253; break;
	case 7:bitbuf[tmp] = bitbuf[tmp] & 254; break;
	}
	update_inode_bitmap();
	gdt[0].gd_free_inodes_count++;
	update_group_description();
}




//�ڴ��ļ����в����Ƿ��Ѵ��ļ�
static unsigned short search_file(unsigned short Inode)
{
	unsigned short fopen_table_point = 0;
	while (fopen_table_point < 16 && fopen_table[fopen_table_point++] != Inode);//�����ļ��򿪱�
	if (fopen_table_point == 16)//����������δ�ҵ�������0
	{
		return 0;
	}
	return 1;
}



void init_disk(void)  //��ʼ������
{
	int i = 0;
	printf("���ڴ���EXT2�ļ�ϵͳ\n");
	printf("��ȴ�һ��QAQ\n ");
	while (i < 1)
	{
		printf("qwq\n ");
		i++;
	}
	printf("\n");
	last_alloc_inode = 1;
	last_alloc_block = 0;
	for (i = 0; i < 16; i++)
	{
		fopen_table[i] = 0; //��ջ����
	}
	for (i = 0; i < BLOCK_SIZE; i++)
	{
		Buffer[i] = 0; // ��ջ�����
	}
	if (fp != NULL)
	{
		fclose(fp);
	}
	fp = fopen("./SpiderExt2", "w+"); //���ļ���С��4612*512=2361344B���ô��ļ���ģ���ļ�ϵͳ
	fseek(fp, DISK_START, SEEK_SET);//���ļ�ָ���0��ʼ
	for (i = 0; i < DISK_SIZE; i++)
	{
		fwrite(Buffer, BLOCK_SIZE, 1, fp); // ����ļ�������մ���ȫ����0��� BufferΪ��������ʼ��ַ��BLOCK_SIZE��ʾ��ȡ��С��1��ʾд�����ĸ���*/
	}
	// ��ʼ������������
	reload_super_block();
	strcpy(sb_block[0].sb_volume_name, NAME);
	sb_block[0].sb_disk_size = DISK_SIZE;
	sb_block[0].sb_blocks_per_group = BLOCKS_GROUP_COUNTS;
	sb_block[0].sb_size_per_block = BLOCK_SIZE;
	update_super_block();
	// ��Ŀ¼��inode��Ϊ1
	reload_inode_entry(1);

	reload_dir(0);
	strcpy(current_path, "[root@ /");  // �޸�·����Ϊ��Ŀ¼
	// ��ʼ��������������
	reload_group_description();

	gdt[0].gd_block_bitmap = BLOCK_BITMAP_START; //��һ����λͼ����ʼ��ַ
	gdt[0].gd_inode_bitmap = INODE_BITMAP_START; //��һ��inodeλͼ����ʼ��ַ
	gdt[0].gd_inode_table = INODE_TABLE_START;   //inode�����ʼ��ַ
	gdt[0].gd_free_blocks_count = DATA_BLOCK_COUNTS; //�������ݿ���
	gdt[0].gd_free_inodes_count = INODE_TABLE_COUNTS; //����inode��
	gdt[0].gd_used_dirs_count = 0; // ��ʼ�����Ŀ¼�Ľڵ�����0
	update_group_description(); // ����������������

	reload_block_bitmap();
	reload_inode_bitmap();

	inode_area[0].i_mode = 518;
	inode_area[0].i_blocks = 0;
	inode_area[0].i_size = 32;
	inode_area[0].i_atime = 0;
	inode_area[0].i_ctime = 0;
	inode_area[0].i_mtime = 0;
	inode_area[0].i_dtime = 0;
	inode_area[0].i_block[0] = alloc_block(); //�������ݿ�
	//printf("%d f\n",inode_area[0].i_block[0]);
	inode_area[0].i_blocks++;
	current_dir = get_inode();
	update_inode_entry(current_dir);

	//��ʼ����Ŀ¼��Ŀ¼��
	dir[0].inode = dir[1].inode = current_dir;
	dir[0].name_len = 0;
	dir[1].name_len = 0;
	dir[0].file_type = dir[1].file_type = 2;
	strcpy(dir[0].name, ".");
	strcpy(dir[1].name, "..");
	update_dir(inode_area[0].i_block[0]);
	printf("EXT2�ļ�ϵͳ�Ѱ�װ���!\n");
	check_disk();
	fclose(fp);
}



//main.h�к���ʵ��


//��ʼ���ڴ�
void init_memory(void)
{
	int i = 0;
	last_alloc_inode = 1;
	last_alloc_block = 0;
	for (i = 0; i < 16; i++)//�ļ��򿪱�����
	{
		fopen_table[i] = 0;
	}
	strcpy(current_path, "[root@ /");	//��ǰ·���õ�root
	current_dir = 1;					//��ǰĿ¼�ڵ����1
	fp = fopen("./SpiderExt2", "r+");
	if (fp == NULL)//���û���ҵ��ʹ���һ��spiderext2
	{
		printf("�ļ�ϵͳ������!\n");
		init_disk();
		return;
	}
	reload_super_block();
	if (strcmp(sb_block[0].sb_volume_name, NAME))
	{
		printf("�ļ�ϵͳ [%s] ��֧��!\n", sb_block[0].sb_volume_name);
		printf("�ļ�ϵͳ���ش���!\n");
		init_disk();
		return;
	}
	reload_group_description();
}


//��ʽ��
void format(void)
{
	init_disk();
	init_memory();
}


//������״̬
void check_disk(void)
{
	reload_super_block();
	printf("volume name       : %s\n", sb_block[0].sb_volume_name);
	printf("disk size         : %d(blocks)\n", sb_block[0].sb_disk_size);
	printf("blocks per group  : %d(blocks)\n", sb_block[0].sb_blocks_per_group);
	printf("ext2 file size    : %d(kb)\n", sb_block[0].sb_disk_size * sb_block[0].sb_size_per_block / 1024);
	printf("block size        : %d(kb)\n", sb_block[0].sb_size_per_block);
}


//����ĳ��Ŀ¼��ʵ�����Ǹı䵱ǰ·��
void cd(char tmp[9])
{
	unsigned short i, j, k, flag;

	flag = reserch_file(tmp, 2, &i, &j, &k);

	if (flag)
	{
		current_dir = i;
		if (!strcmp(tmp, "..") && dir[k - 1].name_len) //����һ��Ŀ¼
		{
			current_path[strlen(current_path) - dir[k - 1].name_len - 1] = '\0';
			current_dirlen = dir[k].name_len;
		}
		else if (!strcmp(tmp, "."))
		{
			return;
		}
		else if (strcmp(tmp, "..")) // cd ����Ŀ¼
		{
			current_dirlen = strlen(tmp);
			strcat(current_path, tmp);
			strcat(current_path, "/");
		}
	}
	else
	{
		printf("Ŀ¼ %s ������!\n", tmp);
	}
}



// ����Ŀ¼
void mkdir(char tmp[9], int type)
{
	//printf("%s %d\n",tmp,type);
	unsigned short tmpno, i, j, k, flag;

	// ��ǰĿ¼������Ŀ¼���ļ�
	reload_inode_entry(current_dir);
	if (!reserch_file(tmp, type, &i, &j, &k)) // δ�ҵ�ͬ���ļ�
	{
		if (inode_area[0].i_size == 4096) // Ŀ¼������
		{
			printf("��Ŀ¼����û�пռ�!\n");
			return;
		}
		flag = 1;
		if (inode_area[0].i_size != inode_area[0].i_blocks * 512) // Ŀ¼����ĳЩ����32�� dir_entry δ��
		{
			i = 0;
			while (flag && i < inode_area[0].i_blocks)
			{
				reload_dir(inode_area[0].i_block[i]);
				j = 0;
				while (j < 32)
				{
					if (dir[j].inode == 0)
					{
						flag = 0; //�ҵ�ĳ��δװ��Ŀ¼��Ŀ�
						break;
					}
					j++;
				}
				i++;
			}
			tmpno = dir[j].inode = get_inode();//���ظ��º�������ڵ��

			dir[j].name_len = strlen(tmp);
			dir[j].file_type = type;
			strcpy(dir[j].name, tmp);
			update_dir(inode_area[0].i_block[i - 1]);
		}
		else // ȫ�� �����ӿ�
		{
			inode_area[0].i_block[inode_area[0].i_blocks] = alloc_block();
			inode_area[0].i_blocks++;
			reload_dir(inode_area[0].i_block[inode_area[0].i_blocks - 1]);
			tmpno = dir[0].inode = get_inode();
			dir[0].name_len = strlen(tmp);
			dir[0].file_type = type;
			strcpy(dir[0].name, tmp);
			// ��ʼ���¿������Ŀ¼��
			for (flag = 1; flag < 32; flag++)
			{
				dir[flag].inode = 0;
			}
			update_dir(inode_area[0].i_block[inode_area[0].i_blocks - 1]);
		}
		inode_area[0].i_size += 16;

		update_inode_entry(current_dir);//д�������ڵ�

		// Ϊ����Ŀ¼���� dir_entry
		dir_prepare(tmpno, strlen(tmp), type);
	}
	else  // �Ѿ�����ͬ���ļ���Ŀ¼
	{
		printf("��Ŀ¼�Ѵ���!\n");
	}

}



//ɾ��һ����Ŀ¼
void rmdir(char tmp[9])
{
	unsigned short i, j, k, flag;
	unsigned short m, n;
	if (!strcmp(tmp, "..") || !strcmp(tmp, "."))//������ɾ��. ��..Ŀ¼
	{
		printf("��Ŀ¼���ɱ�ɾ��!\n");
		return;
	}
	flag = reserch_file(tmp, 2, &i, &j, &k);
	if (flag)
	{
		reload_inode_entry(dir[k].inode); // ����Ҫɾ���Ľڵ�
		if (inode_area[0].i_size == 32)  // ֻ��.and ..
		{
			inode_area[0].i_size = 0;
			inode_area[0].i_blocks = 0;

			remove_block(inode_area[0].i_block[0]);
			// ���� tmp ���ڸ�Ŀ¼
			reload_inode_entry(current_dir);
			reload_dir(inode_area[0].i_block[j]);
			remove_inode(dir[k].inode);
			dir[k].inode = 0;
			update_dir(inode_area[0].i_block[j]);
			inode_area[0].i_size -= 16;
			flag = 0;

			/*ɾ��32 �� dir_entry ȫΪ�յ����ݿ�
			���� inode_area[0].i_block[0] ����Ŀ¼ . �� ..
			����������ݿ�ķǿ� dir_entry ������Ϊ0*/

			//printf("rm: %d\n",inode_area[0]);
			m = 1;
			while (flag < 32 && m < inode_area[0].i_blocks)
			{
				flag = n = 0;
				reload_dir(inode_area[0].i_block[m]);
				while (n < 32)
				{
					if (!dir[n].inode)
					{
						flag++;
					}
					n++;
				}
				//���ɾ�������������ݿ��Ŀ¼��ȫ��Ϊ�ա���������������ɾ��ĳһ��λ��
				if (flag == 32)
				{
					remove_block(inode_area[0].i_block[m]);
					inode_area[0].i_blocks--;
					while (m < inode_area[0].i_blocks)
					{
						inode_area[0].i_block[m] = inode_area[0].i_block[m + 1];
						++m;
					}
				}
			}
			update_inode_entry(current_dir);
			return;
		}
		else
		{
			//printf("%d\n",inode_area[0].i_size);
			for (int l = 0; l < inode_area[0].i_blocks; l++)
			{
				reload_dir(inode_area[0].i_block[l]);
				for (int m = 0; m < 32; m++)
				{
					if (!strcmp(dir[m].name, ".") || !strcmp(dir[m].name, "..") || dir[m].inode == 0)
						continue;
					if (dir[m].file_type == 2)
					{
						strcpy(current_path, tmp);
						current_dir = i;
						// printf("%d\n",current_dir);
						 //printf("%s\n",dir[m].name);
						rmdir(dir[m].name);
					}
					else if (dir[m].file_type == 1)
					{
						//printf("%s\n",dir[m].name);
						del(dir[m].name);
					}
				}
				//printf("%d\n",inode_area[0].i_size);
				if (inode_area[0].i_size == 32)
				{
					strcpy(current_path, "[root@");
					current_dir = 1;
					//printf("tmp: %s\n",tmp);
					rmdir(tmp);
				}
			}
			return;
			printf("��Ŀ¼Ϊ�ǿ�Ŀ¼!\n");
		}
	}
	else
	{
		printf("��Ŀ¼������!\n");
	}
}



//�鿴Ŀ¼�µ�����
void ls(void)
{
	printf("items          type           mode           size\n"); /* 15*4 */
	unsigned short i, j, k, flag;
	i = 0;
	reload_inode_entry(current_dir);
	while (i < inode_area[0].i_blocks)
	{
		k = 0;
		reload_dir(inode_area[0].i_block[i]);
		while (k < 32)
		{
			if (dir[k].inode)
			{
				printf("%s", dir[k].name);
				if (dir[k].file_type == 2)
				{
					j = 0;
					reload_inode_entry(dir[k].inode);
					if (!strcmp(dir[k].name, ".."))
					{
						while (j++ < 13)
						{
							printf(" ");
						}
						flag = 1;
					}
					else if (!strcmp(dir[k].name, "."))
					{
						while (j++ < 14)
						{
							printf(" ");
						}
						flag = 0;
					}
					else
					{
						while (j++ < 15 - dir[k].name_len)
						{
							printf(" ");
						}
						flag = 2;
					}
					printf("<DIR>          ");
					switch (inode_area[0].i_mode & 7)
					{
					case 1:printf("____x"); break;
					case 2:printf("__w__"); break;
					case 3:printf("__w_x"); break;
					case 4:printf("r____"); break;
					case 5:printf("r___x"); break;
					case 6:printf("r_w__"); break;
					case 7:printf("r_w_x"); break;
					}
					if (flag != 2)
					{
						printf("          ----");
					}
					else
					{
						printf("          ");
						printf("%4ld bytes", inode_area[0].i_size);
					}
				}
				else if (dir[k].file_type == 1)
				{
					j = 0;
					reload_inode_entry(dir[k].inode);
					while (j++ < 15 - dir[k].name_len)printf(" ");
					printf("<FILE>         ");
					switch (inode_area[0].i_mode & 7)
					{
					case 1:printf("____x"); break;
					case 2:printf("__w__"); break;
					case 3:printf("__w_x"); break;
					case 4:printf("r____"); break;
					case 5:printf("r___x"); break;
					case 6:printf("r_w__"); break;
					case 7:printf("r_w_x"); break;
					}
					printf("          ");
					printf("%4ld bytes", inode_area[0].i_size);
				}
				printf("\n");
			}
			k++;
			reload_inode_entry(current_dir);
		}
		i++;
	}
}



//�����ļ�
void touch(char tmp[9], int type)
{
	unsigned short tmpno, i, j, k, flag;
	reload_inode_entry(current_dir);
	if (!reserch_file(tmp, type, &i, &j, &k))//���������ļ�
	{
		if (inode_area[0].i_size == 4096)//��������ڵ㻺����
		{
			printf("��Ŀ¼����!\n");
			return;
		}
		flag = 1;
		if (inode_area[0].i_size != inode_area[0].i_blocks * 512)
		{
			i = 0;
			while (flag && i < inode_area[0].i_blocks)
			{
				reload_dir(inode_area[0].i_block[i]);
				j = 0;
				while (j < 32)
				{
					if (dir[j].inode == 0)//�ҵ���δ�����Ŀ¼��
					{
						flag = 0;
						break;
					}
					j++;
				}
				i++;
			}
			tmpno = dir[j].inode = get_inode();//����һ���µ�inode��
			dir[j].name_len = strlen(tmp);
			dir[j].file_type = type;
			strcpy(dir[j].name, tmp);
			update_dir(inode_area[0].i_block[i - 1]);
		}
		else //����һ���µ����ݿ�
		{
			inode_area[0].i_block[inode_area[0].i_blocks] = alloc_block();
			inode_area[0].i_blocks++;
			reload_dir(inode_area[0].i_block[inode_area[0].i_blocks - 1]);
			tmpno = dir[0].inode = get_inode();
			dir[0].name_len = strlen(tmp);
			dir[0].file_type = type;
			strcpy(dir[0].name, tmp);
			//��ʼ���¿�������ĿΪ0
			for (flag = 1; flag < 32; flag++)
			{
				dir[flag].inode = 0;
			}
			update_dir(inode_area[0].i_block[inode_area[0].i_blocks - 1]);
		}
		inode_area[0].i_size += 16;
		update_inode_entry(current_dir);
		//�������ļ���inode�ڵ��ʼ��
		dir_prepare(tmpno, strlen(tmp), type);

	}
	else
	{
		printf("���ļ��ѱ�����!\n");
	}
}





//ɾ���ļ�
void del(char tmp[9])
{
	unsigned short i, j, k, m, n, flag;
	m = 0;
	flag = reserch_file(tmp, 1, &i, &j, &k);
	if (flag)//������ļ�����ɾ��
	{
		flag = 0;
		// ���ļ� tmp �Ѵ�, �򽫶�Ӧ�� fopen_table ����0
		while (fopen_table[flag] != dir[k].inode && flag < 16)
		{
			flag++;
		}
		if (flag < 16)
		{
			fopen_table[flag] = 0;
		}
		reload_inode_entry(i); // ����ɾ���ļ� inode
		
		//ɾ���ļ���Ӧ�����ݿ�
		while (m < inode_area[0].i_blocks)
		{
			remove_block(inode_area[0].i_block[m++]);//ѭ��ɾ�����ݿ�
		}
		inode_area[0].i_blocks = 0;
		inode_area[0].i_size = 0;
		remove_inode(i);
		// ���¸�Ŀ¼
		reload_inode_entry(current_dir);
		reload_dir(inode_area[0].i_block[j]);
		dir[k].inode = 0; //ɾ��inode�ڵ�

		update_dir(inode_area[0].i_block[j]);
		inode_area[0].i_size -= 16;
		m = 1;
		//ɾ��һ����������ݿ�Ϊ�գ��򽫸����ݿ�ɾ��
		while (m < inode_area[i].i_blocks)
		{
			flag = n = 0;
			reload_dir(inode_area[0].i_block[m]);
			while (n < 32)
			{
				if (!dir[n].inode)
				{
					flag++;
				}
				n++;
			}
			if (flag == 32)
			{
				remove_block(inode_area[i].i_block[m]);
				inode_area[i].i_blocks--;
				while (m < inode_area[i].i_blocks)
				{
					inode_area[i].i_block[m] = inode_area[i].i_block[m + 1];
					++m;
				}
			}
		}
		update_inode_entry(current_dir);
	}
	else
	{
		printf("�ļ� %s ������!\n", tmp);
	}
}




//���ļ�
void open_file(char tmp[9])
{
	unsigned short flag, i, j, k;
	flag = reserch_file(tmp, 1, &i, &j, &k);
	if (flag)
	{
		if (search_file(dir[k].inode))//�����ļ��򿪱�
		{
			printf("�ļ� %s �ѱ��򿪣��벻Ҫ�ظ���!\n", tmp);
		}
		else
		{
			flag = 0;
			while (fopen_table[flag])//���ļ��򿪱��Ͻ��б��
			{
				flag++;
			}
			fopen_table[flag] = dir[k].inode;//������ļ��Ľڵ�
			printf("�ļ� %s �ѳɹ���!\n", tmp);
		}
	}
	else printf("�ļ� %s ������!\n", tmp);
}



//�ر��ļ�
void close_file(char tmp[9])
{
	unsigned short flag, i, j, k;
	flag = reserch_file(tmp, 1, &i, &j, &k);

	if (flag)
	{
		if (search_file(dir[k].inode))//�����ļ��򿪱�
		{
			flag = 0;
			while (fopen_table[flag] != dir[k].inode)
			{
				flag++;
			}
			fopen_table[flag] = 0;
			printf("�ļ� %s �ѹر�!\n", tmp);
		}
		else
		{
			printf("�ļ� %s ��δ���򿪣�����ر�!\n", tmp);
		}
	}
	else
	{
		printf("�ļ� %s ������!\n", tmp);
	}
}





// ���ļ�
void read_file(char tmp[9])
{
	unsigned short flag, i, j, k, t;
	flag = reserch_file(tmp, 1, &i, &j, &k);
	if (flag)
	{
		if (search_file(dir[k].inode)) //���ļ���ǰ���Ǹ��ļ��Ѿ���
		{
			reload_inode_entry(dir[k].inode);
			//�ж��Ƿ��ж���Ȩ��
			if (!(inode_area[0].i_mode & 4)) // i_mode:111b:��,д,ִ��
			{
				printf("�ļ� %s Ϊ���ɶ�״̬!\n", tmp);
				return;
			}
			for (flag = 0; flag < inode_area[0].i_blocks; flag++)
			{
				reload_block(inode_area[0].i_block[flag]);
				for (t = 0; t < inode_area[0].i_size - flag * 512; ++t)//���ļ��������
				{
					printf("%c", Buffer[t]);
				}
			}
			if (flag == 0)
			{
				printf("�ļ� %s �ǿյ�!\n", tmp);
			}
			else
			{
				printf("\n");
			}
		}
		else
		{
			printf("�ļ� %s ���ɴ�!\n", tmp);
		}
	}
	else printf("�ļ� %s ������!\n", tmp);
}



//�ļ��Ը��Ƿ�ʽд��
void write_file(char tmp[9]) // д�ļ�
{
	unsigned short flag, i, j, k, size = 0, need_blocks, length;
	flag = reserch_file(tmp, 1, &i, &j, &k);//���ҵ�ǰĿ¼
	if (flag)
	{
		if (search_file(dir[k].inode))//�����ļ��򿪱�
		{
			reload_inode_entry(dir[k].inode);
			if (!(inode_area[0].i_mode & 2)) // i_mode:111b:��,д,ִ��
			{
				printf("�ļ� %s Ϊ����д״̬\n", tmp);
				return;
			}
			fflush(stdin);
			while (1)
			{
				tempbuf[size] = getchar();
				if (tempbuf[size] == '*')
				{
					tempbuf[size] = '\0';
					break;
				}
				if (size >= 4095)
				{
					printf("�ļ����������ֵΪ 4KB����ʡ�ŵ���qwq!\n");
					break;
				}
				size++;
			}
			if (size >= 4095)
			{
				length = 4096;
			}
			else
			{
				length = strlen(tempbuf);
			}
			//������Ҫ�����ݿ���Ŀ
			need_blocks = length / 512;//ÿ���С512������ȡ��
			if (length % 512)
			{
				need_blocks++;
			}

			if (need_blocks < 9) // �ļ���� 8 �� blocks(512 bytes)
			{
				// �����ļ��������Ŀ
				//��Ϊ�Ը���д�ķ�ʽд��Ҫ���ж�ԭ�е����ݿ���Ŀ
				if (inode_area[0].i_blocks <= need_blocks)
				{
					while (inode_area[0].i_blocks < need_blocks)
					{
						inode_area[0].i_block[inode_area[0].i_blocks] = alloc_block();
						inode_area[0].i_blocks++;
					}
				}
				else
				{
					while (inode_area[0].i_blocks > need_blocks)
					{
						remove_block(inode_area[0].i_block[inode_area[0].i_blocks - 1]);
						inode_area[0].i_blocks--;
					}
				}
				j = 0;
				while (j < need_blocks)
				{
					if (j != need_blocks - 1)
					{
						reload_block(inode_area[0].i_block[j]);
						memcpy(Buffer, tempbuf + j * BLOCK_SIZE, BLOCK_SIZE);
						update_block(inode_area[0].i_block[j]);
					}
					else
					{
						reload_block(inode_area[0].i_block[j]);
						memcpy(Buffer, tempbuf + j * BLOCK_SIZE, length - j * BLOCK_SIZE);
						inode_area[0].i_size = length;
						update_block(inode_area[0].i_block[j]);
					}
					j++;
				}
				update_inode_entry(dir[k].inode);
			}
			else
			{
				printf("�ļ����������ֵΪ 4KB����ʡ�ŵ���qwq!\n");
			}
		}
		else
		{
			printf("�ļ� %s δ�򿪣������open������ļ�\n", tmp);
		}
	}
	else
	{
		printf("�ļ� %s ������!\n", tmp);
	}
}
//static void ifle_reload_inode_entry(unsigned short i) // ����i��inode
//{
//	fseek(fp, INODE_TABLE_START + (i - 1) * INODE_SIZE, SEEK_SET);
//	fread(inode_area, INODE_SIZE, 1, fp);
//}


void file_finder(char tmp[9],int *file_flag)//�����ļ��Ĺ��ߺ�������ݹ�������Ե����ó�
{
	unsigned short i, j, k;
	i = 0;

	reload_inode_entry(current_dir);
	while (i < inode_area[0].i_blocks)
	{
		k = 0;
		reload_dir(inode_area[0].i_block[i]);
		while (k < 32)
		{
			if (dir[k].inode)
			{
				printf("%s\n", dir[k].name);
				//printf("\n\n");
				////ls();
				//printf("\n\n");
				if (dir[k].file_type == 2)
				{
					if (strcmp(dir[k].name, "..") && strcmp(dir[k].name, "."))
					{
						cd(dir[k].name);
						file_finder(tmp, file_flag);//���еݹ�����
						if (*file_flag == 1)
						{
							break;
						}
						cd("..");
						//��Ŀ¼���л�һ��Ҫע�����ظ��ֱ�����������������
						reload_inode_entry(current_dir);
						reload_dir(inode_area[0].i_block[i]);
					}

				}
				else if (dir[k].file_type == 1)
				{
					if (!strcmp(dir[k].name, tmp))
					{
						printf("find\n");
						printf("�ļ�·����  %s%s\n", current_path,tmp);
						*file_flag = 1;
					}
				}

			}

			k++;
			reload_inode_entry(current_dir);
			if (*file_flag == 1)
			{
				break;
			}
			
		}
		if (*file_flag == 1)
		{
			break;
		}
		i++;
	}

}

void find(char tmp[9])//�����ļ���ת�����ļ����ϼ�Ŀ¼
{
	int flag = 0;
	int*file_flag = &flag;
	current_dir = 1;
	file_finder(tmp, file_flag);
	if (flag == 0)
	{
		printf("δ�ҵ�\n");
	}
}





void help(void)
{
	printf("ckdisk		���Ӳ��\n");
	printf("initm		��ʼ���ڴ�\n");
	printf("format		��ʽ��Ӳ��\n");
	printf("cd		Ŀ¼��ת\n");
	printf("ls		�鿴��ǰĿ¼������\n");
	printf("mkdir		����һ����Ŀ¼\n");
	printf("rmdir		ɾ��һ����Ŀ¼\n");
	printf("touch		����һ�����ļ�\n");
	printf("open		��һ���ļ�\n");
	printf("close		�ر�һ���ļ�\n");
	printf("read		��һ���ļ�\n");
	printf("write		����ʽд���ļ������ַ�*��β��\n");
	printf("rm		ɾ��һ���ļ�\n");
	printf("find		����ָ���ļ�·����ת��������Ŀ¼\n");

}



























