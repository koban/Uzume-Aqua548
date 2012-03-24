
typedef void (*fptr)(void);

/*
 * ctor_list_beginとdtor_list_beginはマジックナンバー(-1)から始まる。gccはそれぞれの1番の
 * エントリから順に大域オブジェクトの初期化のためのコンストラクタのポインタを埋めていき、
 * 最後にNULLを置く。
 */
static fptr ctor_list_begin[1] __attribute__((section(".ctors"), used)) = { (fptr) -1 };
static fptr dtor_list_begin[1] __attribute__((section(".dtors"), used)) = { (fptr) -1 };


/*
 * _initと_finiは特殊なセクション .initと.finiに格納される。これはgccの推奨である。
 * 詳しくはgccのマニュアル参照
 */
extern void _init(void) __attribute__((section(".init")));
extern void _fini(void) __attribute__((section(".fini")));

void
_init(void)
{
    fptr *fpp;

		// 最後のコンストラクタを探す
    for(fpp = ctor_list_begin + 1;  *fpp != 0;  ++fpp)
        ;
		// 後ろから前へ実行する
    while(--fpp > ctor_list_begin)
        (**fpp)();
}


void
_fini(void)
{
    fptr *fpp;

		// デストラクタを初期化と逆順に実行する
    for(fpp = dtor_list_begin + 1;  *fpp != 0;  ++fpp)
        (**fpp)();

}

