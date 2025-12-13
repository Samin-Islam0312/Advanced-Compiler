#!/bin/bash

PASS_LIB="../build/FMAContractPass.dylib"
SOURCE_DIR="testSuite/testFiles"
IR_DIR="testSuite/irfiles"
OPT_DIR="testSuite/transformedIR"

mkdir -p "$IR_DIR"
mkdir -p "$OPT_DIR"

echo "============================================"
echo "      Running FMA Pass Micro-Test Suite     "
echo "============================================"
echo "Pass Library: $PASS_LIB"
echo "Input Dir:    $SOURCE_DIR"
echo "Output Dir:   $OPT_DIR"
echo "--------------------------------------------"


if [ ! -f "$PASS_LIB" ]; then
    echo "ERROR: Pass library not found at $PASS_LIB"
    echo "Did you run 'make' in the build directory?"
    exit 1
fi

for c_file in "$SOURCE_DIR"/*.c; do
    filename=$(basename -- "$c_file")
    base_name="${filename%.*}"
    
    echo "Processing: $filename"

    # Compile C -> LLVM IR
    clang -O1 -S -emit-llvm -ffp-contract=fast "$c_file" -o "$IR_DIR/${base_name}.ll"

    if [ $? -ne 0 ]; then
        echo "  [FAIL] Clang compilation failed."
        continue
    fi

    # Run FMA Pass
    opt -load-pass-plugin="$PASS_LIB" -passes="fma-pass" "$IR_DIR/${base_name}.ll" -S -o "$OPT_DIR/${base_name}_opt.ll"

    if [ $? -ne 0 ]; then
        echo "  [FAIL] Opt (Pass) execution failed."
        continue
    fi

    # Verification
    if grep -q "llvm.fma" "$OPT_DIR/${base_name}_opt.ll"; then
        echo "  [PASS] FMA Intrinsic Detected! ✅"
    else
        # Tests that SHOULD NOT have FMA
        if [[ "$base_name" == "noContract" || "$base_name" == "constFold" ]]; then
             echo "  [PASS] No FMA Detected (Expected behavior). ✅"
        else
             echo "  [WARN] No FMA Detected (Check logic?). ⚠️"
        fi
    fi
    echo "--------------------------------------------"
done

echo "Done. Results are in $OPT_DIR"