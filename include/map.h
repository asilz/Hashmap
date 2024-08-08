#ifndef MAP_H
#define MAP_H

#include <inttypes.h>
#include <stdio.h>

typedef size_t (*HashFunction)(const uint8_t *inputData, size_t dataSize, uint64_t seed);

struct Hashmap
{
    size_t entriesCount;
    const size_t valueSize;
    const size_t keySize;
    void *data;
    HashFunction hash;
    uint64_t seed;
};

void mapInsert(struct Hashmap *hashmap, void *values, void *keys, size_t insertCount);
void mapDelete(struct Hashmap *hashmap, void *key);
void *mapGet(struct Hashmap *hashmap, void *key);
void mapPrint(struct Hashmap *hashmap);

#endif