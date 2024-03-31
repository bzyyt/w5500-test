#include "SwoAdapter.h"

// 重定向C库函数printf到SWO
int fputc(int ch, FILE *f)
{
    ITM_SendChar(ch);
    return ch;
}
