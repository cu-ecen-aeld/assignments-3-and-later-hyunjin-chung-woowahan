#include <stdio.h>
#include <syslog.h>

#define VALID_NUM_ARGS (2 + 1)

int main(int argc, char* argv[])
{
	openlog(NULL, 0, LOG_USER);
	
	if (argc != VALID_NUM_ARGS)
	{
		syslog(LOG_ERR, "Total number of arguments should be 2");
		closelog();
		return 1;
	}

	const char* writeFile = argv[1];
	const char* writeStr = argv[2];

	FILE* file;

	if (!(file = fopen(writeFile, "w+")))
	{
		syslog(LOG_ERR, "file open is failed");
		closelog();
		return 1;
	}

	if (fprintf(file, "%s", writeStr) < 0)
	{
		syslog(LOG_ERR, "Writting \"%s\" string to $writeFile is failed", writeStr);
		closelog();
		return 1;
	}
	
	syslog(LOG_DEBUG, "Writing %s to %s", writeStr, writeFile);

	if (fclose(file))
	{
		syslog(LOG_ERR, "file close is failed" );
		closelog();
		return 1;
	}

	closelog();

	return 0;
}
