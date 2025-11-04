import sys

config.llvm_tools_dir = "/opt/homebrew/opt/llvm/bin"
config.llvm_shlib_ext = ".dylib"
config.llvm_shlib_dir = "/Users/lazyeater/bril/adv_compiler/LLVM_Exercise/Task3/llvm-tutor/build/lib"

import lit.llvm
# lit_config is a global instance of LitConfig
lit.llvm.initialize(lit_config, config)

# test_exec_root: The root path where tests should be run.
config.test_exec_root = os.path.join("/Users/lazyeater/bril/adv_compiler/LLVM_Exercise/Task3/llvm-tutor/build/test")

# Let the main config do the real work.
lit_config.load_config(config, "/Users/lazyeater/bril/adv_compiler/LLVM_Exercise/Task3/llvm-tutor/test/lit.cfg.py")
