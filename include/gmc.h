#ifndef _GC_GMC_H_
#define _GC_GMC_H_

#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>

// GQ Geiger Counter Communication Protocol: http://www.gqelectronicsllc.com/download/GQ-RFC1201.txt

int gmc_open(const char *device, int baud);
void gmc_close(int device);

int gmc_get_cpm(int device);
float gmc_get_temperature(int device);
int gmc_get_version(int device, char *buf);

bool gmc_set_heartbeat_off(int device);

int gmc_write(int device, const char *cmd);
int gmc_read(int device, char *buf, int length);
bool gmc_flush(int device);

#endif
