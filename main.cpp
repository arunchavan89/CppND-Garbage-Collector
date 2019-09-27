#include "gc_pointer.h"
//#include "LeakTester.h"
#include <assert.h>

int main()
{
   
    Pointer<int> p = new int(19);
    p = new int(21);
    p = new int(28);

    return 0;
}
