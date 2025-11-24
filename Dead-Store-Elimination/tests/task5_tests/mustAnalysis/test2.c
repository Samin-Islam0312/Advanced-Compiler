// Two stores to the same pointer p in one basic block.
// The first store is never read before being overwritten.

int dse_must_simple(int *p, int x) {
    // Store 1 will be overwritten before any load
    *p = x;

    // Some computatuion that does not touch *p
    int y = x * 2;
    if (y > 10) {
        y = y - 3;
    } else {
        y = y + 5;
    }

    // Store 2: overwrites the previous value of *p
    *p = y;

    // First and only read of *p
    int v = *p;
    
    return v + y;
}
