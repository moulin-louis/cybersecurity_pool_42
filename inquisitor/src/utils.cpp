#include "../inc/inquisitor.h"

__attribute__ ((noreturn)) void error(const char *func_error, const char *error_msg, const char *file, int line, const char *func_caller) {
  char errno_tmp = errno;
  dprintf(2, RED "ERROR: %s error: %s, caller: %s, file: %s, line: %d" RESET, func_error,
          error_msg ? error_msg : strerror(errno), func_caller, file, line);
  exit(errno_tmp);
}

__attribute__((unused)) void hexdump(void *data, size_t len, int32_t row) {
  if (row == 0) {
    for (size_t i = 0; i < len; i++)
      dprintf(1, "%02x ", ((uint8_t *) data)[i]);
    dprintf(1, "\n");
    return;
  }
  for (size_t i = 0; i < len; i += row) {
    for (size_t j = i; j < i + row; j++) {
      if (j == len)
        break;
      dprintf(1, "%02x ", ((uint8_t *) data)[j]);
    }
    dprintf(1, "\n");
  }
  dprintf(1, "\n");
}

void mac_str_to_hex(int8_t *mac_addr, uint8_t *dest) {
  int ret = sscanf((char *) mac_addr, "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx", &dest[0], &dest[1], &dest[2], &dest[3],
                   &dest[4], &dest[5]);
  if (ret != 6)
    error("mac_str_to_hex", "failed parsing mac address to byte array", __FILE__, __LINE__, __func__);
}