#pragma once

#ifndef EMIT_H
#define EMIT_H

#include <cstdint>
#include <string>
#include <vector>

// clang-format off

const uint32_t JAVA_CLASS_MAGIC_HEADER = 0xCAFEBABE; // 4-byte header
const uint16_t JAVA_CLASS_MINOR_VERSION = 0x0000;    // minor version 0
const uint16_t JAVA_CLASS_MAJOR_VERSION = 0x0034;    // major version 52 (Java SE 8)

const uint16_t JAVA_CLASS_CONSTANT_POOL_COUNT = 0x1; // faux pool count

const uint16_t JAVA_CLASS_ACCESS_FLAGS = 0x0021;     // (public class)

const uint16_t JAVA_CLASS_THIS_CLASS = 0x0001;

const uint16_t JAVA_CLASS_SUPER_CLASS = 0x0000;      // (0: java/lang/Object)

const uint16_t JAVA_CLASS_INTERFACE_COUNT = 0x0000;
const uint16_t JAVA_CLASS_FIELD_COUNT = 0x0000;
const uint16_t JAVA_CLASS_METHOD_COUNT = 0x0000;
const uint16_t JAVA_CLASS_ATTRIBUTE_COUNT = 0x0000;

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-6.html
const uint8_t OP_NOP =          0x00;
const uint8_t OP_ICONST_0 =     0x03;
const uint8_t OP_ICONST_1 =     0x04;
const uint8_t OP_ICONST_2 =     0x05;
const uint8_t OP_ICONST_3 =     0x06;
const uint8_t OP_ICONST_4 =     0x07;
const uint8_t OP_ICONST_5 =     0x08;
const uint8_t OP_BIPUSH =       0x10;
const uint8_t OP_SIPUSH =       0x11;
const uint8_t OP_LDC =          0x12;
const uint8_t OP_ILOAD =        0x15;
const uint8_t OP_ISTORE =       0x36;
const uint8_t OP_IADD =         0x60;
const uint8_t OP_ISUB =         0x64;
const uint8_t OP_IMUL =         0x68;
const uint8_t OP_IDIV =         0x6C;
const uint8_t OP_IRETURN =      0xAC;
const uint8_t OP_RETURN =       0xB1;

// only used to test against fixtures
void __test_emit__();

void write_class_file(const std::string& filename,
                 const std::vector<uint8_t>& bytecode);

// clang-format on

class jvm_emitter {
 public:
  std::vector<uint8_t> bytecode;
  jvm_emitter();

  // we need a less-redundant way of emitting without
  // keeping the implementation in the visitor

  void emit_nop();
  void emit_iconst(int value);
  void emit_ireturn();
  void emit_return();
  void emit_iadd();
  void emit_isub();
  void emit_imul();
  void emit_idiv();
  void emit_iload(uint8_t index);
  void emit_istore(uint8_t index);
  void trace() const;
};

#endif  // EMIT_H
