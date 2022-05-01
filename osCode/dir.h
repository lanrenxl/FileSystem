#ifndef DIR_H
#define DIR_H
#define blockEnd 20000
#include "phy_block.h"

typedef unsigned int uInt;

//与fcb dir等大小的容器 用于链接空闲数据项
//根目录初始为14块
//其他目录为15块
typedef struct eqlsc{//68字节
    struct eqlsc* P_before; //上一个空闲数据项的指针
    int num;                //当前空闲数据项所属的块号
    char data[56];          
    struct eqlsc* P_after;  //下一个空闲数据项的指针
}eqlSizeContainer;

typedef struct FCB//68字节
{
    u16 fcbBlock;           //fcb所在物理块
    u16 firstBlock;         //第一个物理块号
    u16 lastBlock;          //最后一个物理块号

	char name[11];          //文件名
    char nameLen;
    char last[5];           //后缀名
    char lastLen;

    char createTime[4];    //创建文件
    int createTime_year;
	char lastWriteTime[4]; //文件最后修改时间
    int lastWriteTime_year;
    uInt FileSize;          //文件的大小 字节为单位

	myBOOL rw;                //文件可读可写 rw=1时可读可写, rw=0时仅可读
    myBOOL isShortCut;        //是否为快捷方式文件
    u16 UserEnable;         //每一位对应一个用户是否可以读写

    struct FCB *preFcb;            //目录的上一个兄弟文件指针
	struct FCB *nextFcb;           //目录的下一个兄弟文件指针
    struct DIR *directory;         //文件所在目录

    uInt rPoint;            //读指针, 一字节为单位
    uInt wPoint;            //写指针, 一字节为单位
}fcb;

typedef struct DIR//68字节
{
    int dirBlock;          //DIR控制块所属的物理块
    int DIRBLOCK;          //放置该目录下文件fcb的起始物理块
    char name[11];         //目录名
    char nameLen;

    char createTime[4];    //目录的创建时间
    int createTime_year;
    char alterTime[4];     //目录的修改时间
    int alterTime_year;

    struct FCB *fcbHead;   //目录下的FCB链头
    struct FCB *fcbEnd;    //目录下的FCB链尾
    struct DIR *dirHead;   //目录下的目录链头
    struct DIR *dirEnd;    //目录下的目录链尾
    struct DIR *preDir;    //上一个兄弟目录的指针
    struct DIR *nextDir;   //下一个兄弟目录的指针
    
    eqlSizeContainer* freeHead;//空闲fcb或者dir链头

    struct DIR *parent;    //上级目录指针

}dir;

typedef struct rwData
{
    char data[1024];
    uInt size;//没用到
}rwData;

typedef struct sharedFile
{
    char path[100]; //绝对路径

}sharedFile;

//------------------------------------文件操作--------------------------------------
//创建文件
fcb* fileCreate(dir* parent, char fileName[11], char nameLen, char last[5], char lastLen, myBOOL rw, u16 UserEnable, myBOOL isShortCut);

//删除文件
myBOOL fileDelete(fcb *file);

//文件重命名
myBOOL fileRename(fcb* file, char name[11], char nameLen, char lastName[5], char lastLen);

//文件全部读取
myBOOL fileReadAll(fcb* file, char* data);

//文件全部写入
myBOOL fileWriteAll(fcb* file, char* data, int size);

//移动文件到指定文件夹
//dir* fileMove(char path[100], int pathLen, fcb* file);

//文件属性设置
myBOOL fileReset(fcb* file, myBOOL rw, u16 UserEnable);

//文件移动前半部分
myBOOL fileMovePart1(fcb* file);

//文件移动后半部分
myBOOL fileMovePart2(dir* dest);
//------------------------------------文件操作--------------------------------------

//------------------------------------目录操作--------------------------------------
//申请空闲数据项
eqlSizeContainer* eqlscGet(dir* parent);
//回收空闲数据项
void eqlscSendback(dir* parent, eqlSizeContainer* Contain, uInt index);
//创建目录
dir* dirCreate(dir *parent, char name[11], char nameLen);

//删除目录及以下的目录和文件
myBOOL dirDelete(dir *directory);

//目录重命名
myBOOL dirRename(dir* d, char name[11], char nameLen);

//查找文件
fcb* findFile(dir *directory, char name[11], char last[5]);

//打开目录
myBOOL openDir(dir* d);

//关闭目录
myBOOL closeDir(dir* d);

//从属于父亲目录的物理块中写回一个物理块blockNum
myBOOL sendBackDirBlock(dir* d, int blockNum);

//从属于父亲目录的物理块中被修改的物理块写回
void sendBackDirDirtyBlocks(dir* d);

//查找目录
dir* findDir(dir* directory, char name[11], int len);

//按照绝对路径寻找指定目录
dir* goToDir(char* path, int pathLen, dir* parent);

//从当前目录回退到根目录
void return2Root(dir* current);

//按照相对路径查找指定目录
dir* gotoDirRelative(dir *directory, char path[100], int pathLen);

//移动文件到指定文件夹
//dir* dirMove(char path[100], int pathLen, dir* d);

//目录移动前半部分
myBOOL dirMovePart1(dir* d);

//目录移动后半部分
myBOOL dirMovePart2(dir* dest);

//保存未执行移动后半部分操作的文件或目录
void tmpItemSave(void);
//------------------------------------目录操作--------------------------------------
//--------------------------------- 文件系统操作------------------------------------
dir *root;

dir* tmpDir;

fcb* tmpFcb;

//文件系统初始化
void fileSystemInit(void);

//文件系统载入
void fileSystemLoad(void);
//--------------------------------- 文件系统操作------------------------------------
#endif