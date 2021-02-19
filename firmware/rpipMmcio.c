#include <stdio.h>
#include "mmcio.h"
#include "diskio.h"
#include "pico/stdlib.h"
#include "pico/sd_card.h"

extern void pico_led(int);

volatile BYTE CardType = CT_SD1;

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
    pico_led(1);
    printf("mmc_writesector %d\n", sa);
    // TO BE DONE!!!
    int status = sd_writeblocks_async((uint32_t *)buff, sa, 1);
    int rc;
    static int timeout = 10;
    while (!sd_write_complete(&rc)) {
        printf("Waiting for completion\n");
        if (!--timeout) break;
    }
    printf("Done %d!\n", rc);
    sleep_ms(1);
    pico_led(0);
    return RES_OK;
}

