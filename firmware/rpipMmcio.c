#include <stdio.h>
#include "mmcio.h"
#include "diskio.h"
#include "pico/stdlib.h"
#include "pico/sd_card.h"

extern void pico_led(int);

volatile BYTE CardType = CT_MMC;

// /*--------------------------------------------------------------------------

// Public Functions

// ---------------------------------------------------------------------------*/

DSTATUS mmc_status(void)
{
    if (CardType == 0)
    {
        return STA_NODISK;
    }
    else
    {
        // return STA_NOINIT;
        // return STA_NODISK;
        // return STA_PROTECTED;

        return 0;
    }
}

DSTATUS mmc_initialize(void)
{
    return CardType ? 0 : STA_NOINIT;
}

DRESULT mmc_readsector(BYTE *buff, DWORD lba)
{
    pico_led(1);
    int status = sd_readblocks_sync((uint32_t *)buff, lba, 1);
    pico_led(0);
    return RES_OK;
}

DRESULT mmc_writesector(BYTE *buff, DWORD sa)
{
    printf("mmc_writesector()\n");
    pico_led(1);
    sleep_ms(1000);
    pico_led(0);
}

