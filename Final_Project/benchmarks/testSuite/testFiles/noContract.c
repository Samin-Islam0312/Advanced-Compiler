#pragma clang fp contract(off)  // Explicitly turn OFF contraction

float strict_fp(float a, float b, float c) {
    return (a * b) + c;     // NO @llvm.fma should be generated 
}