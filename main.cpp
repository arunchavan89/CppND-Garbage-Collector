#include "gc_pointer.h"
//#include "LeakTester.h"
#include <assert.h>

int main()
{
    /*
    Test 1:

    int *p1 = new int(10);
    PtrDetails<int>p2(p1);
    PtrDetails<int>p3(p1);
    assert(p2 == p3);
    */

    Pointer<int> p = new int(19);
    p = new int(21);
    p = new int(28);

    return 0;
}
