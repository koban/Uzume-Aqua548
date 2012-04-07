/**
 * \file postmotem533.c
 * \brief ADSP-BF533用のポストモーテムダンプルーチン群
 *
 * ハードウェアエラー用のハンドラと、例外用のハンドラからなる。いずれも呼び出されるとUARTから
 * ポストモーテム出力を表示する。
 */
#include "jsp_kernel.h"
#include <cdefBF537.h>

/**
 * \brief UARTおよび付随するDMAの動作を停止し、すべての割り込みを禁止する。
 *
 * 最後にUART_IERをクリアするのは、UART割り込みを禁止すると同時にDMAも禁止するため。
 * UART DMAは、UARTの割り込み線で駆動されているので、割り込みを禁止すればDMAリクエスト
 * も停止する。
 *
 * UARTの初期化をどうするか悩ましいが、ここではそのまま以前の設定を利用することにする。
 *
 * ポストモーテム・ダンプを目的としているので、この状態からの回復は考えない。
 */
static void pm_occupy_uart()
{
		/* すべてのコア割り込みを禁止する */
	asm( "cli r0;" : : : "R0" );

		/* すべてのシステム割り込みソースを禁止する */
	*pSIC_IMASK = 0;

		/* UART_IERをディセーブルにすることで、DMAを殺せる */
	*pUART0_IER = 0;
}

/**
 * \brief 一文字出力
 *
 * UARTの送信レジスタが空になるのを待って一文字出力する。
 */
static void pm_putc( unsigned char c )
{

		/* THRが空になるまで待つ */
	while ( ! ( *pUART0_LSR & THRE ) )
		;

		/*  THRが空になったら1文字送信 */
	*pUART0_THR = c;
}

/**
 * \brief コンソール入力監視
 *
 * UARTの受信レジスタにデータがあれば、読み込む。データが"!"なら真、
 * それ以外なら偽を返す。
 */
static BOOL is_ready()
{
		/* 受信データはあるか。 */
	if ( *pUART0_LSR & DR )

	{
		char c;

		c= *pUART0_RBR;
		if ( c == '!' )
			return TRUE;
	}
	return FALSE;
}


/**
 * \brief 文字列出力
 *
 * 受け取った文字列をUARTに出力する。
 */
static void pm_putstr( char * s )
{
	int i;

	i=0;
	while( s[i] )	/* 末端のNULLが現れるまで出力 */
		pm_putc(s[i++]);
}

/**
 * \brief 1バイトをヘキサデシマルで出力する。
 */
static void pm_puthex1byte( unsigned int data )
{
	int i;
	int nibble;

		/* 8bit内のすべてのニブルを処理 */
	for ( i=0; i<2; i++ )
	{
			/*　最上位ニブルを抽出　*/
		nibble = ( data >> 4 ) & 0xF;
			/* 抽出したニブルを出力 */
		if ( nibble < 10 )
			pm_putc( nibble + '0' );
		else
			pm_putc( nibble - 10 + 'A' );
			/* 次のニブル */
		data <<= 4;
	}
}

/*
 * \brief 改行記号を出力する
 */
static void pm_putrtn()
{
	pm_putstr("\r\n");
}

/**
 * \brief 4バイトをヘキサデシマルで出力する。
 */
static void pm_puthex4byte( unsigned int data )
{
	int i;
	int nibble;

		/* 32bit内のすべてのニブルを処理 */
	for ( i=0; i<8; i++ )
	{
			/*　最上位ニブルを抽出　*/
		nibble = ( data >> 28 ) & 0xF;
			/* 抽出したニブルを出力 */
		if ( nibble < 10 )
			pm_putc( nibble + '0' );
		else
			pm_putc( nibble - 10 + 'A' );
			/* 次のニブル */
		data <<= 4;
	}

}

/**
 * \brief 例外フラグ
 *
 * 例外が発生したときには真、そうでなければ偽。hwei_handler()に例外か否かを伝える。
 *
 * GCCが張り切ってlink/unlink命令の位置を最適化するため、hwei_hanlder()の
 * 中で性格にfpを手繰れない。そのため、dummyをアクセスすることでlink/unlinkの位置
 * 最適化の抑止を図る役目もある。効果があるかどうかは不明。
 */
static volatile int expFlag =0;
/**
 * \brief ハードウェア・エラー・ハンドラ
 *
 * ハードウェア・エラー時に呼び出されて、ハードウェア・エラー・割り込みのポストモーテム処理を行う。
 * 最初にFPを手繰って、割り込みのスタックフレームを探す。次にすべての割り込みを禁止し、
 * UART0を占有したあと、ポーリングを使ってスタックに保存された各レジスタのダンプを行う。
 * DEF_INH(INHNO_HW_ERROR, { TA_HLNG, hwei_handler });
 *
 */
void spurious_int_handler()
{
	unsigned int * fp, *ptr ;	/* フレーム・ポインタを手繰っていくための変数 */
	unsigned int reg;	/* システムレジスタを受け取るための変数 */
	unsigned int imask, sic_imask; /*マスク記録レジスタ*/

			/* あとで使う */
	imask = *pIMASK;
	sic_imask = *pSIC_IMASK;
			/* UART0を初期化し、DMAと割り込みを禁止する */
	pm_occupy_uart();

	while (1)
	{
		int count = 0;

		pm_putstr( "Type '!' to display post mortem dump" ); pm_putrtn();

		while ( ! is_ready() )
		{
			int i;
			for ( i=0; i<100000000; i++)
				asm volatile ("nop;");
			if ( count > 30 )
			{
				pm_putstr( "Type '!' to display post mortem dump" ); pm_putrtn();
				count = 0;
			}
			else
				count ++;
		}
		pm_putrtn();


		/* 現在の関数のFPを取得する */
		asm ( "%0=fp;" : "=d"((unsigned int)fp) );

		/*
		 * この関数を呼び出した関数 ( interrupt_dispatcher ) のFPを取得する。
		 * FPは呼び出し関数のFPの格納番地を指していることを利用する
		 */
		fp = (void *)*fp;
		/*
		 * interrupt_dispatcher を呼び出した関数のFPを取得する。
		 * その関数は割り込みハンドラの入り口処理部に他ならない。
		 */
		fp = (void *)*fp;

		/* いまや、FPは割り込み受付時の保存されたレジスタ群を指している */

		/* プッシュされた P0を指す */
		ptr = fp + 2;
		/*
		 * 上位
		 * 		0	1	2	3	4	5	6	7	8	9
		 * -----------------------------------------------
		 * 	00	P0	RTS	FP	R0	R1	R2	R3	R4	R5	R6
		 * 	10	R7	P1	P2	P3	P4	P5	I3	I2	I1	I0
		 * 	20	M3	M2	M1	M0	B3	B2	B1	B0	L3	L2
		 * 	30	L1	L0	A0x	A0w	A1x	A1w	LC1	LC0	LT1	LT0
		 * 	40	LB1	LB0	AST	RETI
		 * 下位
		 *
		*/
		if ( expFlag )
			pm_putstr( "Spurious Exception  !!" );
		else
			pm_putstr( "Spurious Interrupt  !!" );
		pm_putrtn();

		pm_putstr( "Registers On Stack :" );	pm_putrtn();
		pm_putstr( "P0   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "RETS " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "FP   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "R0   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "R1   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "R2   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "R3   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "R4   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "R5   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "R6   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "R7   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "P1   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "P2   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "P3   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "P4   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "P5   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "I3   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "I2   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "I1   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "I0   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "M3   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "M2   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "M1   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "M0   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "B3   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "B2   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "B1   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "B0   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "L3   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "L2   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "L1   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "L0   " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "A0   " );	pm_puthex1byte( *(ptr--) ); pm_putstr( ":" );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "A1   " );	pm_puthex1byte( *(ptr--) ); pm_putstr( ":" );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "LC1  " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "LC0  " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "LT1  " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "LT0  " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "LB1  " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "LB0  " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "ASTAT " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putstr( "RETI  " );	pm_puthex4byte( *(ptr--) ); pm_putrtn();
		pm_putrtn();
		pm_putstr( "System Registers :" );	pm_putrtn();
		pm_putstr( "SIC_IMASK " );	pm_puthex4byte( sic_imask ); pm_putrtn();
		pm_putstr( "SIC_ISR   " );	pm_puthex4byte( *pSIC_ISR ); pm_putrtn();
		pm_putstr( "IMASK     " );	pm_puthex4byte( imask ); pm_putrtn();
		pm_putstr( "ILAT      " );	pm_puthex4byte( *pILAT ); pm_putrtn();
		pm_putstr( "IPEND     " );	pm_puthex4byte( *pIPEND ); pm_putrtn();
		asm( "%0=SEQSTAT;" : "=d"(reg) );
		pm_putstr( "SEQSTAT   " );	pm_puthex4byte( reg ); pm_putrtn();
		pm_putstr( "  EXCAUSE    " );	pm_puthex1byte( reg & 0x3F ); pm_putrtn();
		pm_putstr( "  HWERRCAUSE " );	pm_puthex1byte( (reg>>14)&0x1F ); pm_putrtn();
		pm_putstr( "DMA0_IRQ_STATUS    " );	pm_puthex4byte( *pDMA0_IRQ_STATUS ); pm_putrtn();
		pm_putstr( "DMA1_IRQ_STATUS    " );	pm_puthex4byte( *pDMA1_IRQ_STATUS ); pm_putrtn();
		pm_putstr( "DMA2_IRQ_STATUS    " );	pm_puthex4byte( *pDMA2_IRQ_STATUS ); pm_putrtn();
		pm_putstr( "DMA3_IRQ_STATUS    " );	pm_puthex4byte( *pDMA3_IRQ_STATUS ); pm_putrtn();
		pm_putstr( "DMA4_IRQ_STATUS    " );	pm_puthex4byte( *pDMA4_IRQ_STATUS ); pm_putrtn();
		pm_putstr( "DMA5_IRQ_STATUS    " );	pm_puthex4byte( *pDMA5_IRQ_STATUS ); pm_putrtn();
		pm_putstr( "DMA6_IRQ_STATUS    " );	pm_puthex4byte( *pDMA6_IRQ_STATUS ); pm_putrtn();
		pm_putstr( "DMA7_IRQ_STATUS    " );	pm_puthex4byte( *pDMA7_IRQ_STATUS ); pm_putrtn();
		pm_putstr( "DMA8_IRQ_STATUS    " );	pm_puthex4byte( *pDMA8_IRQ_STATUS ); pm_putrtn();
		pm_putstr( "DMA9_IRQ_STATUS    " );	pm_puthex4byte( *pDMA9_IRQ_STATUS ); pm_putrtn();
		pm_putstr( "DMA10_IRQ_STATUS    " );	pm_puthex4byte( *pDMA10_IRQ_STATUS ); pm_putrtn();
		pm_putstr( "DMA11_IRQ_STATUS    " );	pm_puthex4byte( *pDMA11_IRQ_STATUS ); pm_putrtn();
		pm_putstr( "MDMA_D0_IRQ_STATUS " );	pm_puthex4byte( *pMDMA_D0_IRQ_STATUS ); pm_putrtn();
		pm_putstr( "MDMA_S0_IRQ_STATUS " );	pm_puthex4byte( *pMDMA_S0_IRQ_STATUS ); pm_putrtn();
		pm_putstr( "MDMA_D1_IRQ_STATUS " );	pm_puthex4byte( *pMDMA_D1_IRQ_STATUS ); pm_putrtn();
		pm_putstr( "MDMA_S1_IRQ_STATUS " );	pm_puthex4byte( *pMDMA_S1_IRQ_STATUS ); pm_putrtn();
		pm_putstr( "SPI_STAT           " );	pm_puthex4byte( *pSPI_STAT ); pm_putrtn();
		pm_putstr( "PPI_STATUS         " );	pm_puthex4byte( *pPPI_STATUS ); pm_putrtn();
		pm_putstr( "SPORT0_STAT        " );	pm_puthex4byte( *pSPORT0_STAT ); pm_putrtn();
		pm_putstr( "SPORT1_STAT        " );	pm_puthex4byte( *pSPORT1_STAT ); pm_putrtn();
		pm_putstr( "TIMER_STATUS       " );	pm_puthex4byte( *pTIMER_STATUS ); pm_putrtn();
		pm_putstr( "EBIU_SDSTAT        " );	pm_puthex4byte( *pEBIU_SDSTAT ); pm_putrtn();
		pm_putrtn();
		pm_putstr( "Calling Stack :" );	pm_putrtn();

		while( fp )
		{
			pm_putstr( "Called from " );	pm_puthex4byte( *(fp+1) ); pm_putrtn();
			fp = *fp;
		}
	}
}

/**
 * \brief CPU例外ハンドラ
 *
 * CPU例外ハンドラとしてcfgファイルに登録する。 hwei_handler()は呼ばれたら戻ってこないが、
 * そのあとにもexpFlagに値を代入しているのは、最適化によってunlink命令の値がルーチン呼び出しの
 * 前に移動することを防ぐためである。
 *
 * DEF_EXC(CPUEXC1, { TA_HLNG, excp_handler} );
 *
 */
void spurious_exc_handler(VP p_excinf)
{
	expFlag = TRUE;
	spurious_int_handler();
}

