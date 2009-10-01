
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CONFIG_LINE_LENGTH 1024

#include "config.h"


// Typedef for config function handlers
//typedef void (*pfnConfig)(char *arg);



void ReadConfiguration(char *configName, Configuration *config)
{
	memset(config, 0, sizeof(Configuration));

	// Open the configuration file.
	FILE *configFile = fopen(configName, "r");
	if (configFile == NULL) {
		fprintf(stderr, "Can't find configuration file: %s\n", configName);
		exit(-1);
	}

	// Parse each line of the file.
	char buffer[MAX_CONFIG_LINE_LENGTH+1];
	int len;
	while (fgets(buffer, MAX_CONFIG_LINE_LENGTH, configFile)) {
		len = strlen(buffer);

		// Trim off trailing \r\n
		while (buffer[len-1] == '\n' || buffer[len-1] == '\r')
			buffer[--len] = '\0';

		if (strncmp(buffer, "jvmpath ", 8) == 0) {
			config->jvmpath = (char *)malloc(strlen(buffer+8));
			strcpy(config->jvmpath, buffer+8);
			continue;
		}
		if (strncmp(buffer, "mainclass ", 10) == 0) {
			config->mainclass = (char *)malloc(strlen(buffer+10));
			strcpy(config->mainclass, buffer+10);
			continue;
		}
		if (strncmp(buffer, "mainmethod ", 11) == 0) {
			config->mainmethod = (char *)malloc(strlen(buffer+11));
			strcpy(config->mainmethod, buffer+11);
			continue;
		}
		if (strncmp(buffer, "vmoption ", 9) == 0) {
			config->nvmoptions++;
			config->vmoptions = (char **)realloc(config->vmoptions, config->nvmoptions);
			config->vmoptions[config->nvmoptions-1] = (char *)malloc(strlen(buffer+9));
			strcpy(config->vmoptions[config->nvmoptions-1], buffer+9);
			continue;
		}
	}

}
