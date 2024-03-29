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
 *  Copyright (C) 2004,2006,2006 by Takemasa Nakamura
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

/*
 *	ターゲットシステム依存モジュール（ADSP-BF531/2/3用）
 */

#include "jsp_kernel.h"
#include <sil.h>
#include <cdefBF533.h>


#define SYSCFG_VALUE 0x36

/*
 *  ターゲットシステム依存の初期化
 */
void
sys_initialize()
{
	// BF531/2/3のアノーマリー対策 rev 0.1, 0.2用
	Asm( "SYSCFG=%0;"  : :"d"(SYSCFG_VALUE) ) ;

	/*
	 * スプリアス割り込みハンドラの設定
	 *
	 * cpu_initialize()が行うダミーの割り込みハンドラの設定を上書きする。
	 * アプリケーションが割り込みハンドラを設定すると、以下の設定も上書き
	 * される。
	 */
	int i;

	for ( i=0; i<DEVICE_INTERRUPT_COUNT+3; i++ )
		dev_vector[i] = &spurious_int_handler;

	exc_vector = &spurious_exc_handler;

	/*
	 *  PLLの設定
	 *
	 */
	/*
	 *  27MHz*22倍=594MHzに設定する(600MHz)
	 *  27MHz*28倍=756MHzに設定する(750MHz)
	 *  CSEL = 1; SSEL = 5 (600MHz)
	 *  CSEL = 1; SSEL = 6 (750MHz)
	 */
#ifndef FORCE_PLL_INITIALIZE
	 	// PLLが初期値のままであり、かつ、SDRAMが利用中でなければPLLを初期化する
	// EBE	0x00000001
	 if ( ( *__pPLL_CTL == 0x1400 ) && ( !(*__pEBIU_SDBCTL & 0x00000001 /* EBE */ ) ) )
#endif
	 {

		*__pSIC_IWR = 1<<0;				// PLLのみIWRを許す
	// CCLK_DIV1			0x0000
	// CCLK_DIV2			0x0010
	// CCLK_DIV4			0x0020
	// CCLK_DIV8			0x0030
#if CSELVAL == 1
		*__pPLL_DIV = 0x0000 | (SSELVAL);	// CCLK_DIV1
#elif CSELVAL == 2
		*__pPLL_DIV = 0x0010 | (SSELVAL);	// CCLK_DIV2
#elif CSELVAL == 4
		*__pPLL_DIV = 0x0020 | (SSELVAL);	// CCLK_DIV4
#elif CSELVAL == 8
		*__pPLL_DIV = 0x0030 | (SSELVAL);	// CCLK_DIV8
#else
#error Wrong CSELVAL. Must be one of 1,2,4,8.
#endif

		*__pPLL_CTL = MSELVAL << 9;

		asm("cli r0; csync; idle; sti r0;": : :"R0");
		*__pSIC_IWR = 0xFFFFFFFF;	// IWR_ENABLE_ALL;
	}

	    /*
	     *  UART分周比の設定
	     *
	     *  Logtaskが動作する前にsys_putc()を使うための設定を行う
	     */
#define	DLAB 0x80

			/* Blackfin 固有の設定。UARTイネーブル */
	    *pUART_GCTL = 1;

			/* クロックの設定 */
		*pUART_LCR |= DLAB;
		*pUART_DLL = UART0_DIVISOR & 0xFF ;
		*pUART_DLH = UART0_DIVISOR >> 8;
		*pUART_LCR &= ~DLAB;

			/* モード設定, パリティ無し 8bit data, 1 stop bit */
	    *pUART_LCR = 0x03;

			/* 割込み禁止 */
	    *pUART_IER = 0;


}


unsigned int priority_mask[16]={
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x0000007F,
0x00000180,
0x00001E00,
0x0000E000,
0x00070000,
0x00180000,
0x00E00000,
0x00000000,
0x00000000
};


/*
* SIC_IARxを解析して、イベント順位ごとに割り当てられている割り込み
* のビットマップを作る。SIC_IARxのフィールド値は優先順位-7なので
* その分補正する。
*/
#define INSTALL_PRIORITY    \
    for ( i=0; i<8; i++ ){ \
        priority = iar & 0xf;                         /* IARから優先順位を取り出す */ \
        priority_mask[priority + 7] |= device;        /* 取り出した優先順位に基づきデバイスを登録 */ \
        device <<= 1;                                 /* 次のデバイス */ \
        iar >>= 4;                                    /* 次のIARフィールド */ \
    }

void make_priority_mask( void )
{
	unsigned int i, priority, device, iar;


	/*
 *  割り込み順位ごとのISRビットマップの作成
 *  SIC_IARxの設定はこの部分より前に済ませること
 */

 	for ( i=0; i<16; i++ ){
 		priority_mask[i] = 0;
 	}

	device = 1;
	iar = *__pSIC_IAR0;
    INSTALL_PRIORITY

	iar = *__pSIC_IAR1;
    INSTALL_PRIORITY

	iar = *__pSIC_IAR2;
    INSTALL_PRIORITY

}


/*
 * 割り込みの許可、禁止
 *
 */
ER ena_int( INTNO intno )
{
	unsigned int mask;

	if ( intno >= DEVICE_INTERRUPT_COUNT )
		return ( E_PAR );
	else {
		SIL_PRE_LOC;

		mask = 1 << intno;
		SIL_LOC_INT();			// 管理外割り込みまで禁止する
		*__pSIC_IMASK |= mask;
		asm volatile( "ssync;" );
		SIL_UNL_INT();			// 割り込み再許可
		return (0);
	}
}
/*
 * dis_int()は、この実装ではSIC_IMASKの指定ビットをクリアする。
 *
 * しかし、SIC_IMASKのクリアはアプリケーション実行中には危険であり、
 * Blackfinのマニュアルでも婉曲的に禁止されている。そのため、TOPPERS/JSP for Blackfin
 * では、この関数を実装しない。
 *
ER dis_int( INTNO intno )
{
	unsigned int mask;

	if ( intno >= DEVICE_INTERRUPT_COUNT )
		return ( E_PAR );
	else {
		mask = 1 << intno;
		mask = ~mask;
		*__pSIC_IMASK &= mask;
		return (0);
	}
}
*/

/*
 * chg_ims()は、この実装ではSIC_IMASKを変更する。
 *
 * しかし、SIC_IMASKのビットのクリアはアプリケーション実行中には危険であり、
 * Blackfinのマニュアルでも婉曲的に禁止されている。そのため、TOPPERS/JSP for
 * Blackfinでは、この関数を実装しない。
 *
ER chg_ims( IMS ims )
{
	*__pSIC_IMASK = ims;
	return( 0 );
}
*/


extern ER get_ims( IMS * p_ims )
{
	*p_ims = *__pSIC_IMASK;
	return( 0 );;
}


/*
 *  割り込みをデバイスに割り当てる
 */
void device_dispatcher(  unsigned int priority, unsigned int imask )
{
	unsigned int candidates, device;

	candidates = priority_mask[priority] & *__pSIC_ISR & *__pSIC_IMASK;	// 現在のプライオリティに相当する割込み源を特定する

	asm volatile("sti %0;": : "d"(imask) );

	if ( ! candidates ) // 割り込み源が特定できないなら、コア由来である
	{
		if ( priority == ik_hardware_err)
			dev_vector[INHNO_HW_ERROR]();
		else
			if ( priority == ik_timer)
			dev_vector[INHNO_TIMER]();
		else
			dev_vector[INHNO_RAISE]();		//　ソフトウェア割り込み

	}
	else
	{
		if ( candidates & 0x80000000 )
			device = 31;
		else
		{
#ifdef __GNUC__
	asm ( "r1.L = signbits %1; %0 = r1.L(z);":"=d"(device) :"d"(candidates): "R1"  );
#elif defined(__ECC__)
	asm( "%0 = signbits %1;" : "=l"( device ) : "d"( candidates ) );
#else
#error "Compiler is not supported"
#endif
			device = 30 - device;		// bit mask is converted to bit number
		}
		dev_vector[device]();
	}
}



/*
 *  ターゲットシステムの終了
 */
void
sys_exit()
{
	while(1)
		;
}
/*
 *  ターゲットシステムの文字出力
 */
void
sys_putc(char c)
{
	if ( c== 0x0A )			/* もし LF ならば */
		sys_putc( 0x0D );	/* CRを一文字送信 */

	while( !( *pUART_LSR & (1<<5)) )
		;		/* UART0 LSRのTHREが1になるまで待つ。1ならば送信レジスタ空き。*/

	*pUART_THR = c;	/* 一文字送信 */
}



