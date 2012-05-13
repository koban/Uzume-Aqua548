===============================================================
 Natural Tiny Shell - A tiny shell for a small embedded system
 Copyright(C) 2011 Shinichiro Nakamura
===============================================================

 Natural Tiny Shell (NT-Shell) は小規模組み込みシステム用の
 小さなシェルです。

1. 謝辞

 Paul Williamsさん
 - 彼はDEC互換のステートマシンパーサの図を作りました。

 Joshua Habermanさん
 - 彼はステートマシンパーサの実装をC言語で行ないました。
   それは素晴らしいインターフェースを持ち、シンプルで賢いです。

2. 背景

 今日、沢山の優れたMPUが手に入ります。
 我々はそれらを使って小規模組み込みシステムを設計することができます。
 それは素晴らしいことで、20年前には考えられなかったことです。

 システムを設計実装している時点で有効なデバッグ手段の１つにUARTを
 インターフェースとして端末のようなものを実現する手段があります。
 これは我々がUNIX端末上でシステムに対して要求を出すような形式で
 MPUを使ったシステムに対して要求を出すことができます。

 対話形式でシステムの状態を観察したり、状態を変化させるためのパラメータを
 入力することができますので、スムーズに作業することができるのが特徴です。

 しかしながら、世の中に存在する沢山の小規模組み込みシステムの端末の実装は
 あまり良いものがないのが現状でした。
 一部の制御コードのみに対応した結果、様々なキー入力によって表示や入力文字列
 が破綻してしまうものなどが大多数です。

 今回はこれらの状況を打開すべくVT100をエミュレートした小さなシェルを
 実装してみました。
 このシェルを組み込み機器に搭載すれば、キー入力に慎重になる必要がなく、
 スムーズにデバッグを行うことができます。

 このシェルには履歴機能もあります。
 過去に入力したコマンドを選択して編集した後で実行することもできます。

3. 基本コンセプト

 * 小さい :)
 * シンプル :)
 * VT100互換 :)
 * Cランタイムライブラリが不要 :)

4. 機能

 CTRL+A : Head
 CTRL+E : Tail
 CTRL+F : Cursor right
 CTRL+B : Cursor left
 CTRL+C : Cancel
 CTRL+D : Delete
 CTRL+P : History(Prev)
 CTRL+N : History(Next)
 TAB    : Suggestion

5. 構造

 NT-Shellは５つの部品から構成されています。

 * 最上位インターフェースモジュール
    ntshell.c
    ntshell.h
 * ユティリティ
    ntopt.c
    ntopt.h
    ntlibc.c
    ntlibc.h
 * VT100パーサ
    vtparse.c
    vtparse.h
    vtparse_table.c
    vtparse_table.h
 * テキストエディタ
    text_editor.c
    text_editor.h
 * テキストヒストリ
    text_history.c
    text_history.h

6. ポーティングガイド

 NT-Shellのインターフェースは非常にシンプルです。
 UARTのための２つの入出力インターフェースとユーザ要求を取得するための
 １つのコールバックインターフェースを必要とします。

 void ntshell(
        ntshell_t *p,
        int (*func_read)(char *buf, int cnt),
        int (*func_write)(const char *buf, int cnt),
        int (*func_callback)(const char *text, void *extobj),
        void *func_callback_extobj);

 6-1. p

  ntshell_t構造体へのポインタです。
  １つのシェルに付き、１つのオブジェクトを用意します。

 6-2. func_read

  func_readは、ユーザキー入力を読むためのインターフェースです。
  キーを読み込むまでブロックするインターフェースとして実装されている
  必要があります。

 6-3. func_write

  func_writeは、シェル出力を書くためのインターフェースです。
  書き込みが終了するまでブロックするインターフェースとして実装されている
  必要があります。

 6-4. func_callback

  func_callbackは、システムに対するユーザ要求文字列を取得するための
  インターフェースです。

 6-5. func_callback_extobj

  func_callback_extobjは、コールバック関数にそのまま渡される拡張オブジェクトです。
  ユーザが自由に使用することができます。

7. ライセンス

 * vtparse, vtparse_tableはパブリックドメインとして提供されています。
 * ntshell, ntopt, ntlibc, text_editor, text_historyはMITライセンスです。
   (あるいはTOPPERSライセンスから選択可能です。)

 ===============================================================
 The MIT license for Natural Tiny Shell (NT-Shell)
 ===============================================================
 Copyright (c) 2010-2011 Shinichiro Nakamura

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or
 sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.
 ===============================================================

 よってNatural Tiny Shell (NT-Shell)は非商用、商用を問わず製品に使用する
 ことができます。
 しかしながら、無保証です。

 再配布する場合に、著作権文字列を取り除かないで下さい。

8. 参考文献

 * A parser for DEC’s ANSI-compatible video terminal
   http://vt100.net/emu/dec_ansi_parser

9. バージョン

 * 2011/03/01   Version 0.0.1    公開初版。
 * 2011/03/02   Version 0.0.2    ユティリティモジュールを追加。
 * 2011/03/04   Version 0.0.3    バグ修正。
 * 2011/04/30   Version 0.0.4    vtparseのテーブルがROMに配置されるようにした。
                                 ntoptの欠落を修正した。
                                 ntlibcを追加した。
 * 2011/05/16   Version 0.0.5    入力補完機能を追加した。
                                 ntshellのインターフェースを変更した。
 * 2011/05/22   Version 0.0.6    カーソルの先頭ジャンプ機能を追加した。
 * 2011/09/22   Version 0.0.7    ntoptインターフェースにextobjを追加した。
 * 2011/10/30   Version 0.0.8    ntoptインターフェースの引数順序を変更した。
                                 ntshellインターフェースにextobjを追加した。

