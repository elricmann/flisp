#include "./emit.h"

#include <iostream>
#include <memory>
#include <string>
// #include <unordered_map>

jvm_emitter::jvm_emitter() : bytecode_() {}

void jvm_emitter::emit_nop() { bytecode_.push_back(OP_NOP); }

void jvm_emitter::emit_iconst(int value) {
  switch (value) {
    case 0:
      bytecode_.push_back(OP_ICONST_0);
      break;
    case 1:
      bytecode_.push_back(OP_ICONST_1);
      break;
    case 2:
      bytecode_.push_back(OP_ICONST_2);
      break;
    case 3:
      bytecode_.push_back(OP_ICONST_3);
      break;
    case 4:
      bytecode_.push_back(OP_ICONST_4);
      break;
    case 5:
      bytecode_.push_back(OP_ICONST_5);
      break;
    default:
      if (value >= -128 && value <= 127) {
        bytecode_.push_back(OP_BIPUSH);
        bytecode_.push_back(static_cast<uint8_t>(value));
      } else if (value >= -32768 && value <= 32767) {
        bytecode_.push_back(OP_SIPUSH);
        bytecode_.push_back(static_cast<uint8_t>(value >> 8));
        bytecode_.push_back(static_cast<uint8_t>(value & 0xFF));
      } else {
        throw std::runtime_error("value out of range for iconst/bipush/sipush");
      }

      break;
  }
}

void jvm_emitter::emit_ireturn() { bytecode_.push_back(OP_IRETURN); }

void jvm_emitter::emit_return() { bytecode_.push_back(OP_RETURN); }

void jvm_emitter::emit_iadd() { bytecode_.push_back(OP_IADD); }

void jvm_emitter::emit_isub() { bytecode_.push_back(OP_ISUB); }

void jvm_emitter::emit_imul() { bytecode_.push_back(OP_IMUL); }

void jvm_emitter::emit_idiv() { bytecode_.push_back(OP_IDIV); }

void jvm_emitter::emit_iload(uint8_t idx) {
  bytecode_.push_back(OP_ILOAD);
  bytecode_.push_back(idx);
}

void jvm_emitter::emit_istore(uint8_t idx) {
  bytecode_.push_back(OP_ISTORE);
  bytecode_.push_back(idx);
}

void jvm_emitter::trace() const {
  for (auto byte : bytecode_) {
    printf("%02X ", byte);
  }

  std::cout << std::endl;
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
}
