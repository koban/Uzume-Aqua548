/**
 * @file ntshell.h
 * @author Shinichiro Nakamura
 * @brief 小規模組み込みシステム向けのシェルシステムの定義。
 */

/*
 * ===============================================================
 *  Natural Tiny Shell (NT-Shell)
 *  Version 0.0.8
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

#ifndef NTSHELL_H
#define NTSHELL_H

#include "vtrecv.h"
#include "vtsend.h"
#include "text_editor.h"
#include "text_history.h"

/**
 * @brief NT-Shelハンドラの実装。
 * @details
 * 本来このハンドラは定義のみを公開し、実装の詳細を隠蔽すべきだが、
 * ヒープからメモリを確保するインターフェースのない環境も有り得る
 * 事を考慮して公開してある。
 * このため、スタックに置いた構造体を使って制御する事も可能である。
 */
typedef struct {
    vtrecv_t vtrecv;        /**< vtrecvハンドラ。 */
    vtsend_t vtsend;        /**< vtsendハンドラ。 */
    text_editor_t editor;   /**< text_editorハンドラ。 */
    text_history_t history; /**< text_historyハンドラ。 */
} ntshell_t;

void ntshell_version(int *major, int *minor, int *release);
void ntshell_execute(
        ntshell_t *p,
        int (*func_read)(char *buf, int cnt),
        int (*func_write)(const char *buf, int cnt),
        int (*func_callback)(const char *text, void *extobj),
        void *func_callback_extobj);

#endif

