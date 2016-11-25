#include "malloc.h"

mallinfo mallinfo(void)
{
    mallinfo m;
    memset(&m, sizeof(m), 0);
    
    return m;
}
