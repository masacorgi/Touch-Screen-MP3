/********************************************************************************/
/* main.c                                                                       */
/* STM32F407ZGT6                                                                */
/* (Lee ChangWoo HL2IRW  hl2irw@daum.net 010-8573-6860)                 	*/
/* stm32f4x_test								*/
/********************************************************************************/
#include "hwdefs.h"
#include "source/prototype.h"
#include "source/color.h"


volatile unsigned short tick,jiffes;
volatile unsigned char time_led,read_key,old_key,volume,volume_flag;
volatile short mx,my,old_x,old_y,pen_x,pen_y;
volatile unsigned short tpad_press;

FLASH_Status FLASHStatus = FLASH_COMPLETE;


extern volatile unsigned char rxck1,rxck2,rxck3,rx_led,tx_led;
extern volatile unsigned short rxcnt1,rxcnt2,rxcnt3;
extern volatile unsigned char play_key,play_audio;
extern unsigned char display_line_flag,audio_delay,audio_pause;
extern volatile unsigned char audio_run;



void wait_ms (unsigned short delay)
{
      unsigned short old_jiffes;
      jiffes = 0;
      old_jiffes = 0;
      while (jiffes < delay) {
      	    if (old_jiffes != jiffes) {
      	       old_jiffes = jiffes;
               /* Reload IWDG counter */
               IWDG_ReloadCounter();
            }
      }
}


char hex2dec (const char ch)
{
      if (ch <= '9') return (ch - '0');
      if (ch >= 'a' && ch <= 'f') return (ch - 'a' + 10);
      if (ch >= 'A' && ch <= 'F') return (ch - 'A' + 10);
      return 0;
}


char dec2hex (const char ch)
{
      if (ch <= 9) return (ch + '0');
      if (ch >= 10 && ch <= 15) return (ch + 'A' - 10);
      return 0;
}


void Periph_Configuration (void)
{
      /* Enable GPIO clocks */
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);
      /* Enable USART1 clock */
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
      /* Enable USART2 clock */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
      /* Enable USART3 clock */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
      /* Enable PWR and BKP clocks */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_AHB1Periph_BKPSRAM | RCC_AHB1Periph_SRAM1 | RCC_AHB1Periph_SRAM2, ENABLE);
}


void GPIO_Configuration (void)
{
      GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
      GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
      GPIO_Init_Pin(GPIOA,TXD2,GPIO_Speed_100MHz,GPIO_Mode_AF_PP_PU);
      GPIO_Init_Pin(GPIOA,RXD2,GPIO_Speed_100MHz,GPIO_Mode_AF_PP_PU);

      GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
      GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
      GPIO_Init_Pin(GPIOA,TXD1,GPIO_Speed_100MHz,GPIO_Mode_AF_PP_PU);
      GPIO_Init_Pin(GPIOA,RXD1,GPIO_Speed_100MHz,GPIO_Mode_AF_PP_PU);

      GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
      GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
      GPIO_Init_Pin(GPIOB,TXD3,GPIO_Speed_100MHz,GPIO_Mode_AF_PP_PU);
      GPIO_Init_Pin(GPIOB,RXD3,GPIO_Speed_100MHz,GPIO_Mode_AF_PP_PU);

      GPIO_Init_Pin(GPIOF,LED0,GPIO_Speed_100MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(GPIOF,LED1,GPIO_Speed_100MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(GPIOG,TXEN,GPIO_Speed_100MHz,GPIO_Mode_Out_PP);

      LED_OUT0 = 1;
      LED_OUT1 = 1;
      TXEN_485 = 0;
}


#ifdef VECT_TAB_RAM
/* vector-offset (TBLOFF) from bottom of SRAM. defined in linker script */
extern unsigned int _isr_vectorsram_offs;
void NVIC_Configuration (void)
{
      /* Set the Vector Table base location at 0x20000000 +_isr_vectorsram_offs */
      NVIC_SetVectorTable(NVIC_VectTab_RAM, (unsigned int)&_isr_vectorsram_offs);
}
#else
extern unsigned int _isr_vectorsflash_offs;
void NVIC_Configuration (void)
{
      /* Set the Vector Table base location at 0x08000000 +_isr_vectorsflash_offs */
      NVIC_SetVectorTable(NVIC_VectTab_FLASH, (unsigned int)&_isr_vectorsflash_offs);
}
#endif /* VECT_TAB_RAM */


RAMFUNC void SysTick_Handler (void)
{
      static unsigned short cnt = 0;
      static unsigned char flip = 0;
      cnt++;
      if (cnt >= 500) {
         cnt = 0;
         /* alive sign */
         if (flip) {
            time_led = ON;
         } else {
            time_led = OFF;
         }
      	 flip = !flip;
      }
      tick++;
      jiffes++;
      if (rxcnt1) rxck1++;
      if (rxcnt2) rxck2++;
      if (rxcnt3) rxck3++;
      audio_delay++;
}


void watch_dog_init (void)
{
      /* Enable write access to IWDG_PR and IWDG_RLR registers */
      IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
      /* IWDG counter clock: LSI/32 */
      IWDG_SetPrescaler(IWDG_Prescaler_32);
      /* Set counter reload value to obtain 250ms IWDG TimeOut. */
      IWDG_SetReload(2048);
      /* Reload IWDG counter */
      IWDG_ReloadCounter();
      /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
      IWDG_Enable();
}


void tick_process (void)
{
      /* Reload IWDG counter */
      IWDG_ReloadCounter();
      read_key = key_read();
      if (read_key != old_key) {
      	  old_key = read_key;
      	  //key...
      	  //TP_Drow_Touch_Point(old_x,old_y,background_color);
      	  if (read_key & K_UP) {
      	     if (my > 5) my -= 5;else my = 0;
      	     old_y = my;
      	     volume++;
      	     if (volume >= 63) volume = 63;
      	     volume_flag = 1;
      	     lcd_printf(20,28,"Volume: %2d ",volume);
      	  }
      	  if (read_key & K_DOWN) {
      	     if (my < (maxy - 6)) my += 5;else my = maxy - 1;
      	     old_y = my;
      	     if (volume) volume--;else volume = 0;
      	     volume_flag = 1;
      	     lcd_printf(20,28,"Volume: %2d ",volume);
      	  }
      	  if (read_key & K_RIGHT) {
      	     if (mx < (maxx - 6)) mx += 5;else mx = maxx - 1;
      	     old_x = mx;
             if (play_audio == 0) {
              	 play_audio = 1;
             } else {
             	 play_key |= 0x02;
             }
      	  }
      	  if (read_key & K_LEFT) {
      	     if (mx > 5) mx -= 5;else mx = 0;
      	     old_x = mx;
             play_key |= 0x04;
      	  }
      	  //TP_Drow_Touch_Point(mx,my,YELLOW);
      }
      serial_check();
      TP_Scan(0);
      if ((tp_dev.sta & 0xC0) == TP_CATH_PRES) {
         tp_dev.sta &= ~(TP_CATH_PRES);
         pen_x = tp_dev.x[0];
         pen_y = tp_dev.y[0];
         lcd_printf(1,1,"TP_X: %3d TP_Y: %3d ",pen_x,pen_y);
         //set_pixel(pen_x,pen_y,foreground_color);
	 if(0<pen_x && pen_x<92){ //c1, 이전곡재생
		if(374<pen_y && pen_y<466){
			lcd_printf(25,2,"c1 touched");
			if (mx > 5) mx -= 5;else mx = 0;
			old_x = mx;
			play_key |= 0x04;
		}
	}
	else if(95<pen_x && pen_x<187){ //c2, 재생/정지
		if(374<pen_y && pen_y<466){
			lcd_printf(25,2,"c2 touched");
			play_key |= 0x01;
		}
	}
	else if(190<pen_x && pen_x<282){ //c3, 다음곡재생
		if(374<pen_y && pen_y<466){
			lcd_printf(25,2,"c3 touched");
			if (mx < (maxx - 6)) mx += 5;else mx = maxx - 1;
			old_x = mx;
			if (play_audio == 0) {
				play_audio = 1;
			} else {
				play_key |= 0x02;
			}
		}
	}
	else if(285<pen_x && pen_x<315){
		if(380<pen_y && pen_y<410){ // r1,볼륨up
			lcd_printf(25,2,"r1 touched");
			if (my > 5) my -= 5;else my = 0;
			old_y = my;
			volume++;
			if (volume >= 63) volume = 63;
			volume_flag = 1;
			lcd_printf(20,22,"Volume: %2d ",volume);
		}
		if(430<pen_y && pen_y<460){ // r2,볼륨down
			lcd_printf(25,2,"r2 touched");
			if (my < (maxy - 6)) my += 5;else my = maxy - 1;
			old_y = my;
			if (volume) volume--;else volume = 0;
			volume_flag = 1;
			lcd_printf(20,22,"Volume: %2d ",volume);
		}
	}
      }
      
      ir_process();
      //if (TPAD_Scan(0)) {
      //	  tpad_press++;
      //	  lcd_printf(1,1,"Touch_pad press %5d ",tpad_press);
      //}
      adc_process();
      if (display_line_flag) {
         audio_file_list();
      }
      if (time_led == ON) LED_OUT0 = 0;else LED_OUT0 = 1;
      if ((tx_led) || (rx_led)) LED_OUT1 = 0;
      if ((tx_led == 0) && (rx_led == 0)) LED_OUT1 = 1;
      audio_process();
}


int main (void)
{
      RCC_ClocksTypeDef RCC_Clocks;
      /* System Clocks Configuration */
      Periph_Configuration();
      /* NVIC configuration */
      NVIC_Configuration();
      /* Configure the GPIO ports */
      GPIO_Configuration();
      RCC_GetClocksFreq(&RCC_Clocks);
      /* Setup SysTick Timer for 1 millisecond interrupts, also enables Systick and Systick-Interrupt */
      if (SysTick_Config(SystemCoreClock / 1000)) {
         /* Capture error */
         while (1);
      }
      /* 4 bit for pre-emption priority, 0 bits for subpriority */
      NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
      if (FLASH_OB_GetRDP() != SET) {
         FLASH_Unlock();                           // this line is critical!
         FLASH_OB_Unlock();
         FLASH_OB_RDPConfig(OB_RDP_Level_1);
         FLASH_OB_Launch();                        // Option Bytes programming
         FLASH_OB_Lock();
         FLASH_Lock();
         NVIC_SystemReset();
      }
      serial_init();
      watch_dog_init();
      init_i2c_24xx();
      key_init();
      LCD_Init();
      TP_Init();
      ir_remocon_init();
      set_color(GREEN);
      lcd_printf(1,7,"TP_Info.");
      lcd_printf(1,8,"xfac: %d",tp_dev.xfac * 100);
      lcd_printf(1,9,"yfac: %d",tp_dev.yfac);
      lcd_printf(1,10,"xoffset: %d yoffset:%d",tp_dev.xoff,tp_dev.yoff);
      lcd_printf(1,11,"status: %d type: %d",tp_dev.sta,tp_dev.touchtype);
      mx = maxx / 2;
      my = maxy / 2;
      old_x = mx;
      old_y = my;
      //TP_Drow_Touch_Point(mx,my,YELLOW);
      tpad_press = 0;
      ADC_Config();
      Dac1_Init();
      //TPAD_Init(8);
      audio_init();
      lcd_printf(25,1,"rtc point");
      // 시간출력
      //RTC_Get_Time (unsigned char *hour,unsigned char *min,unsigned char *sec,unsigned char *ampm);
      //unsigned char *hour=00;
      //unsigned char *min=00;
      //unsigned char *sec=00;
      //unsigned char *ampm=00;
      //RTC_Set_Time (*hour,*min,*sec,*ampm);
      //RTC_Get_Time(hour,min,sec,ampm);
      //lcd_printf(20,1,"time%s %s %s",hour,min,sec);
      
      volume = 30;
      while (1) {
		
      	    if (audio_run) {
      	       audio_play_function();
      	    }
            if (tick) {
               tick = 0;
               tick_process();
      	    }
      }
      ///
      //set_color(GREEN);
      //LCD_Draw_Circle (50, 100, 2);
}
