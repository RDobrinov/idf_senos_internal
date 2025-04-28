/*
 * SPDX-FileCopyrightText: 2024 No Company name
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _IDF_SENOS_INTERNAL_H_
#define _IDF_SENOS_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

typedef struct {
    char *chip;
    char *type;
    char snid[10];
    struct {
        uint32_t model:8;
        uint32_t minor_chip_ver:8;
        uint32_t major_chip_ver:8;
        uint32_t cores:8;
    };
    struct {
        uint32_t features:16;
        uint32_t pkg_ver:16;
    };
    uint64_t uuid;
} full_chip_info_t;

void intr_get_full_chip_info(full_chip_info_t *fi);

#ifdef __cplusplus
}
#endif 
#endif /* _IDF_SENOS_INTERNAL_H_ */
