#include "bsp.h"

void SendDataToServer(uint8_t flag, uint8_t rw, uint8_t *data, uint16_t len);

NetData_T g_tNetData;

//构造命令格式,把数据写入网络的buf数组中
void makeCommmand(uint8_t cmdFlag, uint8_t rw, uint8_t *data, uint16_t len)
{
    uint16_t i;
    uint8_t crcRet;
    
    g_tNetData.buf[0]=0xA5;
    g_tNetData.buf[1]=0xA5;
    g_tNetData.buf[2]=cmdFlag;
    g_tNetData.buf[3]=g_tParam.netCfg.mcuID[0];
    g_tNetData.buf[4]=g_tParam.netCfg.mcuID[1];
    g_tNetData.buf[5]=g_tParam.netCfg.mcuID[2];
    g_tNetData.buf[6]=g_tParam.netCfg.mcuID[3];
    g_tNetData.buf[7]=rw;
    g_tNetData.buf[8]=len >> 8;
    g_tNetData.buf[9]=(len & 0xFF);
    
    for(i=0; i<len;i++)
    {
        g_tNetData.buf[10+i] = data[i];
    }
    
    crcRet = Get_Crc8(g_tNetData.buf, 10+len);
    
    g_tNetData.buf[10+len+0] = crcRet;
    g_tNetData.buf[10+len+1] = 0x5A;
    g_tNetData.buf[10+len+2] = 0x5A;   
}

//执行命令,写入参数时，还要把参数更新到IIC存储中
void processCommand(uint8_t *data, uint16_t len)
{
    uint8_t ret=0;
    //uint8_t temp[512];
    //先判断ID号
    if(data[0] != 0xA5 || data[1] != 0xA5  || \
       data[3] != g_tParam.netCfg.mcuID[0] || \
       data[4] != g_tParam.netCfg.mcuID[1] || \
       data[5] != g_tParam.netCfg.mcuID[2] || \
       data[6] != g_tParam.netCfg.mcuID[3] || \
       data[len-2] != 0x5A || data[len-1] != 0x5A)
    {
        return;
    }
    
    //对于设置命令，要进行参数有效性判断
    switch(data[2])
    {
    
    case 0x00://心跳包和echo
        ret = g_tDoorStatus.doorA.feedBackStatus;
        ret = g_tDoorStatus.doorB.feedBackStatus << 4;
        SendDataToServer(data[2], 0, &ret, 1);
        break;
    
    //0x01用来主动上传刷卡的卡号和读头号
    
    case 0x02://读写实时时钟DS1302芯片的时间
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, g_tRunInfo.time, 5);
        }
        else if(data[7] == 1)
        {
            //判断时间范围
            if(data[10]<17 || data[10]>99 || \
               data[11]<=0 || data[11]>12 || \
               data[12]<=0 || data[12]>31 || \
               data[13]<=0 || data[13]>24 || \
               data[14]<=0 || data[14]>60)
            {
                ret = 0xAA;
                SendDataToServer(data[2], 1, &ret, 1);
                return;
            }
            ds1302_settime(&data[10], 5);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        }
        break;
        
    case 0x03://本地网络ip配置
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, g_tParam.netCfg.local_ip, 4);
        }
        else if(data[7] == 1)
        {
            memcpy(g_tParam.netCfg.local_ip, &data[10], 4);
            g_tParam.updateNetCfg(&g_tParam.netCfg, e_local_ip);
            //重新设置网络
            set_default(&g_tParam.netCfg);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
    
    case 0x04://本地网络port配置//重启板子生效
        if(data[7] == 0)
        {
            data[10] = g_tParam.netCfg.local_port >> 8;
            data[11] = g_tParam.netCfg.local_port & 0xFF;
            SendDataToServer(data[2], 0, &data[10], 2);
        }
        else if(data[7] == 1)
        {
            g_tParam.netCfg.local_port = (data[10]<<8) + data[10+1];
            g_tParam.updateNetCfg(&g_tParam.netCfg, e_local_port);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x05://服务器网络ip配置
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, g_tParam.netCfg.server_ip, 4);
        }
        else if(data[7] == 1)
        {
            memcpy(g_tParam.netCfg.server_ip, &data[10], 4);
            g_tParam.updateNetCfg(&g_tParam.netCfg, e_server_ip);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
    
    case 0x06://服务器网络port配置
        if(data[7] == 0)
        {
            data[10] = g_tParam.netCfg.server_port >> 8;
            data[11] = g_tParam.netCfg.server_port & 0xFF;
            SendDataToServer(data[2], 0, &data[10], 2);
        }
        else if(data[7] == 1)
        {
            g_tParam.netCfg.server_port = (data[10]<<8) + data[10+1];
            g_tParam.updateNetCfg(&g_tParam.netCfg, e_server_port);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x07://开门时长
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.systemCfg.openTime, 1);
        }
        else if(data[7] == 1)
        {
            if(data[10]==0)
            {
                ret = 0xAA;
                SendDataToServer(data[2], 1, &ret, 1);
                return;
            }
            memcpy(&g_tParam.systemCfg.openTime, &data[10], 1);
            g_tParam.updateSystemCfg(&g_tParam.systemCfg, e_openTime);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
    
    case 0x08://关门时长
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.systemCfg.waitTime, 1);
        }
        else if(data[7] == 1)
        {
            if(data[10]==0)
            {
                ret = 0xAA;
                SendDataToServer(data[2], 1, &ret, 1);
                return;
            }
            memcpy(&g_tParam.systemCfg.waitTime, &data[10], 1);
            g_tParam.updateSystemCfg(&g_tParam.systemCfg, e_waitTime);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x09://读头A关系
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.relation.relationA.reader_switcher, 1);
        }
        else if(data[7] == 1)
        {
            memcpy(&g_tParam.relation.relationA.reader_switcher, &data[10], 1);
            g_tParam.updateRelation(&g_tParam.relation, e_READER_A);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x0A://读头B关系
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.relation.relationB.reader_switcher, 1);
        }
        else if(data[7] == 1)
        {
            memcpy(&g_tParam.relation.relationB.reader_switcher, &data[10], 1);
            g_tParam.updateRelation(&g_tParam.relation, e_READER_B);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x0B://按键A关系
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.relation.relationA.button_switcher, 1);
        }
        else if(data[7] == 1)
        {
            memcpy(&g_tParam.relation.relationA.button_switcher, &data[10], 1);
            g_tParam.updateRelation(&g_tParam.relation, e_BUTTON_A);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x0C://按键B关系
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.relation.relationB.button_switcher, 1);
        }
        else if(data[7] == 1)
        {
            memcpy(&g_tParam.relation.relationB.button_switcher, &data[10], 1);
            g_tParam.updateRelation(&g_tParam.relation, e_BUTTON_B);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
//8个开门方式的配置        
    case 0x0D://互锁
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.systemCfg.multipleOpenCfg[0], 1);
        }
        else if(data[7] ==1 )
        {
            //互锁、首卡、多重卡不能同时开启
            if(data[10]>1 || \
                g_tParam.systemCfg.multipleOpenCfg[1]==1 || \
                g_tParam.systemCfg.multipleOpenCfg[2]==1)
            {
                ret = 0xAA;
                SendDataToServer(data[2], 1, &ret, 1);
                return;
            }
            memcpy(&g_tParam.systemCfg.multipleOpenCfg[0], &data[10], 1);
            g_tParam.updateSystemCfg(&g_tParam.systemCfg, e_interlock);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x0E://首卡
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.systemCfg.multipleOpenCfg[1], 1);
        }
        else if(data[7] == 1)
        {
            //互锁、首卡、多重卡不能同时开启
            if(data[10]>1 || \
                g_tParam.systemCfg.multipleOpenCfg[0]==1 || \
                g_tParam.systemCfg.multipleOpenCfg[2]==1)
            {
                ret = 0xAA;
                SendDataToServer(data[2], 1, &ret, 1);
                return;
            }
            memcpy(&g_tParam.systemCfg.multipleOpenCfg[1], &data[10], 1);
            g_tParam.updateSystemCfg(&g_tParam.systemCfg, e_firstCard);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x0F://多重卡
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.systemCfg.multipleOpenCfg[2], 1);
        }
        else if(data[7] == 1)
        {
            //互锁、首卡、多重卡不能同时开启
            if((data[10]>>4)<2 || (data[10]>>4)>10 ||(data[10]&0x0F)>1 || \
                g_tParam.systemCfg.multipleOpenCfg[0]==1 || \
                g_tParam.systemCfg.multipleOpenCfg[1]==1)           
            {
                ret = 0xAA;
                SendDataToServer(data[2], 1, &ret, 1);
                return;
            }
            memcpy(&g_tParam.systemCfg.multipleOpenCfg[2], &data[10], 1);
            g_tParam.updateSystemCfg(&g_tParam.systemCfg, e_multipleCard);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x10://超级卡
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.systemCfg.multipleOpenCfg[3], 1);
        }
        else if(data[7] == 1)
        {
            if(data[10]>1)
            {
                ret = 0xAA;
                SendDataToServer(data[2], 1, &ret, 1);
                return;
            }
            memcpy(&g_tParam.systemCfg.multipleOpenCfg[3], &data[10], 1);
            g_tParam.updateSystemCfg(&g_tParam.systemCfg, e_superCard);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x11://超级密码
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.systemCfg.multipleOpenCfg[4], 1);
        }
        else if(data[7] == 1)
        {
            if(data[10]>1)
            {
                ret = 0xAA;
                SendDataToServer(data[2], 1, &ret, 1);
                return;
            }
            memcpy(&g_tParam.systemCfg.multipleOpenCfg[4], &data[10], 1);
            g_tParam.updateSystemCfg(&g_tParam.systemCfg, e_superPassword);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x12://胁迫卡
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.systemCfg.multipleOpenCfg[5], 1);
        }
        else if(data[7] == 1)
        {
            if(data[10]>1)
            {
                ret = 0xAA;
                SendDataToServer(data[2], 1, &ret, 1);
                return;
            }
            memcpy(&g_tParam.systemCfg.multipleOpenCfg[5], &data[10], 1);
            g_tParam.updateSystemCfg(&g_tParam.systemCfg, e_threatCard);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x13://胁迫码
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.systemCfg.multipleOpenCfg[6], 1);
        }
        else if(data[7] == 1)
        {
            if(data[10]>1)
            {
                ret = 0xAA;
                SendDataToServer(data[2], 1, &ret, 1);
                return;
            }
            memcpy(&g_tParam.systemCfg.multipleOpenCfg[6], &data[10], 1);
            g_tParam.updateSystemCfg(&g_tParam.systemCfg, e_threatPassword);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x14://指纹模块
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.systemCfg.multipleOpenCfg[7], 1);
        }
        else if(data[7] == 1)
        {
            if(data[10]>1)
            {
                ret = 0xAA;
                SendDataToServer(data[2], 1, &ret, 1);
                return;
            }
            memcpy(&g_tParam.systemCfg.multipleOpenCfg[7], &data[10], 1);
            g_tParam.updateSystemCfg(&g_tParam.systemCfg, e_finger);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;

//9个卡号的配置       
    case 0x15://首卡
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, g_tParam.multipleCardID.firstCardID, 3);
        }
        else if(data[7] == 1)
        {
            memcpy(g_tParam.multipleCardID.firstCardID, &data[10], 3);
            g_tParam.updateMultipleCardID(g_tParam.multipleCardID.firstCardID, 3, e_firstCardID);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        }
        break;
        
    case 0x16://超级卡
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, g_tParam.multipleCardID.superCardID, 3);
        }
        else if(data[7] == 1)
        {
            memcpy(g_tParam.multipleCardID.superCardID, &data[10], 3);
            g_tParam.updateMultipleCardID(g_tParam.multipleCardID.superCardID, 3, e_superCardID);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        }
        break;
        
    case 0x17://超级密码
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, g_tParam.multipleCardID.superPasswordID, 3);
        }
        else if(data[7] == 1)
        {
            memcpy(g_tParam.multipleCardID.superPasswordID, &data[10], 3);
            g_tParam.updateMultipleCardID(g_tParam.multipleCardID.superPasswordID, 3, e_superPasswordID);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        }
        break;
        
    case 0x18://胁迫卡,3张卡
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, g_tParam.multipleCardID.threatCardID, 9);
        }
        else if(data[7] == 1)
        {
            memcpy(g_tParam.multipleCardID.threatCardID, &data[10], 9);
            g_tParam.updateMultipleCardID(g_tParam.multipleCardID.threatCardID, 9, e_threatCardID);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        }
        break;
        
    case 0x19://胁迫码
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, g_tParam.multipleCardID.threatPasswordID, 3);
        }
        else if(data[7] == 1)
        {
            memcpy(g_tParam.multipleCardID.threatPasswordID, &data[10], 3);
            g_tParam.updateMultipleCardID(g_tParam.multipleCardID.threatPasswordID, 3, e_threatPasswordID);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        }
        break;
        
    case 0x1A://按键密码
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, g_tParam.multipleCardID.keyBoardID, 3);
        }
        else if(data[7] == 1)
        {
            memcpy(g_tParam.multipleCardID.keyBoardID, &data[10], 3);
            g_tParam.updateMultipleCardID(g_tParam.multipleCardID.keyBoardID, 3, e_keyBoardID);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        }
        break;
        
    case 0x1B://多重卡
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, g_tParam.multipleCardID.multipleCardID, 30);
        }
        else if(data[7] == 1)
        {
            memcpy(g_tParam.multipleCardID.multipleCardID, &data[10], 30);
            g_tParam.updateMultipleCardID(g_tParam.multipleCardID.multipleCardID, 30, e_multipleCardID);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        }
        break;
        
    case 0x1C://指纹ID，数据的第一个字节表示第几包,1+300
        if(data[7] == 0)
        {
            memcpy(&data[11], &g_tParam.multipleCardID.fingerID[data[10]*300], 300);
            SendDataToServer(data[2], 0, &data[10], 301);
        }
        else if(data[7] == 1)
        {
            memcpy(&g_tParam.multipleCardID.fingerID[data[10]*300], &data[11], 300);
            if(data[10] == 4)//0,1,2,3,4
            {
                //5个包发完
                g_tParam.updateMultipleCardID(g_tParam.multipleCardID.fingerID, 1500, e_fingerID);
            }
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        }
        break;
        
    case 0x1D://普通卡，数据的第一个字节表示第几包,1+300
        if(data[7] == 0)
        {
            memcpy(&data[11], &g_tParam.multipleCardID.generalCardID[data[10]*300], 300);
            SendDataToServer(data[2], 0, &data[10], 301);
        }
        else if(data[7] == 1)
        {
            memcpy(&g_tParam.multipleCardID.generalCardID[data[10]*300], &data[11], 300);
            if(data[10] == 4)//0,1,2,3,4
            {
                //5个包发完
                g_tParam.updateMultipleCardID(g_tParam.multipleCardID.generalCardID, 1500, e_generalCardID);
            }
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        }
        break;
        
    case 0x1E://升级文件
        //读写位用来表示第几包
        if(data[7] < 64)//spi分配前256k字节,(0--63)
        {
            if(data[7]%4 == 0)
            {
                sf_EraseSector(data[7]*1024);//扇区擦除4k字节
            }
            sf_PageWrite(&data[10], data[7]*1024, 1024);//按照每包1024字节大小顺序写入spi flash
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        }
        else
        {
            ret = 0xAA;
            SendDataToServer(data[2], 1, &ret, 1);
            return;
        }
        break;
    
    case 0x1F://重置所有参数
        resetParam();
        ret = 0x55;
        SendDataToServer(data[2], 1, &ret, 1);
        break;
    
    case 0x20://远程开门,A/B        
        g_tRunInfo.remoteOpen = data[10];//低a高b
        //发送远程开门的事件标志 
        os_evt_set(REMOTE_OPEN_BIT, HandleTaskButton);
        break;
        
    case 0x21://重启（并升级）
        ret = 0x55;
        SendDataToServer(data[2], 1, &ret, 1);
        //改写升级标志
        ee_WriteOneBytes(1, 0);//1表示需要升级,0表示在iic的首地址
        //关门放狗
        bsp_DelayMS(25000);//25>20
        break;
                      
    default:
        break;
    }
}

//把数据发送到服务器
void SendDataToServer(uint8_t flag, uint8_t rw, uint8_t *data, uint16_t len)
{
    if(g_tNetData.status == e_Link)
    {
        makeCommmand(flag, rw, data, len);
        sendto(0, g_tNetData.buf, 10+len+3, g_tParam.netCfg.server_ip, g_tParam.netCfg.server_port);//在断网情况下调用发送会延时
    }
}

