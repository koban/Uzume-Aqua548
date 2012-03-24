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
 *	ターゲットシステム依存モジュール（ADSP-BF518用）
 */

#include "jsp_kernel.h"
#include <sil.h>

#ifdef __GNUC__
#include <cdefBF518.h>		/* gnu tool chain */
#elif defined(__ECC__)
#include <cdefbf518.h>				/* VisualDSP++ */
#include <sys/exception.h>
#include <ccblkfn.h>
#include <sysreg.h>
#else
#error "Compiler is not supported"
#endif




/*
 *  ターゲットシステム依存の初期化
 */
void
sys_initialize()
{
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
	 *  SSELVAL, CSELVALはboard_config.hにて定義。FORCE_PLL_INITIALIZEはsys_config.hで
	 *  必要に応じて宣言する。
	 */
#ifndef FORCE_PLL_INITIALIZE
	 	// PLLが初期値のままであり、かつ、SDRAMが利用中でなければPLLを初期化する
	 if ( ( *pPLL_CTL == 0x1400 ) && ( !(*pEBIU_SDBCTL & EBE ) ) )
#endif
	 {

		*pSIC_IWR = IWR_ENABLE(0);				// PLLのみIWRを許す
#if CSELVAL == 1
		*pPLL_DIV = CSEL_DIV1 | SET_SSEL(SSELVAL);
#elif CSELVAL == 2
		*pPLL_DIV = CSEL_DIV2 | SET_SSEL(SSELVAL);
#elif CSELVAL == 4
		*pPLL_DIV = CSEL_DIV4 | SET_SSEL(SSELVAL);
#elif CSELVAL == 8
		*pPLL_DIV = CSEL_DIV8 | SET_SSEL(SSELVAL);
#else
#error Wrong CSELVAL. Must be one of 1,2,4,8.
#endif
				// PLLの分周器に値を設定する
		*pPLL_CTL = MSELVAL << 9;

				// PLLを起動する。
		asm("cli r0; csync; idle; sti r0;": : :"R0");
		*pSIC_IWR = IWR_ENABLE_ALL;
	}

	/*
	 *  UART分周比の設定
	 *
	 *  Logtaskが動作する前にsys_putc()を使うための設定を行う。以下の設定は
	 *  serial関連のタスクが起動したときに上書きされる。
	 */
#define	DLAB 0x80

		/* Blackfin 固有の設定。UARTイネーブル */
	*pUART0_GCTL = 1;

		/* クロックの設定 */
	*pUART0_LCR |= DLAB;
	*pUART0_DLL = UART0_DIVISOR & 0xFF ;
	*pUART0_DLH = UART0_DIVISOR >> 8;
	*pUART0_LCR &= ~DLAB;

		/* モード設定, パリティ無し 8bit data, 1 stop bit */
	*pUART0_LCR = 0x03;

		/* 割込み禁止 */
	*pUART0_IER = 0;

    *pPORTG_MUX &= ~(0x0C00);	// bit11:10 だけを0にする
    *pPORTG_MUX |= 0x01<<10;	// bit11:10 だけを01にする
    *pPORTG_FER |= 0x0600;		// PG9,PG10をUART0に割り振る
}

/*
 * priority_maskは、event順位0..15に応じた割り込み要求のビットマップを保持する。
 * priority_mask[]のインデックスはevent順位と同じである。割り込み要因は BF51xでは
 * 64要因あるので、unsigned long longによる64bit型としている。
 *
 * device_dispatch()はpriority_mask[]を参照して、現在起きたイベントがどの割り込み
 * 要因であるか決定する助けとする。
 *
 * この変数はmake_priority_mask()を呼ぶ度に、実際のIARxの値に応じて上書きされる。
 *
 */
unsigned long long int priority_mask[16]={
	0x0000000000000000ull,	/* EMU */
	0x0000000000000000ull,	/* RST */
	0x0000000000000000ull,	/* NMI */
	0x0000000000000000ull,	/* EVX */
	0x0000000000000000ull,
	0x0000000000000000ull,	/* IVHW */
	0x0000000000000000ull,	/* IVTMR */
	0x0007800000003FFFull,	/* IVG7 */
	0x000000000000C000ull,	/* IVG8 */
	0x00000000000F0000ull,	/* IVG9 */
	0x00F8000003F00000ull,	/* IVG10 */
	0x00000000FC000000ull,	/* IVG11 */
	0x000003FF00000000ull,	/* IVG12 */
	0x00007C0000000000ull,	/* IVG13 */
	0x0000000000000000ull,	/* IVG14 */
	0x0000000000000000ull	/* IVG15 */
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

/*
*  割り込み順位ごとのISRビットマップの作成SIC_IARxの設定はこの部分より前に済ませること。
*  この関数はuITRONのイニシャライザで使用することを想定しており、特に割り込みから保護していない。
*/
void make_priority_mask( void )
{
	unsigned int i, priority, iar;
	unsigned long long int device;



        // priority_maskは、event順位0..15に応じた割り込み要求のビットマップを保持する。
		// 最初にクリアする
 	for ( i=0; i<16; i++ ){
 		priority_mask[i] = 0;
 	}

	device = 1;
	iar = *pSIC_IAR0;
	INSTALL_PRIORITY

	iar = *pSIC_IAR1;
    INSTALL_PRIORITY

	iar = *pSIC_IAR2;
    INSTALL_PRIORITY

    iar = *pSIC_IAR3;
    INSTALL_PRIORITY

	iar = *pSIC_IAR4;
	INSTALL_PRIORITY

	iar = *pSIC_IAR5;
    INSTALL_PRIORITY

	iar = *pSIC_IAR6;
    INSTALL_PRIORITY

	iar = *pSIC_IAR7;
    INSTALL_PRIORITY

}


/*
 * 割り込みの許可。ADSP-BF51xは効率的な割り込み処理と安全な割り込み禁止を両立する
 * 手段を持たないため、禁止関数は置いていない。
 *
 * 不便ではあるが、プログラマに注意を促すためにそうしている。
 */
ER ena_int( INTNO intno )
{
	unsigned int mask;
	SIL_PRE_LOC;

	if ( intno >= DEVICE_INTERRUPT_COUNT )
		return ( E_PAR );
	else {
		if ( intno < 32 ){
			mask = 1 << intno;
			SIL_LOC_INT();
			*pSIC_IMASK0 |= mask;
			asm volatile( "ssync;" );
			SIL_UNL_INT();
		}
		else{
			mask = 1 << (intno-32);
			SIL_LOC_INT();
			*pSIC_IMASK1 |= mask;
			asm volatile( "ssync;" );
			SIL_UNL_INT();
		}
		return (0);
	}
}


/*
 * 割り込みマスクの取得。ADSP-BF51xは効率的な割り込み処理と安全な割り込み禁止を両立する
 * 手段を持たないため、禁止関数は置いていない。
 *
 * 不便ではあるが、プログラマに注意を促すためにそうしている。
 */


extern ER get_ims( IMS * p_ims )
{
	SIL_PRE_LOC;

	SIL_LOC_INT();
	*p_ims = (((IMS)*pSIC_IMASK1)<<32 ) | (IMS)*pSIC_IMASK0;
	SIL_UNL_INT();
	return( 0 );
}


/*
 *  割り込みをデバイスに割り当てる。
 *
 *  この間数は割り込み発生時に共通割り込みハンドラの一部としてアセンブリ言語から割り込み禁止状態で
 *  呼ばれる。実割り込みハンドラを割り込み可能にするため、asm文を使って途中で割り込み可能にしている。
 *  割り込み禁止状態で呼ぶのは割り込み源の特定を安全におこなうためである。
 *
 */
void device_dispatcher( unsigned int priority, unsigned int imask )
{
	unsigned int device, candidates;
	unsigned long long int longcandidates, sic_isr, sic_imask;

		//　以下では SIC_IMASK0,1をまとめて64bitレジスタとして扱っている。SIC_ISRも同じである。
	get_ims(&sic_imask);
	sic_isr = (((IMS)*pSIC_ISR1)<<32 ) | (IMS)*pSIC_ISR0;

		// 現在のプライオリティに相当する割込み源を特定する。
	longcandidates = priority_mask[priority] & sic_isr & sic_imask;

	asm volatile("sti %0;": : "d"(imask) );

	if ( ! longcandidates ) // 割り込み源が特定できないなら、コア由来である
	{
		if ( priority == ik_hardware_err)
			dev_vector[INHNO_HW_ERROR]();
		else
			if ( priority == ik_timer)
			dev_vector[INHNO_TIMER]();
		else
			dev_vector[INHNO_RAISE]();		//　ソフトウェア割り込み

	}
	else if ( longcandidates & 0xFFFFFFFF00000000ull )
	{
		candidates = longcandidates >> 32;
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
		dev_vector[device+32]();
	}
	else
	{
		candidates = (unsigned int)longcandidates;
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
 *  ターゲットシステムの終了。TOPPERS/JSPは対話型ROMモニタに戻ることを想定しているが、
 *  このボードにROMはない。
 */
void
sys_exit()
{
	while(1)
		;
}

/*
 *  ターゲットシステムの文字出力。割り込みが無効な状態でポーリングによる出力を行う。
 */
void
sys_putc(char c)
{
	if ( c== 0x0A )			/* もし LF ならば */
		sys_putc( 0x0D );	/* CRを一文字送信 */

	while( !( *pUART0_LSR & (1<<5)) )
		;		/* UART0 LSRのTHREが1になるまで待つ。1ならば送信レジスタ空き。*/

	*pUART0_THR = c;	/* 一文字送信 */
}



