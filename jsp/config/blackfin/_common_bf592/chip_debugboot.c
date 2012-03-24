#include "jsp_kernel.h"
#include "sil.h"

#ifdef __GNUC__
#include <cdefBF592-A.h>		/* gnu tool chain */
#elif defined(__ECC__)
#include <cdefbf592-A.h>				/* VisualDSP++ */
#include <sys/exception.h>
#include <ccblkfn.h>
#include <sysreg.h>
#else
#error "Compiler is not supported"
#endif

/*
* gdbserverがターゲットのリセット機能を提供しないため、gdb経由でターゲット
* にアプリッケーションをダウンロードすると正しく動作しないことがある。
* このルーチンはターゲットを一度だけリセットする。
*
* なお、パラメータ debugが FALSEの場合、リセットはしない
*/
void boot_for_gdb(void)
{
	// SYSTEM_RESET		0x0007
	// DOUBLE_FAULT		0x0008
	// RESET_DOUBLE		0x2000
	// RESET_WDOG		0x4000
	// RESET_SOFTWARE	0x8000
    if ( enable_boot_for_gdb ){
    	enable_boot_for_gdb = 0;			/* 次はブートしない */
        *pSWRST = 0x07;                     /* 内蔵ペリフェラルのリセット　*/
        asm volatile( "ssync;" );
        *pSWRST = 0x00;                     /* 内蔵ペリフェラルのリセット解除。 */
        asm volatile( "ssync;" );
        *pSYSCR |= 0x10;                    /* no boot on core reset */
        asm volatile( "ssync;" );
        asm volatile( "raise 1;" );         /* コアリセット */
        while( 1 )
            ;                               /*リセットが発生するまでループ*/
    }
}
