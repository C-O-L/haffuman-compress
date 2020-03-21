#include <stdlib.h>
#include "compress.h"

const int DO_COMPRESS = 1;
const int DO_DECOMPRESS = 1;

const char* InFile = "compress.txt"; //The file to compress.
const char* CompressedFile = "compressed.txt"; //Compressed data of the file.
const char* OutFile = "decompressed.txt"; //The decompressed file of the data.

int main(int argc, char** argv)
{
    //1. compress file
    if (DO_COMPRESS)
    {
        FCS* fcs1;
        fcs1 = fcs_new();
        fcs_compress(fcs1, InFile, CompressedFile);
        fcs_free(fcs1);
    }
    //2. decompress file
    if (DO_DECOMPRESS)
    {
        FCS* fcs2;
        fcs2 = fcs_new();
        fcs_decompress(fcs2, CompressedFile, OutFile);
        fcs_free(fcs2);
    }
    system("pause");
    return 0;
}