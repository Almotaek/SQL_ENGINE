#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include "stdlib.h"
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

void* createBlock();
void* createIndexBlock();
void* getLast(void* block, int byteSize);
void addIndexBlk(void* indextBlk, void* blkToBeAdded);
void addVoidPtr(void* block, void* blkToBeAdded);
void addInt(void* block, int num);
void addChar(void* block, char letter);
void addCharPtr(void* block, char* blkToBeAdded);
void addIntPtr(void* block, int* blkToBeAdded);
void create_table(const char *table_name, const char *primary_key, int length, ...);
void selectt(const char *table_name, int length, ...);
void insert(const char *table_name, int length, ...);
void* getString(void* start, char* string);
void* next(void* start);
void* getBlock(void* start, int ByteSize);
void* nextChar(void* start);
void* locateTable(const char *table_name);
int getStringSize(void* start,int* size);
void* getBlockChar(void* start, int ByteSize);

#endif