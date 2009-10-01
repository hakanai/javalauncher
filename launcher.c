

#include "config.h"
#include "javadll.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/**
 * Main function.
 *
 * The beautiful thing is we're compiling this using MinGW's version of GCC,
 * so even if we're compiling for windows without a console, we don't have to
 * define Microsoft's stupid WinMain function.
 */
int main(int argc, char *argv[])
{
	Configuration config;


	// Strip the executable ending off the first argument, to use
	// as a base for other filenames.
	char *p = argv[0];
	while (*p != '\0') p ++;
	if (*(p-1) == 'e' && *(p-2) == 'x' && *(p-3) == 'e' && *(p-4) == '.') p -= 4;
	if (*(p-1) == 'W') p --;
	*p = '\0';

#ifdef REDIRECT_OUTPUT

	// Redirect stdout to <launcher>.stdout
	char *stdoutName = (char *)malloc(strlen(argv[0]) + 8);
	strcpy(stdoutName, argv[0]);
	strcat(stdoutName, ".stdout");
	freopen(stdoutName, "w", stdout);

	// Redirect stderr to <launcher>.stderr
	char *stderrName = (char *)malloc(strlen(argv[0]) + 8);
	strcpy(stderrName, argv[0]);
	strcat(stderrName, ".stderr");
	freopen(stderrName, "w", stderr);

#endif

	// Read configuration from <launcher>.config
	char *configName = (char *)malloc(strlen(argv[0]) + 8);
	strcpy(configName, argv[0]);
	strcat(configName, ".config");
	ReadConfiguration(configName, &config);

	// Load and Launch the JVM.
	LoadJavaDLL(config.jvmpath);
	LaunchJavaVM(config.nvmoptions, config.vmoptions, config.mainclass, config.mainmethod, (argc-1), (argv+1));
	UnloadJavaDLL();
}

