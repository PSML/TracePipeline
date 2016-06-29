#ifndef __TRACE_FORMAT_H__
#define __TRACE_FORMAT_H__

#include <stdint.h>

#define PSML_DATASIZE (sizeof(psml_address_t) + PSML_VALSIZE)

enum { T_INSTRUCTION=0,
       T_MEM_RD=1, T_MEM_WR=2, T_REG_RD=3, T_REG_WR=4,
       T_INTERRUPT=5 };

typedef union {
  uint8_t action_raw[1 + PSML_DATASIZE];
  struct __attribute__((__packed__)) {
    union {
      uint8_t hdr_raw;
      struct __attribute__((__packed__)) {
	uint8_t id:3;
	uint8_t len:4;
	uint8_t ext:1;
      } hdr;
    };
    union {
      uint8_t data[PSML_DATASIZE];
      struct __attribute__((__packed__)) {
	psml_address_t addr;
	uint8_t        val[PSML_VALSIZE];
      };
    };
  };
} action_t;

#define ACTIONSIZE sizeof(action_t)

#endif
