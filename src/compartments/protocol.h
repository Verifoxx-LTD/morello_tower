/*
 * Copyright (c) 2020 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <iostream>

// Data types used in the client-server communication.
#define blockSize 16
#define OUTPUT_BUFLEN 16


enum class RequestType {
  kGetServerPublicKey,
  kGenerateClientKey,
};

struct Key {
  char data[64];
};

struct KeyPair {
  Key public_key;
  Key private_key;
};

struct Stream {
  char data[OUTPUT_BUFLEN];
};

struct Secret {
  Stream output;
};

struct Password {
  char data[100];
};

struct KDF_Inputs {
  char passwd[100];
  char salt[100];
};

struct Datablocks {
  uint8_t chunks[128 * blockSize];
};

struct Chunk {
  uint8_t X[64];
};

enum class CCallType {
  MORELLO_TA_ENCRYPT,
  MORELLO_TA_DECRYPT,
};


// Use a template to allow pasing both a pointer and a capability to the key.
template <typename Kp>
static inline void PrintKey(Kp key_ptr) {
  for (size_t i = 0; i < sizeof(key_ptr->data); ++i) {
    unsigned c = static_cast<unsigned>(key_ptr->data[i]);
    std::cout << std::hex << (c >> 4) << (c & 0xf);
  }
  std::cout << std::endl;
}

// template <typename Kp>
// static inline void Printuuid(Kp key_ptr) {
//   // for (size_t i = 0; i < sizeof(key_ptr.passwd); ++i) {
//   //   unsigned c = static_cast<unsigned>(key_ptr.passwd[i]);
//   //   std::cout << std::hex << (c >> 4) << (c & 0xf);
//   // }
//   // std::cout << std::endl;
//   for (size_t i = 0; i < sizeof(key_ptr->uuid); ++i) {
//     unsigned c = static_cast<unsigned>(key_ptr_uuid[i]);
//     std::cout << std::hex << (c >> 4) << (c & 0xf);
//   }
//   std::cout << std::endl;
// }
