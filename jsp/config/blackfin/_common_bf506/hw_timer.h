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
 *  Copyright (C) 2010,2011 by Kaneko System Co., Ltd.
 *
 *  上記著作権者は，以下の (1)～(4) の条件か，Free Software Foundation
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
 *	タイマドライバ（ADSP-BF506用）
 *
 */

#ifndef _HW_TIMER_H_
#define _HW_TIMER_H_

#include <s_services.h>


#ifndef _MACRO_ONLY

#ifdef __GNUC__			// gcc
#include "cdefBF506F.h"
#elif defined(__ECC__)	// visualdsp
#include <cdefbf506F.h>
#else
#error "Compiler is not supported"
#endif

//#include <ccblkfn.h>




/**************************************************************
 *  タイマの起動処理
 *
 *  タイマを初期化し，周期的なタイマ割込み要求を発生させる．
 *
 **************************************************************/
Inline void
hw_timer_initialize()
{

#ifdef USE_TIC_CORE
	// Core timer
	*pTCNTL = TMPWR;					// power up timer;
	*pTPERIOD = CORECLOCK/1000;			// CORE CLOCK is defined in sys_config.h
	*pTCNTL = TMPWR | TMREN | TAUTORLD;
#else
	// GPT7(General-purpose timer7)

	*pTIMER7_CONFIG = PERIOD_CNT | OUT_DIS |IRQ_ENA | PWM_OUT;	// PWM_OUT, Output Pad disable
	*pTIMER7_PERIOD = SYSCLOCK/1000;	// SYS CLOCK is defined in sys_config.h
	*pTIMER7_WIDTH = 1;					// 0 < width < period
	*pTIMER_ENABLE = TIMEN7;			// timer7 start

	ena_int( INHNO_TIMER );				// enable Timer Interrupt

#endif
	asm("ssync;");

}

/**************************************************************
 *  タイマ割込み要求のクリア
 **************************************************************/
Inline void
hw_timer_int_clear()
{
#ifdef USE_TIC_CORE
	// Core timer
	*pTCNTL = TMPWR | TMREN | TAUTORLD | TINT;
#else
	// GPT7(General-purpose timer7)
	*pTIMER_STATUS = TIMIL7;
#endif
	asm("ssync;");
}

/**************************************************************
 *  タイマの停止処理
 *
 *  タイマの動作を停止させる．
 **************************************************************/
Inline void
hw_timer_terminate()
{
	/*
	 *  タイマの動作を停止する．
	 */
#ifdef USE_TIC_CORE
	// Core timer
	*pTCNTL = TMPWR | TAUTORLD | TINT;		// 停止
	*pTCNTL = 0;							// パワーダウンモード
#else
	// GPT2(General-purpose timer7)
	*pTIMER_DISABLE = TIMDIS7;			// timer7 disable
#endif
	asm("ssync;");
}


#endif /* _MACRO_ONLY */
#endif /* _HW_TIMER_H_ */
