//
//  sensirion.h
//  sps30
//
//  Created by Benjamin Rannow on 02.02.19.
//  Copyright © 2019 Fuyukai Rannow. All rights reserved.
//

#ifndef sensirion_h
#define sensirion_h

#include <stdint.h>

#define SENSIRION_BIG_ENDIAN 0

#define STATUS_OK 0
#define STATUS_FAIL (-1)

#ifndef NULL
#define NULL ((void *)0)
#endif

#if SENSIRION_BIG_ENDIAN
#define be16_to_cpu(s) (s)
#define be32_to_cpu(s) (s)
#define be64_to_cpu(s) (s)
#define SENSIRION_WORDS_TO_BYTES(a, w) ()

#else /* SENSIRION_BIG_ENDIAN */

#define be16_to_cpu(s) (((uint16_t)(s) << 8) | (0xff & ((uint16_t)(s)) >> 8))
#define be32_to_cpu(s) (((uint32_t)be16_to_cpu(s) << 16) | \
(0xffff & (be16_to_cpu((s) >> 16))))
#define be64_to_cpu(s) (((u64)be32_to_cpu(s) << 32) | \
(0xffffffff & ((u64)be32_to_cpu((s) >> 32))))
/**
 * Convert a word-array to a bytes-array, effectively reverting the
 * host-endianness to big-endian
 * @a:  word array to change (must be (uint16_t *) castable)
 * @w:  number of word-sized elements in the array (SENSIRION_NUM_WORDS(a)).
 */
#define SENSIRION_WORDS_TO_BYTES(a, w) \
for (uint16_t *__a = (uint16_t *)(a), __e = (w), __w = 0; __w < __e; ++__w) { \
__a[__w] = be16_to_cpu(__a[__w]); \
}
#endif /* SENSIRION_BIG_ENDIAN */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))
#endif

#define CRC8_POLYNOMIAL             0x31
#define CRC8_INIT                   0xFF
#define CRC8_LEN                    1

#define SENSIRION_COMMAND_SIZE      2
#define SENSIRION_WORD_SIZE         2
#define SENSIRION_NUM_WORDS(x) (sizeof(x) / SENSIRION_WORD_SIZE)
#define SENSIRION_MAX_BUFFER_WORDS  32

int8_t sensirion_init(const char *path, uint8_t address);

int16_t sensirion_read_cmd(uint16_t cmd, uint16_t *data_words, uint16_t num_words);

int16_t sensirion_read_delayed_cmd(uint16_t cmd, uint32_t delay_us, uint16_t *data_words, uint16_t num_words);

int16_t sensirion_write_cmd(uint16_t command);

int16_t sensirion_write_cmd_with_args(uint16_t command, const uint16_t *data_words, uint16_t num_words);

#endif /* sensirion_h */
