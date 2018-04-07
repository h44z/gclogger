/*
 * Copyright 2018 Christoph Haas
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the standard MIT license.  See LICENSE for more details.
 */

#ifndef _GCLOGGER_H_
#define _GCLOGGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <curl/curl.h>

#include "ini.h"
#include "gmc.h"

#define GCLOGGER_VERSION	"0.1"

typedef struct
{
	bool debug;
    char* dev_port;
    int dev_baud;
    char* dev_location;
    float dev_latitude;
	float dev_longitude;
	int dev_interval;

    char* radmon_user;
    char* radmon_pass;
    char* safecast_key;
    char* safecast_device;
    char* netc_id;
    char* custlog_url;
    char* custlog_type;
    char* custlog_id;
    char* custlog_param_id;
    char* custlog_param_cpm;
    char* custlog_param_temp;
    char* custlog_param_lng;
    char* custlog_param_lat;
    char* custlog_param_loc;
    char* custlog_param_version;
    char* custlog_param_time;
    char* csv_path;
} configuration;

static int confighandler(void* config, const char* section, const char* name, const char* value);
bool str_isset(char *str);
void init_configuration(configuration* config);
void signal_handler(int sig);
bool send_custlog(const configuration config, int cpm, float temperature, const char *version, struct tm *tm);
bool send_tocsv(const configuration config, int cpm, float temperature, const char *version, const struct tm *tm);
void show_usage();

#endif