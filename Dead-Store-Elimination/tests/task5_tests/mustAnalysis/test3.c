// overwriting the same pointer twice with no reads in between.

int dse_must_basic2(int *p, int n) {
    *p = n;        // dead
    int t = n + 5; // does not touch *p
    *p = t;        // live
    return *p;     // only read
}
