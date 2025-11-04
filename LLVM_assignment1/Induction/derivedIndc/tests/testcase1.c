// file: simple_iv.c
int sum_basic(int *a, int n) {
    int sum = 0;
    for (int i = 0; i < n; ++i) {   // i is an induction variable
        sum += a[i];
    }
    return sum;
}
