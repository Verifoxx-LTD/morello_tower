/*
 * Copyright (c) 2020 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <filesystem>
#include <iostream>
#include <archcap.h>

#include "compartment_config.h"
#include "compartment_manager.h"

struct Password {
  char data[100];
};

namespace {

std::string DefaultClientPath(const std::string& dirname) {
  // return dirname + "compartments/client_generate_keys";
  return dirname + "compartments/client_derive_secret_key";
}

std::string DefaultNodeOnePath(const std::string& dirname) {
  return dirname + "compartments/nodeone";
}

std::string DefaultNodeTwoPath(const std::string& dirname) {
  return dirname + "compartments/nodetwo";
}

std::string DefaultNodeThreePath(const std::string& dirname) {
  return dirname + "compartments/nodethree";
}

std::string DefaultServerPath(const std::string& dirname) {
  return dirname + "compartments/server";
}

void Usage(const std::string& progname, const std::string& dirname) {
  std::cout << "Usage: " << progname << " [client_path [server_path]]\n";
  std::cout << "Default compartment paths (if not specified):\n";
  std::cout << "    client_path: " << DefaultClientPath(dirname) << "\n";
  std::cout << "    server_path: " << DefaultServerPath(dirname) << "\n";
  std::cout << "    nodeone_path: " << DefaultNodeOnePath(dirname) << "\n";
  std::cout << "    nodetwo_path: " << DefaultNodeTwoPath(dirname) << "\n";
  std::cout << "    nodethree_path: " << DefaultNodeThreePath(dirname) << "\n";
}

}

int main(int argc, char** argv) {
  std::string progname{argv[0]};

  // Get our dirname.
  std::string dirname{progname};
  size_t pos = dirname.find_last_of('/');
  // If there is no '/', then erase all the string (that's good enough for constructing relative
  // paths).
  dirname.erase(pos == std::string::npos ? 0 : pos + 1);

  std::string client_path = DefaultClientPath(dirname);
  std::string server_path = DefaultServerPath(dirname);
  std::string nodeone_path = DefaultNodeOnePath(dirname);
  std::string nodetwo_path = DefaultNodeTwoPath(dirname);
  std::string nodethree_path = DefaultNodeThreePath(dirname);
  Password input;
  
  

  switch (argc) {
    case 3:
      // if (!std::filesystem::exists(argv[2])) {
      //   std::cerr << "Error: " << argv[2] << " does not exist\n";
      //   return 1;
      // }
      // server_path = argv[2];

      [[fallthrough]];
    case 2: {
      // std::string arg = argv[1];
      // if (arg == "-h" || arg == "--help") {
      //   Usage(progname, dirname);
      //   return 0;
      // }

      // if (!std::filesystem::exists(argv[1])) {
      //   std::cerr << "Error: " << argv[1] << " does not exist\n";
      //   return 1;
      // }
      // client_path = argv[1];
      strcpy(input.data, argv[1]);

      break;
    }
    case 1:
      break;
    default:
      Usage(progname, dirname);
      return 1;
  }

  std::cout << "[Main]  Entered Client Password: " << input.data << std::endl;
  Password* __capability input_cap = archcap_c_ddc_cast(&input);
  input_cap = archcap_c_perms_set(input_cap, ARCHCAP_PERM_GLOBAL | ARCHCAP_PERM_LOAD);

  CompartmentManagerInit();
  CompartmentAdd(kClientCompartmentId, client_path, {}, kCompartmentMemoryRangeLength);
  // CompartmentAdd(kServerCompartmentId, server_path, {}, kCompartmentMemoryRangeLength);
  CompartmentAdd(kNodeOneCompartmentId, nodeone_path, {}, kCompartmentMemoryRangeLength);
  CompartmentAdd(kNodeTwoCompartmentId, nodetwo_path, {}, kCompartmentMemoryRangeLength);
  CompartmentAdd(kNodeThreeCompartmentId, nodethree_path, {}, kCompartmentMemoryRangeLength);

  // Start the client compartment and wait until it's done.
  CompartmentCall(kClientCompartmentId, AsUintcap(input_cap));

  std::cout << "[MCC-Scrypt] KDF demo completed\n";

  return 0;
}
