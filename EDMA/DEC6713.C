
/*********************************************************************************
* DEC6713.C	v1.00	     							                            *
* Copyright	2003 by SEED Electronic Technology Ltd.
* All rights reserved. Property of SEED Electronic Technology Ltd.			                *
* Designed by:	Hongshuai.Li								                                *
*********************************************************************************/ 
//#include <csl.h>
//#include <csl_mcasp.h>
//#include <csl_i2c.h>
//#include <csl_emif.h>
//#include <csl_pll.h>
#include <DEC6713.h>
#include <irq.h>
#include <c6x.h>

/*Local software delay function*/
static void PLLDelay(int Count)
{
    volatile int i = Count;
    while(i--);
}

/********************************************************************************\
\*DEC6713_init()	-Initialize DEC6713 board.
\*Parameters: NO.
\*		
\*Return:No.
\********************************************************************************/

void DEC6713_init()      //初始化程序
{

		volatile Uint32 *emif_base=(volatile Uint32*)0x01800000,*pll_base=(volatile Uint32*)0x01b7c100;		//emif的首地址和pll的首地址
		register int x0,x1,x2,x3,x4,x5,x6;
		register int y0,y1,y2,y3,y4,y5,y6;
		x0=emif_base[GBLCTL];
		x1=emif_base[CECTL0];
		x2=emif_base[CECTL1];
		x3=emif_base[CECTL2];
		x4=emif_base[CECTL3];
		x5=emif_base[SDCTL];
		x6=emif_base[SDTIM];

		x0&=0xffffff67;		//5、6置1； 3、4、7置0
		x0|=0x00000020;    //第5位置1；单个使能
		x1=0xffffbf33;		//4-7选3，15-14TA 0
		x5=0x56227000;
		x6=0x00281578;
		y0=pll_base[PLLCSR];
		y1=pll_base[PLLM];
		y2=pll_base[PLLDIV0];
		y3=pll_base[PLLDIV1];
		y4=pll_base[PLLDIV2];
		y5=pll_base[PLLDIV3];
		y6=pll_base[OSDDIV];
		y1=0x00000017;
		y2=0x00008000;
		y3=0x00008001;
		y4=0x00008003;
		y5=0x00008004;
		y6=0x00008004;

		/*设置时钟*/
		y0&=0xfffffffe;   //最低位置0，不使能
		pll_base[PLLCSR]=y0;
	/* Initialize PLL Registers */         
		/* Put PLL in bypass */

	//PLL_bypass();
		PLLDelay(20);
	
		/* Reset PLL */
	    x0|=0x00000008;   //设置复位位，第三位置1
		pll_base[PLLCSR]=y0;

		//PLL_reset();

	
		/* Set main multiplier/divisor */
	    pll_base[PLLM]=y1;			// 25MHz  x 18 = 450MHz
		pll_base[PLLDIV0]=y2;		// 450MHz  / 1 = 450MHz
		pll_base[OSDDIV]=y6;		//25MHz  / 5 = 5Mhz
		PLLDelay(20);


    /* Set DSP clock */
        pll_base[PLLDIV1]=y3;    //450/2=225
        PLLDelay(20);
    
    /* Set peripheral clock */
        pll_base[PLLDIV2]=y4;	//450/4=112.5
        PLLDelay(20);
    
    /* Set EMIF clock */
       pll_base[PLLDIV3]=y5;     //450/5=90
       PLLDelay(20);
    
    /* Take PLL out of reset */
       x0&=0xfffffff7;
       pll_base[PLLCSR]=y0;
       PLLDelay(1500);
    
    	/* Enalbe PLL */
        x0|=0x00000001;
        pll_base[PLLCSR]=y0;
        PLLDelay(20);
   
   	/* Initialize EMIF */
        emif_base[GBLCTL]=x0;      //控制字送到寄存器中去
        emif_base[CECTL0]=x1;
        emif_base[SDCTL]=x5;
        emif_base[SDTIM]=x6;

    //EMIF_config(&MyEMIFcfg0);
    
	/* Set CPLD registers to default state  */    
   	//DEC6713_cpld_rset(DEC6713_CTL_REG, 0x00);   	
      	
}
/********************************************************************************\
\*Uint8 DEC6713_cpld_rget()		-Read CPLD register?
\*Parameters:
\*		regnum: The related register.

\*Return: The related register value.
\********************************************************************************/
   	
   	/* Read CPLD register(8bits) */
	Uint8 DEC6713_cpld_rget(Int8 regnum)
	{
    	Uint8 *pdata;
    
    	/* Return lower 8 bits of register */
    	pdata = (Uint8 *)(DEC6713_CPLD_BASE + regnum);
    	return (*pdata & 0xff);
	}
/********************************************************************************\
\*Uint8 DEC6713_cpld_rset()		-Write CPLD register.
\*Parameters:
\*		regnum: The related register
\*		regval:To be writen register value.

\*Return: No.
\********************************************************************************/

	/* Write CPLD register(8bits) */
	void DEC6713_cpld_rset(Int8 regnum, Uint8 regval)
	{
	    Uint8 *pdata;
    
    	/* Write lower 8 bits of register */
    	pdata = (Uint8 *)(DEC6713_CPLD_BASE + regnum);
    	*pdata = (regval & 0xff);
	}
/********************************************************************************\
\*Uint8 DEC6713_wait()		-DEC6713 Delay function
\*Parameters:
\*		delay:Delay time.

\*Return:NO.
\********************************************************************************/
	/* Spin in a delay loop for delay iterations */
	void DEC6713_wait(Uint32 delay)
	{
	    volatile Uint32 i, n;
	    
	    n = 0;
	    for (i = 0; i < delay; i++)
	    {
	        n = n + 1;
	    }
    }
	void *IRQ_setVecs(void *vecs)     //装载中断向量表
	{
		register int p,x;
		p=(Uint32)vecs;
		x=ISTP;
		x&=0xfffffc00;
		p&=0xfffffc00;
		x=p;
		ISTP=x;

	}
/********************************************************************************\
\* End of DEC6713.C *\
\********************************************************************************/
