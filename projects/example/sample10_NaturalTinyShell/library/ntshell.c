/**
 * @file ntshell.c
 * @author Shinichiro Nakamura
 * @brief 小規模組み込みシステム向けのシェルシステムの実装。
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

#include "ntshell.h"
#include "ntlibc.h"

#define VERSION_MAJOR 0     /**< メジャー番号。 */
#define VERSION_MINOR 0     /**< マイナー番号。 */
#define VERSION_RELEASE 8   /**< リリース番号。 */

/**
 * @brief 処理で用いるデータ構造体。
 *
 * @details
 * vtrecvはユーザデータのポインタを設定することができる。
 * Natural Tiny Shellはこれを使って自身の処理で必要な情報を保持する。
 */
typedef struct {
    text_editor_t *editor;
    text_history_t *history;
    int suggest_index;
    char suggest_source[TEXTEDITOR_MAXLEN];
    int (*func_read)(char *buf, int cnt);
    int (*func_write)(const char *buf, int cnt);
    int (*func_callback)(const char *text, void *extobj);
    void *func_callback_extobj;
} ntshell_user_data_t;

#define SUGGEST_INDEX(vtp) \
    ((ntshell_user_data_t *)(vtp)->user_data)->suggest_index
#define SUGGEST_SOURCE(vtp) \
    ((ntshell_user_data_t *)(vtp)->user_data)->suggest_source

/**
 * @brief テキストエディタを取得する。
 *
 * @param vtp vtrecv構造体。
 */
#define GET_EDITOR(vtp) \
    ((ntshell_user_data_t *)(vtp)->user_data)->editor

/**
 * @brief テキストヒストリを取得する。
 *
 * @param vtp vtrecv構造体。
 */
#define GET_HISTORY(vtp) \
    ((ntshell_user_data_t *)(vtp)->user_data)->history

/**
 * @brief シリアルポートから読み込む。
 *
 * @param vtp vtrecv構造体。
 * @param buf 読み込みバッファ。
 * @param cnt 読み込み文字数。
 */
#define SERIAL_READ(vtp,buf,cnt) \
    ((ntshell_user_data_t *)(vtp)->user_data)->func_read(buf, cnt)

/**
 * @brief シリアルポートへ書き込む。
 *
 * @param vtp vtrecv構造体。
 * @param buf 書き込みバッファ。
 * @param cnt 書き込み文字数。
 */
#define SERIAL_WRITE(vtp,buf,cnt) \
    ((ntshell_user_data_t *)(vtp)->user_data)->func_write(buf, cnt)

/**
 * @brief コールバックを呼び出す。
 *
 * @param vtp vtrecv構造体。
 * @param text コールバック関数へ渡す文字列。
 */
#define CALLBACK(vtp, text) \
    ((ntshell_user_data_t *)(vtp)->user_data)->func_callback(text, ((ntshell_user_data_t *)(vtp)->user_data)->func_callback_extobj)

/**
 * @brief テキストヒストリで１つ後ろを辿る。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * @param vtrecv パーサー。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_history_prev(
        vtrecv_t *vtrecv,
        vtrecv_action_t action,
        unsigned char ch) {
    if (text_history_read_point_prev(GET_HISTORY(vtrecv))) {
        char txt[TEXTHISTORY_MAXLEN];
        int n = text_history_read(GET_HISTORY(vtrecv), &txt[0], sizeof(txt));
        if (0 < n) {
            SERIAL_WRITE(vtrecv, "\x1b[2K", 4);
            SERIAL_WRITE(vtrecv, "\x1b[80D", 5);
            SERIAL_WRITE(vtrecv, ">", 1);
            SERIAL_WRITE(vtrecv, txt, n);
            text_editor_set_text(GET_EDITOR(vtrecv), txt);
        }
    }
}

/**
 * @brief テキストヒストリで１つ前を辿る。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * @param vtrecv パーサー。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_history_next(
        vtrecv_t *vtrecv,
        vtrecv_action_t action,
        unsigned char ch) {
    if (text_history_read_point_next(GET_HISTORY(vtrecv))) {
        char txt[TEXTHISTORY_MAXLEN];
        int n = text_history_read(GET_HISTORY(vtrecv), &txt[0], sizeof(txt));
        if (0 < n) {
            SERIAL_WRITE(vtrecv, "\x1b[2K", 4);
            SERIAL_WRITE(vtrecv, "\x1b[80D", 5);
            SERIAL_WRITE(vtrecv, ">", 1);
            SERIAL_WRITE(vtrecv, txt, n);
            text_editor_set_text(GET_EDITOR(vtrecv), txt);
        }
    }
}

/**
 * @brief カーソルを左へ移動させる。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * @param vtrecv パーサー。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_cursor_left(
        vtrecv_t *vtrecv,
        vtrecv_action_t action,
        unsigned char ch) {
    if (text_editor_cursor_left(GET_EDITOR(vtrecv))) {
        SERIAL_WRITE(vtrecv, "\x1b[1D", 4);
    }
}

/**
 * @brief カーソルを右へ移動させる。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * @param vtrecv パーサー。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_cursor_right(
        vtrecv_t *vtrecv,
        vtrecv_action_t action,
        unsigned char ch) {
    if (text_editor_cursor_right(GET_EDITOR(vtrecv))) {
        SERIAL_WRITE(vtrecv, "\x1b[1C", 4);
    }
}

/**
 * @brief エンターキーの処理を実行する。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * @param vtrecv パーサー。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_enter(
        vtrecv_t *vtrecv,
        vtrecv_action_t action,
        unsigned char ch) {
    char txt[TEXTEDITOR_MAXLEN];
    text_editor_get_text(GET_EDITOR(vtrecv), &txt[0], sizeof(txt));
    text_editor_clear(GET_EDITOR(vtrecv));
    text_history_write(GET_HISTORY(vtrecv), txt);
    SERIAL_WRITE(vtrecv, "\r\n", 2);
    CALLBACK(vtrecv, txt);
    SERIAL_WRITE(vtrecv, ">", 1);
}

/**
 * @brief キャンセルキーの処理を実行する。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * 一般的なOSのCTRL+C処理はシグナルを発行し、受信したプロセスが
 * 中断処理を実行する。
 * ここでのキャンセルは見た目を再現したもので、
 * 入力中の文字列を破棄してカーソルを新しい入力に備えて復帰させるものだ。
 *
 * @param vtrecv パーサー。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_cancel(
        vtrecv_t *vtrecv,
        vtrecv_action_t action,
        unsigned char ch) {
    SERIAL_WRITE(vtrecv, "^C\r\n", 4);
    text_editor_clear(GET_EDITOR(vtrecv));
    SERIAL_WRITE(vtrecv, ">", 1);
}

/**
 * @brief 挿入処理を実行する。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * @param vtrecv パーサー。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_insert(
        vtrecv_t *vtrecv,
        vtrecv_action_t action,
        unsigned char ch) {

    /*
     * 入力があった場合、入力補完状態から抜ける。
     */
    SUGGEST_INDEX(vtrecv) = -1;

    /*
     * テキストエディタを使って文字を文字列に挿入する。
     */
    if (text_editor_insert(GET_EDITOR(vtrecv), ch)) {
        char txt[TEXTEDITOR_MAXLEN];
        int len = text_editor_get_text(GET_EDITOR(vtrecv), &txt[0], sizeof(txt));
        int pos = text_editor_cursor_get_position(GET_EDITOR(vtrecv));
        int n = len - pos;
        SERIAL_WRITE(vtrecv, (char *)&ch, sizeof(ch));
        if (n > 0) {
            int i;
            SERIAL_WRITE(vtrecv, txt + pos, len - pos);
            for (i = 0; i < n; i++) {
                SERIAL_WRITE(vtrecv, "\x1b[1D", 4);
            }
        }
    }
}

/**
 * @brief バックスペース処理を実行する。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * @param vtrecv パーサー。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_backspace(
        vtrecv_t *vtrecv,
        vtrecv_action_t action,
        unsigned char ch) {
    if (text_editor_backspace(GET_EDITOR(vtrecv))) {
        char txt[TEXTEDITOR_MAXLEN];
        int len = text_editor_get_text(GET_EDITOR(vtrecv), &txt[0], sizeof(txt));
        int pos = text_editor_cursor_get_position(GET_EDITOR(vtrecv));
        int n = len - pos;
        SERIAL_WRITE(vtrecv, "\x1b[1D", 4);
        if (n > 0) {
            int i;
            SERIAL_WRITE(vtrecv, txt + pos, n);
            SERIAL_WRITE(vtrecv, " ", 1);
            for (i = 0; i < n + 1; i++) {
                SERIAL_WRITE(vtrecv, "\x1b[1D", 4);
            }
        } else {
            SERIAL_WRITE(vtrecv, " ", 1);
            SERIAL_WRITE(vtrecv, "\x1b[1D", 4);
        }
    }
}

/**
 * @brief デリート処理を実行する。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * @param vtrecv パーサー。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_delete(
        vtrecv_t *vtrecv,
        vtrecv_action_t action,
        unsigned char ch) {
    if (text_editor_delete(GET_EDITOR(vtrecv))) {
        char txt[TEXTEDITOR_MAXLEN];
        int len = text_editor_get_text(GET_EDITOR(vtrecv), &txt[0], sizeof(txt));
        int pos = text_editor_cursor_get_position(GET_EDITOR(vtrecv));
        int n = len - pos;
        if (n > 0) {
            int i;
            SERIAL_WRITE(vtrecv, txt + pos, n);
            SERIAL_WRITE(vtrecv, " ", 1);
            for (i = 0; i < n + 1; i++) {
                SERIAL_WRITE(vtrecv, "\x1b[1D", 4);
            }
        } else {
            SERIAL_WRITE(vtrecv, " ", 1);
            SERIAL_WRITE(vtrecv, "\x1b[1D", 4);
        }
    }
}

/**
 * @brief 入力補完処理を実行する。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * @param vtrecv パーサー。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_suggest(
        vtrecv_t *vtrecv,
        vtrecv_action_t action,
        unsigned char ch) {
    char buf[TEXTEDITOR_MAXLEN];
    if (SUGGEST_INDEX(vtrecv) < 0) {
        /*
         * 入力補完モードにこれから入る場合。
         * 現在の入力文字列を元に補完候補を取得する。
         */
        if (text_editor_get_text(
                    GET_EDITOR(vtrecv),
                    SUGGEST_SOURCE(vtrecv),
                    sizeof(SUGGEST_SOURCE(vtrecv))) > 0) {
            SUGGEST_INDEX(vtrecv) = 0;
            if (text_history_find(
                        GET_HISTORY(vtrecv),
                        SUGGEST_INDEX(vtrecv),
                        SUGGEST_SOURCE(vtrecv),
                        buf,
                        sizeof(buf)) == 0) {
                // 候補が見つかればテキストを設定して、インデックスをメモする。
                int n = ntlibc_strlen((const char *)buf);
                SERIAL_WRITE(vtrecv, "\x1b[2K", 4);
                SERIAL_WRITE(vtrecv, "\x1b[80D", 5);
                SERIAL_WRITE(vtrecv, ">", 1);
                SERIAL_WRITE(vtrecv, buf, n);
                text_editor_set_text(GET_EDITOR(vtrecv), buf);
            } else {
                // 候補がなければ入力補完モードから抜ける。
                SUGGEST_INDEX(vtrecv) = -1;
            }
        }
    } else {
        /*
         * 既に入力補完モードに入っている場合、
         * 次の候補を探して見つかればテキストとして設定する。
         */
        SUGGEST_INDEX(vtrecv) = SUGGEST_INDEX(vtrecv) + 1;
        if (text_history_find(
                    GET_HISTORY(vtrecv),
                    SUGGEST_INDEX(vtrecv),
                    SUGGEST_SOURCE(vtrecv),
                    buf,
                    sizeof(buf)) == 0) {
            // 候補が見つかればテキストを設定する。
            int n = ntlibc_strlen((const char *)buf);
            SERIAL_WRITE(vtrecv, "\x1b[2K", 4);
            SERIAL_WRITE(vtrecv, "\x1b[80D", 5);
            SERIAL_WRITE(vtrecv, ">", 1);
            SERIAL_WRITE(vtrecv, buf, n);
            text_editor_set_text(GET_EDITOR(vtrecv), buf);
        } else {
            // 候補が見つからなければ元の入力文字列に戻し、入力補完モードから抜ける。
            int n = ntlibc_strlen(SUGGEST_SOURCE(vtrecv));
            SERIAL_WRITE(vtrecv, "\x1b[2K", 4);
            SERIAL_WRITE(vtrecv, "\x1b[80D", 5);
            SERIAL_WRITE(vtrecv, ">", 1);
            SERIAL_WRITE(vtrecv, SUGGEST_SOURCE(vtrecv), n);
            text_editor_set_text(GET_EDITOR(vtrecv), SUGGEST_SOURCE(vtrecv));
            SUGGEST_INDEX(vtrecv) = -1;
        }
    }
}

/**
 * @brief カーソルを行頭へ移動させる。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * @param vtrecv パーサー。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_cursor_head(
        vtrecv_t *vtrecv,
        vtrecv_action_t action,
        unsigned char ch) {
    SERIAL_WRITE(vtrecv, "\x1b[80D", 5);
    SERIAL_WRITE(vtrecv, ">", 1);
    text_editor_cursor_head(GET_EDITOR(vtrecv));
}

/**
 * @brief カーソルを行末へ移動させる。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * @param vtrecv パーサー。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_cursor_tail(
        vtrecv_t *vtrecv,
        vtrecv_action_t action,
        unsigned char ch) {
    char buf[TEXTEDITOR_MAXLEN];
    int len;
    text_editor_get_text(GET_EDITOR(vtrecv), buf, sizeof(buf));
    len = ntlibc_strlen((const char *)buf);
    SERIAL_WRITE(vtrecv, "\x1b[80D", 5);
    SERIAL_WRITE(vtrecv, ">", 1);
    SERIAL_WRITE(vtrecv, buf, len);
    text_editor_cursor_tail(GET_EDITOR(vtrecv));
}

/**
 * @brief アクションテーブルのデータ構造体。
 * @details
 * アクションは状態と入力文字によって与えられる。
 * アクションに対する関数もここで定義する。
 */
typedef struct {
    vtrecv_action_t action;
    unsigned char ch;
    void (*func)(
            vtrecv_t *vtrecv,
            vtrecv_action_t action,
            unsigned char ch);
} ntshell_action_table_t;

/**
 * @brief アクションに対する処理関数テーブル。
 * @details
 * やってくるコードは仮想端末側の処理に依存する。
 * よって様々なプラットフォームの様々な仮想端末で試すと良い。
 *
 * <table>
 *   <th>
 *     <td>Platform</td>
 *     <td>Tools</td>
 *   </th>
 *   <tr>
 *     <td>Windows</td>
 *     <td>Hyper Terminal, Poderossa, TeraTerm</td>
 *   </tr>
 *   <tr>
 *     <td>Linux</td>
 *     <td>minicom, screen, kermit</td>
 *   </tr>
 * </table>
 */
static const ntshell_action_table_t action_table[] = {
    {VTRECV_ACTION_EXECUTE, 0x01, actfunc_cursor_head},
    {VTRECV_ACTION_EXECUTE, 0x02, actfunc_cursor_left},
    {VTRECV_ACTION_EXECUTE, 0x03, actfunc_cancel},
    {VTRECV_ACTION_EXECUTE, 0x04, actfunc_delete},
    {VTRECV_ACTION_EXECUTE, 0x05, actfunc_cursor_tail},
    {VTRECV_ACTION_EXECUTE, 0x06, actfunc_cursor_right},
    {VTRECV_ACTION_EXECUTE, 0x08, actfunc_backspace},
    {VTRECV_ACTION_EXECUTE, 0x09, actfunc_suggest},
    {VTRECV_ACTION_EXECUTE, 0x0d, actfunc_enter},
    {VTRECV_ACTION_EXECUTE, 0x0e, actfunc_history_next},
    {VTRECV_ACTION_EXECUTE, 0x10, actfunc_history_prev},
    {VTRECV_ACTION_CSI_DISPATCH, 0x41, actfunc_history_prev},
    {VTRECV_ACTION_CSI_DISPATCH, 0x42, actfunc_history_next},
    {VTRECV_ACTION_CSI_DISPATCH, 0x43, actfunc_cursor_right},
    {VTRECV_ACTION_CSI_DISPATCH, 0x44, actfunc_cursor_left},
    {VTRECV_ACTION_PRINT, 0x7f, actfunc_backspace},
};

/**
 * @brief パーサーに対するコールバック関数。
 * @details vtrecvモジュールのコールバック関数に従った実装である。
 *
 * @param vtrecv パーサー。
 * @param action アクション。
 * @param ch キャラクタ。
 */
void vtrecv_callback(
        vtrecv_t *vtrecv,
        vtrecv_action_t action,
        unsigned char ch) {
    ntshell_action_table_t *p;
    int i;
    const int ACTTBLSIZ = sizeof(action_table) / sizeof(action_table[0]);

    /*
     * 制御コードに対する処理はテーブルから探す。
     */
    p = (ntshell_action_table_t *)action_table;
    for (i = 0; i < ACTTBLSIZ; i++) {
        if ((p->action == action) && (p->ch == ch)) {
            p->func(vtrecv, action, ch);
            return;
        }
        p++;
    }

    /*
     * 通常の文字列は入力として扱う。
     */
    if (VTRECV_ACTION_PRINT == action) {
        actfunc_insert(vtrecv, action, ch);
        return;
    }

    /*
     * ここに到達する場合、
     * テーブルに含まれない制御コードか、
     * 通常の文字でない事が考えられる。
     *
     * 必要なキー入力に対する動作を加えたい場合、
     * vtrecvによって得られるコードを調べるために
     * ここにテストコードを加える事ができる。
     */
}

/**
 * @brief Natural Tiny Shellのバージョンを返す。
 * @details 返すバージョンはリリースバージョンである。
 *
 * @param major メージャーバージョン。
 * @param minor マイナーバージョン。
 * @param release リリースバージョン。
 */
void ntshell_version(int *major, int *minor, int *release)
{
    *major = VERSION_MAJOR;
    *minor = VERSION_MINOR;
    *release = VERSION_RELEASE;
}

/**
 * @brief Natural Tiny Shellを実行する。
 * @details この関数は実行を返さない。
 *
 * @param p NT-Shellハンドラ。
 * @param func_read シリアルリード関数。
 * @param func_write シリアルライト関数。
 * @param func_callback コールバック関数。
 * @param func_callback_extobj コールバック関数呼び出し時に渡す拡張オブジェクト。
 */
void ntshell_execute(
        ntshell_t *p,
        int (*func_read)(char *buf, int cnt),
        int (*func_write)(const char *buf, int cnt),
        int (*func_callback)(const char *text, void *extobj),
        void *func_callback_extobj)
{
    /*
     * vtrecvはユーザデータへのポインタを設定できるようになっている。
     * Natural Tiny Shellはこれを利用してテキストエディタやヒストリ、
     * リード関数やライト関数、コールバック関数を処理の中で使用できる
     * ようにしてある。
     */
    ntshell_user_data_t user_data;

    user_data.editor = &(p->editor);
    user_data.history = &(p->history);
    user_data.func_read = func_read;
    user_data.func_write = func_write;
    user_data.func_callback = func_callback;
    user_data.func_callback_extobj = func_callback_extobj;

    p->vtrecv.user_data = &user_data;

    /*
     * 各モジュールを初期化する。
     */
    vtrecv_init(&(p->vtrecv), vtrecv_callback);
    text_editor_init(GET_EDITOR(&(p->vtrecv)));
    text_history_init(GET_HISTORY(&(p->vtrecv)));
    SUGGEST_INDEX(&(p->vtrecv)) = -1;

    /*
     * ユーザ入力ループ。
     */
    SERIAL_WRITE(&(p->vtrecv), ">", 1);
    while(1)
    {
        unsigned char ch;
        SERIAL_READ(&(p->vtrecv), (char *)&ch, sizeof(ch));
        vtrecv_execute(&(p->vtrecv), &ch, sizeof(ch));
    }
}

