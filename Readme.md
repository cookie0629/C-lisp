# C-Lisp Machine: A Lisp-to-C Transpiler

C-Lisp is a minimal, Turing-complete Lisp dialect and transpiler written entirely in C. Instead of interpreting Lisp code or compiling it to a custom bytecode, C-Lisp transpiles Lisp Abstract Syntax Trees (AST) directly into standard C code, which is then compiled to a native binary by GCC or MSVC.

## Key Features

- **Native C Transpilation** — Converts Lisp expressions directly into C statements and comma operators. The output is 100% standard C compilable by GCC, Clang, or MSVC.
- **Macro System** — A full compile-time macro system with `defmacro`, quasiquote (`` ` ``), unquote (`,`), and unquote-splicing (`,@`). Macro bodies are evaluated by a built-in compile-time Lisp interpreter, allowing macros to contain `if`, `let`, arithmetic, and string operations. Macros can also emit raw C code fragments via `c-code`.
- **First-Class Functions & Closures** — Full support for `lambda` and `funcall`. Functions are first-class citizens with lexical scoping, hoisted into global C functions and dynamically dispatched.
- **Tail Call Optimization (TCO)** — Implements a Trampoline mechanism. Deep recursions are flattened into O(1) stack space, completely preventing C-level stack overflow.
- **Short-Circuit Evaluation** — `and` / `or` compile directly to C ternary operators (`? :`), guaranteeing lazy evaluation order.
- **Mark-and-Sweep Garbage Collector** — A custom GC with a free-list heap, a root-tracking stack (`gc_push` / `gc_pop`), and a `#ifdef GC_DEBUG` logging mode that prints every reclaimed object during the sweep phase.
- **Inline C Escape Hatches** — `c-expr` and `c-stmt` let you inject arbitrary C code directly into Lisp source for systems-level programming.

## Project Structure

```
C-Lisp/
├── src/
│   ├── parser/
│   │   ├── parser.c        Lexer + recursive-descent parser
│   │   └── parser.h
│   ├── runtime/
│   │   ├── nucleus.c/h     Type system, constructors, trampoline
│   │   └── gc.c/h          Mark-and-Sweep garbage collector
│   ├── macro.c/h           Compile-time macro expander + Lisp evaluator
│   ├── transpiler.c/h      AST → C code generator
│   ├── builder.c/h         Dynamic string buffer
│   └── main.c              CLI entry point
├── test_transpile.lisp     Core feature tests (variables, closures, TCO, inline C)
├── demo.lisp               Macro system demo (for/repeat/assert/square/max2) + GC stats
├── demo_tree_gc.lisp       GC tree demo — builds a binary tree, drops the root,
│                           shows recursive sweep log in GC_DEBUG mode
└── out.c / demo_out.c /    Generated C output examples
    demo_tree_gc_out.c
```

## Build Instructions

The project uses CMake. All commands are run from the project root.

### 1. Build the Transpiler

```powershell
cd build
cmake --build .
```

This produces `build/src/Debug/c-lisp.exe`.

### 2. Transpile a Lisp File to C

```powershell
.\build\src\Debug\c-lisp.exe file <input.lisp> <output.c>
```

Example:

```powershell
.\build\src\Debug\c-lisp.exe file test_transpile.lisp out.c
```

### 3. Compile and Run the Generated C

Link the generated file with the runtime:

```powershell
gcc out.c src/runtime/nucleus.c src/runtime/gc.c -I src -o my_lisp_app.exe
.\my_lisp_app.exe
```

## Demo Files

### `test_transpile.lisp` — Core Feature Test

Covers variables, lexical scope (`let`), short-circuit logic, `while` loops, lists, higher-order functions (`lambda` / `funcall`), inline C (`c-expr` / `c-stmt`), and tail-call optimization with a 50 000-iteration recursive sum.

```powershell
.\build\src\Debug\c-lisp.exe file test_transpile.lisp out.c
gcc out.c src/runtime/nucleus.c src/runtime/gc.c -I src -o my_lisp_app.exe
.\my_lisp_app.exe
```

### `demo.lisp` — Macro System + GC Stats

Defines and calls five complex macros (`for`, `unless`, `repeat`, `square`, `max2`, `assert`), then demonstrates the GC by building a 300-element list, dropping the reference, and printing heap stats before and after collection.

```powershell
.\build\src\Debug\c-lisp.exe file demo.lisp demo_out.c
gcc demo_out.c src/runtime/nucleus.c src/runtime/gc.c -I src -o demo_app.exe
.\demo_app.exe
```

### `demo_tree_gc.lisp` — GC Debug Logging + Tree Sweep Demo

Builds an 8-node binary tree using `make-node` macros, drops all references to the root, then forces a GC collection. Compile with `-DGC_DEBUG` to see every individual object reclaimed during the sweep phase, proving the collector recursively traces and frees the entire tree structure.

```powershell
.\build\src\Debug\c-lisp.exe file demo_tree_gc.lisp demo_tree_gc_out.c

# Release build (no logging)
gcc demo_tree_gc_out.c src/runtime/nucleus.c src/runtime/gc.c -I src -o demo_tree_gc.exe
.\demo_tree_gc.exe

# Debug build (full sweep log)
gcc demo_tree_gc_out.c src/runtime/nucleus.c src/runtime/gc.c -I src -DGC_DEBUG -o demo_tree_gc_debug.exe
.\demo_tree_gc_debug.exe
```

Expected debug output (excerpt):

```
[GC] ========== Collection START  (live before=72) ==========
[GC][sweep] reclaim INT      = 7  (addr=...)   <- leaf node n7
[GC][sweep] reclaim CONS     (car=... cdr=...)  <- n7's cons cell
[GC][sweep] reclaim INT      = 8  (addr=...)   <- leaf node n8
...
[GC][sweep] reclaim INT      = 0  (addr=...)   <- root node
[GC][sweep] reclaim CONS     (car=... cdr=...)  <- root's cons cell
[GC][sweep] --- total reclaimed: 72 objects ---
[GC] ========== Collection END    (live after=0, reclaimed=72) ==========
```

## Macro System

Macros are defined with `defmacro` and expanded at transpile time — they produce zero runtime overhead.

```lisp
; Quasiquote template macro
(defmacro when (cond body)
  `(if ,cond ,body nil))

; Compile-time arithmetic macro
(defmacro square (x)
  `(* ,x ,x))

; Macro that directly emits a C expression
(defmacro fast-print (msg)
  (c-code (string-append "lisp_int(puts(\"" (symbol->string msg) "\"))")))
```

Supported compile-time operations inside macro bodies: `if`, `let`, `progn`, `cons`/`car`/`cdr`, `list`, arithmetic (`+` `-` `*` `/`), comparisons, `string-append`, `symbol->string`, `number->string`, `c-code`.

## GC Debug Mode

Compile any generated C file with `-DGC_DEBUG` to enable per-object sweep logging:

```powershell
gcc output.c src/runtime/nucleus.c src/runtime/gc.c -I src -DGC_DEBUG -o app_debug.exe
```

Each reclaimed object is printed with its type, value, and heap address. The flag has zero cost in release builds.
