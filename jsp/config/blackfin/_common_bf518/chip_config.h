/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *
 *  TOPPERS/JSP for Blackfin
 *
 *  Copyright (C) 2004,2006,2009 by Takemasa Nakamura
 *  Copyright (C) 2004 by Ujinosuke
 *
 *  上記著作権者は，以下の (1)〜(4) の条件か，Free Software Foundation
 *  によって公表されている GNU General Public License の Version 2 に記
 *  述されている条件を満たす場合に限り，本ソフトウェア（本ソフトウェア
 *  を改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 *  利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，その適用可能性も
 *  含めて，いかなる保証も行わない．また，本ソフトウェアの利用により直
 *  接的または間接的に生じたいかなる損害に関しても，その責任を負わない．
 *
 *
 */


#ifndef _CHIP_CONFIG_H_
#define _CHIP_CONFIG_H_


/*
 *	ターゲットシステム依存モジュール（EZKIT-BF534,6,7用）
 *
 *  このインクルードファイルは，t_config.h のみからインクルードされる．
 *  他のファイルから直接インクルードしてはならない．
*/

/*
 *  ターゲットシステムのハードウェア資源の定義
 *
 *  DEVICE_INTERRUPT_COUNTはSIC_IMASKのビット数
 *  を与える。
 *  ADSP-BF518のビット数は56なので、値は56である。
 */

#define DEVICE_INTERRUPT_COUNT 56



/*
 *  PLLクロック周波数
 */
#define PLLCLOCK (CLKIN*MSELVAL)
#define SYSCLOCK  (PLLCLOCK / SSELVAL)
#define CORECLOCK (PLLCLOCK / CSELVAL)



/*
 *  TICの割込みハンドラのベクタ番号
 */
#ifdef USE_TIC_CORE
#define INHNO_TIMER	INHNO_CORE_TIMER
#else
#define INHNO_TIMER	INHNO_TIMER7
#endif


/*
 *  微少時間待ちのための定義
 *  sil_dly_nseの呼び出しオーバーヘッドとループボディーサイクルを
 *  nSで表す。関数はcpu_support.asmに定義してある。
 *  SIL_DLY_TIM1は24サイクル。32nS( 750MHz ), 40nS( 600MHz )
 *  SIL_DLY_TIM2は12サイクル。16nS( 750MHz ), 20nS( 600MHz )
 */


#define	SIL_DLY_TIM1	(24*1000000000/CORECLOCK)
#define	SIL_DLY_TIM2	(12*1000000000/CORECLOCK)

/*************************************************************************
 * uart.c用構成マクロ
 */

/*  シリアルの割込みハンドラのベクタ番号 */
#define INHNO_SIO0_TX   INHNO_UART0_TX
#define INHNO_SIO0_RX   INHNO_UART0_RX
#define INHNO_SIO1_TX   INHNO_UART1_TX
#define INHNO_SIO1_RX   INHNO_UART1_RX

/*
 *  UARTレジスタの配置境界。ADSP-BF518の内蔵UARTは4バイト周期でレジスタが
 *  並んでいるので、ここには4を指定する。
 */
#define UART_BOUNDARY 4

/*  UARTレジスタにsil_xxb_iop()を使ってアクセスするときにはUART_IOP_ACCESSを 宣言する。*/
#define UART_IOP_ACCESS

/*  シリアルポートのアドレス。 */
#define UART0_ADDRESS 0xFFC00400	// 内蔵UART0 THRのアドレス。
#define UART1_ADDRESS 0xFFC02000    // 内蔵UART0 THRのアドレス。

/*  UARTのボーレートジェネレータに設定すべき値． */
#define UART0_DIVISOR	SYSCLOCK/16/UART0_BAUD_RATE
#define UART1_DIVISOR   SYSCLOCK/16/UART1_BAUD_RATE

/*  内蔵UARTを使う場合は、UARTx_BLACKFIN_UCENを宣言してUCENを初期化しなければならない。*/
#define UART0_BLACKFIN_UCEN
#define UART1_BLACKFIN_UCEN


/*
 * uart.c用構成マクロ終わり
 *************************************************************************/



/*
 *  割り込みベクトル番号
 *
 *  この定義はコンフィグレーションファイルでDEF_INHの引数として使う
 *  SIC_ISRにおけるビット番号である。
 *
 */

/* Peripheral Masks For SIC_ISR0, SIC_IWR0, SIC_IMASK0 */
#define INHNO_PLL_WAKEUP		0	/* PLL Wakeup Interrupt			 					*/
#define INHNO_DMA_ERR0   		1  /* Error Interrupt (DMA error 0 interrupt (generic)) 			*/
#define INHNO_DMAR0      		2  /* DMAR0 Block (DMAR0 block interrupt) 					*/
#define INHNO_DMAR1      		3  /* DMAR1 Block  (DMAR1 block interrupt) 					*/
#define INHNO_DMAR0_ERR  		4  /* Error Interrupt (DMAR0 overflow error interrupt)  			*/
#define INHNO_DMAR1_ERR  		5  /* Error Interrupt (DMAR1 overflow error interrupt)  			*/
#define INHNO_PPI_ERR    		6  /* Error Interrupt (PPI error interrupt) 					*/
#define INHNO_MAC_ERR    		7  /* Error Interrupt (MAC status interrupt) 				*/
#define INHNO_SPORT0_ERR 		8  /* Error Interrupt (SPORT0 status interrupt) 				*/
#define INHNO_SPORT1_ERR 		9  /* Error Interrupt (SPORT1 status interrupt) 				*/
#define INHNO_PTP_ERR			10  /* Error Interrupt (PTP error interrupt)					*/
/* reserved */
#define INHNO_UART0_ERR 		12  /* Error Interrupt (UART0 status interrupt) 				*/
#define INHNO_UART1_ERR  		13  /* Error Interrupt (UART1 status interrupt) 				*/
#define INHNO_RTC		   		14  /* Real Time Clock Interrupt 							*/
#define INHNO_PPI	   			15  /* DMA channel 0 (PPI/NFC) Interrupt 					*/
#define INHNO_SPORT0_RX	   	16  /* DMA Channel 3 (SPORT0 RX) Interrupt 					*/
#define INHNO_SPORT0_TX	   	17  /* DMA Channel 4 (SPORT0 TX) Interrupt 					*/
#define INHNO_SPORT1_RX	   	18  /* DMA Channel 5 (SPORT1 RX) Interrupt 					*/
#define INHNO_SPORT1_TX	   	19  /* DMA Channel 6 (SPORT1 TX) Interrupt 		 			*/
#define INHNO_TWI		   		20  /* TWI Interrupt									*/
#define INHNO_SPI	   			21  /* DMA Channel 7 (SPI) Interrupt 						*/
#define INHNO_UART0_RX	   		22  /* DMA Channel 8 (UART0 RX) Interrupt 					*/
#define INHNO_UART0_TX	   		23  /* DMA Channel 9 (UART0 TX) Interrupt 					*/
#define INHNO_UART1_RX	   		24  /* DMA Channel 10 (UART1 RX) Interrupt 					*/
#define INHNO_UART1_TX	   		25  /* DMA Channel 11 (UART1 TX) Interrupt 					*/
#define INHNO_OTP	   			26  /* OTP Interrupt 									*/
#define INHNO_CNT  	   		27  /* GP Counter Interrupt  							*/
#define INHNO_ETHERNET_RX	   	28  /* DMA Channel 1 (EthernetRX/HOSTDP) Interrupt				*/
#define INHNO_PFA_PORTH  		29  /* PF Port H Interrupt A 							*/
#define INHNO_ETHERNET_TX	   	30  /* DMA Channel 2 (Ethernet TX/NFC) Interrupt 				*/
#define INHNO_PFB_PORTH  		31  /* PF Port H  Interrupt B 							*/

/* Peripheral Masks For SIC_ISR1, SIC_IWR1, SIC_IMASK1 */
#define INHNO_TIMER0			32	/* Timer 0 Interrupt								*/
#define INHNO_TIMER1			33	/* Timer 1 Interrupt 								*/
#define INHNO_TIMER2			34	/* Timer 2 Interrupt 								*/
#define INHNO_TIMER3			35	/* Timer 3 Interrupt 								*/
#define INHNO_TIMER4			36	/* Timer 4 Interrupt 								*/
#define INHNO_TIMER5			37	/* Timer 5 Interrupt 								*/
#define INHNO_TIMER6			38	/* Timer 6 Interrupt 								*/
#define INHNO_TIMER7			39	/* Timer 7 Interrupt 								*/
#define INHNO_PFA_PORTG		40	/* PF Port G Interrupt A 							*/
#define INHNO_PFB_PORTG		41	/* PF Port G Interrupt B		 					*/
#define INHNO_MDMA0_DST		42	/* DMA Channels 12 (MDMA0 Destination) TX Interrupt			*/
#define INHNO_MDMA0_SRC		42	/* DMA Channels 13 (MDMA0 Source) RX Interrupt				*/
#define INHNO_MDMA1_DST		43	/* DMA Channels 14 (MDMA1 Destination) TX Interrupt			*/
#define INHNO_MDMA1_SRC		43	/* DMA Channels 15 (MDMA1 Source) RX Interrupt 				*/
#define INHNO_WDOG				44	/* Software Watchdog Timer Interrupt 					*/
#define INHNO_PFA_PORTF		45	/* PF Port F Interrupt A		 					*/
#define INHNO_PFB_PORTF		46	/* PF Port F Interrupt B 							*/
#define INHNO_SPI0_ERR     	47  /* Error Interrupt (SPI0 status interrupt) 				*/
#define INHNO_SPI1_ERR			48	/* Error Interrupt (SPI1 status interrupt)				*/
/* reserved */
/* reserved */
#define INHNO_RSI_INT0			51	/* USB EINT interrupt 								*/
#define INHNO_RSI_INT1			52	/* USB INT0 interrupt 								*/
#define INHNO_PWM_TRIPINT		53	/* USB INT1 interrupt 								*/
#define INHNO_PWM_SYNCINT		54	/* USB INT1 interrupt 								*/
#define INHNO_PTP_STATINT		55	/* USB DMAINT interrupt								*/


// SIC_ISRにない特殊な割り込み

#define INHNO_HW_ERROR      56
#define INHNO_CORE_TIMER    57
#define INHNO_RAISE         58


/*
 *  SIC_ISRの割り込みのベクタ番号
 *
 *  この定義はena_int, dis_intの引数として使う。
 */

/* Peripheral Masks For SIC_ISR0, SIC_IWR0, SIC_IMASK0 */
#define INTNO_PLL_WAKEUP		0	/* PLL Wakeup Interrupt			 					*/
#define INTNO_DMA_ERR0   		1  /* Error Interrupt (DMA error 0 interrupt (generic)) 			*/
#define INTNO_DMAR0      		2  /* DMAR0 Block (DMAR0 block interrupt) 					*/
#define INTNO_DMAR1      		3  /* DMAR1 Block  (DMAR1 block interrupt) 					*/
#define INTNO_DMAR0_ERR  		4  /* Error Interrupt (DMAR0 overflow error interrupt)  			*/
#define INTNO_DMAR1_ERR  		5  /* Error Interrupt (DMAR1 overflow error interrupt)  			*/
#define INTNO_PPI_ERR    		6  /* Error Interrupt (PPI error interrupt) 					*/
#define INTNO_MAC_ERR    		7  /* Error Interrupt (MAC status interrupt) 				*/
#define INTNO_SPORT0_ERR 		8  /* Error Interrupt (SPORT0 status interrupt) 				*/
#define INTNO_SPORT1_ERR 		9  /* Error Interrupt (SPORT1 status interrupt) 				*/
#define INTNO_PTP_ERR			10  /* Error Interrupt (PTP error interrupt)					*/
/* reserved */
#define INTNO_UART0_ERR 		12  /* Error Interrupt (UART0 status interrupt) 				*/
#define INTNO_UART1_ERR  		13  /* Error Interrupt (UART1 status interrupt) 				*/
#define INTNO_RTC		   		14  /* Real Time Clock Interrupt 							*/
#define INTNO_PPI	   			15  /* DMA channel 0 (PPI/NFC) Interrupt 					*/
#define INTNO_SPORT0_RX	   	16  /* DMA Channel 3 (SPORT0 RX) Interrupt 					*/
#define INTNO_SPORT0_TX	   	17  /* DMA Channel 4 (SPORT0 TX) Interrupt 					*/
#define INTNO_SPORT1_RX	   	18  /* DMA Channel 5 (SPORT1 RX) Interrupt 					*/
#define INTNO_SPORT1_TX	   	19  /* DMA Channel 6 (SPORT1 TX) Interrupt 		 			*/
#define INTNO_TWI		   		20  /* TWI Interrupt									*/
#define INTNO_SPI	   			21  /* DMA Channel 7 (SPI) Interrupt 						*/
#define INTNO_UART0_RX	   		22  /* DMA Channel 8 (UART0 RX) Interrupt 					*/
#define INTNO_UART0_TX	   		23  /* DMA Channel 9 (UART0 TX) Interrupt 					*/
#define INTNO_UART1_RX	   		24  /* DMA Channel 10 (UART1 RX) Interrupt 					*/
#define INTNO_UART1_TX	   		25  /* DMA Channel 11 (UART1 TX) Interrupt 					*/
#define INTNO_OTP	   			26  /* OTP Interrupt 									*/
#define INTNO_CNT  	   		27  /* GP Counter Interrupt  							*/
#define INTNO_ETHERNET_RX	   	28  /* DMA Channel 1 (EthernetRX/HOSTDP) Interrupt				*/
#define INTNO_PFA_PORTH  		29  /* PF Port H Interrupt A 							*/
#define INTNO_ETHERNET_TX	   	30  /* DMA Channel 2 (Ethernet TX/NFC) Interrupt 				*/
#define INTNO_PFB_PORTH  		31  /* PF Port H  Interrupt B 							*/

/* Peripheral Masks For SIC_ISR1, SIC_IWR1, SIC_IMASK1 */
#define INTNO_TIMER0			32	/* Timer 0 Interrupt								*/
#define INTNO_TIMER1			33	/* Timer 1 Interrupt 								*/
#define INTNO_TIMER2			34	/* Timer 2 Interrupt 								*/
#define INTNO_TIMER3			35	/* Timer 3 Interrupt 								*/
#define INTNO_TIMER4			36	/* Timer 4 Interrupt 								*/
#define INTNO_TIMER5			37	/* Timer 5 Interrupt 								*/
#define INTNO_TIMER6			38	/* Timer 6 Interrupt 								*/
#define INTNO_TIMER7			39	/* Timer 7 Interrupt 								*/
#define INTNO_PFA_PORTG		40	/* PF Port G Interrupt A 							*/
#define INTNO_PFB_PORTG		41	/* PF Port G Interrupt B		 					*/
#define INTNO_MDMA0_DST		42	/* DMA Channels 12 (MDMA0 Destination) TX Interrupt			*/
#define INTNO_MDMA0_SRC		42	/* DMA Channels 13 (MDMA0 Source) RX Interrupt				*/
#define INTNO_MDMA1_DST		43	/* DMA Channels 14 (MDMA1 Destination) TX Interrupt			*/
#define INTNO_MDMA1_SRC		43	/* DMA Channels 15 (MDMA1 Source) RX Interrupt 				*/
#define INTNO_WDOG				44	/* Software Watchdog Timer Interrupt 					*/
#define INTNO_PFA_PORTF		45	/* PF Port F Interrupt A		 					*/
#define INTNO_PFB_PORTF		46	/* PF Port F Interrupt B 							*/
#define INTNO_SPI0_ERR     	47  /* Error Interrupt (SPI0 status interrupt) 				*/
#define INTNO_SPI1_ERR			48	/* Error Interrupt (SPI1 status interrupt)				*/
/* reserved */
/* reserved */
#define INTNO_RSI_INT0			51	/* USB EINT interrupt 								*/
#define INTNO_RSI_INT1			52	/* USB INT0 interrupt 								*/
#define INTNO_PWM_TRIPINT		53	/* USB INT1 interrupt 								*/
#define INTNO_PWM_SYNCINT		54	/* USB INT1 interrupt 								*/
#define INTNO_PTP_STATINT		55	/* USB DMAINT interrupt								*/



#ifndef _MACRO_ONLY

/*
* SIC_IARxを解析して、イベント順位ごとに割り当てられている割り込み
* のビットマップを作る。SIC_IARxのフィールド値は優先順位-7なので
* その分補正する。
*/
void make_priority_mask( void );

/*
 *  ターゲットシステム依存の初期化
 */
extern void	sys_initialize(void);

/*
 *  ターゲットシステムの終了
 *
 *  システムを終了する時に使う．ROMモニタ／GDB STUB呼出しで実現する．
 */
extern void	sys_exit(void);

/*
 *  ターゲットシステムの文字出力
 *
 *  システムの低レベルの文字出力ルーチン．ROMモニタ／GDB STUB呼出しで実
 *  現する．
 */
extern void	sys_putc(char c);

/*
* SIC_IARxを解析して、イベント順位ごとに割り当てられている割り込み
* のビットマップを作る。SIC_IARxのフィールド値は優先順位-7なので
* その分補正する。
*/
void make_priority_mask( void );

/*
 * 割り込みの許可、禁止
 *
 */
typedef unsigned int INTNO;
extern ER ena_int( INTNO intno );
extern ER dis_int( INTNO intno );

/*
 * 割り込みマスクの操作
 *
 *
 */
typedef unsigned long long int IMS;
extern ER chg_ims( IMS ims );
extern ER get_ims( IMS * p_ims );

/*
 *  割り込みをデバイスに割り当てる
 */
extern void device_dispatcher( unsigned int priority, unsigned int imask );

/**
 * スプリアス・イベント・ハンドラ
 */
void spurious_exc_handler(VP p_excinf);
void spurious_int_handler();

#endif /* _MACRO_ONLY */
#endif /* _SYS_CONFIG_H_ */
