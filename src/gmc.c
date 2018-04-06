/*
 * Copyright 2018 Christoph Haas
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the standard MIT license.  See LICENSE for more details.
 */

#include "gmc.h"

int gmc_open(const char *device, int baud) {
	int gc_fd = -1;
	struct termios tio;

	memset(&tio, 0, sizeof(struct termios));
	tio.c_cflag = CS8 | CREAD | CLOCAL;		// 8n1
	tio.c_cc[VMIN] = 0;
	tio.c_cc[VTIME] = 5;

	int tio_baud = B115200;
	switch(baud) {
		case 9600: tio_baud = B9600; break;
		case 19200: tio_baud = B19200; break;
		case 38400: tio_baud = B38400; break;
		case 57600: tio_baud = B57600; break;
		case 115200: tio_baud = B115200; break;
	}

	if ((gc_fd = open(device, O_RDWR)) != -1) {
		if (cfsetspeed(&tio, tio_baud) == 0) { // set baud speed
			if (tcsetattr(gc_fd, TCSANOW, &tio) == 0) { // apply baud speed change
				if (gmc_set_heartbeat_off(gc_fd)) { // disable heartbeat, we use polling
					return gc_fd;
				}
			}
		} else {
			// something failed
			close(gc_fd);
			gc_fd = -1;
		}
	}

	return gc_fd;
}

void gmc_close(int device) {
	close(device);
}

int gmc_get_cpm(int device) {
	const char cmd[] = "<GETCPM>>";
	char buf[2] = { 0 };

	if (gmc_write(device, cmd) == (ssize_t) strlen(cmd))
		gmc_read(device, buf, 2);

	return buf[0] * 256 + buf[1];
}

float gmc_get_temperature(int device) {
	const char cmd[] = "<GETTEMP>>";
	char buf[4] = { 0 };

	if (gmc_write(device, cmd) == (ssize_t) strlen(cmd))
		gmc_read(device, buf, 4);

	int sign =  buf[2] == 0 ? 1 : -1;
	float temp = buf[0];
	temp += (float) buf[1] / 10;
	temp = temp * sign;
	return temp;
}

int gmc_get_version(int device, char *version) {
	const char cmd[] = "<GETVER>>";

	if (gmc_write(device, cmd) == (ssize_t) strlen(cmd))
		return gmc_read(device, version, 14);

	return -1;
}

bool gmc_set_heartbeat_off(int device) {
	const char cmd[] = "<HEARTBEAT0>>";

	if (gmc_write(device, cmd) == (ssize_t) strlen(cmd))
		return gmc_flush(device);

	return false;
}

int gmc_write(int device, const char *cmd) {
	return write(device, cmd, strlen(cmd));
}

int gmc_read(int device, char *buf, int length) {
	return read(device, buf, length);
}

bool gmc_flush(int device) {
	char ch;

	// flush input stream (max 100 bytes)
	for (int i = 0; i < 100; i++) {
		if (read(device, &ch, 1) == 0)
			return true;
	}

	return false;
}
