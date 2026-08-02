/****************************************************************************
*
* demo program for HASP HL licensing services
*
*
* Copyright (c) Aladdin Knowledge Systems Ltd.
*
*
* HASP HL DEMOMA key required with program number 1 and 42 enabled
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hasp_hl.h"
#include "hasp_vcode.h"       /* contains HASP HL DEMOMA vendor code */

unsigned char data[] = {
  0x74, 0x65, 0x73, 0x74, 0x20, 0x73, 0x74, 0x72,
  0x69, 0x6E, 0x67, 0x20, 0x31, 0x32, 0x33, 0x34
};

hasp_size_t datalen = sizeof(data);

#define DEMO_MEMBUFFER_SIZE   64

unsigned char membuffer[DEMO_MEMBUFFER_SIZE];

/****************************************************************************
 * helper function: dumps a given block of data, in hex
 */

#define DUMP_COLS_PER_ROW     16
#define DUMP_SPACE_COLS        8

void dump_hex(unsigned char *data, unsigned int datalen, char *margin)
{
  unsigned int i;

  for (i = 0; i < datalen; i++) {
    if (((i % DUMP_SPACE_COLS) == 0) && (i != 0)) printf(" ");

    if ((i % DUMP_COLS_PER_ROW) == 0) {
      if (i != 0) {
        printf("\n");
      }

      if (margin != NULL) printf("%s", margin);
    }

    /* dump character in hex */

    printf("%02X ", data[i]);
  }
  return;

} /* dump_hex */

/****************************************************************************
 * helper function: dumps a given block of data, in ascii
 */

void dump_ascii(unsigned char *data, unsigned int datalen, char *margin)
{
  unsigned int i;

  for (i = 0; i < datalen; i++) {
    if (((i % DUMP_SPACE_COLS) == 0) && (i != 0)) printf(" ");

    if ((i % DUMP_COLS_PER_ROW) == 0) {
      if (i != 0) {
        printf("\n");
      }

      if (margin != NULL) printf("%s", margin);
    }

    /* dump printable character in ascii */

    printf("%c", ((data[i] > 31) && (data[i] < 128)) ? data[i] : '.');
  }
  return;

} /* dump_ascii */

/****************************************************************************
 * helper function: dumps a given block of data, in hex and ascii
 */

void dump(unsigned char *data, unsigned int datalen, char *margin)
{
  unsigned int i, icols;

  for (i = 0; i < datalen; ) {
    icols = datalen - i;

    if (icols > DUMP_COLS_PER_ROW) icols = DUMP_COLS_PER_ROW;

    dump_hex(&data[i], icols, margin);
    dump_ascii(&data[i], icols, "  ");
    printf("\n");

    i += icols;
  }
  return;

} /* dump */

/***************************************************************************/

int main(void)
{
  hasp_status_t   status;
  hasp_handle_t   handle;
  hasp_time_t     time;
  unsigned int    day, month, year, hour, minute, second;

  char           *info;         /* pointer to key info data */
  hasp_size_t     fsize;
  unsigned int    i;

  printf("\nThis is a simple demo program for the HASP HL licensing functions\n");
  printf("Copyright (c) Aladdin Knowledge Systems Ltd.\n\n");

/****************************************************************************
 * hasp_login
 *   establishes a context for HASP HL services
 */

  printf("login to default feature         : ");

  /* login program number 0 */
  /* this default feature is available on any key */
  /* search for local and remote HASP HL key */

  status = hasp_login(HASP_PROGNUM_DEFAULT_FID,
                      (hasp_vendor_code_t *)vendor_code,
                      &handle);

  switch (status) {
    case HASP_STATUS_OK:
      printf("OK\n");
      hasp_logout(handle);
      break;

    case HASP_FEATURE_NOT_FOUND:
      printf("login to default feature failed\n");
      break;

    case HASP_CONTAINER_NOT_FOUND:
      printf("no HASP HL DEMOMA key/license container found\n");
      break;

    case HASP_OLD_DRIVER:
      printf("outdated driver version installed\n");
      break;

    case HASP_NO_DRIVER:
      printf("HASP HL driver not installed\n");
      break;

    case HASP_INV_VCODE:
      printf("invalid vendor code\n");
      break;

    default:
      printf("login to default feature failed\n");
  }

/***************************************************************************/

  printf("login to program number 42       : ");

  /* search for local and remote HASP HL key */

  status = hasp_login(42 | HASP_PROGNUM_FEATURETYPE,
                      (hasp_vendor_code_t *)vendor_code,
                      &handle);

  switch (status) {
    case HASP_STATUS_OK:
      printf("OK\n");
      hasp_logout(handle);
      break;

    case HASP_FEATURE_NOT_FOUND:
      printf("no HASP HL DEMOMA key/license container found with feature 42 enabled\n");
      break;

    case HASP_CONTAINER_NOT_FOUND:
      printf("key/license container not available\n");
      break;

    case HASP_OLD_DRIVER:
      printf("outdated driver version installed\n");
      break;

    case HASP_NO_DRIVER:
      printf("HASP HL driver not installed\n");
      break;

    case HASP_INV_VCODE:
      printf("invalid vendor code\n");
      break;

    default:
      printf("login program number 42 failed\n");
  }

/***************************************************************************/

  printf("login to program number 1        : ");

  /* search for local and remote HASP HL key */

  status = hasp_login(1 | HASP_PROGNUM_FEATURETYPE,
                      (hasp_vendor_code_t *)vendor_code,
                      &handle);

  switch (status) {
    case HASP_STATUS_OK:
      printf("OK\n");
      break;

    case HASP_FEATURE_NOT_FOUND:
      printf("no HASP HL DEMOMA key/license container found with feature 1 enabled\n");
      break;

    case HASP_CONTAINER_NOT_FOUND:
      printf("key/license container not available\n");
      break;

    case HASP_OLD_DRIVER:
      printf("outdated driver version installed\n");
      break;

    case HASP_NO_DRIVER:
      printf("HASP HL driver not installed\n");
      break;

    case HASP_INV_VCODE:
      printf("invalid vendor code\n");
      break;

    default:
      printf("login program number 1 failed\n");
  }

/****************************************************************************
 * hasp_get_sessioninfo        HASP_KEYINFO
 *   retrieve HASP HL key attributes
 */

  printf("\nget session info                 : ");

  status = hasp_get_sessioninfo(handle,
                                HASP_KEYINFO,
                                &info);

  switch (status) {
    case HASP_STATUS_OK:
      printf("OK, HASP HL key attributes retrieved\n\n"
             "Key info:\n===============\n%s===============\n", info);
      hasp_free(info);
      break;

    case HASP_INV_HND:
      printf("handle not active\n");
      break;

    case HASP_INV_FORMAT:
      printf("unrecognized format\n");
      break;

    case HASP_CONTAINER_NOT_FOUND:
      printf("key/license container not available\n");
      break;

    default:
      printf("hasp_get_sessioninfo failed\n");
  }

/****************************************************************************
 * hasp_get_size
 *   retrieve the memory size of the HASP HL key
 */

  printf("\nretrieving the key's memory size : ");

  status = hasp_get_size(handle,
                         HASP_FILEID_MAIN,
                         &fsize);

  switch (status) {
    case HASP_STATUS_OK:
      printf("HASP HL memory size is %d bytes\n", fsize);
      break;

    case HASP_INV_HND:
      printf("handle not active\n");
      break;

    case HASP_INV_FILEID:
      printf("invalid file id\n");
      break;

    case HASP_CONTAINER_NOT_FOUND:
      printf("key/license container not available\n");
      break;

    default:
      printf("could not retrieve memory size\n");
  }

  if (fsize != 0)       /* skip memory access if no memory available */
  {

/****************************************************************************
 * hasp_read
 *   read from HASP HL memory
 */

    /* limit memory size to be used in this demo program */

    if (fsize > DEMO_MEMBUFFER_SIZE) fsize = DEMO_MEMBUFFER_SIZE;

    printf("\nreading %4d bytes from memory   : ", fsize);

    status = hasp_read(handle,
                       HASP_FILEID_MAIN,
                       0,                 /* offset */
                       fsize,             /* length */
                       &membuffer);

    switch (status) {
      case HASP_STATUS_OK:
        printf("OK\n");
        dump(membuffer, fsize, "    ");
        break;

      case HASP_INV_HND:
        printf("handle not active\n");
        break;

      case HASP_INV_FILEID:
        printf("invalid file id\n");
        break;

      case HASP_MEM_RANGE:
        printf("beyond memory range of attached HASP HL key\n");
        break;

      case HASP_CONTAINER_NOT_FOUND:
        printf("key/license container not available\n");
        break;

      default:
        printf("read memory failed\n");
    }

/****************************************************************************
 * hasp_write
 *   write to HASP HL memory
 */

    printf("\nincrementing every byte in memory buffer\n");
    for (i = 0; i < fsize; i++) membuffer[i]++;

    printf("\nwriting %4d bytes to memory     : ", fsize);

    status = hasp_write(handle,
                        HASP_FILEID_MAIN,
                        0,                /* offset */
                        fsize,            /* length */
                        &membuffer);

    switch (status) {
      case HASP_STATUS_OK:
        printf("OK\n");
        break;

      case HASP_INV_HND:
        printf("handle not active\n");
        break;

      case HASP_INV_FILEID:
        printf("invalid file id\n");
        break;

      case HASP_MEM_RANGE:
        printf("beyond memory range of attached HASP HL key\n");
        break;

      case HASP_CONTAINER_NOT_FOUND:
        printf("key/license container not available\n");
        break;

      default:
        printf("write memory failed\n");
    }

/****************************************************************************
 * hasp_read
 *   read from HASP HL memory
 */

    printf("\nreading %4d bytes from memory   : ", fsize);

    status = hasp_read(handle,
                       HASP_FILEID_MAIN,
                       0,                 /* offset */
                       fsize,             /* length */
                       &membuffer);

    switch (status) {
      case HASP_STATUS_OK:
        printf("OK\n");
        dump(membuffer, fsize, "    ");
        break;

      case HASP_INV_HND:
        printf("handle not active\n");
        break;

      case HASP_INV_FILEID:
        printf("invalid file id\n");
        break;

      case HASP_MEM_RANGE:
        printf("beyond memory range of attached HASP HL key\n");
        break;

      case HASP_CONTAINER_NOT_FOUND:
        printf("key/license container not available\n");
        break;

      default:
        printf("read memory failed\n");
    }

  } /* end of memory demo */

/****************************************************************************
 * hasp_encrypt
 *   encrypts a block of data using the HASP HL key
 *   (minimum buffer size is 16 bytes)
 */

  printf("\nencrypting a data buffer:\n");
  dump(data, datalen, "    ");

  status = hasp_encrypt(handle,
                        data,
                        datalen);

  switch (status) {
    case HASP_STATUS_OK:
      printf("encryption ok:\n");
      dump(data, datalen, "    ");
      break;

    case HASP_INV_HND:
      printf("handle not active\n");
      break;

    case HASP_TOO_SHORT:
      printf("data length too short\n");
      break;

    case HASP_ENC_NOT_SUPP:
      printf("attached key does not support AES encryption\n");
      break;

    case HASP_CONTAINER_NOT_FOUND:
      printf("key/license container not available\n");
      break;

    default:
      printf("encryption failed\n");
  }

/****************************************************************************
 * hasp_decrypt
 *   decrypts a block of data using the HASP HL key
 *   (minimum buffer size is 16 bytes)
 */

  status = hasp_decrypt(handle, data, datalen);

  switch (status) {
    case HASP_STATUS_OK:
      printf("decryption ok:\n");
      dump(data, datalen, "    ");
      break;

    case HASP_INV_HND:
      printf("handle not active\n");
      break;

    case HASP_TOO_SHORT:
      printf("data length too short\n");
      break;

    case HASP_ENC_NOT_SUPP:
      printf("attached key does not support AES encryption\n");
      break;

     default:
      printf("decryption failed\n");
  }

/****************************************************************************
 * hasp_get_rtc
 *   read current time from HASP HL Time key
 */

  printf("\nreading current time and date    : ");

  status = hasp_get_rtc(handle, &time);

  switch (status) {
    case HASP_STATUS_OK:
      /* convert HASP HL time value to time and date */
      status = hasp_hasptime_to_datetime(time, &day, &month, &year,
                                         &hour, &minute, &second);

      switch (status) {
        case HASP_STATUS_OK:
          printf("HASP HL time: %d:%d:%d H/M/S\n                  "
                 "                 HASP HL date: %d/%d/%d D/M/Y\n",
                 hour, minute, second, day, month, year);
          break;

        case HASP_INV_TIME:
          printf("time value outside supported range\n");
          break;

        default:
          printf("time conversion failed\n");
      }
      break;

    case HASP_INV_HND:
      printf("handle not active\n");
      break;

    case HASP_NO_TIME:
      printf("no HASP HL Time connected\n");
      break;

    case HASP_CONTAINER_NOT_FOUND:
      printf("key/license container not available\n");
      break;

    default:
      printf("could not read time from HASP HL key\n");
  }

/****************************************************************************
 * hasp_logout
 *   closes established session and releases allocated memory
 */

  printf("\nlogout from program number 1     : ");

  status = hasp_logout(handle);

  switch (status) {
    case HASP_STATUS_OK:
      printf("OK\n");
      break;

    case HASP_INV_HND:
      printf("failed: handle not active\n");
      break;

    default:
      printf("failed\n");
  }

/***************************************************************************/

  printf("\npress ENTER to close the sample\n");

  fflush(stdout);
  fflush(stdin);

  while (getchar() == EOF);

  return 0;

} /* main */

