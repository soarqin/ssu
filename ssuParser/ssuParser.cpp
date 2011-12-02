#include "Process.h"

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		fprintf(stderr, "Usage: ssuParser <input file> [<output file>]\n");
		return 0;
	}

	SSUStruct ssus;

	parse(argv[1], ssus);

	FILE * outputFile = stdout;

	if(argc > 2)
	{
		outputFile = fopen(argv[2], "wt");
		if(outputFile == NULL)
		{
			fprintf(stderr, "Error write to target file!\n");
			exit(0);
			return 0;
		}
	}

	process(outputFile, ssus);

	if(argc > 2)
		fclose(outputFile);

	return 0;
}
