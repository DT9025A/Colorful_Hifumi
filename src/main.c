#include "stc8g.h"
#include "INTRINS.H"
#include "button.h"

// LED Drive Pin
sbit PWM_OUT = P3 ^ 3;

// Sin wave data
unsigned char code SinData[256] = {16, 17, 17, 17, 18, 18, 19, 19, 19, 20, 20, 21, 21, 21, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 32, 32, 32, 32, 32, 32, 32, 32, 32, 31, 31, 31, 31, 31, 30, 30, 30, 30, 29, 29, 29, 29, 28, 28, 28, 28, 27, 27, 27, 26, 26, 26, 25, 25, 25, 24, 24, 24, 23, 23, 22, 22, 22, 21, 21, 20, 20, 20, 19, 19, 18, 18, 18, 17, 17, 16, 16, 16, 15, 15, 14, 14, 14, 13, 13, 12, 12, 12, 11, 11, 10, 10, 10, 9, 9, 8, 8, 8, 7, 7, 7, 6, 6, 6, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 15, 16, 16};

// Delay cycle
unsigned char tDelay  = 1;

// PWM Comtrol threshold in [0,100]
unsigned char PWM_CNT = 0;

// Button structure
Button button1;

void Timer0_Init(void) // 20微秒@20.000MHz
{
    AUXR |= 0x80; // 定时器时钟1T模式
    TMOD &= 0xF0; // 设置定时器模式
    TL0 = 0x70;   // 设置定时初始值
    TH0 = 0xFE;   // 设置定时初始值
    TF0 = 0;      // 清除TF0标志
    TR0 = 1;      // 定时器0开始计时
}

void Delay10ms(void) //@20.000MHz
{
    unsigned char data i, j, k;

    _nop_();
    i = 2;
    j = 4;
    k = 185;
    do {
        do {
            while (--k)
                ;
        } while (--j);
    } while (--i);
}

unsigned char Button_Pin_Read(unsigned char button_id)
{
    return P54;
}

void Button1_Single_Click_Callback(void *btn)
{
    tDelay += 1;
    tDelay %= 5;
}

void main()
{
    unsigned int index  = 0;
    unsigned char delay = 0;

    P3M0 = 0;
    P3M1 = 0;
    P5M0 = 0;
    P5M1 = 0;

    PWM_OUT = 0;

    Timer0_Init();
    PWM_CNT = SinData[index++];

    button_init(&button1, Button_Pin_Read, 0, 0);
    button_attach(&button1, SINGLE_CLICK, Button1_Single_Click_Callback);
    button_start(&button1);

    ET0 = 1;
    EA  = 1;

    while (1) {
        while (delay++ < tDelay) {
            Delay10ms();
        }
        delay   = 0;
        PWM_CNT = SinData[index++];
        index %= 256;
    }
}

void TIM0_Handler() interrupt 1
{
    static unsigned char count = 0;

    if (++count <= PWM_CNT)
        PWM_OUT = 1;
    else
        PWM_OUT = 0;

    if (count == 100) {
        count = 0;
    }
    if (count % 50 == 0) {
        button_ticks();
    }
}
