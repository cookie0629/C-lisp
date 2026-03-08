# C-Lisp Machine: A High-Performance Lisp-to-C Transpiler

C-Lisp is a minimal, Turing-complete Lisp dialect and compiler written entirely in C. Instead of interpreting Lisp code or compiling it to a custom bytecode, this "Lisp Machine" transpiles Lisp Abstract Syntax Trees (AST) directly into zero-overhead, highly optimized native C code.

## Key Features

* **Native C Transpilation:** Converts Lisp expressions directly into C statements and comma operators. The output is 100% standard C code that can be compiled by GCC, Clang, or MSVC.
* **First-Class Functions & Closures:** Full support for `lambda` and `funcall`. Functions are first-class citizens with proper lexical scoping, safely hoisted into global C functions and dynamically dispatched.
* **Tail Call Optimization (TCO):** Implements a robust **Trampoline** mechanism. Deep recursions are flattened into $O(1)$ space $O(N)$ time iterations, completely preventing C-level Stack Overflow errors.
* **Short-Circuit Evaluation:** Logical operators (`and`, `or`) strictly follow delayed execution order, compiled directly to C's native ternary operators (`? :`).
* **Mark-and-Sweep Garbage Collection:** Includes a custom memory manager with a root-tracking stack (`gc_push`/`gc_pop`) to safely manage Lisp values, linked lists, and dynamically allocated strings.
* **Inline C "Super Macros":** Provides a backdoor for systems programming. Using `c-expr` and `c-stmt`, developers can inject raw C code, pointers, and OS-level system calls directly inside the Lisp source.

## Build and Execution Instructions

The project uses CMake for building the transpiler, and GCC/MSVC for compiling the generated C output.

### 1. Build the Transpiler

Open your terminal in the project root directory and build the core executable using CMake:

```powershell
# Create or enter the build directory
cd build

# Build the project
cmake --build .

```

### 2. Transpile Lisp to C

Use the generated `c-lisp.exe` CLI tool to parse your Lisp file and transpile it into C code.

```powershell
# Run the transpiler (assuming test_transpile.lisp is in the root directory)
.\src\Debug\c-lisp.exe file ..\test_transpile.lisp ..\out.c

```

*Note: This will output a standard C file named `out.c` in the root directory.*

### 3. Compile and Run the Native Binary

Navigate back to the project root directory and use your C compiler (e.g., GCC) to link the generated `out.c` with the C-Lisp runtime:

```powershell
# Return to the project root
cd ..

# Compile the generated C code with the runtime environment
gcc out.c src/runtime/nucleus.c src/runtime/gc.c -I src -o my_lisp_app

# Execute the final native application
.\my_lisp_app.exe
```


