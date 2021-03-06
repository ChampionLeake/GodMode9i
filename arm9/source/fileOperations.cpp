#include <nds.h>
#include <stdio.h>

#define copyBufSize 0x8000

u32 copyBuf[copyBufSize];

char clipboard[256];
char clipboardFilename[256];
bool clipboardOn = false;
bool clipboardUsed = false;
bool clipboardDrive = false;	// false == SD card, true == Flashcard
bool clipboardInNitro = false;

off_t getFileSize(const char *fileName)
{
    FILE* fp = fopen(fileName, "rb");
    off_t fsize = 0;
    if (fp) {
        fseek(fp, 0, SEEK_END);
        fsize = ftell(fp);			// Get source file's size
		fseek(fp, 0, SEEK_SET);
	}
	fclose(fp);

	return fsize;
}

int fcopy(const char *sourcePath, const char *destinationPath)
{
    FILE* sourceFile = fopen(sourcePath, "rb");
    off_t fsize = 0;
    if (sourceFile) {
        fseek(sourceFile, 0, SEEK_END);
        fsize = ftell(sourceFile);			// Get source file's size
		fseek(sourceFile, 0, SEEK_SET);
	} else {
		fclose(sourceFile);
		return -1;
	}

    FILE* destinationFile = fopen(destinationPath, "wb");
	//if (destinationFile) {
		fseek(destinationFile, 0, SEEK_SET);
	/*} else {
		fclose(sourceFile);
		fclose(destinationFile);
		return -1;
	}*/

	off_t offset = 0;
	int numr;
	while (1)
	{
		scanKeys();
		if (keysHeld() & KEY_B) {
			// Cancel copying
			fclose(sourceFile);
			fclose(destinationFile);
			return -1;
			break;
		}
		printf ("\x1b[16;0H");
		printf ("Progress:\n");
		printf ("%i/%i Bytes", (int)offset, (int)fsize);

		// Copy file to destination path
		numr = fread(copyBuf, 2, copyBufSize, sourceFile);
		fwrite(copyBuf, 2, numr, destinationFile);
		offset += copyBufSize;

		if (offset > fsize) {
			fclose(sourceFile);
			fclose(destinationFile);

			printf ("\x1b[17;0H");
			printf ("%i/%i Bytes", (int)fsize, (int)fsize);
			for (int i = 0; i < 60; i++) swiWaitForVBlank();

			return 1;
			break;
		}
	}

	return -1;
}
