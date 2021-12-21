// See README.txt for information and build instructions.

#include <ctime>
#include <fstream>
#include <google/protobuf/util/time_util.h>
#include <iostream>
#include <string>
#include <chrono>

#include "demo.pb.h"
constexpr int BUFFER_LEN = 100000;

using namespace std;


uint8_t buffer[BUFFER_LEN];
uint8_t memcpy_dest[BUFFER_LEN];

void testProto() {
  demo::Person* person = new demo::Person();
  person->set_data(buffer, BUFFER_LEN);
  //delete person; // The main answer! Makes things 4x faster.
}

void testMemcpy() {
  demo::Person* person = new demo::Person();
  memcpy(memcpy_dest, buffer, BUFFER_LEN);
}

int main(int argc, char* argv[]) {
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  std::chrono::high_resolution_clock::time_point time = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 10000; i++) {
	testProto();
  }
  int us = std::chrono::duration_cast<std::chrono::microseconds>(
				  std::chrono::high_resolution_clock::now() - time).count();
  printf("Proto time: %dms\n", us / 1000);

  time = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 10000; i++) {
	testMemcpy();
  }
  us = std::chrono::duration_cast<std::chrono::microseconds>(
				  std::chrono::high_resolution_clock::now() - time).count();
  printf("Memcpy time: %dms\n", us / 1000);

  return 0;
}
