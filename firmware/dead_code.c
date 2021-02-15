FRESULT scan_files(
    char *path /* Start node to be scanned (***also used as work area***) */
)
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;

    res = f_opendir(&dir, path); /* Open the directory */
    if (res == FR_OK)
    {
        for (;;)
        {
            res = f_readdir(&dir, &fno); /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0)
                break; /* Break on error or end of dir */
            if ((fno.fattrib & AM_DIR) && fno.fname[0] != '.')
            { /* It is a directory */
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
                res = scan_files(path); /* Enter the directory */
                if (res != FR_OK)
                    break;
                path[i] = 0;
            }
            else
            { /* It is a file. */
                printf("%s/%s\n", path, fno.fname);
            }
        }
    }

    return res;
}


int test_sd_card()
{

#define STREAMING

#ifdef STREAMING
#define BLOCK_COUNT 1
    int i = 0;
    static int block_base = 0;

    static uint32_t b[BLOCK_COUNT * 128];
    //    for(int div = 4; div >= 1; div--)
    for (int div = 1; div >= 1; div--)
    {
        uint8_t *buf = (uint8_t *)b;
        printf("-----------------------\n");
        printf("SPEED %uMB/s\n", 12 / div);
        sd_set_clock_divider(div);
        printf("1 bit no crc\n");
        sd_set_wide_bus(false);
        memset(buf, 0xaa, 512);
        sd_readblocks_sync(b, block_base, BLOCK_COUNT);
        for (int byte = 0; byte < 512; byte += 16)
        {
            printf("%08x ", i * 512 + byte);
            for (int j = 0; j < 16; j++)
                printf("%02x ", buf[byte + j]);
            for (int j = 0; j < 16; j++)
                putchar(isprint(buf[byte + j]) ? buf[byte + j] : '.');
            printf("\n");
        }
#if ENABLE_4_PIN
        memset(buf, 0xaa, 512);
        printf("4 bit no crc\n");
        sd_set_wide_bus(true);
        sd_readblocks_sync(b, block_base, BLOCK_COUNT);
        for (int byte = 0; byte < 512; byte += 16)
        {
            printf("%08x ", i * 512 + byte);
            for (int j = 0; j < 16; j++)
                printf("%02x ", buf[byte + j]);
            for (int j = 0; j < 16; j++)
                putchar(isprint(buf[byte + j]) ? buf[byte + j] : '.');
            printf("\n");
        }
#endif
        memset(buf, 0xaa, 512);
        printf("1 bit crc\n");
        sd_read_sectors_1bit_crc_async(b, block_base, BLOCK_COUNT);
        int status = 0;
        while (!sd_scatter_read_complete(&status))
            ;
        printf("Status: %d\n", status);
#endif
        for (i = 0; i < BLOCK_COUNT; i++)
        {
#ifndef STREAMING
            uint8_t *buf = sd_readblock(i);
#endif
            //if (i == BLOCK_COUNT-1)
            for (int byte = 0; byte < 512; byte += 16)
            {
                printf("%08x ", i * 512 + byte);
                for (int j = 0; j < 16; j++)
                    printf("%02x ", buf[byte + j]);
                for (int j = 0; j < 16; j++)
                    putchar(isprint(buf[byte + j]) ? buf[byte + j] : '.');
                printf("\n");
            }
            printf("\n");
#ifdef STREAMING /*  */
            buf += 512;
#endif
        }
    }
}



   FRESULT res;
    char buff[256];
    if (res == FR_OK)
    {
        strcpy(buff, "");
        //res = scan_files(buff);
    }
