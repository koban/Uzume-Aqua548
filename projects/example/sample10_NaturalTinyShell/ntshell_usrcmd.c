/**
 * @file ntshell_usrcmd.c
 * @author Shinichiro Nakamura
 * @brief ナチュラルタイニーシェルタスクコマンドの実装。
 * @details
 */

/*
 * ===============================================================
 *  Natural Tiny Shell - User Command Module
 * ===============================================================
 * Copyright (c) 2010-2011 Shinichiro Nakamura
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * ===============================================================
 */

#include <kernel.h>
//#include <target_syssvc.h>
//#include <syssvc/serial.h>

#include "ntopt.h"
#include "ntlibc.h"
#include "kobanzame.h"
//#include "ntshell_config.h"

static int usrcmd_ntopt_callback(int argc, char **argv, void *extobj);
static int usrcmd_help(int argc, char **argv);
static int usrcmd_info(int argc, char **argv);

/**
 * @brief コマンドテーブル。
 */
typedef struct {
    char *cmd;  /**< コマンド名。 */
    char *desc; /**< コマンド説明。 */
    int (*func)(int argc, char **argv); /**< コールバック関数。 */
} cmd_table_t;

/**
 * @brief コマンドの定義。
 * @details システムで必要なコマンドの実装を追加すると良い。
 */
static const cmd_table_t cmdlist[] = {
    { "help", "Help.", usrcmd_help },
    { "info", "Information.", usrcmd_info },
    { NULL, NULL, NULL }
};

/**
 * @brief NT-Shellコマンドを実行する。
 * @details
 *
 * @param text テキスト。
 *
 * @retval 0 成功。
 * @retval !0 失敗。
 */
int ntshell_usrcmd_execute(const char *text, void *extobj)
{
    /*
     * ntoptライブラリを使って引数を解析し実行する。
     */
    return ntopt_parse(text, usrcmd_ntopt_callback, NULL);
}

/**
 * @brief ユーザコマンドを実行する。
 * @details
 *
 * @param argc 引数の数。
 * @param argv 引数。
 *
 * @retval 0 成功。
 * @retval !0 失敗。
 */
static int usrcmd_ntopt_callback(int argc, char **argv, void *extobj)
{
    /*
     * 引数の数が0ならば何もしない。
     * 単にenterが押されただけと解釈する。
     */
    if (argc == 0) {
        return 0;
    }

    /*
     * コマンドテーブルを探索して、
     * コマンド名が一致したらコールバック関数を呼び出す。
     */
    const cmd_table_t *p = &cmdlist[0];
    while (p->cmd != NULL) {
        if (ntlibc_strcmp((const char *)argv[0], p->cmd) == 0) {
            return p->func(argc, argv);
        }
        p++;
    }

    /*
     * ここに到達するということは、未知のコマンドである。
     */
    {
    	printf( "Unknown command. (help: display help.)\n" ) ;
//        static const char *msg = "Unknown command. (help: display help.)\n";
//        serial_wri_dat(SHELL_PORTID,
//                (const char_t *)msg, ntlibc_strlen(msg));
    }
    return 0;
}

/**
 * @brief helpコマンド。
 * @details
 *
 * @param argc 引数の数。
 * @param argv 引数。
 *
 * @retval 0 成功。
 * @retval !0 失敗。
 */
static int usrcmd_help(int argc, char **argv)
{
    const cmd_table_t *p = &cmdlist[0];
    char buf[128];

    /*
     * コマンド名とコマンド説明を列挙する。
     */
    while (p->cmd != NULL) {
        ntlibc_strcpy(buf, p->cmd);
        ntlibc_strcat(buf, "\t:");
        ntlibc_strcat(buf, p->desc);
        ntlibc_strcat(buf, "\n");
//        serial_wri_dat(SHELL_PORTID,
//                (const char_t *)buf, ntlibc_strlen(buf));
		printf( buf );
        tslp_tsk(5);
        p++;
    }
    return 0;
}

/**
 * @brief infoコマンド。
 * @details
 *
 * @param argc 引数の数。
 * @param argv 引数。
 *
 * @retval 0 成功。
 * @retval !0 失敗。
 */
static int usrcmd_info(int argc, char **argv)
{
    if (argc != 2) {
        static const char *msg = "info sys\ninfo ver\n";
        //serial_wri_dat(SHELL_PORTID,
        //        (const char_t *)msg, ntlibc_strlen(msg) + 1);
        printf( msg );
        return 0;
    }
    if (ntlibc_strcmp(argv[1], "sys") == 0) {
        static const char *msg = "NT-Shell Example System\n";
        //serial_wri_dat(SHELL_PORTID,
        //        (const char_t *)msg, ntlibc_strlen(msg) + 1);
        printf( msg );
        return 0;
    }
    if (ntlibc_strcmp(argv[1], "ver") == 0) {
        static const char *msg = "0.0.1\n";
        //serial_wri_dat(SHELL_PORTID,
        //        (const char_t *)msg, ntlibc_strlen(msg) + 1);
        printf( msg );
        return 0;
    }
    {
        static const char *msg = "Unknown sub command.\n";
        //serial_wri_dat(SHELL_PORTID,
        //        (const char_t *)msg, ntlibc_strlen(msg) + 1);
        printf( msg );
    }
    return -1;
}

