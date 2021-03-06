#ifndef __PARAM_H
#define __PARAM_H

#define HISTORY_START_SECTOR 64//前面00-63用来放升级程序
#define HISTORY_START_ADDR 0
#define HISTORY_END_SECTOR 512//最后的区块与地址，若等于，则该从头开始
#define HISTORY_END_ADDR 0

#define MAX_CARD_NUM 500
#define MAX_FINGER_NUM 500
#define MAX_MULTIPLE_CARD_NUM 10
#define MAX_THREAT_CARD_NUM 3

#define MAX_OPEN_CFG_NUM 8
//单个
typedef struct
{
    uint8_t reader_switcher;//读头与继电器对应关系，低4位-继电器a，高4位-继电器b，0表示无对应关系，1有
    uint8_t button_switcher;//按键与继电器对应关系，低4位-继电器a，高4位-继电器b
}SingleRelation_T; 

//所有门
typedef struct
{
    SingleRelation_T relationA;//关系A（读头A/按键A）
    SingleRelation_T relationB;//关系B（读头B/按键B）
}Relation_T;

//网络配置
typedef struct 
{   
    uint16_t local_port;// 初始化一个本地端口
    uint16_t server_port;// 配置远程服务器端口
    uint8_t local_ip[4];
    uint8_t server_ip[4];// 配置远程服务器IP地址,例如[192][168][1][100]
    uint8_t mcuID[4];//芯片的唯一ID //用作mac地址
}NetCfg_T;

//系统配置
typedef struct
{
    uint8_t openTime;//开门时长
    uint8_t waitTime;//等待时长  
    //多重卡的修订：高4位表示卡号数（2-10），低4位表示启用或不启用
    uint8_t multipleOpenCfg[MAX_OPEN_CFG_NUM];//多种开门功能是否开启，0-互锁，1-首卡，2-多重卡，3-超级卡，4-超级密码，5-胁迫卡，6-胁迫码，7-指纹模块(指纹id和按键密码id)      
}SystemCfg_T;

//历史记录的下一次写入地址
typedef struct 
{
    //第一个字节存高8位，第二个字节存低8位
    uint8_t nextStartSector[2];//下一次要写入历史记录的区块
    uint8_t nextStartAddr[2];//下一次要写入历史记录的地址(在区块中)
}NextStartAddr_T;

//记录卡号的结构体
typedef struct
{
    //9个
    uint8_t firstCardID[3];//首卡  
    uint8_t superCardID[3];//超级卡
    uint8_t superPasswordID[3];//超级密码
    uint8_t threatCardID[MAX_THREAT_CARD_NUM*3];//胁迫卡,3张
    uint8_t threatPasswordID[3];//胁迫码
    uint8_t keyBoardID[3];//按键密码ID
    uint8_t multipleCardID[MAX_MULTIPLE_CARD_NUM*3];//多重卡,10张 
    uint8_t fingerID[MAX_FINGER_NUM*3];//指纹ID
    uint8_t generalCardID[MAX_CARD_NUM*3];//普通卡
}MultipleCardID_T;

typedef struct
{
    void (*updateRelation)(Relation_T *relation, enum ReaderOrButton_Enum type);
    void (*updateNetCfg)(NetCfg_T *netCfg, enum NetCfg_Enum type);
    void (*updateSystemCfg)(SystemCfg_T *systemCfg, enum OpenCfg_Enum type);
    void (*updateNextStartAddr)(NextStartAddr_T *nextStartAddr);
    void (*updateMultipleCardID)(uint8_t *data, uint16_t len, enum ID_Enum type);
    Relation_T relation;
    NetCfg_T netCfg;
    SystemCfg_T systemCfg;
    NextStartAddr_T nextStartAddr;
    MultipleCardID_T multipleCardID;
}Param_T;

/* 供外部调用的函数声明 */
 
extern Param_T g_tParam;
void paramInit(void);//从IIC中读取参数
void resetParam(void);//重置参数为0xFF
uint8_t searchID(uint8_t *id);//检查id是否相符，能否开门
#endif

