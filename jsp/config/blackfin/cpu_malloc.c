#include <t_services.h>

static void * heapPtr = 0;
extern void *_heap_start, *_heap_end;

/*
 * 要求があればヒープ上にメモリ領域を割り当てる。freeは使わないことを仮定している。
 */

void * malloc(size_t size)
{
	void * retPtr;
	SIL_PRE_LOC;

	SIL_LOC_INT();
	// 最初の呼び出しでヒープを初期化する
	if (! heapPtr)
		heapPtr = _heap_start;

	retPtr = heapPtr;

	if ((heapPtr+size) >= _heap_end)
		retPtr =  NULL;
	else
	{
		heapPtr += size;
			// ポインタを32bit境界に揃える
		while ((unsigned int)heapPtr % 4)
			heapPtr++;
	}
	SIL_UNL_INT();
	return retPtr;
}

/*
 * なにもせずに戻る
 */
void free( void * ptr )
{
}
