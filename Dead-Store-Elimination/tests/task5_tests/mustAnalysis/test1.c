// three stores to the SAME pointer p.
// pass should delete the first two stores, keep the last one.

#include <stdlib.h>

int dse_must1(int *p, int flag) {
    // Store1 - dead
    *p = 10;

    int x = flag ? 1 : 2;
    x = x * 3 + 5;

    // Store2 - dead
    *p = 20;

    // Some computation
    int y = x * x;
    if (y & 1) {
        y += 7;
    } else {
        y -= 3;
    }

    // Store 3 - live so that you can be read
    *p = 30;

    // First and only read of *p in this function
    int v = *p;

    return v + y;
}
