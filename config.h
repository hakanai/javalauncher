
typedef struct {
	char *jvmpath;
	char *mainmethod;
	char *mainclass;
	int nvmoptions;
	char **vmoptions;
} Configuration;


void ReadConfiguration(char *configName, Configuration *config);
