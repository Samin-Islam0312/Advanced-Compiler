// a and b are different SSA values. They may alias at runtime,
// so must keep both stores.

int dse_may1(int *a, int *b, int cond) {
    // Store to a
    *a = 10;

    int x = cond ? 123 : 456;
    x = x * 2;

    // Store to b (may overwrite *a if a == b)
    *b = 20;

    int v = *a; // may see 10 or 20 depending on aliasing

    return v + x;
}
