// Two different pointer arguments a and b.
// They may point to the same location at runtime, but pass only treats exact same SSA values as must-alias
// keep both stores.

int dse_may_simple(int *a, int *b, int x, int flag) {
    // Store through a
    *a = x;

    int y = x + 1;
    if (flag) {
        y = y * 2;
    } else {
        y = y - 3;
    }

    // Store through b (could be the same location if a == b)
    *b = y;

    // Read from a; may see either x or y depending on aliasing
    int v = *a;

    return v + y;
}
