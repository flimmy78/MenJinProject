/*
*********************************************************************************************************
*	                                  
*	ģ������ : �������Ź�����
*	�ļ����� : bsp_iwdg.c
*	��    �� : V1.0
*	˵    �� : IWDG��������
*	�޸ļ�¼ :
*		�汾��    ����        ����       ˵��
*		V1.0    2015-08-10   Eric2013   ��ʽ����
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_IWDG_H
#define _BSP_IWDG_H



/** ����    : �������Ź���ʼ��*/
void bsp_InitIwdg(uint32_t _ulIWDGTime);


/** ����    : ι�������Ź�*/
void IWDG_Feed(void);

#endif