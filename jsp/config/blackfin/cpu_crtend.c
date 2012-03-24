
typedef void (*fptr)(void);

/*
 * ctor_list_endとdtor_list_endはctor_listとdtor_listの番兵役である。必ず0で終わる。
 */
static fptr ctor_list_end[1] __attribute__((section(".ctors"), used)) = { (fptr) 0 };
static fptr dtor_list_end[1] __attribute__((section(".dtors"), used)) = { (fptr) 0 };



