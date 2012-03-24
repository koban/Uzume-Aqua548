#include "jsp_kernel.h"
#include "sil.h"



/*
* gdbserverがターゲットのリセット機能を提供しないため、gdb経由でターゲット
* にアプリッケーションをダウンロードすると正しく動作しないことがある。
* このルーチンはターゲットを一度だけリセットする。
*
* なお、パラメータ debugが FALSEの場合、リセットはしない
*/
void boot_for_gdb(void)
{
	// RESET_SOFTWARE		0x8000
	if ( enable_boot_for_gdb ){
		enable_boot_for_gdb = 0;			/* rebootは一回だけ*/
        *__pSWRST = 0x07;                     /* 内蔵ペリフェラルのリセット　*/
        asm volatile( "ssync;" );
        *__pSWRST = 0x00;                     /* 内蔵ペリフェラルのリセット解除。 */
        asm volatile( "ssync;" );
        *__pSYSCR |= 0x10;                    /* no boot on core reset */
        asm volatile( "ssync;" );
        asm volatile( "raise 1;" );         /* コアリセット */
        while( 1 )
            ;                               /*リセットが発生するまでループ*/
    }
}
