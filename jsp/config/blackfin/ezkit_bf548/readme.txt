Analog Devices製 EZ-KIT Lite BF548対応システム依存部。

sys_config.hとsys_defs.hは、#include_nextを使って、それぞれblackfin/_common_bf508/sys_config.h
とblackfin/_common_bf548/sys_defs.hを読み込んでいる。

Makefile.configは、インクルード・パスおよびソースファイルへのパスとしてblackfin/_common_bf548も
指定している。

- Makefile.config : gnu環境のためのシステム依存部定義ファイル
- readme.txt : このファイル
- sys_config.h : システム依存部のうち、アプリケーションから見えない宣言
- sys_defs.h : システム依存部のうち、アプリケーションから見える宣言

