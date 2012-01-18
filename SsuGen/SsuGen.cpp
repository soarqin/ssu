/*
 * Copyright (c) 2011, Soar Qin<soarchin@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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

	int pos = (int)strlen(argv[3]) - 1;
	while(pos >= 0 && argv[3][pos] != '/' && argv[3][pos] != '\\') -- pos;
	++ pos;

	char hprotect[4096] = "_SSU_";
	strcpy(hprotect + 5, argv[3] + pos);
	size_t len = strlen(hprotect);
	for(size_t i = 5; i < len; ++ i)
	{
		if(hprotect[i] >= 'a' && hprotect[i] <= 'z')
			hprotect[i] -= 32;
		else if(!((hprotect[i] >= '0' && hprotect[i] <= '9') || (hprotect[i] >= 'A' && hprotect[i] <= 'Z')))
			hprotect[i] = '_';
	}
	strcat(hprotect, "_");

	fprintf(outputFileC, "#include \"%s\"\n\n", argv[3] + pos);
	fprintf(outputFileH, "#ifndef %s\n", hprotect);
	fprintf(outputFileH, "#define %s\n\n", hprotect);
	process(outputFileC, outputFileH, ssus);
	fprintf(outputFileH, "#endif // %s\n", hprotect);

	fclose(outputFileH);
	fclose(outputFileC);

	return 0;
}
