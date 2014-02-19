#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <prussdrv.h>
#include <pruss_intc_mapping.h>

#include "util.h"
#include "config.h"

unsigned char *wavetable()
{
  unsigned char *pruDataMem = (unsigned char *)0;

  //Initialize pointer to PRU data memory
#if PRUNUM == 0
    prussdrv_map_prumem (PRUSS0_PRU0_DATARAM, (void**)&pruDataMem);
#elif PRUNUM == 1
    prussdrv_map_prumem (PRUSS0_PRU1_DATARAM, (void**)&pruDataMem);
#endif

  return pruDataMem;
}

int setskip (double skiplenf)
{
  int mem_fd;
  void *ddrMem;
  void *DDR_regaddr1;
  int skiplen;

  /* open the device */
  mem_fd = open("/dev/mem", O_RDWR);
  if (mem_fd < 0) {
    return -1;
    }

  /* map the DDR memory */
  ddrMem = mmap(0, 0x0FFFFFFF, PROT_WRITE | PROT_READ, MAP_SHARED, mem_fd, DDR_BASEADDR);
  if (ddrMem == NULL) {
    close(mem_fd);
    return -1;
    }

  /* Store skiplen in DDR memory location */
  DDR_regaddr1 = ddrMem + OFFSET_DDR;

  if (skiplenf > (TABLELEN/2)) {
    skiplenf = *(unsigned long*) DDR_regaddr1;
    skiplenf /= 524288;
    close(mem_fd);
    return skiplenf;
    }

  skiplenf *= 524288;  // <<=19
  skiplen = (int)skiplenf;

  *(unsigned long*) DDR_regaddr1 = skiplen;

  return(0);
}

