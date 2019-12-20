#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fpzip.h"

static double
double_rand()
{
  static unsigned long seed = 1;
  double val;
  seed = 0x5deece66dul * seed + 13ul;
  seed &= 0xfffffffffffful;
  val = ldexp(seed, -48);
  val = 2 * val - 1;
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
      fprintf(stderr, "actual checksum %#x does not match expected checksum %#x\n", actual_checksum, expected_checksum);
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
      fprintf(stderr, "actual checksum %#x does not match expected checksum %#x\n", actual_checksum, expected_checksum);
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
      { 0x3000789du, 0x82ff21a9u, 0x8ba839cau }, /* prec = 8 */
      { 0x4e83f5b6u, 0x65bd23deu, 0xe7dfa496u }, /* prec = 16 */
      { 0x8c3d93f8u, 0xd76feb4bu, 0xaa01387cu }, /* lossless */
    },
    { /* FPZIP_FP_SAFE */
      { 0xe2652208u, 0xf63432f4u, 0x29ff853eu }, /* prec = 8 */
      { 0xec094b7au, 0xe28a7309u, 0xafcbfb1cu }, /* prec = 16 */
      { 0xa7e95552u, 0xb036a419u, 0x3bb63058u }, /* lossless */
    },
    { /* FPZIP_FP_EMUL */
      { 0x5c55f580u, 0xd792786au, 0x6f58736bu }, /* prec = 8 */
      { 0x6120c7ebu, 0x88b63ac7u, 0x41b48927u }, /* prec = 16 */
      { 0xfb5bbb6du, 0xf032dbb1u, 0x316cb73cu }, /* lossless */
    },
    { /* FPZIP_FP_INT */
      { 0xe915d3cfu, 0xb99c594eu, 0x74edb83cu }, /* prec = 8 */
      { 0x7ea41342u, 0x230aabebu, 0x6192c4cau }, /* prec = 16 */
      { 0xd2cdf77au, 0x9cdd54acu, 0xa7addbd2u }, /* lossless */
    },
  };
  float* field = float_field(nx, ny, nz, -4);
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
      { 0x4c9467edu, 0x1001c8e4u, 0xe43b6147u }, /* prec = 16 */
      { 0xbb57b84au, 0x49d952edu, 0x3994f942u }, /* prec = 32 */
      { 0x8680e06eu, 0xd00fdc85u, 0x8d63188eu }, /* lossless */
    },
    { /* FPZIP_FP_SAFE */
      { 0x91f8f8c5u, 0xfe9110c8u, 0xceaf0a86u }, /* prec = 16 */
      { 0x2faf5806u, 0xc5da48e9u, 0xfb922152u }, /* prec = 32 */
      { 0x1f8b2600u, 0xa7bd7c44u, 0xa14ef493u }, /* lossless */
    },
    { /* FPZIP_FP_EMUL */
      { 0x3b8aa65cu, 0x9b82e8fbu, 0x62cbdcddu }, /* prec = 16 */
      { 0x219a7aa0u, 0x187ddda3u, 0xaeb32ebcu }, /* prec = 32 */
      { 0x1ceaa5ecu, 0x8509dad8u, 0x9944c23fu }, /* lossless */
    },
    { /* FPZIP_FP_INT */
      { 0x340802f9u, 0x9141329bu, 0x17603dc7u }, /* prec = 16 */
      { 0x0a948b36u, 0x0175645eu, 0x70f8052cu }, /* prec = 32 */
      { 0xe24d010bu, 0x852cc790u, 0xe668cf86u }, /* lossless */
    },
  };
  double* field = double_field(nx, ny, nz, -4);
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

int main()
{
  const int nx = 65;
  const int ny = 64;
  const int nz = 63;

  int success = 1;
  success &= test_float(nx, ny, nz);
  success &= test_double(nx, ny, nz);
  fprintf(stderr, "\n");

  if (success) {
    fprintf(stderr, "all tests passed\n");
    return EXIT_SUCCESS;
  }
  else {
    fprintf(stderr, "one or more tests failed\n");
    return EXIT_FAILURE;
  }
}
