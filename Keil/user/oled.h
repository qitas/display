#ifndef __PORT_H
#define __PORT_H		


sbit LCD_CS     =P2^4;     //片选	
sbit LCD_DC     =P2^3;	  //数据/命令切换
sbit LCD_SDI    =P2^6;	  //数据
sbit LCD_SCK    =P2^5;	  //时钟
sbit LCD_REST   =P2^2;	  //复位 


  
sbit CS        =    P3^3;
sbit DOUT      =    P^1;																						   
sbit Penirq    =    P3^4;   //检测触摸屏响应信号
					  		 
#endif  ·