#include <sys/random.h>

#include <iostream>
#include <random>
#include <ctime>

#include <archcap.h>

#include "compartment_helpers.h"
#include "protocol.h"
// #include "tec_client/libtec/include/freertos/tec.h"

const uint32_t ENCLAVE_SECRET = 6;
const uint32_t ONE = 1;
const uint32_t TWO = 2;
const uint32_t MULTIPLIER = 1000;


uint32_t modulo(uint32_t a, uint32_t b, uint32_t n){
    uint32_t x=ONE;
    uint32_t y=a; 
    while (b > 0) {
        if (b%2 == 1) {
            x = (x*y) % n; // multiplying with base
            // std::cout << "X: " << x << std::endl;
        }
        y = (y*y) % n; // squaring the base
        b /= 2;
        // std::cout << "B: " << b << " , Y: " << y << std::endl;
    }
    return x % n;
}

uint32_t getEnclaveFactor() {
   time_t now = time(0);
   char* dt = ctime(&now);
   std::cout << "The local date and time is: " << dt << std::endl;
   uint32_t eFactor;
   memcpy(&eFactor,dt, 4);
   std::cout << "eFactor - " << eFactor << std::endl;
//    // convert now to tm struct for UTC
//    tm *gmtm = gmtime(&now);
//    dt = asctime(gmtm);
//    std::cout << "The UTC date and time is:"<< dt << std::endl;
   return eFactor;
}

uint32_t deriveCommitment(uint32_t input, CCallType request){
    // std::random_device rdev;
    uint32_t result;
    uint32_t seed = getEnclaveFactor();    
    std::cout << "Seed - " << seed << std::endl;
    std::seed_seq seed_gen{seed, seed * 2, seed * 3};
    
    std::mt19937 eng(seed_gen);

    std::uniform_real_distribution<double> urd(0, 1);
    double p = urd(eng);
    double q = urd(eng);
    std::cout << "P: " << p << " ,Q: " << q << std::endl;
    uint32_t p1 = (uint32_t)(p * MULTIPLIER);
    uint32_t q1 = (uint32_t)(q * MULTIPLIER);
    uint32_t n = p1 * q1;
    uint32_t phi_n = (q1- (p1/2));
    uint32_t c = modulo(p1,q1,n);
    std::cout << "P: " << p1 << " ,Q: " << q1 << " , N: " << n << " , Phi_N: " << phi_n << std::endl;
    std::cout << "C: " << c << std::endl;

    switch (request)
    {
    case CCallType::MORELLO_TA_ENCRYPT:{
        result = (c*input);
        std::cout << "Commitment - " << result << std::endl;
        auto x = ((phi_n*result)/(c)) + ONE;
        // std::cout << "Original Default - " << x << std::endl;
        break;
    }
    default:
        result = (input)/(c);
        std::cout << "Original Default - " << result << std::endl;
        break;
    }
    return result;
    
}

COMPARTMENT_ENTRY_POINT(CCallType request, uint32_t* __capability client_data) {

    uint32_t result;
    switch (request)
    {
    case CCallType::MORELLO_TA_ENCRYPT:{
        std::cout << "[Enclave] MORELLO_TA_ENCRYPT" << std::endl;
        // Check that the client-provided capability is appropriate for storing the generated key pair
        // to, by inspecting its tag, bounds and permissions. Note that this is not an exhaustive
        // check, and there is no reliable way to ensure that the underlying memory is accessible.
        if (archcap_c_tag_get(client_data) &&
          (archcap_c_limit_get(client_data) - archcap_c_address_get(client_data)) >= sizeof(result) &&
          (archcap_c_perms_get(client_data) & ARCHCAP_PERM_LOAD) != 0 && (archcap_c_perms_get(client_data) & ARCHCAP_PERM_STORE) != 0) {
            std::cout << "[Enclave] inside" << std::endl;
            // Use memcpy_c() to write via the client capability. We use DDC to construct a source
            // capability.
            uint32_t incoming_val;
            memcpy_c(&incoming_val, client_data, sizeof(incoming_val));
            std::cout << "[Enclave] Incoming Data : " << incoming_val << std::endl;
            incoming_val = deriveCommitment(incoming_val, request);
            // auto incoming_data = reinterpret_cast<const uint32_t* __capability>(client_data);
            // incoming_val = incoming_val * ENCLAVE_SECRET;
            std::cout << "[Enclave] Result  : " << incoming_val << std::endl;

            memcpy_c(client_data, archcap_c_ddc_cast(&incoming_val), sizeof(incoming_val));
        
            CompartmentReturn(0);
        } else {
            CompartmentReturn(-1);
        }
        break;
    }
    default:
        std::cout << "[Enclave] MORELLO_TA_DECRYPT" << std::endl;
        if (archcap_c_tag_get(client_data) &&
            (archcap_c_limit_get(client_data) - archcap_c_address_get(client_data)) >= sizeof(result) &&
            (archcap_c_perms_get(client_data) & ARCHCAP_PERM_LOAD) != 0 && (archcap_c_perms_get(client_data) & ARCHCAP_PERM_STORE) != 0) {
            // Use memcpy_c() to write via the client capability. We use DDC to construct a source
            // capability.
            uint32_t incoming_val;
            memcpy_c(&incoming_val, client_data, sizeof(incoming_val));
            std::cout << "[Enclave] Incoming Data : " << incoming_val << std::endl;
            incoming_val = deriveCommitment(incoming_val, request);
            // incoming_val = incoming_val / ENCLAVE_SECRET;
            std::cout << "[Enclave] Result  : " << incoming_val << std::endl;
            memcpy_c(client_data, archcap_c_ddc_cast(&incoming_val), sizeof(incoming_val));
            CompartmentReturn(0);
        } else {
            CompartmentReturn(-1);
        }
        break;
    } 
}


int main(int, char** argv) {
  std::cout << "[Enclave] Compartment @" << argv[0] << " initialized" << std::endl;
  // Return to the compartment manager, letting it know that we have completed our initialization.
  CompartmentReturn();
}