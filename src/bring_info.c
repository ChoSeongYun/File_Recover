#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fs.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "fs.h"
#define BASE_OFFSET 1024
#define BLK_SIZE 4096

super_block_t read_superblock(int fd, super_block_t sb)
{								// fd : 파일 기술자
								// sb : super_block 구조체
		// super block 정보 가져오기
		lseek(fd, BASE_OFFSET, SEEK_SET);
		read(fd, &sb, sizeof(sb));

		return sb;
}

group_desc_t read_group_des(int fd, group_desc_t gdt)
{
							// fd : 파일 기술자
							// gdt : 그룹 디스크립터 구조체.
		// 그룹 디스크립터 정보 가져오기
		lseek(fd, BASE_OFFSET*4, SEEK_SET);
		read(fd, &gdt, sizeof(gdt));

		return gdt;
}

ind_t read_inode(int fd, ind_t ino, int i, int table_block)
{
							// fd : 파일 기술자
							// ino : 아이노드 구조체
							// i : 해당 아이노드 번호
							// table_block : inode table이 위치한 블록 주소
	char buffer[4096];						
	lseek(fd, BLK_SIZE * (table_block) + 256*(i-1), SEEK_SET);
	read(fd, &ino, sizeof(ino));
	read(fd, buffer, sizeof(buffer));

	
	for(i = 0; i < 256; i++)
	{
		printf("%d",buffer[i]);
		if(i % 15 == 0)
		{
			printf("\n");
		}
	}
	
	return ino;
}

int find_delected_file_inode(ind_t ino, int i_num)
{
							// fd : 파일 기술자
							// table_block : inode table이 위치한 블록 주소
							// ind_count : 파티션의 최대 아이노드 개수(번호).

	if(ino.i_filemode != 0 && ino.i_del_time != 0)
	{
		return i_num;
	}
	return -1;
}

void read_data(int fd, int block_adr)
{
	int i;
	char buffer[BLK_SIZE];
	lseek(fd, BLK_SIZE * block_adr,SEEK_SET);
	read(fd, buffer, BLK_SIZE);

	for(i = 0; i < BLK_SIZE; i++)
	{
		if(buffer[i] >= 32 && buffer[i] <= 126)
		{
			printf("%c",buffer[i]);
		}
		else
		{
			printf(".");
		}
	}
}

void print_inode_info(ind_t ino)
{
	printf("====== inode의 정보 ======\n");
	printf("파일 타입,권한 : %d\n",ino.i_filemode);
	printf("파일 UID : %d\n",ino.i_uid);
	printf("파일 크기 : %u\n",ino.i_sizebyte);
	printf("파일 접근 시간 : %u\n",ino.i_acc_time);
	printf("파일 변경 시간 : %u\n",ino.i_chg_time);
	printf("파일 삭제 시간 : %u\n",ino.i_del_time);
	printf("파일 그룹 식별자 : %d\n",ino.i_grp_id);
	printf("파일 하드 링크 수 : %d\n",ino.i_link_cnt);
	printf("파일 블록 개수 : %u\n",ino.i_blk_cnt);
	printf("파일 플래그 : %u\n",ino.i_flags);
	printf("파일 특정 운영체제 정보 : %u\n",ino.i_os);
	printf("Block Pointer 배열 출력\n");
	for(int i = 0 ; i < 15; i++)
	{
		printf("%d ",ino.i_blk_point[i]);
	}
	printf("\n=========================\n");
}

int main()
{
	super_block_t sb;	// 슈퍼블록 구조체
	group_desc_t gdt;	// 그룹 디스크립터
	ind_t ino;			// 아이노드 정보.
	char s1[] = "extundelete ";
	char * s2 = " --restore-inode ";
	char * s3 = malloc(sizeof(char) * 50);

	unsigned int ino_table_block;	// 아이노드 테이블 블록의 위치.
	int fd;							// 파일 기술자.
	int i_num;						// 아이노드 정보.
	int del_ino_num;
	char partition[50];
	int i = 0;
	
	printf("파티션 입력 : ");
	gets(partition);
	printf("정보를 얻고 싶은 inode 번호 : ");
	scanf("%d",&i_num);

	// 파일 기술자로 파일 열기
	if((fd = open(partition,O_RDONLY)) < 0)
	{
		perror("partition");
		exit(1);
	}

	sb = read_superblock(fd, sb); 	// 슈퍼 블록 읽기
	gdt = read_group_des(fd, gdt);	// 그룹 디스크립터 읽기

	// 아이노드 정보 읽기
	ino_table_block = gdt.bg_inode_table; // 아이노드 테이블 위치 가져오기.
	ino = read_inode(fd, ino, i_num, ino_table_block);

	close(fd);

	// inode 정보 출력
	print_inode_info(ino);

	// 삭제된 파일 찾기

	del_ino_num = find_delected_file_inode(ino, i_num);
	//read_data(fd, 33795);
	//if(del_ino_num != -1)
	//{
		//sprintf(s3,"%d",del_ino_num);
		//strcat(s1,partition);
		//strcat(s1,s2);
		//strcat(s1,s3);
		//system(s1);
	//}


	free(s3);
	return 0;
}

	/*
	printf("파일 크기 : %u",ino.i_sizebyte);
	printf("파일 모드 : %u",ino.i_filemode);
	printf("전체 아이노드 수 : %u\n",sb.s_inodes_count);
	printf("전체 블록 수 : %u\n",sb.s_blocks_count);
	printf("예약된 블록의 수 : %u\n",sb.s_r_blocks_count);
	printf("사용 가능한 블록 수 : %u\n",sb.s_free_blocks_count);
	printf("사용 가능한 아이노드 수 : %u\n",sb.s_free_inodes_count);
	printf("첫 번째 데이터 블록 : %u\n",sb.s_first_data_block);
	printf("블록의 사이즈 : %d\n",sb.s_log_block_size);
	printf("inode의 사이즈 : %d\n",sb.s_inode_size);
	printf("inode 테이블 : %u\n",gdt.bg_inode_table);
	printf("쓰여진 시간 : %d\n",sb.s_wtime);
	printf("예약되지 않은 첫 번째 inode : %d\n", sb.s_first_ino);
	*/
