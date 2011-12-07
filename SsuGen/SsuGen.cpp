#include "Process.h"
#include <cstdlib>

int main(int argc, char *argv[])
{
	if(argc < 4)
	{
		fprintf(stderr, "Usage: ssuParser <input file> <cpp source file name> <header file name>\n");
		return 0;
	}

	SSUStruct ssus;

	parse(argv[1], ssus);

	FILE * outputFileC = fopen(argv[2], "wt");
	if(outputFileC == NULL)
	{
		fprintf(stderr, "Error write to target file!\n");
		exit(0);
		return 0;
	}

	FILE * outputFileH = fopen(argv[3], "wt");
	if(outputFileH == NULL)
	{
		fprintf(stderr, "Error write to target file!\n");
		exit(0);
		return 0;
	}

	char hprotect[4096] = "_SSU_";
	strcpy(hprotect + 5, argv[3]);
	size_t len = strlen(hprotect);
	for(size_t i = 5; i < len; ++ i)
	{
		if(hprotect[i] >= 'a' && hprotect[i] <= 'z')
			hprotect[i] -= 32;
		else if(!((hprotect[i] >= '0' && hprotect[i] <= '9') || (hprotect[i] >= 'A' && hprotect[i] <= 'Z')))
			hprotect[i] = '_';
	}
	strcat(hprotect, "_");

	fprintf(outputFileC, "#include \"%s\"\n\n", argv[3]);
	fprintf(outputFileH, "#ifndef %s\n", hprotect);
	fprintf(outputFileH, "#define %s\n\n", hprotect);
	process(outputFileC, outputFileH, ssus);
	fprintf(outputFileH, "#endif // %s\n", hprotect);

	fclose(outputFileH);
	fclose(outputFileC);

	return 0;
}
