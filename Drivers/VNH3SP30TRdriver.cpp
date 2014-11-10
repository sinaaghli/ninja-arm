#include "VNH3SP30TRdriver.h"
#include "stm32f2xx.h"
#include "stm32f2xx_rcc.h"
#include "stm32f2xx_tim.h"
#include "stm32f2xx_gpio.h"
//#include <algorithm>

uint16_t VNH3SP30TRDriver::m_nPwmPeriod = 27;

///////////////////////////////////////////////////////////////////////////////
VNH3SP30TRDriver::VNH3SP30TRDriver()
{
}

///////////////////////////////////////////////////////////////////////////////
void VNH3SP30TRDriver::Initialize(TIM_TypeDef* pPwmTimer,
                                  const uint32_t pwmTimerClk,
                                  TIM_TypeDef* pPwmTimer2,
                                  const uint32_t pwmTimerClk2,
                                  TIM_TypeDef* pPwmTimer3,
                                  const uint32_t pwmTimerClk3,
                                  const GPIO_Pin inAPin,
                                  const GPIO_Pin inBPin,
                                  const GPIO_Pin pwmPin,
                                  const GPIO_Pin inAPin2,
                                  const GPIO_Pin inBPin2,
                                  const GPIO_Pin pwmPin2,
                                  const GPIO_Pin inAPin3,
                                  const GPIO_Pin inBPin3,
                                  const GPIO_Pin pwmPin3
                                  )
{
    m_InAPin = inAPin;
    m_InBPin = inBPin;
    m_PwmPin = pwmPin;
    m_nTimer = pPwmTimer;

    m_InAPin2 = inAPin2;
    m_InBPin2 = inBPin2;
    m_PwmPin2 = pwmPin2;
    m_nTimer2 = pPwmTimer2;

    m_InAPin3 = inAPin3;
    m_InBPin3 = inBPin3;
    m_PwmPin3 = pwmPin3;
    m_nTimer3 = pPwmTimer3;

    //enable the timer clock
    RCCSetClock(pPwmTimer,pwmTimerClk, ENABLE);
    RCCSetClock(pPwmTimer2,pwmTimerClk2, ENABLE);
    RCCSetClock(pPwmTimer3,pwmTimerClk3, ENABLE);

    //initialize the control GPIOs
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);

    //enable the GPIO clocks
    RCCSetClock(inAPin.m_pGpio,inAPin.m_nGpioClk, ENABLE);
    RCCSetClock(inAPin.m_pGpio,inBPin.m_nGpioClk, ENABLE);

    RCCSetClock(inAPin2.m_pGpio,inAPin2.m_nGpioClk, ENABLE);
    RCCSetClock(inAPin2.m_pGpio,inBPin2.m_nGpioClk, ENABLE);

    RCCSetClock(inAPin3.m_pGpio,inAPin3.m_nGpioClk, ENABLE);
    RCCSetClock(inAPin3.m_pGpio,inBPin3.m_nGpioClk, ENABLE);

    //configure the GPIO pins    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;

    GPIO_InitStructure.GPIO_Pin = inAPin.m_nPin;
    GPIO_Init(inAPin.m_pGpio, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = inBPin.m_nPin;
    GPIO_Init(inBPin.m_pGpio, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = inAPin2.m_nPin;
    GPIO_Init(inBPin.m_pGpio, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = inBPin2.m_nPin;
    GPIO_Init(inBPin.m_pGpio, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = inBPin3.m_nPin;
    GPIO_Init(inBPin.m_pGpio, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = inBPin3.m_nPin;
    GPIO_Init(inBPin.m_pGpio, &GPIO_InitStructure);

    //configure the PWM pin
    //enable the GPIO clocks
    RCCSetClock(pwmPin.m_pGpio,pwmPin.m_nGpioClk, ENABLE);
    RCCSetClock(pwmPin2.m_pGpio,pwmPin2.m_nGpioClk, ENABLE);
    RCCSetClock(pwmPin3.m_pGpio,pwmPin3.m_nGpioClk, ENABLE);

    //configure the GPIO pins   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    
    //configure pin AF
    GPIO_InitStructure.GPIO_Pin = pwmPin.m_nPin;
    GPIO_Init(pwmPin.m_pGpio, &GPIO_InitStructure);
    GPIO_PinAFConfig(pwmPin.m_pGpio, pwmPin.m_nPinSource, pwmPin.m_nPinAF);

    GPIO_InitStructure.GPIO_Pin = pwmPin2.m_nPin;
    GPIO_Init(pwmPin.m_pGpio, &GPIO_InitStructure);
    GPIO_PinAFConfig(pwmPin2.m_pGpio, pwmPin2.m_nPinSource, pwmPin2.m_nPinAF);

    GPIO_InitStructure.GPIO_Pin = pwmPin3.m_nPin;
    GPIO_Init(pwmPin.m_pGpio, &GPIO_InitStructure);
    GPIO_PinAFConfig(pwmPin3.m_pGpio, pwmPin3.m_nPinSource, pwmPin3.m_nPinAF);

    //configure the timer
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);

    // Compute the prescaler value
    uint16_t prescalerValue = (uint16_t)((RCC_Clocks.PCLK2_Frequency) / 1000000) - 1;

    // Time base configuration
    TIM_TimeBaseStructure.TIM_Period = m_nPwmPeriod;
    TIM_TimeBaseStructure.TIM_Prescaler = prescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(pPwmTimer, &TIM_TimeBaseStructure);

    TIM_TimeBaseInit(pPwmTimer2, &TIM_TimeBaseStructure);

    TIM_TimeBaseInit(pPwmTimer3, &TIM_TimeBaseStructure);

    TIM_OCInitTypeDef OCInitStructure;
    TIM_OCStructInit(&OCInitStructure);

    /* PWM1 Mode configuration: Channel1 */
    OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    OCInitStructure.TIM_Pulse = 0;
    OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//TIM_OCPolarity_High;

    TIM_OC1Init(pPwmTimer, &OCInitStructure);
    TIM_OC1Init(pPwmTimer2, &OCInitStructure);
    TIM_OC1Init(pPwmTimer3, &OCInitStructure);

    TIM_OC1PreloadConfig(pPwmTimer, TIM_OCPreload_Enable);
    TIM_OC1PreloadConfig(pPwmTimer2, TIM_OCPreload_Enable);
    TIM_OC1PreloadConfig(pPwmTimer3, TIM_OCPreload_Enable);

    /* TIM3 enable counter */
    TIM_Cmd(pPwmTimer, ENABLE);
    TIM_Cmd(pPwmTimer2, ENABLE);
    TIM_Cmd(pPwmTimer3, ENABLE);
}

///////////////////////////////////////////////////////////////////////////////
void VNH3SP30TRDriver::SetMode(const VNH3SP30TRDriver::Mode eMode)
{
    switch(eMode){
        case eModeForward:
            GPIO_SetBits(m_InAPin.m_pGpio,m_InAPin.m_nPin);
            GPIO_ResetBits(m_InBPin.m_pGpio,m_InBPin.m_nPin);

//            GPIO_SetBits(m_InAPin2.m_pGpio,m_InAPin2.m_nPin);
//            GPIO_ResetBits(m_InBPin2.m_pGpio,m_InBPin2.m_nPin);

//            GPIO_SetBits(m_InAPin3.m_pGpio,m_InAPin3.m_nPin);
//            GPIO_ResetBits(m_InBPin3.m_pGpio,m_InBPin3.m_nPin);
            break;

        case eModeReverse:
            GPIO_ResetBits(m_InAPin.m_pGpio,m_InAPin.m_nPin);
            GPIO_SetBits(m_InBPin.m_pGpio,m_InBPin.m_nPin);

//            GPIO_ResetBits(m_InAPin2.m_pGpio,m_InAPin2.m_nPin);
//            GPIO_SetBits(m_InBPin2.m_pGpio,m_InBPin2.m_nPin);

//            GPIO_ResetBits(m_InAPin3.m_pGpio,m_InAPin3.m_nPin);
//            GPIO_SetBits(m_InBPin3.m_pGpio,m_InBPin3.m_nPin);
            break;

        case eModeBrakeGND:
            GPIO_ResetBits(m_InAPin.m_pGpio,m_InAPin.m_nPin);
            GPIO_ResetBits(m_InBPin.m_pGpio,m_InBPin.m_nPin);

//            GPIO_ResetBits(m_InAPin2.m_pGpio,m_InAPin2.m_nPin);
//            GPIO_ResetBits(m_InBPin2.m_pGpio,m_InBPin2.m_nPin);

//            GPIO_ResetBits(m_InAPin3.m_pGpio,m_InAPin3.m_nPin);
//            GPIO_ResetBits(m_InBPin3.m_pGpio,m_InBPin3.m_nPin);
            break;

        case eModeBrakeVCC:
            GPIO_SetBits(m_InAPin.m_pGpio,m_InAPin.m_nPin);
            GPIO_SetBits(m_InBPin.m_pGpio,m_InBPin.m_nPin);

//            GPIO_SetBits(m_InAPin2.m_pGpio,m_InAPin2.m_nPin);
//            GPIO_SetBits(m_InBPin2.m_pGpio,m_InBPin2.m_nPin);

//            GPIO_SetBits(m_InAPin3.m_pGpio,m_InAPin3.m_nPin);
//            GPIO_SetBits(m_InBPin3.m_pGpio,m_InBPin3.m_nPin);
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
void VNH3SP30TRDriver::SetSpeed(float nSpeed)
{
    //clamp the speed
    if(nSpeed < 0.05){
        nSpeed = 0;
    }
    nSpeed *= 1;  // Calibration Parameter

    //int nClampedSpeed = std::max(0,std::min((int)m_nPwmPeriod,(int)(nSpeed*(float)m_nPwmPeriod)));
    int nClampedSpeed;
    float dClampedPos;
    if((int)m_nPwmPeriod>(int)(nSpeed*(float)m_nPwmPeriod))
      nClampedSpeed = (int)(nSpeed*(float)m_nPwmPeriod);
    else
      nClampedSpeed = (int)m_nPwmPeriod;

    if(nClampedSpeed<0)
      nClampedSpeed = 0;

    
    TIM_SetCompare1(m_nTimer, nClampedSpeed);

    TIM_SetCompare1(m_nTimer2, nClampedSpeed);

    TIM_SetCompare1(m_nTimer3, nClampedSpeed);
}