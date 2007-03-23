#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int main(int argc, char* argv[])
{
	FILE* fp;
	uint32_t  nImgSize;
	char* pHeader1 = "CSYS";
	uint32_t  nHeader2 = 0x80500000;

	if (argc != 3)
	{
		printf("Usage: mksyshdr <header file> <image file>\n");
		return -1;
	}

	fp = fopen(argv[2], "rb");
	if (fp != NULL)
	{
		fseek(fp, 0, SEEK_END);
		nImgSize = ftell(fp);
		fclose(fp);
		fp = fopen(argv[1], "wb+");
		if (fp != NULL)
		{
			fwrite(pHeader1,  sizeof(char), 4, fp);
			fwrite(&nHeader2, sizeof(nHeader2), 1, fp);
			fwrite(&nImgSize, sizeof(nImgSize), 1, fp);
			fclose(fp);
		}
		else
		{
			printf("Cannot create %s.\n", argv[1]);
			return -1;
		}
	}
	else
	{
		printf("Cannot open %s.\n", argv[2]);
		return -1;
	}

	return 0;
}

