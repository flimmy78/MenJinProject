/*
*********************************************************************************************************
*
*	ģ������ : ������ģ�顣
*	�ļ����� : main.c(��������ȼ���ֵԽС���ȼ�Խ�ͣ������uCOS�෴)
*   AppTaskReader����ͷ�¼����������ȼ�N����ߣ�
*   AppTaskButton�������¼����������ȼ�3
*   AppTaskNet���������񡢰���ɨ�裬���ȼ�2
*   AppTaskStart:��������ι���ȣ����ȼ�1����ͣ�
*********************************************************************************************************
*/
#include "bsp.h"			/* �ײ�Ӳ������ */
/*
**********************************************************************************************************
											��������
**********************************************************************************************************
*/
static void AppTaskCreate (void);//������start���������������
static void AppObjCreate (void);//������ʼ�������������ʱ��

//������
__task void AppTaskReader(void);

__task void AppTaskFirst(void);
__task void AppTaskMulti(void);
__task void AppTaskInterLock(void);

__task void AppTaskButton(void);
__task void AppTaskNet(void);
__task void AppTaskStart(void);

/*
**********************************************************************************************************
											 ����
**********************************************************************************************************
*/
static uint64_t AppTaskReaderStk[512/8];   /* ����ջ */

static uint64_t AppTaskFirstStk[512/8];   /* ����ջ */
static uint64_t AppTaskMultiStk[512/8];     /* ����ջ */
static uint64_t AppTaskInterLockStk[512/8];   /* ����ջ */

static uint64_t AppTaskButtonStk[512/8];   /* ����ջ */
static uint64_t AppTaskNetStk[512/8];     /* ����ջ */
static uint64_t AppTaskStartStk[512/8];   /* ����ջ */

/* ������ */
OS_TID HandleTaskReader = NULL;

OS_TID HandleTaskFirst = NULL;
OS_TID HandleTaskMulti = NULL;
OS_TID HandleTaskInterLock = NULL;

OS_TID HandleTaskButton = NULL;
OS_TID HandleTaskNet = NULL;
OS_TID HandleTaskStart = NULL;

/* ����֧��8����Ϣ����Ϣ���� */
os_mbx_declare (mailboxCardRX, 8);

os_mbx_declare (mailboxCardFirst, 8);
os_mbx_declare (mailboxCardMulti, 8);
os_mbx_declare (mailboxCardInterLock, 8);

/* ��ʱ����� */
OS_ID  OneTimerA;//�����̵���A
OS_ID  OneTimerB;//�����̵���B
/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: ��׼c������ڡ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int main(void) 
{	
	/* ��ʼ������ */
	bsp_Init();
	
	/* ������������ */
 	os_sys_init_user (AppTaskStart,             /* ������ */
	                  1,                        /* �������ȼ� */
	                  &AppTaskStartStk,         /* ����ջ */
	                  sizeof(AppTaskStartStk)); /* ����ջ��С����λ�ֽ��� */
	while(1);
}


/*
*********************************************************************************************************
*	�� �� ��: AppTaskReader
*	��Ҫ����: �����������ź�Զ�̿����¼�
*   �� �� ��: ��� 
*   ���ʱ��: ��
*********************************************************************************************************
*/
__task void AppTaskReader(void)
{
//    uint8_t type;
//    SingleReader_T *readerMsg;
    while(1)
    {
        //����Ҫ�Ĳ���
//        1.�ȴ�Τ������������������Ϣ����ת��һ�Σ�Ȼ���Լ��ٴ���
//        2.�ж϶��ֿ��Ź��ܵĿ��أ���SingleReader_T�ŵ�����������
//        3���׿�����ؿ������յ�������Ϣ�󣬽���ID���������ŷ�Ӧ����Ӽ������������ 
//        if(A)//�׿�
//        {ת����A}
//        if(B)//���ؿ�
//        {ת����B}
//        if(C)//����
//        {ת����C}
//        
//        type = searchID(readerMsg->ID);
//        if(type Ϊ������/���в�ȿ�/��)
//        {�ж�AB��ͷ��AB�̵����󣬿��Ż򱨾�}
        
//        if(os_mbx_wait(&mailboxCardRX, (void *)&readerMsg, 0xFFFF) != OS_R_TMO)
//        {
//            switch(readerMsg->readerID)
//            {
//                case e_READER_A:                    
//                    g_tReader.dataCheck(readerMsg);
//                    type = searchID(readerMsg->ID);//searchID�����ȶ����ã��ټ�⿨����û��
//                 
//                //����ʱA��B����ͬʱ��;//���ڵ����ţ�ֻ���ڹ�������£��ſ������¿���(ȷ���������ζ�ʱ���ѱ�����)
//                    if((g_tParam.systemCfg.multipleOpenCfg[0]==1 && g_tDoorStatus.doorB.switcherStatus == NC && g_tDoorStatus.doorA.switcherStatus == NC) || \
//                    (g_tParam.systemCfg.multipleOpenCfg[0]==0 && g_tDoorStatus.doorA.switcherStatus == NC))                          
//                    {
//                        if(type == e_superCardID || type == e_superPasswordID)//�������򳬼�������㿪
//                        {
//                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
//                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
//                        }
//                        if(type == e_threatCardID || type == e_threatPasswordID)//в�ȿ���в����
//                        {
//                            //�ϴ�������Ϣ
//                        }
//                        if((g_tParam.systemCfg.multipleOpenCfg[1]!=1 || g_tRunInfo.firstCardStatus == 1) && \
//                            (type == e_keyBoardID || type == e_generalCardID || type == e_fingerID))//�׿�����û�����߿�����ˢ��Ȼ��Ϳ�������ͨ���ˣ�����ȴ��׿�
//                        {
//                        
//                        }
//                        if(type == e_firstCardID)//�׿�
//                        {
//                            g_tRunInfo.firstCardStatus = 1;//�׿���ˢ�����������Լ���ˢ
//                        }
//                        if(type == e_firstCardID)//���ؿ�
//                        {
//                            g_tRunInfo.multipleCardStatus++;
//                            if(g_tRunInfo.multipleCardStatus == (g_tParam.systemCfg.multipleOpenCfg[2]>>4))
//                            {
//                                g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
//                                OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
//                            }
//                        }
//                        
//                    }
//                    //TODO:�洢��¼��spi
//                    storeRecord(readerMsg->ID, e_READER_A);
//                    //�ѿ����ϴ���������
//                    SendDataToServer(0x06, 0, g_tReader.readerA.ID, 3);
//                    break;
//                
//                case e_READER_B:
//                    //ͬ��

//                    break;
                
//                default:
//                    break;
//            }//end of switch
//                
//        }//end of if
    }//end of while
}

//�����׿�������
__task void AppTaskFirst(void)
{
//    uint8_t type;
    SingleReader_T *readerMsg;
    while(1)
    {
        if(os_mbx_wait(&mailboxCardFirst, (void *)&readerMsg, 0xFFFF) != OS_R_TMO)
        {
            switch(readerMsg->readerID)
            {
                case e_READER_A: 
                    break;
                
                case e_READER_B:
                    break;
                
                default:
                    break;
            }//end of switch
        }//if
    }//end of while
}

//�������ؿ�������
__task void AppTaskMulti(void)
{
//    uint8_t type;
    SingleReader_T *readerMsg;
    while(1)
    {
        if(os_mbx_wait(&mailboxCardMulti, (void *)&readerMsg, 0xFFFF) != OS_R_TMO)
        {
            switch(readerMsg->readerID)
            {
                case e_READER_A: 
                    break;
                
                case e_READER_B:
                    break;
                
                default:
                    break;
            }//end of switch
        }//if
    }//end of while
}


//��������������
__task void AppTaskInterLock(void)
{
//     uint8_t type;
    SingleReader_T *readerMsg;
    while(1)
    {
        if(os_mbx_wait(&mailboxCardInterLock, (void *)&readerMsg, 0xFFFF) != OS_R_TMO)
        {
            switch(readerMsg->readerID)
            {
                case e_READER_A: 
                    break;
                
                case e_READER_B:
                    break;
                
                default:
                    break;
            }//end of switch
        }//if
    }//end of while
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskButton
*	��Ҫ����: �����������ź�Զ�̿����¼�
*   �� �� ��: 3 
*   ���ʱ��: ��
*********************************************************************************************************
*/
__task void AppTaskButton(void)
{
    uint16_t ret_flags;
    SingleRelation_T t_remoteOpen;
    while(1)
    {
        //���û�ȴ���ʽ;�˳�ǰbitλ�����
        if(os_evt_wait_or(BIT_ALL, 0xFFFF) == OS_R_EVT)
        {
            ret_flags = os_evt_get();//����ֵΪ��Ӧ��bit
        
            switch(ret_flags)
            {
                case REMOTE_OPEN_BIT://Զ�̿���,���ð������ŷ�ʽ
                    t_remoteOpen.button_switcher = g_tRunInfo.remoteOpen;
                    if(g_tDoorStatus.doorA.switcherStatus == NC)
                    {
                        g_tDoorStatus.openDoor(&t_remoteOpen, e_BUTTON_A);
                        OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);  /* ��ʱ1000*n��ϵͳʱ�ӽ��� ��1�ǻص������Ĳ��������������ֲ�ͬ�Ķ�ʱ�� */
                    }
                    break;
                
                case BUTTON_A_BIT://����A
                    if(g_tDoorStatus.doorA.switcherStatus == NC)
                    {
                        g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_BUTTON_A);
                        OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);  /* ��ʱ1000*n��ϵͳʱ�ӽ��� ��1�ǻص������Ĳ��������������ֲ�ͬ�Ķ�ʱ�� */
                    }
                    break;
                
                case BUTTON_B_BIT://����B
                    if(g_tDoorStatus.doorB.switcherStatus == NC)
                    {
                        g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_BUTTON_B);
                        OneTimerB = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 3);  /* ��ʱ1000*n��ϵͳʱ�ӽ��� ��1�ǻص������Ĳ��������������ֲ�ͬ�Ķ�ʱ�� */
                    }
                    break; 
                
                default:
                    break;
            }//end of switch
        }//end of if
    }
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskNet
*	����˵��: ���紦�����񣬸��Ӱ���ɨ��,��ȡ��������	
*   �� �� ��: 2  
*   ���ʱ��: 100ms
*********************************************************************************************************
*/
__task void AppTaskNet(void)
{
    static uint16_t timesA, timesB;
    while(1)
    {
        KEY_Scan();//�������
        g_tDoorStatus.readFeedBack();//��ȡ��������
        //����ŷ����ǿ����Ҽ̵����ǹصģ���ôN�����ñ�����ֱ��״̬��
        //���ݼ�⵽�Ķ�ͷ״̬����������δ���ӵģ�ֻ�������ϵı���
        if(g_tReader.readerA.status == 1)
        {
            if(g_tDoorStatus.doorA.feedBackStatus == NO && g_tDoorStatus.doorA.switcherStatus == NC)
            {
                timesA++;
                if(timesA >= g_tParam.systemCfg.waitTime*10)
                {
                    if(timesA == g_tParam.systemCfg.waitTime*10*25)
                    {
                        timesA = g_tParam.systemCfg.waitTime*10;//����times���
                    }
                    //TODO:alram������os_dly�ͷ�ת
                    //���ö�ͷ�ϵķ�������led
                }
            }
            else timesA = 0;
        }
        if(g_tReader.readerB.status == 1)
        {
            if(g_tDoorStatus.doorB.feedBackStatus == NO && g_tDoorStatus.doorB.switcherStatus == NC)
            {
                timesB++;
                if(timesB >= g_tParam.systemCfg.waitTime*10)
                {
                    if(timesB == g_tParam.systemCfg.waitTime*10*25)
                    {
                        timesB = g_tParam.systemCfg.waitTime*10;//����times���
                    }
                    //TODO:alram������os_dly�ͷ�ת
                    //���ö�ͷ�ϵķ�������led
                }
            }
            else timesB = 0;
        }
        
        if(g_tNetData.status == e_Link)
        {
            /*Socket״̬����MCUͨ����Sn_SR(0)��ֵ�����ж�SocketӦ�ô��ں���״̬*/
            switch(getSn_SR(0))	// ��ȡsocket0��״̬
            {
            case SOCK_UDP:		// Socket���ڳ�ʼ�����(��)״̬
                if(getSn_IR(0) & Sn_IR_RECV)
                {
                    setSn_IR(0, Sn_IR_RECV);// Sn_IR��RECVλ��1
                }
                //���ݽ���
                g_tNetData.len = getSn_RX_RSR(0);
                if((g_tNetData.len>0) && (g_tNetData.len <= (DATA_LEN+8)))
                { 
                    g_tNetData.len -= 8;//ȥ��8�ֽڳ��ȵ�ipͷ��Ϣ
                    // W5500��������Զ����λ�������ݣ���ͨ��SPI���͸�MCU
                    recvfrom(0, g_tNetData.buf, g_tNetData.len, g_tParam.netCfg.server_ip ,&g_tParam.netCfg.server_port);
                    DEBUG(COM1, g_tNetData.buf, g_tNetData.len);
                    processCommand(g_tNetData.buf, g_tNetData.len);
                }
                break;
                
            case SOCK_CLOSED:// Socket���ڹر�״̬
                socket(0, Sn_MR_UDP, g_tParam.netCfg.local_port, 0);	// ��Socket0��������ΪUDPģʽ����һ�����ض˿�
                break; 
            
            default:
                break;         
            }//end of switch
        }
        os_dly_wait(100);
    }
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskStart
*	��Ҫ����: ������������ι������DS1302ʱ�䣬����״̬
*   �� �� ��: 1 
*   ���ʱ��: 15s
*********************************************************************************************************
*/
__task void AppTaskStart(void)
{
    uint8_t data=0;
    
    /* ��ȡ��������ľ�� */
	HandleTaskStart = os_tsk_self();
    //ͨ��start�����ӵش�����������
	AppTaskCreate();
    /* ��������ͨ�Ż��� */
	AppObjCreate();
	
    while(1)
    {
        IWDG_Feed();//ι��
        
        ds1302_readtime(g_tRunInfo.time, 5);//��ȡʱ��
        //�ж��Ƿ�0�㣬��Ҫ��λ�׿����߶��ؿ���״̬
        if(g_tRunInfo.time[3] == 0)
        {
            //״̬��0
            g_tRunInfo.firstCardStatus = 0;
            g_tRunInfo.multipleCardStatus = 0;
        }
 
        //��תϵͳ״̬��
        bsp_LedToggle(3);
        os_dly_wait(50);
        bsp_LedToggle(3);
        //��ȡ����״̬
        wiz_read_buf(PHYCFGR, &data, 1);//��ȡphy���ж������Ƿ���
        if(data & 0x01 == 1)
        {
            g_tNetData.status = e_Link;//���߲����
            //��������������
            data = g_tDoorStatus.doorA.feedBackStatus;
            data = g_tDoorStatus.doorB.feedBackStatus << 4;
            SendDataToServer(0, 0, &data, 1);//���������� 
        }
        else g_tNetData.status = e_NoLink;//����û���
        
        os_dly_wait(15000);
    }
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
    HandleTaskReader = os_tsk_create_user(AppTaskReader,              /* ������ */ 
                                       4,                       /* �������ȼ� */ 
                                       &AppTaskReaderStk,          /* ����ջ */
                                       sizeof(AppTaskReaderStk));  /* ����ջ��С����λ�ֽ��� */
    
    HandleTaskFirst = os_tsk_create_user(AppTaskFirst,              /* ������ */ 
                                       4,                       /* �������ȼ� */ 
                                       &AppTaskFirstStk,          /* ����ջ */
                                       sizeof(AppTaskFirstStk));  /* ����ջ��С����λ�ֽ��� */
    
    HandleTaskMulti = os_tsk_create_user(AppTaskMulti,              /* ������ */ 
	                                   3,                       /* �������ȼ� */ 
	                                   &AppTaskMultiStk,          /* ����ջ */
	                                   sizeof(AppTaskMultiStk));  /* ����ջ��С����λ�ֽ��� */
    
	HandleTaskInterLock = os_tsk_create_user(AppTaskInterLock,              /* ������ */ 
	                                   2,                       /* �������ȼ� */ 
	                                   &AppTaskInterLockStk,          /* ����ջ */
	                                   sizeof(AppTaskInterLockStk));  /* ����ջ��С����λ�ֽ��� */
    
    HandleTaskButton = os_tsk_create_user(AppTaskButton,              /* ������ */ 
	                                   3,                       /* �������ȼ� */ 
	                                   &AppTaskButtonStk,          /* ����ջ */
	                                   sizeof(AppTaskButtonStk));  /* ����ջ��С����λ�ֽ��� */
    
	HandleTaskNet = os_tsk_create_user(AppTaskNet,              /* ������ */ 
	                                   2,                       /* �������ȼ� */ 
	                                   &AppTaskNetStk,          /* ����ջ */
	                                   sizeof(AppTaskNetStk));  /* ����ջ��С����λ�ֽ��� */
}

/*
*********************************************************************************************************
*	�� �� ��: AppObjCreate
*	����˵��: ��������ͨ�Ż���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppObjCreate (void)
{
	/* ������Ϣ���� */
	os_mbx_init(&mailboxCardRX, sizeof(mailboxCardRX));
    
    os_mbx_init(&mailboxCardFirst, sizeof(mailboxCardFirst));
    os_mbx_init(&mailboxCardMulti, sizeof(mailboxCardMulti));
    os_mbx_init(&mailboxCardInterLock, sizeof(mailboxCardInterLock));
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/