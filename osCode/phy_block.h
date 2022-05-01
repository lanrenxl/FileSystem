#ifndef __PHY_BLOCK_H__
#define __PHY_BLOCK_H__
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define false 0
#define ture 1
//重申u16
typedef unsigned short u16;
typedef char myBOOL;
//创建物理块结构体
typedef struct{
    u16 P_before;
    u16 Data[510];
    u16 P_after;
} BLOCK_structure;

//声明全局变量用于调用
extern BLOCK_structure* (array_base[10000]);
extern myBOOL isDirtyBlock[10000];
extern u16 usedBlockNum;

BLOCK_structure* copyBlock(BLOCK_structure* blockPtr);
myBOOL fileReboot(void);
char* getDiskBlock(int blockNum);
myBOOL sendBackDisk(char* buf, int blockNum, myBOOL isDelete);

myBOOL ArrayInit(void);
void StackInit(void);
myBOOL StackStore(void);
myBOOL StackLoad(void);
int BlockGet(void);
myBOOL BlockSendback(int block_number);
#endif
