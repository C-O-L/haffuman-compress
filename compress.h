#pragma once
#ifndef _FILE_COMPRESSION_H
#define _FILE_COMPRESSION_H


//Haffuman Tree ½Úµã
typedef struct HaffumanTreeNode HTN;
struct HaffumanTreeNode
{
    char _ch;   //character
    int _count; //frequency
    struct HaffumanTreeNode* _left; //left child
    struct HaffumanTreeNode* _right;//rigth child
};

//FileCompress Struct
#define BITS_PER_CHAR 8     //the number of bits in a char
#define MAX_CHARS 256            //the max number of chars
#define FILE_BUF_SIZE 8192  //the size of Buffer for FILE I/O
typedef struct FileCompressStruct FCS;
struct FileCompressStruct
{
    HTN* _haffuman;        //A pointer to the root of hafumman tree
    unsigned int _charsCount; //To store the number of chars
    unsigned int _total; //Total bytes in a file.
    char* _dictionary[MAX_CHARS]; //to store the encoding of each character
    int _statistic[MAX_CHARS]; //To store the number of each character
};

FCS* fcs_new();
void fcs_compress(FCS* fcs, const char* inFileName, const char* outFileName);
void fcs_decompress(FCS* fcs, const char* inFileName, const char* outFileName);
void fcs_free(FCS* fcs);
#endif