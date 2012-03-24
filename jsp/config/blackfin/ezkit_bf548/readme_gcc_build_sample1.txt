【注意】
gcc環境でBF548 EZ-KITのsample1をビルドする場合、sample1.hのTASK_PORTID定義を変更すること。

#define TASK_PORTID 2       /* 文字入力するシリアルポートID */

EZ-KITでは、UART1が割り当てられているため、出力先を変更しなくてはならないためである。
