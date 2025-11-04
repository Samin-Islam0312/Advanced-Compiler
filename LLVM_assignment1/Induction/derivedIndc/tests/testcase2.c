// file: nested_iv.c
int nested_sum(int *a, int n, int m) {
    int sum = 0;
    for (int i = 0; i < n; ++i) {          // outer loop IV: i
        for (int j = 0; j < m; j += 2) {   // inner loop IV: j
            sum += a[i * m + j];
        }
    }
    return sum;
}
