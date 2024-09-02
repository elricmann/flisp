## flisp

An embeddable Lisp with dynamic typing and a basic expression-level interpreter.

The alternative backend is a _WIP_ emitter that enables direct JVM bytecode serialization. In this case, forms may either be opcodes or predefined constructs like labels. The bytecode is aligned with class file directives/attributes and validated with `javap`. Refer to [emit.cc](https://github.com/elricmann/flisp/blob/main/src/emit.cc).

> OpenJDK uses the ASM Java package for working with class files, which is ideal.

### Quick overview

Forms represent both structural and semantic constructs, which is what makes lisps more unique than mainstream PL design.

- [x] `def` for defining variables (uses `std::unordered_map`)
- [x] `set` as the default assignment form (mutable-by-default)
- [x] `debug` is an alias for printing values to `std::cout`
- [x] `+`, `-`, `*`, `/` operators

In the evaluation loop, non-terminals are forward definitions and terminals are recursively evaluated (e.g. forms for binary operations), implying that a `def` may not be assigned to a `def` since terminals do not return an `expr_value`.

### Installation & usage

Clone this repository, run `make` and link with `build/libflisp.a`. All sources and headers are in [`src`](https://github.com/elricmann/flisp/blob/main/src/).

A more-detailed overview of flisp's forms can be found at [`main.lsp`](https://github.com/elricmann/flisp/blob/main/tests/main.lsp).

### Missing features

- [ ] Embed callable C++ expressions into the interpreter
- [ ] Skip expression parse tree when serializing to JVM bytecode
- [ ] Introduce static typing (Hindley-Milner) & FP constructs
- [ ] Replace expression-level interpreter with register-based VM
- [ ] Basic macros with recursion & templating/metaprogramming

On a sidenote, flisp is a _WIP_ and not ready for usage at this point.

### Resources

- [Clojure](https://clojure.org/)
- [Java class file - Wikipedia](https://en.wikipedia.org/wiki/Java_class_file)
- [All Opcodes by Opcode Value - The Java Version Almanac](https://javaalmanac.io/bytecode/opcodes/)
- [0xCAFEBABE? - java class file format, an overview](https://blog.lse.epita.fr/2014/04/28/0xcafebabe-java-class-file-format-an-overview.html)
