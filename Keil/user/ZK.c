#include <reg51.h>
#include <intrins.h>
#include <font\font.h>
#include <sys\sys.h>
#include <lcd\lcd.h>
/******************************************************
接线方式
特别说明：由于生产时间丝印印刷偏移了一个管脚；大家在使用的特别注意一下
顺序不要接反了；给大家造成不便，非常抱歉
GND=GND
VCC=3.3V
P00=CLK
P01=MOSI
P02=RES
P03=DC
P04=CS1
BLK为LCD背光控制管脚，低电平关闭背光，高电平打开；默认可以不接打开背光
////////////////////////以上是LCD工作所需要的/////////////////////////////////////
P05=FSO
P06=CS2

******************************************************/
unsigned char  FontBuf[228];//字库缓存	
unsigned char  Font_Map[1];//转换后的字库缓存

void CL_Mem(void)
{
  unsigned char  i;
	 
	for(i=0;i<128;i++ )
	{
      Font_Map[i]=0;
//	  FontBuf[i]=0;
	  }

}
void GBZK_ROMOUTSET(void)		//字库输出设置
{		
	  	 ;

}

void GBZK_ROMOUTRESET(void)	 	//字库输出设置
{		
	  	 
}

void GBZK_GPIO_Config(void)	 //字库芯片初始化
{		
   
;


}


//------------送指令到字库IC---中景园--------

void ZK_command( unsigned char  dat )
{
    unsigned char  i;
	 
	for(i=0;i<8;i++ )
	{
		if(dat&0x80)
			{LCD_SDI=1;
			}
		else
			{LCD_SDI=0; 
			}
		dat = dat<<1;
		LCD_SCK=0;  //字库时钟拉低 
		LCD_SCK=1;  //字库时钟拉高
			}
 }
 
//---从字库IC中取汉字或字符数据（1个字节）--中景园科技/
 unsigned char  get_data_from_ROM(void)
{
	unsigned char  i;
	unsigned char  ret_data=0; //返回数据初始化
	LCD_SCK=1;//字库时钟拉高
	GBZK_ROMOUTSET();  //下拉输入

	for(i=0;i<8;i++)
	{
		ZK_MISO=1; 
		
		LCD_SCK=0;  //字库时钟拉低  
		ret_data=ret_data<<1;
		if( ZK_MISO )
			{ret_data=ret_data+1; 
			}
		else
			{ret_data=ret_data+0; 
			}
		LCD_SCK=1; //字库时钟拉高
	}
	ZK_MISO=0;	 //开漏输出
	return(ret_data); //返回读出的一个字节
}


//--从相关地址（AddrHigh：地址高字节,AddrMid：地址中字节,AddrLow：地址低字节）中连续读出DataLen个字节的数据到 pBuff的地址//

void get_n_bytes_data_from_ROM(unsigned char  AddrHigh,unsigned char  AddrMid,unsigned char  AddrLow,unsigned char  *pBuff,unsigned char  DataLen )
{
 	unsigned char  i;
//	OLED_CS_Set(); //取消OLED片选
	//ZK_CS2=0; //字库片选
  	LCD_SCK=0;  //字库时钟拉低
   	ZK_CS2=1;//取消字库片选
		ZK_CS2=0; //字库片选
  	LCD_SCK=0;  //字库时钟拉低



   
	ZK_command(0x03);//写指令
	ZK_command(AddrHigh);//写地址高字节
	ZK_command(AddrMid);//写地址中字节
	ZK_command(AddrLow);//写地址低字节
	for(i = 0; i < DataLen; i++ )
	     *(pBuff+i) =get_data_from_ROM();//读一个字节数据

	ZK_CS2=1;//取消字库片选
}
 

 /////////////////////////////////////////////////////////////////////////////////
 ///////	Display_GB2312_String（）汉字字符串显示程序	/////
 //zk_num定义：汉字库的标示符 1:GB2312_12*12,2:GB2312_15*16, 3:GB2312_24*24, 4:GB2312_32*32,
 //x：列坐标 y：行坐标
 //text[]：要显示的汉字
 //**************************************************************************
void Display_GB2312_String(unsigned char zk_num,unsigned int  x,unsigned int  y, unsigned char  text[])
{
   
	unsigned char  i= 0;
	unsigned char  AddrHigh,AddrMid,AddrLow ; //字高、中、低地址
	
	unsigned long  FontAddr=0; //字地址
	unsigned long  BaseAdd=0; //字库基地址	
	unsigned char  n,h,w,d,p;// 不同点阵字库的计算变量
 	CL_Mem(); //清缓存数组，不清会导致12*12显示异常

	switch(zk_num)
		{  // n个数，h：字高度，w：字宽度， d：字间距，c：页大小
		case '1':  BaseAdd=0x0;     n=24;  h=12; w=12; d=12; p=2;  break;  // 12*12  
		case '2':  BaseAdd=0x2C9D0; n=32;  h=16; w=16; d=16; p=2;  break;   // 15*16  
	    case '3':  BaseAdd=0x68190; n=72;  h=24; w=24; d=24; p=3;  break;   // 24*24  
	    case '4':  BaseAdd=0xEDF00; n=128; h=32; w=32; d=32; p=4;  break;   // 32*32  
 		}

		while((text[i]>0x00))
		{
		    if(x>(240-d))
		      {  y=y+p; 
			     x=0; 
				 	 }//溢出换行
		
			if(((text[i]>=0xA1) &&(text[i]<=0xA9))&&(text[i+1]>=0xA1))
			{						
				//国标简体（GB2312）汉字在 字库IC中的地址由以下公式来计算：//
				//Address = ((MSB - 0xA1) * 94 + (LSB - 0xA1))*n+ BaseAdd; 分三部取地址///
			 	FontAddr = (text[i]- 0xA1)*94; 
				FontAddr += (text[i+1]-0xA1);
				FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
				
				AddrHigh = (FontAddr&0xff0000)>>16;  //地址的高8位,共24位//
				AddrMid = (FontAddr&0xff00)>>8;      //地址的中8位,共24位//
				AddrLow = FontAddr&0xff;	     //地址的低8位,共24位//
				get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );//取一个汉字的数据，存到"FontBuf[]"//
			  // 	get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );//取一个汉字的数据，存到"FontBuf[ ]" 
			//	zk_map(Font_Map,FontBuf,h,w);//字符转换
				Display_GB2312(zk_num,x,y);//显示一个汉字到OLED上/ 
	 		 		}
	
			else if(((text[i]>=0xB0) &&(text[i]<=0xF7))&&(text[i+1]>=0xA1))
		 	{						
				//国标简体（GB2312） 字库IC中的地址由以下公式来计算：//
				//Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+846)*n+ BaseAdd; 分三部取地址//
				 
				FontAddr = (text[i]- 0xB0)*94; 
				FontAddr += (text[i+1]-0xA1)+846;
				FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
				
				AddrHigh = (FontAddr&0xff0000)>>16;  //地址的高8位,共24位//
				AddrMid = (FontAddr&0xff00)>>8;      //地址的中8位,共24位//
				AddrLow = FontAddr&0xff;	     //地址的低8位,共24位//
				get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );//取一个汉字的数据，存到"FontBuf[ ]"//
				//get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );//取一个汉字的数据，存到"FontBuf[ ]"
				zk_map(Font_Map,FontBuf,h,w);//字符转换
				Display_GB2312(zk_num,x,y);//显示一个汉字到OLED上/
		 		}
			 x=x+d; //下一个字坐标
		 	 i+=2;  //下个字符
			}
	 }


//***************************************************************
//  显示12*12 15*16 24*24 32*32 GB2312点阵汉字 2017-10中景园测试通过
//   zk_num：汉字 库中的类型  GB_1212 1616 2424 3232 
//   x: 开始列 
//   y: 开始页 0~7
//*************************************************************** 

void Display_GB2312( unsigned char zk_num,unsigned int x, unsigned int y )
{
   	unsigned char i,k;
 
		
	switch(zk_num)
	{
		case '1':  
			 {
		 	Address_set(x,y,x+15,y+11);
			 for(i=0;i<24;i++)		 
	{
		for(k=0;k<8;k++)
		{ 		     
		 	if((FontBuf[i]&(0x80>>k))!=0)
		{
				LCD_WR_DATA(POINT_COLOR);
			} 
			else
			{
				LCD_WR_DATA(BACK_COLOR);
			}   
		}
	
	 }
			 }break;  // 12*12  

		case '2': 
			 {
		 	Address_set(x,y,x+15,y+15);
			 for(i=0;i<32;i++)		 
	{
		for(k=0;k<8;k++)
		{ 		     
		 	if((FontBuf[i]&(0x80>>k))!=0)
		{
				LCD_WR_DATA(POINT_COLOR);
			} 
			else
			{
				LCD_WR_DATA(BACK_COLOR);
			}   
		}
	
	 }
		  }break;     // 15*16 

	    case '3':  
			 {
		 	Address_set(x,y,x+23,y+23);
			 for(i=0;i<72;i++)		 
	{
		for(k=0;k<8;k++)
		{ 		     
		 	if((FontBuf[i]&(0x80>>k))!=0)
		{
				LCD_WR_DATA(POINT_COLOR);
			} 
			else
			{
				LCD_WR_DATA(BACK_COLOR);
			}   
		}
	
	 }
		  }break;     // 24*24  

	    case '4':  
			 {
		 	Address_set(x,y,x+31,y+31);
			 for(i=0;i<128;i++)		 
	{
		for(k=0;k<8;k++)
		{ 		     
		 	if((FontBuf[i]&(0x80>>k))!=0)
		{
				LCD_WR_DATA(POINT_COLOR);
			} 
			else
			{
				LCD_WR_DATA(BACK_COLOR);
			}   
		}
	
	 }
		  }break;    // 32*32  

			}
 	 


 
}


 

 
void Display_Asc_String(unsigned char zk_num,unsigned int x, unsigned int y, unsigned char  text[])
{
   
	unsigned char  i= 0;
	unsigned char  AddrHigh,AddrMid,AddrLow ; //字高、中、低地址

	unsigned long  FontAddr=0; //字地址
	unsigned long  BaseAdd=0; //字库基地址	
    unsigned char  n,h,w,d,c;// 不同点阵字库的计算变量
    CL_Mem(); //清缓存数组 ,不清会导致6*12显示异常

	switch(zk_num)
	{	// n个数，h：字高度，w：字宽度， d：字间距，c：页大小
		case '1':  BaseAdd=0x1DDF80; n=8;  h=7;  w=5;  d=6 ; c=1;  break;	 //	  5x7 ASCII
		case '2':  BaseAdd=0x1DE280; n=8;  h=8;  w=7;  d=8;  c=1;  break;	 //   7x8 ASCII
		case '3':  BaseAdd=0x1DBE00; n=12; h=12; w=6;  d=6;  c=2;  break;	 //  6x12 ASCII
	   case '4':  BaseAdd=0x1DD780; n=16; h=16; w=8;  d=8;  c=2;  break;	 //  8x16 ASCII	
	   case '5':  BaseAdd=0x1DFF00; n=48; h=24; w=12; d=12; c=3;  break;	 //  12x24 ASCII
	 	case '6':  BaseAdd=0x1E5A50; n=64; h=32; w=16; d=16; c=4;  break;	 //  16x32 ASCII

			}

	while((text[i]>0x00))
	{	
	   if(x>(240-d))   //溢出换行
	      {  y=y+c; 
		     x=0; 
			 	 }
	   if((text[i] >= 0x20) &&(text[i] <= 0x7E))
		{						
		    FontAddr = 	text[i]-0x20;
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
			
			AddrHigh = (FontAddr&0xff0000)>>16;  /*地址的高8位,共24位*/
			AddrMid = (FontAddr&0xff00)>>8;      /*地址的中8位,共24位*/
			AddrLow = FontAddr&0xff;	     /*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*取一个汉字的数据，存到"FontBuf[]"*/
	      	zk_map(Font_Map,FontBuf, h, w);	//数据变换
			Display_Asc(zk_num,x,y);/*显示一个ascii到OLED上 */
  
		}
    i+=1;  //下个数据
	x+=d;//下一个字坐标 
	}	    
	
}


//***************************************************************
//  显示 ASCII 2017-10中景园测试通过
//zk_num定义：汉字库的标示符 1:5x7 ASCII,2:7x8 ASCII, 3:6x12 ASCII, 4:8x16 ASCII,	5: 12x24 ASCII,6:16x32 ASCII;
//   x: Start Column  开始列  
//   y: Start page   开始页 0~7 
//*************************************************************** 
void Display_Asc( unsigned char zk_num,unsigned int x, unsigned int y )
{
    
    unsigned char i  ,k;
 
		
	switch(zk_num)
	{
	    case '1':   
			 {
		 	Address_set(x,y,x+7,y+7);
			 for(i=0;i<7;i++)		 
	{
		for(k=0;k<8;k++)
		{ 		     
		 	if((FontBuf[i]&(0x80>>k))!=0)
		{
				LCD_WR_DATA(POINT_COLOR);
			} 
			else
			{
				LCD_WR_DATA(BACK_COLOR);
			}   
		}
	
	 }
			 }break;    //	  5x7 ASCII

	 	case '2':   	
			 {
		 	Address_set(x,y,x+7,y+7);
			 for(i=0;i<8;i++)		 
	{
		for(k=0;k<8;k++)
		{ 		     
		 	if((FontBuf[i]&(0x80>>k))!=0)
		{
				LCD_WR_DATA(POINT_COLOR);
			} 
			else
			{
				LCD_WR_DATA(BACK_COLOR);
			}   
		}
	
	 }
			 }break;   //	  7x8 ASCII

	    case '3':   
			 {
		 	Address_set(x,y,x+7,y+11);
			 for(i=0;i<12;i++)		 
	{
		for(k=0;k<8;k++)
		{ 		     
		 	if((FontBuf[i]&(0x80>>k))!=0)
		{
				LCD_WR_DATA(POINT_COLOR);
			} 
			else
			{
				LCD_WR_DATA(BACK_COLOR);
			}   
		}
	
	 }
			 }break;  //  6x12 ASCII

	    case '4':  
	 {
		 	Address_set(x,y,x+7,y+15);
			 for(i=0;i<16;i++)		 
	{
		for(k=0;k<8;k++)
		{ 		     
		 	if((FontBuf[i]&(0x80>>k))!=0)
		{
				LCD_WR_DATA(POINT_COLOR);
			} 
			else
			{
				LCD_WR_DATA(BACK_COLOR);
			}   
		}
	
	 }
			 }break;     //  8x16 ASCII

	    case '5':  
	 {
		 	Address_set(x,y,x+15,y+24);
			 for(i=0;i<48;i++)		 
	{
		for(k=0;k<8;k++)
		{ 		     
		 	if((FontBuf[i]&(0x80>>k))!=0)
		{
				LCD_WR_DATA(POINT_COLOR);
			} 
			else
			{
				LCD_WR_DATA(BACK_COLOR);
			}   
		}
	
	 }
			 }break;    //  12x24 ASCII

	    case '6':  	 
	 {
		 	Address_set(x,y,x+15,y+31);
			 for(i=0;i<64;i++)		 
	{
		for(k=0;k<8;k++)
		{ 		     
		 	if((FontBuf[i]&(0x80>>k))!=0)
		{
				LCD_WR_DATA(POINT_COLOR);
			} 
			else
			{
				LCD_WR_DATA(BACK_COLOR);
			}   
		}
	
	 }
			 }break;   //  16x32 ASCII


			}
 

}





//--------------------------------------------------------------------------------
// void lcdram_map( BYTE *getdate,BYTE *putdata,BYTE high,BYTE width,BYTE style )
// 汉字转换程序  
//putdata 原汉字点 (横置横排)
//getdate 目标汉字点，high 原汉字高度，width 原汉字宽度，
//------------------------------------------------------------------------------
void zk_map(  unsigned char *getdate, unsigned char *putdata, unsigned char high, unsigned char width)	 //byte
{
	unsigned short i,j,wbyte;			   //word
	unsigned char i_8;
	wbyte = (width+7)/8;
//	hbyte = (high+7)/8;

		for( i = 0; i < high; i++ )
		{
			for( j = 0; j < width; j++ )
			{
				i_8 = i/8;
				if((*(putdata+wbyte*i+j/8)&(0x80>>(j%8))) > 0)
					getdate[i_8*width+j] |= (0x01<<(i%8));
				else
					getdate[i_8*width+j] &= (~(0x01<<(i%8)));

			}
	
			 }
	}










