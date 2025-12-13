#pragma clang fp contract(fast)

float multi_use(float a, float b, float c, float *output_ptr) {
    float prod = a * b;
    *output_ptr = prod;
    return prod + c;
}