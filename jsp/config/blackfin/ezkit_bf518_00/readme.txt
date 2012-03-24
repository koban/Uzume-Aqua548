Analog Devices製 EZ-KIT Lite BF518 Rev 0.0 対応システム依存部。

sys_config.hとsys_defs.hは、#include_nextを使って、それぞれblackfin/_common_bf518/sys_config.h
とblackfin/_common_bf518/sys_defs.hを読み込んでいる。

Makefile.configは、インクルード・パスおよびソースファイルへのパスとしてblackfin/_common_bf518も
指定している。

- Makefile.config : gnu環境のためのシステム依存部定義ファイル
- readme.txt : このファイル
- sys_config.h : システム依存部のうち、アプリケーションから見えない宣言
- sys_defs.h : システム依存部のうち、アプリケーションから見える宣言

このシステム依存部は、Rev 0.1以降のシリコンでは使用できない。Rev 0.1以降を使う場合にはezkit_bf518
依存部を使用すること。