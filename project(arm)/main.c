#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f4xx.h"
#include "uart2.h"
#include "uart4.h"
#include "adc1.h"
#include "tim7_fnd.h"
#include "tim10_motor.h"
#include "tim4_counter.h"
#include "key.h"
#include "lcd.h"
#include "led.h"


/******************************************************************************
* General-purpose timers TIM10

    포트 연결:

    PA0,PA1 : UART4 RX,TX
    PA2,PA3 : UART2 RX,TX
    PA5 ~ PA12 : CLCD D0~D7

    PB0 ~ PB2  : CLCD RS, RW, E

    PA7 :  M_SEN(Motor Sensor)
    PB8  : DC Motor M_EN
    PB9  : DC Motor M_D2
    PB10 : DC Motor M_D1

    PC0~PC11 ==> FND A~H, C0~C3
    PC12~PC15 ==> Button0 ~ 3 

    FND 출력값
    FND12 ==> PWM
    FND34 ==> Duty rate

******************************************************************************/
#define CMD_SIZE 50
#define ARR_CNT 5  
#define PORTC_FND 

volatile int pwm = 50;
volatile int counter = 0;
extern uint16_t adc_data;
extern volatile int fndNumber;
extern volatile int adc1Flag;
extern volatile unsigned long systick_sec;            //1sec
extern volatile int systick_secFlag;
extern volatile unsigned char rx2Flag;
extern char rx2Data[50];
extern volatile unsigned char rx4Flag;
extern char rx4Data[50];
extern int key;
extern unsigned int tim1_counter;
long map(long x, long in_min, long in_max, long out_min, long out_max);
void Motor_Right();
void Motor_Left();
void Motor_Stop();
void Motor_Pwm(int val);
void window_open();
void window_close();

static void Delay_us(const uint32_t Count) 
{
__IO uint32_t index = 0;
for(index = (16 * Count); index != 0; index--); 
}

int main()
{
  int old_pwm=50;
  int adc_pwm=50;
  int ccr1;
  int pwmCount = 0;
  int vresFlag = 0;
  
  Key_Init();

#ifdef PORTC_FND
  TIM7_Fnd_Init();
#else
  PORTC_Led_Init();  
#endif  
  UART2_init();
  UART4_init();
  TIM10_Motor_Init();
  TIM4_Counter_Init();        //PORTA  사용
  GPIOAB_LCD_Init();
  lcd(0,0,"AIoT Embedded");    // 문자열 출력
  lcd(0,1,"Bluetooth Test");    // 문자열 출력
  Init_ADC1(); 
  
  Serial2_Send_String("start main()\r\n");
  pwmCount = pwm * 100;
  while(1)
  {
    if(rx2Flag)
    {
      printf("rx2Data : %s\r\n",rx2Data);

      rx2Flag = 0;
    }  
    if(rx4Flag)
    {
      Serial4_Event();
      rx4Flag = 0;
    }
    
    if(key != 0)
    {
      printf("Key : %d  \r\n",key);
      if(key == 1) //motor right
      {
        Motor_Right();
      }
      else if(key == 2) //motor left
      {
        Motor_Left();
      }
      else if(key == 3) //motor stop 
      {
        Motor_Stop();    
      }
      else if(key == 4)
      {
        if(vresFlag)
          vresFlag = 0;
        else
          vresFlag = 1;
      }
      key = 0;
    } 


#ifdef PORTC_FND
    fndNumber = pwmCount + tim1_counter%100;
#endif  

  }
}

void Serial4_Event()
{


  int i=0;
  int num = 0;
  char * pToken;
  char * pArray[ARR_CNT]={0};
  char recvBuf[CMD_SIZE]={0};
  char sendBuf[CMD_SIZE]={0}; 
  strcpy(recvBuf,rx4Data);
  i=0; 

  printf("rx4Data : %s\r\n",recvBuf);
     
  pToken = strtok(recvBuf,"[@]");

  while(pToken != NULL)
  {
    pArray[i] =  pToken;
    if(++i >= ARR_CNT)
      break;
    pToken = strtok(NULL,"[@]");
  }
  
  printf("pArray[1] : %s\r\n", pArray[1]); 

  if(!strcmp(pArray[1],"SENSOR"))               //  [KSJ_STM]SENSOR@__@__@__
  {
    if(atoi(pArray[4]) >= 25)                   //온도 25도 이상일 때 팬 on
    {
      Motor_Right();
    }
    else if(atoi(pArray[4]) <18)
    {
       Motor_Stop();
    }
    if(atoi(pArray[3])>60)                      //습도가 60보다 클 때 창문 오픈
    {
      window_open();
    }
    else if(atoi(pArray[3])<=40)
    {
      window_close();
    }
  }
  if(!strcmp(pArray[1],"LED"))
  {
    if(pArray[3] != NULL)              
      num = atoi(pArray[3]);
    else
      num = 0;
    if(!strcmp(pArray[2],"ON"))
    {
     GPIO_SetBits(GPIOC, 0x01 << num); 
    }
    else 
    {
      GPIO_ResetBits(GPIOC, 0x01 << num); 
    }
  } 
  
  else if(!strcmp(pArray[1],"MOTOR"))
  {
    if(!strcmp(pArray[2],"OPEN"))  
      Motor_Left();

    else if(!strcmp(pArray[2],"STOP")) 
      Motor_Stop();

  }
  
   else if(!strcmp(pArray[1],"WINDOW"))                         //[KSJ_STM]WINDOW@OPEN
  {
    if(!strcmp(pArray[2],"OPEN"))
    {
     window_open() ;
    }
    else 
    {
      window_close();
    }
  } 
  
  else if(!strncmp(pArray[1]," New conn",sizeof(" New conn")))
  {
    return;
  }
  else if(!strncmp(pArray[1]," Already log",sizeof(" Already log")))
  {
    return;
  }    
  else
    return;
  
  sprintf(sendBuf,"[%s]%s@%s\n",pArray[0],pArray[1],pArray[2]);
  Serial4_Send_String(sendBuf);
}

void Motor_Right()
{
  GPIO_WriteBit(GPIOB,GPIO_Pin_9,Bit_SET); 
  GPIO_WriteBit(GPIOB,GPIO_Pin_10,Bit_RESET); 
}
void Motor_Left()
{
  GPIO_WriteBit(GPIOB,GPIO_Pin_9,Bit_RESET); 
  GPIO_WriteBit(GPIOB,GPIO_Pin_10,Bit_SET);
}
void Motor_Stop()
{
  GPIO_WriteBit(GPIOB,GPIO_Pin_9,Bit_RESET);  
  GPIO_WriteBit(GPIOB,GPIO_Pin_10,Bit_RESET); 
}
void Motor_Pwm(int val)
{
    pwm = val;
}
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void window_open()
{
  for(int i = 0; i <50 ; i++ ){
GPIO_SetBits(GPIOB, GPIO_Pin_6); 
// 0.7[ms]
Delay_us(700);
GPIO_ResetBits(GPIOB, GPIO_Pin_6);;
// 19.3[ms]
Delay_us(19300); 
  }
}
void window_close()
{
  for(int i = 0; i <50 ; i++ ){
GPIO_SetBits(GPIOB, GPIO_Pin_6); 
// 1.5[ms]
Delay_us(1500);
GPIO_ResetBits(GPIOB, GPIO_Pin_6);; 
// 18.5[ms]
Delay_us(18500);
}
}