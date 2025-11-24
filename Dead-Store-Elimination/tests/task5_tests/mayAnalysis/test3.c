// Two different pointer values derived from the same base,
// but with unknown offset relationships. 
// but the pass only identifies exact pointer equality as must-alias.

int dse_may_alt(int *base, int i, int j) {
    int *p = base + i;   // pointer 1
    int *q = base + j;   // pointer 2 (may or may not alias with p, depending on whether i=j)

    // Store through p
    *p = i * 10;

    // Some computation unrelated to p/q
    int x = (i + j) * 3;
    if (x % 2 == 0)
        x += 5;
    else
        x -= 7;

    // Store through q; might overwrite p if p == q in runtime
    *q = x;

    // Load through p (result depends on whether p == q)
    int r = *p;

    return r + x;
}
