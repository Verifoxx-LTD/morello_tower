#ifndef __TEC_H
#define __TEC_H

#pragma once
#include <iostream>

#define TEC_GEN_CAP_GP		(1 << 0)/* GlobalPlatform compliant TEE */
#define TEC_GEN_CAP_REG_MEM	(1 << 2)/* Supports registering shared memory */

#define TEC_MANAGER_BASE  0

struct tec_open_session_arg {
	// uint8_t uuid[16];
	TEC_UUID uuid;
	uint32_t clnt_login;
	uint32_t session_original;
	uint32_t session;
	uint32_t ret;
	// uint32_t ret_origin;
	// uint32_t num_params;
	
} ; 


#define TEC_MANAGER_OPEN_COMPARTMENT_SESSION	TEC_MANAGER_BASE + 2

struct tec_invoke_arg {
	uint32_t func;
	uint32_t session;
	// uint32_t cancel_id;
	uint32_t ret;
    uint32_t param;
	// uint32_t ret_origin;
	// uint32_t num_params;
};


#define TEC_MANAGER_COMPARTMENT_INVOKE	TEC_MANAGER_BASE + 3

struct tec_close_session_arg {
	uint32_t session;
};


#define TEC_MANAGER_CLOSE_COMPARTMENT_SESSION	TEC_MANAGER_BASE + 5

struct tec_buf_data {
	uint32_t buf_ptr;
	uint32_t buf_len;
};

enum class CCallType {
  MORELLO_TA_ENCRYPT,
  MORELLO_TA_DECRYPT,
};

template <typename Kp>
static inline void Printuuid(Kp key_ptr) {
  // for (size_t i = 0; i < sizeof(key_ptr.passwd); ++i) {
  //   unsigned c = static_cast<unsigned>(key_ptr.passwd[i]);
  //   std::cout << std::hex << (c >> 4) << (c & 0xf);
  // }
  // std::cout << std::endl;
//   for (size_t i = 0; i < sizeof(key_ptr->uuid); ++i) {
//     unsigned c = static_cast<unsigned>(key_ptr->uuid[i]);
//     std::cout << std::hex << (c >> 4) << (c & 0xf);
//   }
  std::cout << std::endl;
}

#endif