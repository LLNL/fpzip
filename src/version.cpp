#include "fpzip.h"
#include "types.h"

const unsigned int fpzip_codec_version = FPZIP_CODEC;
const unsigned int fpzip_library_version = FPZIP_VERSION;
const char* const fpzip_version_string = "fpzip version " FPZIP_VERSION_STRING " (December 20, 2019)";
const unsigned int fpzip_data_model = (unsigned int)(
  ((sizeof(uint64) - 1) << 12) +
  ((sizeof(void*) - 1) << 8) +
  ((sizeof(unsigned long int) - 1) << 4) +
  ((sizeof(unsigned int) - 1) << 0)
);
