/*
 * SPDX-FileCopyrightText: 2024 No Company name
 * SPDX-FileCopyrightText: For Internal use only
 *
 * SPDX-License-Identifier: Apache-2.0
*/

#include <inttypes.h>
#include "esp_chip_info.h"
#include "esp_mac.h"
#include "esp_system.h"

#include "idf_senos_internal.h"
#include "hal/efuse_ll.h"
#include "hal/efuse_hal.h"
//#include "efuse.h"
#include "esp_efuse_table.h"
/** */

#include "xxhash.h"

const char ccSNGen[] = "AB1CD9EF2GH8JK3MN7PQ4RS6TV5WX0YZ";
const char ccModelGen[] = "XABXXCXXXDXXEFXXGXJXKXXKXXXXXXXX";

typedef struct {
    uint8_t mac[6];
    uint8_t rev[2];
} xxhash_seed;

void intr_get_full_chip_info(full_chip_info_t *fi) {
    esp_chip_info_t chip;
    xxhash_seed seed;
    memset(fi, 0, sizeof(full_chip_info_t));
    fi->chip = "ESP32";
    esp_chip_info(&chip);
    fi->features = chip.features;
    fi->model = chip.model > 255 ? 255 : chip.model;
    fi->cores = chip.cores;
    fi->major_chip_ver = efuse_hal_get_major_chip_version();
    fi->minor_chip_ver = efuse_hal_get_minor_chip_version();
    *((uint16_t *)seed.rev) = chip.revision;
    esp_read_mac(seed.mac, ESP_MAC_BASE);

    #if defined CONFIG_IDF_TARGET_ESP32C6 || defined CONFIG_IDF_TARGET_ESP32S3
        uint8_t efuse_uuid[16];
        esp_efuse_read_field_blob(ESP_EFUSE_OPTIONAL_UNIQUE_ID, efuse_uuid, sizeof(efuse_uuid) * 8);
        fi->uuid = XXH64(efuse_uuid, sizeof(efuse_uuid), *((uint64_t *)&seed));
    #elif 
        fi->uuid = XXH64(&chip, sizeof(chip), *((uint64_t *)&seed));
    #endif

    fi->snid[0] = ccModelGen[fi->model & 0x1F];
    for(int i = 0; i < sizeof(fi->uuid); i++) {
        fi->snid[i+1] = ccSNGen[((char *)&fi->uuid)[i] & 0x1F];
    }
    #if defined CONFIG_IDF_TARGET_ESP32
        fi->pkg_ver = efuse_ll_get_chip_ver_pkg();
        switch(fi->pkg_ver) {
            case EFUSE_RD_CHIP_VER_PKG_ESP32D0WDQ6:
                fi->type = "D0WDQ6";
            break;
            case EFUSE_RD_CHIP_VER_PKG_ESP32D0WDQ5:
                fi->type = "D0WDQ5";
            break;
            case EFUSE_RD_CHIP_VER_PKG_ESP32D2WDQ5:
                fi->type = "D2WDQ5";
            break;
            case EFUSE_RD_CHIP_VER_PKG_ESP32U4WDH:
                fi->type = "U4WDH";
            break;
            case EFUSE_RD_CHIP_VER_PKG_ESP32PICOD4:
                fi->type = "PICOD4";
            break;
            case EFUSE_RD_CHIP_VER_PKG_ESP32PICOV302:
                fi->type = "PICOV302";
            break;
            case EFUSE_RD_CHIP_VER_PKG_ESP32D0WDR2V3:
                fi->type = "D0WDR2V3";
            break;
            default:
                fi->type = "UNK";
        }
    #elif defined CONFIG_IDF_TARGET_ESP32C2
        fi->type = "C2";
    #elif defined CONFIG_IDF_TARGET_ESP32C3
        switch(fi->pkg_ver) {
            case 0:
                //fi->type = "C3 QFN32";
                fi->type = "C3";
            break;
            case 1:
                //fi->type = "8685 QFN28";
                fi->chip = "ESP";
                fi->type = "8685";
            break;
                //fi->type = "C3AZ QFN32";
                fi->type = "C3AZ";
            case 3:
                //fi->type = "8685 QFN24";
                fi->chip = "ESP";
                fi->type = "8686";
            break;
            default:
                fi->type = "UNK C3";
        }
    #elif defined CONFIG_IDF_TARGET_ESP32C5
        fi->type = "C5";
    #elif defined CONFIG_IDF_TARGET_ESP32C6
        switch(fi->pkg_ver) {
            case 0:
                //fi->type = "C6 QFN40";
                fi->type = "C6";
            break;
            case 1:
                //fi->type = "C6FH4 QFN32";
                fi->type = "C6FH4";
            break;
            default:
                fi->type = "UNK C6";
        }
    #elif defined CONFIG_IDF_TARGET_ESP32C61
        fi->type = "C61";
    #elif defined CONFIG_IDF_TARGET_ESP32H2
        fi->type = "H2";
    #elif defined CONFIG_IDF_TARGET_ESP32P4
        fi->type = "P4";
    #elif defined CONFIG_IDF_TARGET_ESP32S2
        switch(fi->pkg_ver) {
            case 0:
                fi->type = "S2";
            break;
            case 1:
                fi->type = "S2FH2";
            break;
            case 1:
                fi->type = "S2FH4";
            break;
            default:
                fi->type = "UNK S2";
        }
    #elif defined CONFIG_IDF_TARGET_ESP32S3
    fi->chip = "ESP32";
    switch(fi->pkg_ver) {
        case 0:
            //fi->type = "S3 QFN40";
            fi->type = "S3";
        break;
        case 1:
            //fi->type = "S3PICO1 LGA56";
            fi->type = "S3PICO1";
        break;
        default:
            fi->type = "UNK S3";
    }
    #elif
        fi->type = "UNK";
    #endif
}
