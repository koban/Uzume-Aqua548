/**
 * @file ntopt.h
 * @author Shinichiro Nakamura
 * @brief NT-Shell用オプション解析モジュールの定義。
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

#ifndef NTOPT_H
#define NTOPT_H

#include "ntconf.h"

/**
 * @brief パース可能な最大語句数。
 */
#define NTOPT_MAXCNT_ARGC   (NTCONF_PARSE_MAXARGS)

/**
 * @brief 1語句あたりの文字列最大長。
 */
#define NTOPT_MAXLEN_ARGV   (NTCONF_PARSE_WORDLEN)

/**
 * @brief 与えられた文字列をデリミタで分割する。
 *
 * @param str 文字列。
 * @param func コールバック関数。
 * @param extobj 拡張オブジェクト。
 *
 * @return コールバック関数が返した値。
 */
int ntopt_parse(
        const char *str,
        int (*func)(int argc, char **argv, void *extobj),
        void *extobj);

#endif

