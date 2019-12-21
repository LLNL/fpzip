#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fpzip.h"

static double
double_rand()
{
  static unsigned int seed = 1;
  double val;
  seed = 1103515245 * seed + 12345;
  seed &= 0x7fffffffu;
  val = ldexp((double)seed, -31);
  val = 2 * val - 1;
  val *= val * val;
  val *= val * val;
  return val;
}

static float
float_rand()
{
  return (float)double_rand();
}

/* generate a trilinear field perturbed by random noise */
float*
float_field(int nx, int ny, int nz, float offset)
{
  int n = nx * ny * nz;
  float* field = malloc(n * sizeof(float));
  int i, x, y, z;
  /* generate random field */
  *field = offset;
  for (i = 1; i < n; i++)
    field[i] = float_rand();
  /* integrate along x */
  for (z = 0; z < nz; z++)
    for (y = 0; y < ny; y++)
      for (x = 1; x < nx; x++)
        field[x + nx * (y + ny * z)] += field[(x - 1) + nx * (y + ny * z)];
  /* integrate along y */
  for (z = 0; z < nz; z++)
    for (y = 1; y < ny; y++)
      for (x = 0; x < nx; x++)
        field[x + nx * (y + ny * z)] += field[x + nx * ((y - 1) + ny * z)];
  /* integrate along z */
  for (z = 1; z < nz; z++)
    for (y = 0; y < ny; y++)
      for (x = 0; x < nx; x++)
        field[x + nx * (y + ny * z)] += field[x + nx * (y + ny * (z - 1))];
  return field;
}

/* generate a trilinear field perturbed by random noise */
double*
double_field(int nx, int ny, int nz, double offset)
{
  int n = nx * ny * nz;
  double* field = malloc(n * sizeof(double));
  int i, x, y, z;
  /* generate random field */
  *field = offset;
  for (i = 1; i < n; i++)
    field[i] = double_rand();
  /* integrate along x */
  for (z = 0; z < nz; z++)
    for (y = 0; y < ny; y++)
      for (x = 1; x < nx; x++)
        field[x + nx * (y + ny * z)] += field[(x - 1) + nx * (y + ny * z)];
  /* integrate along y */
  for (z = 0; z < nz; z++)
    for (y = 1; y < ny; y++)
      for (x = 0; x < nx; x++)
        field[x + nx * (y + ny * z)] += field[x + nx * ((y - 1) + ny * z)];
  /* integrate along z */
  for (z = 1; z < nz; z++)
    for (y = 0; y < ny; y++)
      for (x = 0; x < nx; x++)
        field[x + nx * (y + ny * z)] += field[x + nx * (y + ny * (z - 1))];
  return field;
}

/* compress floating-point data */
static size_t
compress(FPZ* fpz, const void* data)
{
  size_t size;
  /* write header */
  if (!fpzip_write_header(fpz)) {
    fprintf(stderr, "cannot write header: %s\n", fpzip_errstr[fpzip_errno]);
    return 0;
  }
  /* perform actual compression */
  size = fpzip_write(fpz, data);
  if (!size) {
    fprintf(stderr, "compression failed: %s\n", fpzip_errstr[fpzip_errno]);
    return 0;
  }
  return size;
}

/* decompress floating-point data */
static int
decompress(FPZ* fpz, void* data, size_t inbytes)
{
  /* read header */
  if (!fpzip_read_header(fpz)) {
    fprintf(stderr, "cannot read header: %s\n", fpzip_errstr[fpzip_errno]);
    return 0;
  }
  /* make sure array size stored in header matches expectations */
  if ((fpz->type == FPZIP_TYPE_FLOAT ? sizeof(float) : sizeof(double)) * fpz->nx * fpz->ny * fpz->nz * fpz->nf != inbytes) {
    fprintf(stderr, "array size does not match dimensions from header\n");
    return 0;
  }
  /* perform actual decompression */
  if (!fpzip_read(fpz, data)) {
    fprintf(stderr, "decompression failed: %s\n", fpzip_errstr[fpzip_errno]);
    return 0;
  }
  return 1;
}

static unsigned int
checksum(const void* buffer, size_t size)
{
  unsigned int h = 0;
  const unsigned char* p;
  for (p = buffer; size; p++, size--) {
    /* Jenkins one-at-a-time hash */
    /* See http://www.burtleburtle.net/bob/hash/doobs.html */
    h += *p;
    h += h << 10;
    h ^= h >>  6;
  }
  h += h <<  3;
  h ^= h >> 11;
  h += h << 15;
  return h;
}

static int
test(const char* name, int success)
{
  fprintf(stderr, "%-40s [%s]\n", name, success ? " OK " : "FAIL");
  return success;
}

/* perform compression, decompression, and validation of float array */
static int
test_float_array(const float* field, int nx, int ny, int nz, int prec, unsigned int expected_checksum)
{
  int success = 1;
  int status;
  unsigned int actual_checksum;
  int dims = (nz == 1 ? ny == 1 ? 1 : 2 : 3);
  size_t inbytes = nx * ny * nz * sizeof(float);
  size_t bufbytes = 1024 + inbytes;
  size_t outbytes = 0;
  void* buffer = malloc(bufbytes);
  float* copy = malloc(inbytes);
  char name[0x100];

  /* compress to memory */
  FPZ* fpz = fpzip_write_to_buffer(buffer, bufbytes);
  fpz->type = FPZIP_TYPE_FLOAT;
  fpz->prec = prec;
  fpz->nx = nx;
  fpz->ny = ny;
  fpz->nz = nz;
  fpz->nf = 1;
  outbytes = compress(fpz, field);
  status = (0 < outbytes && outbytes <= bufbytes);
  fpzip_write_close(fpz);
  sprintf(name, "test.float.%dd.prec%d.compress", dims, prec);
  success &= test(name, status);

  if (success) {
    /* test checksum */
    actual_checksum = checksum(buffer, outbytes);
    status = (actual_checksum == expected_checksum);
    if (!status)
      fprintf(stderr, "actual checksum %#010x does not match expected checksum %#010x\n", actual_checksum, expected_checksum);
    sprintf(name, "test.float.%dd.prec%d.checksum", dims, prec);
    success &= test(name, status);

    if (success) {
      /* decompress */
      fpz = fpzip_read_from_buffer(buffer);
      status = decompress(fpz, copy, inbytes);
      fpzip_read_close(fpz);
      sprintf(name, "test.float.%dd.prec%d.decompress", dims, prec);
      success &= test(name, status);

      if (success && !(0 < prec && prec < 32)) {
        /* validate */
        status = !memcmp(field, copy, inbytes);
        sprintf(name, "test.float.%dd.prec%d.validate", dims, prec);
        success &= test(name, status);
      }
    }
  }

  free(copy);
  free(buffer);

  return success;
}

/* perform compression, decompression, and validation of double array */
static int
test_double_array(const double* field, int nx, int ny, int nz, int prec, unsigned int expected_checksum)
{
  int success = 1;
  int status;
  unsigned int actual_checksum;
  int dims = (nz == 1 ? ny == 1 ? 1 : 2 : 3);
  size_t inbytes = nx * ny * nz * sizeof(double);
  size_t bufbytes = 1024 + inbytes;
  size_t outbytes = 0;
  void* buffer = malloc(bufbytes);
  float* copy = malloc(inbytes);
  char name[0x100];

  /* compress to memory */
  FPZ* fpz = fpzip_write_to_buffer(buffer, bufbytes);
  fpz->type = FPZIP_TYPE_DOUBLE;
  fpz->prec = prec;
  fpz->nx = nx;
  fpz->ny = ny;
  fpz->nz = nz;
  fpz->nf = 1;
  outbytes = compress(fpz, field);
  status = (0 < outbytes && outbytes <= bufbytes);
  fpzip_write_close(fpz);
  sprintf(name, "test.double.%dd.prec%d.compress", dims, prec);
  success &= test(name, status);

  if (success) {
    /* test checksum */
    actual_checksum = checksum(buffer, outbytes);
    status = (actual_checksum == expected_checksum);
    if (!status)
      fprintf(stderr, "actual checksum %#010x does not match expected checksum %#010x\n", actual_checksum, expected_checksum);
    sprintf(name, "test.double.%dd.prec%d.checksum", dims, prec);
    success &= test(name, status);

    if (success) {
      /* decompress */
      fpz = fpzip_read_from_buffer(buffer);
      status = decompress(fpz, copy, inbytes);
      fpzip_read_close(fpz);
      sprintf(name, "test.double.%dd.prec%d.decompress", dims, prec);
      success &= test(name, status);

      if (success && !(0 < prec && prec < 64)) {
        /* validate */
        status = !memcmp(field, copy, inbytes);
        sprintf(name, "test.double.%dd.prec%d.validate", dims, prec);
        success &= test(name, status);
      }
    }
  }

  free(copy);
  free(buffer);

  return success;
}

/* single-precision tests */
static int
test_float(int nx, int ny, int nz)
{
  int success = 1;
  const unsigned int cksum[][3][3] = {
    { /* FPZIP_FP_FAST */
      { 0x4cfacd05u, 0x0231ab7du, 0x8b8996d3u }, /* prec = 8 */
      { 0x85d0f4c2u, 0x2235e041u, 0xb17f5c48u }, /* prec = 16 */
      { 0x60ae0230u, 0x57e68ccdu, 0x2cb98d38u }, /* lossless */
    },
    { /* FPZIP_FP_SAFE */
      { 0xf5f4b20bu, 0x7887478du, 0xbbb34337u }, /* prec = 8 */
      { 0x69b605c4u, 0x150cc19du, 0xac6df12fu }, /* prec = 16 */
      { 0x987330bcu, 0x00b05244u, 0x6e756ea3u }, /* lossless */
    },
    { /* FPZIP_FP_EMUL */
      { 0x9450495cu, 0x146b1324u, 0xa6a8a407u }, /* prec = 8 */
      { 0xb41100a6u, 0x81f424a5u, 0xd1d81fecu }, /* prec = 16 */
      { 0xfc8c1c4au, 0x4fcd3866u, 0x15852fafu }, /* lossless */
    },
    { /* FPZIP_FP_INT */
      { 0x53dace3eu, 0xd5c02207u, 0x3507af15u }, /* prec = 8 */
      { 0x99de7d80u, 0xe9cc6e16u, 0x7971d6bau }, /* prec = 16 */
      { 0x3e32e8c1u, 0x8bb6d562u, 0x5d710559u }, /* lossless */
    },
  };
  float* field = float_field(nx, ny, nz, 0);
  int i;
  for (i = 0; i < 3; i++) {
    int prec = 8 << i;
    success &= test_float_array(field, nx * ny * nz, 1, 1, prec, cksum[FPZIP_FP - 1][i][0]);
    success &= test_float_array(field, nx, ny * nz, 1, prec, cksum[FPZIP_FP - 1][i][1]);
    success &= test_float_array(field, nx, ny, nz, prec, cksum[FPZIP_FP - 1][i][2]);
  }
  free(field);

  return success;
}

/* double-precision tests */
static int
test_double(int nx, int ny, int nz)
{
  int success = 1;
  const unsigned int cksum[][3][3] = {
    { /* FPZIP_FP_FAST */
      { 0xec303d31u, 0x761c82ecu, 0x1582bfc9u }, /* prec = 16 */
      { 0x19b1ed74u, 0xcd914c6bu, 0x4e9fa29du }, /* prec = 32 */
      { 0x84ab1addu, 0xba119745u, 0x0dd20faau }, /* lossless */
    },
    { /* FPZIP_FP_SAFE */
      { 0x649c2280u, 0xa410d07au, 0x7bdf9bb9u }, /* prec = 16 */
      { 0xb533993du, 0xff1ae747u, 0x2904d64au }, /* prec = 32 */
      { 0x4f5e1168u, 0x7821f9d7u, 0xb02f2dbau }, /* lossless */
    },
    { /* FPZIP_FP_EMUL */
      { 0x8f9d916eu, 0x5a1f2d53u, 0xdd8a74a5u }, /* prec = 16 */
      { 0x90b11f76u, 0x03e0f555u, 0x48e6acf7u }, /* prec = 32 */
      { 0x77a819e1u, 0xf5a906c9u, 0xf50b7a8bu }, /* lossless */
    },
    { /* FPZIP_FP_INT */
      { 0x914f81ddu, 0x3f845616u, 0xe09ab2d4u }, /* prec = 16 */
      { 0x670ccd29u, 0x1725b2d2u, 0x2421464au }, /* prec = 32 */
      { 0x7cc58c60u, 0xc5f53ff4u, 0xbfc5a355u }, /* lossless */
    },
  };
  double* field = double_field(nx, ny, nz, 0);
  int i;
  for (i = 0; i < 3; i++) {
    int prec = 16 << i;
    success &= test_double_array(field, nx * ny * nz, 1, 1, prec, cksum[FPZIP_FP - 1][i][0]);
    success &= test_double_array(field, nx, ny * nz, 1, prec, cksum[FPZIP_FP - 1][i][1]);
    success &= test_double_array(field, nx, ny, nz, prec, cksum[FPZIP_FP - 1][i][2]);
  }
  free(field);

  return success;
}

static int
init()
{
  int fpzip_fp = FPZIP_FP;

  fprintf(stderr, "%s\n", fpzip_version_string);
  fprintf(stderr, "library version %#x\n", fpzip_library_version);
  fprintf(stderr, "CODEC version %#x\n", fpzip_codec_version);

  fprintf(stderr, "data model ");
  switch (fpzip_data_model) {
    case 0x7331u:
      fprintf(stderr, "LP32\n");
      break;
    case 0x7333u:
      fprintf(stderr, "ILP32\n");
      break;
    case 0x7733u:
      fprintf(stderr, "LLP64\n");
      break;
    case 0x7773u:
      fprintf(stderr, "LP64\n");
      break;
    case 0x7777u:
      fprintf(stderr, "ILP64\n");
      break;
    default:
      fprintf(stderr, "unsupported (%#0x)", fpzip_data_model);
      if ((fpzip_data_model >> 12) != 0x7u)
        fprintf(stderr, "uint64 is not 8 bytes; must set FPZIP_INT64\n");
      return 0;
  }

  fprintf(stderr, "floating-point mode ");
  switch (fpzip_fp) {
    case FPZIP_FP_FAST:
      fprintf(stderr, "FAST\n");
      break;
    case FPZIP_FP_SAFE:
      fprintf(stderr, "SAFE\n");
      break;
    case FPZIP_FP_EMUL:
      fprintf(stderr, "EMUL\n");
      break;
    case FPZIP_FP_INT:
      fprintf(stderr, "INT\n");
      break;
    default:
      fprintf(stderr, "unsupported (%d)n", fpzip_fp);
      return 0;
  }

  fprintf(stderr, "\n");

  return 1;
}

int main()
{
  int success = 1;
  const int nx = 65;
  const int ny = 64;
  const int nz = 63;

  if (init()) {
    success &= test_float(nx, ny, nz);
    success &= test_double(nx, ny, nz);
    fprintf(stderr, "\n");
  }
  else
    success = 0;

  if (success) {
    fprintf(stderr, "all tests passed\n");
    return EXIT_SUCCESS;
  }
  else {
    fprintf(stderr, "one or more tests failed\n");
    return EXIT_FAILURE;
  }
}
