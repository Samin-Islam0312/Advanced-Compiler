#pragma clang fp contract(fast)

float basic_fma(float a, float b, float c) {
    return (a * b) + c;
}