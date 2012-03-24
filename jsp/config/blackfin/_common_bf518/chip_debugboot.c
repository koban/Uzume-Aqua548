#include "jsp_kernel.h"

#ifdef __GNUC__
#include "cdefBF518.h"       /* gnu tool chain */
#elif defined(__ECC__)
#error "Don't use sys_debugboot.c for VDSP "
#else
#error "Compiler is not supported"
#endif


/*
* gdbserverがターゲットのリセット機能を提供しないため、gdb経由でターゲット
* にアプリッケーションをダウンロードすると正しく動作しないことがある。
* このルーチンはターゲットを一度だけリセットする。
*/
void boot_for_gdb( void )
{
    if ( enable_boot_for_gdb ){   /* ソフトウェアリセットが起きていないなら以下実行　*/
    	enable_boot_for_gdb = 0;			/* リブートは一回だけ */
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
    else
    {
    	*pPLL_LOCKCNT = 0x200;			/* Fix the anormaly 05000430 */
    }
}
