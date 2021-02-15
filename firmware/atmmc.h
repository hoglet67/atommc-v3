#ifndef __MMC2_H
#define __MMC2_H

#define VSN_MAJ 3
#define VSN_MIN 0

#define SECBUFFSIZE 512
#define GLOBUFFSIZE 256

#define EE_SYSFLAGS 0xff
#define EE_PORTBVALU 0xfe
#define EE_PORTBTRIS 0xfe

extern unsigned char configByte;
extern unsigned char CardType;
extern unsigned char blVersion;
extern unsigned char portBVal;

extern unsigned char globalData[];

extern void WriteEEPROM(unsigned char address, unsigned char val);
extern unsigned char ReadEEPROM(unsigned char address);

extern void at_initprocessor(void);
extern void at_process(void);


#define MODEREAD 1
#define MODEWRITE 2


typedef void (*WORKERFN)(void);

#define WFUNC(x)  extern void wfn##x(void); const WORKERFN WFN_##x = wfn##x;



#define SDOS_SECTOR_SIZE	256

typedef struct
{
   char filename[13];
   unsigned char attribs;
}
imgInfo;

extern unsigned char *sectorData;


#endif // __MMC2_H
