#include <err.h>
#include <stdio.h>
#include <string.h>
#include <filesystem>
#include <iostream>

#include "tec_client_api.h"
#include "compartment_manager.h"

#define MORELLO_TA_MTOWER_UUID { 0x8aaaf200, 0x2450, 0x11e4, { 0xab, 0xe2, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b} }

#define MORELLO_TA_ENCRYPT		0
#define MORELLO_TA_DECRYPT		1

uint32_t morelloTower(uint32_t input_param, uint32_t TEC_FUNC_TO_INVOKE, TEC_UUID uuid){

  TEC_Result res;
	TEC_Context ctx;
	TEC_Session sess;
  // TEC_UUID uuid = MORELLO_TA_MTOWER_UUID;
	uint32_t err_origin;

  std::cout <<  "[Main] ... Initialising Context" << std::endl;

  res = TEC_InitializeContext(NULL, &ctx);

  if (res != TEC_SUCCESS)
		errx(1, "TEC_InitializeContext failed with code 0x%x", res);

  std::cout <<  "[Main] MORELLO_TOWER UUID - " << uuid.timeMid << uuid.timeLow << uuid.timeHiAndVersion << uuid.clockSeqAndNode << std::endl;

  res = TEC_OpenSession(&ctx, &sess, &uuid, TEC_LOGIN_PUBLIC, NULL, &err_origin);
	if (res != TEC_SUCCESS)
		errx(1, "TEC_OpenSession failed with code 0x%x origin 0x%x", res, err_origin);

  std::cout << "[Main] Enclave Compartment Opened (ID) " << sess.session_id << std::endl;   

  std::cout << "[Main] Invoke Enclave Function (ID) " << sess.session_id << std::endl;   

  res = TEC_InvokeCommand(&sess, TEC_FUNC_TO_INVOKE, &input_param, &err_origin);
  if (res != TEC_SUCCESS){
		  errx(1, "TEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
  }

  std::cout << "[Main] ECall Success - Response " << input_param << std::endl;

    // // printf(input_param);

  TEC_CloseSession(&sess);

	TEC_FinalizeContext(&ctx);

  return input_param;

}

int main(int argc, char** argv) {
    
    std::string progname{argv[0]};

    // Get our dirname.
    std::string dirname{progname};
    size_t pos = dirname.find_last_of('/');
    // If there is no '/', then erase all the string (that's good enough for constructing relative
    // paths).
    dirname.erase(pos == std::string::npos ? 0 : pos + 1);

    char *dir = new char[dirname.length() + 1];
    strcpy(dir, dirname.c_str());

    // std::cout << "[Main] Dirname" << dir << std::endl;
    CompartmentManagerInit();

    // uint32_t input_param = 220; 
    // uint32_t encrypted_output;
    // encrypted_output = morelloTower(input_param, MORELLO_TA_ENCRYPT, MORELLO_TA_MTOWER_UUID);
    // std::cout << "[Main] ENCRYPTED OUTPUT -  " << encrypted_output << std::endl;

    // std::cout << "-------------------------------------------------------------" << std::endl << std::endl;
    uint32_t decrypted_output;
    uint32_t encrypted_output = 50716160;
    decrypted_output = morelloTower(encrypted_output, MORELLO_TA_DECRYPT, MORELLO_TA_MTOWER_UUID);
    std::cout << "[Main] DECRYPTED OUTPUT -  " << decrypted_output << std::endl;

	return 0;

}

