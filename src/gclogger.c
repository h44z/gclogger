/*
 * Copyright 2018 Christoph Haas
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the standard MIT license.  See LICENSE for more details.
 */

#include "gclogger.h"

extern char *optarg;
extern int optind;

static volatile bool running = true;

static int confighandler(void* config, const char* section, const char* name, const char* value) {
    configuration* pconfig = (configuration*)config;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("device", "port")) {
        pconfig->dev_port = strdup(value);
    } else if (MATCH("device", "baud")) {
        pconfig->dev_baud = atoi(value);
    } else if (MATCH("device", "location")) {
        pconfig->dev_location = strdup(value);
    } else if (MATCH("device", "latitude")) {
        pconfig->dev_latitude = atof(value);
    } else if (MATCH("device", "longitude")) {
        pconfig->dev_longitude = atof(value);
    } else if (MATCH("device", "interval")) {
        pconfig->dev_interval = atoi(value);
    } else if (MATCH("custlog", "url")) {
        pconfig->custlog_url = strdup(value);
    } else if (MATCH("custlog", "id")) {
        pconfig->custlog_id = strdup(value);
    } else if (MATCH("custlog", "param_id")) {
        pconfig->custlog_param_id = strdup(value);
    } else if (MATCH("custlog", "param_cpm")) {
        pconfig->custlog_param_cpm = strdup(value);
    } else if (MATCH("custlog", "param_temp")) {
        pconfig->custlog_param_temp = strdup(value);
    } else if (MATCH("custlog", "param_lng")) {
        pconfig->custlog_param_lng = strdup(value);
    } else if (MATCH("custlog", "param_lat")) {
        pconfig->custlog_param_lat = strdup(value);
    } else if (MATCH("custlog", "param_loc")) {
        pconfig->custlog_param_loc = strdup(value);
    } else if (MATCH("custlog", "param_version")) {
        pconfig->custlog_param_version = strdup(value);
    } else if (MATCH("custlog", "param_time")) {
        pconfig->custlog_param_time = strdup(value);
    } else if (MATCH("csv", "path")) {
        pconfig->csv_path = strdup(value);
    } else {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

bool str_isset(char *str) {
	return str != NULL && str[0] != '\0';
}

void init_configuration(configuration* config) {
	config->debug = false;

	config->dev_port = "";
	config->dev_baud = 115200;
	config->dev_location = "";
	config->dev_latitude = 0.0;
	config->dev_longitude = 0.0;
	config->dev_interval = 60;
	config->custlog_url = "";
	config->custlog_id = "";
	config->custlog_param_id = "id";
	config->custlog_param_cpm = "cpm";
	config->custlog_param_temp = "temp";
	config->custlog_param_lng = "lng";
	config->custlog_param_lat = "lat";
	config->custlog_param_loc = "loc";
	config->custlog_param_version = "version";
	config->custlog_param_time = "time";
	config->csv_path = "";
}

void signal_handler(int sig) {
	switch (sig) {
		case SIGTERM:
		case SIGINT:
		case SIGQUIT:
		case SIGHUP:
			running = false;
		default:
			break;
	}
}

bool send_custlog(const configuration config, int cpm, float temperature, const char *version, struct tm *tm) {
	CURL *curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();

	if(curl) {
		char *url_buffer;
		size_t url_size;
		// first get size of final url
		url_size = snprintf(NULL, 0, "%s?%s=%s&%s=%d&%s=%f&%s=%s&%s=%ld&%s=%f&%s=%f&%s=%s", 
			config.custlog_url,
			config.custlog_param_id, config.custlog_id,
			config.custlog_param_cpm, cpm,
			config.custlog_param_temp, temperature,
			config.custlog_param_version, version,
			config.custlog_param_time, mktime(tm),
			config.custlog_param_lng, config.dev_longitude,
			config.custlog_param_lat, config.dev_latitude,
			config.custlog_param_loc, config.dev_location);

		// now allocate buffer and build url
		url_buffer = (char *)malloc(url_size + 1);
		snprintf(url_buffer, url_size+1,"%s?%s=%s&%s=%d&%s=%f&%s=%s&%s=%ld&%s=%f&%s=%f&%s=%s", 
			config.custlog_url,
			config.custlog_param_id, config.custlog_id,
			config.custlog_param_cpm, cpm,
			config.custlog_param_temp, temperature,
			config.custlog_param_version, version,
			config.custlog_param_time, mktime(tm),
			config.custlog_param_lng, config.dev_longitude,
			config.custlog_param_lat, config.dev_latitude,
			config.custlog_param_loc, config.dev_location);

		printf("final url: %s\n", url_buffer);

		curl_easy_setopt(curl, CURLOPT_URL, url_buffer);

		#ifdef SKIP_PEER_VERIFICATION
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		#endif
	
		#ifdef SKIP_HOSTNAME_VERIFICATION
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		#endif
	
		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
			printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		else
			printf("curl_easy_perform() ok!");
	 
	 	free(url_buffer);
		curl_easy_cleanup(curl);

		return true;
	}

	return false;
}

bool send_tocsv(const configuration config, int cpm, float temperature, const char *version, const struct tm *tm) {
	FILE *fp;

	if ((fp = fopen(config.csv_path, "a")) == NULL) {
		printf("Error while opening the csv file '%s'.\n", config.csv_path);
        return false;
	}

	char *ts = asctime(tm);
	ts[strlen(ts) - 1] = 0; // get rid of \n
    fprintf(fp, "%s,%d,%07.3f,%s\n", ts, cpm, temperature, version);

	fclose(fp);

	return true;
}

void show_usage() {
	printf("Geiger Counter Logger\n");
	printf("Version %s\n", GCLOGGER_VERSION);
	printf("Copyright (C) 2018 Christoph Haas, christoph.h@sprinternet.at\n\n");
	printf("This program comes with ABSOLUTELY NO WARRANTY.\n");
	printf("This is free software, and you are welcome to redistribute it.\n\n");
	printf("Usage: gclogger -c <file> [-d]\n");
	printf("  -c <file>  configuration file path\n");
	printf("  -d         enable verbose mode\n\n");
}

int main(int argc, char *argv[]) {
	int opt = 0;
	int gc_fd = -1;
	configuration config;

	// setup signal handlers
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGHUP, signal_handler);

	// parse config file
	init_configuration(&config);
	while ((opt = getopt(argc, argv, "c:d")) != -1) {
		switch (opt) {
			case 'd':
				config.debug = true;
				break;

			case 'c':
				if (ini_parse(optarg, confighandler, &config) < 0) {
					printf("Can't load config file '%s'\n", optarg);
					return 1;
				}
				printf("Config loaded from '%s': device_port=%s, interval=%d\n",
					optarg, config.dev_port, config.dev_interval);
				break;

			default: /* '?' */
				show_usage();
            	exit(EXIT_FAILURE);
		}
	}

 	// additional check if extra/no arguments where given
	if (optind < argc || argc == 1) {
		show_usage();
		exit(EXIT_FAILURE);
	}

	// check if device port is set
	if(!str_isset(config.dev_port)) {
		printf("Device port must be set!'\n");
		exit(EXIT_FAILURE);
	}

	gc_fd = gmc_open(config.dev_port, config.dev_baud);

	// check if device connection was successful
	if(gc_fd == -1) {
		printf("Connection to device (%s) failed!'\n", config.dev_port);
		exit(EXIT_FAILURE);
	}

	// read version
	char version[15] = { 0 };
	if(gmc_get_version(gc_fd, version) == -1) {
		printf("Unable to read Geiger counter version!'\n");
		exit(EXIT_FAILURE);
	}

	if (config.debug) {
		printf("GC VERSION: %s\n", version);
	}

	time_t last = time(NULL);
	int cpm, sum = 0, count = 0, tcount = 0;
	float temperature, tsum = 0;

	// main loop
	while (running) {
		// read cpm
		if ((cpm = gmc_get_cpm(gc_fd)) > 0) {
			sum += cpm;
			count++;
		}

		// read temperature
		tsum += gmc_get_temperature(gc_fd);
		tcount++;

		if (difftime(time(NULL), last) >= config.dev_interval) {
			if (count > 0) {
				struct tm *tm = gmtime(&last);
				cpm = sum / count;
				temperature = tsum / tcount;

				if (config.debug) {
					printf("CPM: %d (= %d/%d), TEMP: %07.3f, Timestamp: %s\n", cpm, sum, count, temperature, asctime(tm));
				}

				// log to custom REST api
				if (str_isset(config.custlog_url)) {
					printf("Uploading to %s.\n", config.custlog_url);
					if(!send_custlog(config, cpm, temperature, version, tm)) {
						printf("Upload to %s failed.\n", config.custlog_url);
					}
				}

				// log to csv
				if (str_isset(config.csv_path)) {
					if(!send_tocsv(config, cpm, temperature, version, tm)) {
						printf("Logging to %s failed.\n", config.csv_path);
					}
				}

				time(&last);
				sum = tsum = count = tcount = 0;
			} else {
				printf("Reading ZERO value from Geiger tube.\n");
			}
		}

		sleep(1); // sleep one second
	}

	gmc_close(gc_fd);

	return EXIT_SUCCESS;
}
