/**
 *	@file	main_task.c
 *	@brief	KOBANZAME SDK Sample Task
 *
 *	KOBANZAME SDK
 *		Software Developers Kit for Blackfin DSP Evaluation Board(KOBANZAME).
 *		
 *	Copyright (C) 2010, KOBANZAME SDK Project, all right reserved
 *
 *	LICENSE:
 *	The software is a free and you can use and redistribute it for
 *	personal, non-profit or commercial products. Redistributions of
 *	source code must retain the above copyright notice. There is no 
 *	warranty in this software, if you suffer any damages by using 
 *	the software.
 */

 /*--- include -----------------------------------*/
#include "kobanzame.h"
#include "ntshell.h"
#include "ntshell_usrcmd.h"

static ntshell_t ntshell;

static int func_read(char *buf, int cnt)
{
	int i;
	for (i = 0; i < cnt; i++) {
		int c;
		c = getc( stdin );
		buf[i] = c;
	}

    return 0;
}

static int func_write(const char *buf, int cnt)
{
	int i;
	for (i = 0; i < cnt; i++) {
		int c = buf[i];
		putc( c, stdout );
	
	}
    return 0;
}


/*=========================================================================*/
/**
 * @fn		void main_task(VP_INT exinf)
 * @brief	Main Task
 * @param	extinf from uITRON Configurator
 */
/*=========================================================================*/
void main_task(VP_INT exinf)
{
	int major, minor, release;
	
	ntshell_version( &major, &minor, &release );
	
	printf("Natural Tiny Shell (ver %d.%d.%d)\n", major, minor, release );
	
	ntshell_execute(&ntshell, func_read, func_write, ntshell_usrcmd_execute, NULL);
	
	/* main_tsk exit */
	ext_tsk();
}
