#include "./emit.h"

#include <iostream>
#include <memory>
// #include <unordered_map>
#include <fstream>

jvm_emitter::jvm_emitter() : bytecode() {}

void jvm_emitter::emit_nop() { bytecode.push_back(OP_NOP); }

void jvm_emitter::emit_iconst(int value) {
  switch (value) {
    case 0:
      bytecode.push_back(OP_ICONST_0);
      break;
    case 1:
      bytecode.push_back(OP_ICONST_1);
      break;
    case 2:
      bytecode.push_back(OP_ICONST_2);
      break;
    case 3:
      bytecode.push_back(OP_ICONST_3);
      break;
    case 4:
      bytecode.push_back(OP_ICONST_4);
      break;
    case 5:
      bytecode.push_back(OP_ICONST_5);
      break;
    default:
      if (value >= -128 && value <= 127) {
        bytecode.push_back(OP_BIPUSH);
        bytecode.push_back(static_cast<uint8_t>(value));
      } else if (value >= -32768 && value <= 32767) {
        bytecode.push_back(OP_SIPUSH);
        bytecode.push_back(static_cast<uint8_t>(value >> 8));
        bytecode.push_back(static_cast<uint8_t>(value & 0xFF));
      } else {
        throw std::runtime_error("value out of range for iconst/bipush/sipush");
      }

      break;
  }
}

void jvm_emitter::emit_ireturn() { bytecode.push_back(OP_IRETURN); }

void jvm_emitter::emit_return() { bytecode.push_back(OP_RETURN); }

void jvm_emitter::emit_iadd() { bytecode.push_back(OP_IADD); }

void jvm_emitter::emit_isub() { bytecode.push_back(OP_ISUB); }

void jvm_emitter::emit_imul() { bytecode.push_back(OP_IMUL); }

void jvm_emitter::emit_idiv() { bytecode.push_back(OP_IDIV); }

void jvm_emitter::emit_iload(uint8_t idx) {
  bytecode.push_back(OP_ILOAD);
  bytecode.push_back(idx);
}

void jvm_emitter::emit_istore(uint8_t idx) {
  bytecode.push_back(OP_ISTORE);
  bytecode.push_back(idx);
}

void jvm_emitter::trace() const {
  for (auto byte : bytecode) {
    printf("%02X ", byte);
  }

  std::cout << std::endl;
}

// /!\ endianness of class files and bytes written to C++ filestream
//     require re-ordering, javap appears to parse metadata this way

void write_uint16_be(std::ofstream& file, uint16_t value) {
  file.put(static_cast<char>((value >> 8) & 0xFF));
  file.put(static_cast<char>(value & 0xFF));
}

void write_uint32_be(std::ofstream& file, uint32_t value) {
  file.put(static_cast<char>((value >> 24) & 0xFF));
  file.put(static_cast<char>((value >> 16) & 0xFF));
  file.put(static_cast<char>((value >> 8) & 0xFF));
  file.put(static_cast<char>(value & 0xFF));
}

// @todo: emit based on subpath/dirname of input
[[deprecated]] void write_class_file(const std::string& filename,
                                     const std::vector<uint8_t>& bytecode) {
  std::ofstream file(filename, std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file (for write): " + filename);
  }

  write_uint32_be(file, JAVA_CLASS_MAGIC_HEADER);

  write_uint16_be(file, JAVA_CLASS_MINOR_VERSION);
  write_uint16_be(file, JAVA_CLASS_MAJOR_VERSION);

  write_uint16_be(file, 5);

  file.put(CONSTANT_Class);
  write_uint16_be(file,
                  3);  // "__flisp_module__" @ index 3 in CP

  // constant pool entry 2: class ref (java/lang/Object)
  file.put(CONSTANT_Class);
  write_uint16_be(file, 4);  // "java/lang/Object" @ index 4 in CP

  // constant pool entry 3: utf-8 (class name)
  file.put(CONSTANT_Utf8);
  write_uint16_be(file, JAVA_CLASS_NAME.size());
  file.write(JAVA_CLASS_NAME.c_str(), JAVA_CLASS_NAME.size());

  // constant pool entry 4: utf-8 (super class name)
  file.put(CONSTANT_Utf8);
  write_uint16_be(file, JAVA_SUPER_CLASS_NAME.size());
  file.write(JAVA_SUPER_CLASS_NAME.c_str(), JAVA_SUPER_CLASS_NAME.size());

  write_uint16_be(file, JAVA_CLASS_ACCESS_FLAGS);

  // there likely needs to be a hidden method and big-endian attributes
  // after the bytecode is appended. for now this should stay deprecated

  /* references of "this" and super classes */

  write_uint16_be(file, 1);  // "this" class @ index 1 in CP
  write_uint16_be(file, 2);  // superclass @ index 2 in CP
  write_uint16_be(file, 0);  // interfaces count
  write_uint16_be(file, 0);  // fields count
  write_uint16_be(file, 0);  // methods count
  write_uint16_be(file, 0);  // attributes count

  // force write the bytecode & clamp to bytecode.size()
  file.write(reinterpret_cast<const char*>(bytecode.data()), bytecode.size());

  file.close();
}

// make && clear && make run ARGS="-c tests/main.lsp"
// javap tests/main.class
void __test_emit__() {
  jvm_emitter emitter;
  emitter.emit_iconst(2);
  emitter.emit_iconst(3);
  emitter.emit_iadd();
  emitter.emit_ireturn();

  std::cout << "--------------------------" << std::endl;
  std::cout << "     tracing bytecode     " << std::endl;
  std::cout << "--------------------------" << std::endl;

  emitter.trace();
  write_class_file("./tests/main.class", emitter.bytecode);
}
