#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"

extern volatile uint32_t TimingDelay;  // ���������ļ����Ǹ��ⲿȫ�ֱ���

void Delay(volatile uint32_t nTime);

#endif
