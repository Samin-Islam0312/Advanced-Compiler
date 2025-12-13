import os
import subprocess
import statistics


PASS_LIB = "../build/FMAContractPass.dylib"
POLYBENCH_ROOT = "polyBench/PolyBenchC-4.2.1"

# Kernels to test
KERNELS = [
    "linear-algebra/blas/gemm/gemm.c",       # Matrix Mul
    "linear-algebra/kernels/2mm/2mm.c",      # 2 Matrix Muls
    "linear-algebra/kernels/bicg/bicg.c"     # BiCG Sub Kernel
]

INCLUDE_FLAGS = f"-I {POLYBENCH_ROOT}/utilities -I {POLYBENCH_ROOT}/linear-algebra/blas/gemm -DPOLYBENCH_TIME -D_POSIX_C_SOURCE=200809L"

# Baseline: Strict IEEE (No FMA allowed) -ffp-contract=off
FLAGS_BASE = "-O3 -ffp-contract=off -fno-slp-vectorize" 

# Optimized: Allow Contract, FOR MY PASS
FLAGS_OPT  = "-O3 -ffp-contract=fast"

def run_command(cmd):
    """Runs a shell command and returns True if successful."""
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"[ERROR] Command failed: {cmd}")
       
       
        print("\n".join(result.stderr.splitlines()[:5])) 
        return False
    return True

def get_time(exe_path):
    """Runs the executable 3 times and returns the median execution time."""
    times = []
    for _ in range(3):
        result = subprocess.run(exe_path, capture_output=True, text=True)
        try:
            t = float(result.stdout.strip())
            times.append(t)
        except ValueError:
            pass
    
    if not times: return 0.0
    return statistics.median(times)

def main():
    if not os.path.exists(PASS_LIB):
        print(f"Error: Pass library not found at {PASS_LIB}")
        print("Check if you are running this from the 'benchmarks' directory and if 'build' exists in root.")
        return
    if not os.path.exists(POLYBENCH_ROOT):
        print(f"Error: PolyBench directory not found at {POLYBENCH_ROOT}")
        return

    print(f"{'KERNEL':<20} | {'BASE (s)':<10} | {'OPT (s)':<10} | {'SPEEDUP':<10}")
    print("-" * 60)

    for kernel_rel_path in KERNELS:
        # Full path to kernel source
        kernel_src = f"{POLYBENCH_ROOT}/{kernel_rel_path}"
        
        # Utility source (polybench.c)
        poly_src = f"{POLYBENCH_ROOT}/utilities/polybench.c"

        if not os.path.exists(kernel_src):
            print(f"[WARN] Kernel not found: {kernel_src}")
            continue

        kernel_name = os.path.basename(kernel_src).replace(".c", "")
        
        # Intermediate Files
        base_exe = f"bench_base_{kernel_name}"
        opt_exe  = f"bench_opt_{kernel_name}"
        temp_ll  = f"temp_{kernel_name}.ll"
        opt_ll   = f"temp_{kernel_name}_opt.ll"

        # COMPILE BASELINE (Strict IEEE)
        cmd_base = f"clang {FLAGS_BASE} {INCLUDE_FLAGS} {poly_src} {kernel_src} -o {base_exe}"
        if not run_command(cmd_base): continue

        # COMPILE OPTIMIZED (Emit IR)
        cmd_ir = f"clang {FLAGS_OPT} {INCLUDE_FLAGS} -S -emit-llvm {kernel_src} -o {temp_ll}"
        if not run_command(cmd_ir): continue
        
        # RUN FMA PASS
        cmd_opt = f"opt -load-pass-plugin={PASS_LIB} -passes='fma-pass' {temp_ll} -S -o {opt_ll}"
        if not run_command(cmd_opt): continue

        # LINK (Optimized IR + Polybench Utility)
        cmd_link = f"clang {FLAGS_OPT} {INCLUDE_FLAGS} {poly_src} {opt_ll} -o {opt_exe}"
        if not run_command(cmd_link): continue

        # MEASURE
        t_base = get_time(f"./{base_exe}")
        t_opt  = get_time(f"./{opt_exe}")
        
        speedup = 0.0
        if t_opt > 0:
            speedup = t_base / t_opt

        print(f"{kernel_name:<20} | {t_base:<10.6f} | {t_opt:<10.6f} | {speedup:<10.2f}x")

        # Cleanup
        for f in [base_exe, opt_exe, temp_ll, opt_ll]:
            if os.path.exists(f): os.remove(f)

if __name__ == "__main__":
    main()