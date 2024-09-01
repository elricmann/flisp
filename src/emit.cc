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

// @todo: emit based on subpath/dirname of input
void write_class(const std::string& filename,
                 const std::vector<uint8_t>& bytecode) {
  std::ofstream file(filename, std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file (for write): " + filename);
  }

  file.write(reinterpret_cast<const char*>(&JAVA_CLASS_MAGIC_HEADER),
             sizeof(JAVA_CLASS_MAGIC_HEADER));
  file.write(reinterpret_cast<const char*>(bytecode.data()), bytecode.size());

  file.close();
}

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
  write_class("./tests/main.class", emitter.bytecode);
}
