#include <stdlib.h>
#include <string.h>
#include <map.h>

void mapInsert(struct Hashmap *hashmap, void *values, void *keys, size_t insertCount)
{
    hashmap->entriesCount += insertCount;
    uint8_t *occupiedIndices = calloc(hashmap->entriesCount, 1);
    void *newValues = malloc((hashmap->valueSize + hashmap->keySize) * hashmap->entriesCount);
    void *newKeys = (uint8_t *)newValues + hashmap->entriesCount * hashmap->valueSize;
    for (; hashmap->seed < UINT64_MAX; ++hashmap->seed)
    {
        uint8_t successful = 1;
        for (size_t i = 0; i < hashmap->entriesCount; ++i)
        {
            uint64_t hash;
            if (i >= hashmap->entriesCount - insertCount)
            {
                hash = hashmap->hash((uint8_t *)keys + (i - hashmap->entriesCount + insertCount) * hashmap->keySize, hashmap->keySize, hashmap->seed) % hashmap->entriesCount;
                printf("0x%lX, %lu\n", hash, (i - hashmap->entriesCount + insertCount));
                if (occupiedIndices[hash])
                {
                    successful = 0;
                    break;
                }
                memcpy((uint8_t *)newValues + hash * hashmap->valueSize, (uint8_t *)values + (i - hashmap->entriesCount + insertCount) * hashmap->valueSize, hashmap->valueSize);
                memcpy((uint8_t *)newKeys + hash * hashmap->keySize, (uint8_t *)keys + (i - hashmap->entriesCount + insertCount) * hashmap->keySize, hashmap->keySize);
            }
            else
            {
                hash = hashmap->hash((uint8_t *)hashmap->data + (hashmap->entriesCount - insertCount) * hashmap->valueSize + hashmap->keySize * i, hashmap->keySize, hashmap->seed) % hashmap->entriesCount;
                if (occupiedIndices[hash])
                {
                    successful = 0;
                    break;
                }
                memcpy((uint8_t *)newValues + hash * hashmap->valueSize, (uint8_t *)hashmap->data + i * hashmap->valueSize, hashmap->valueSize);
                memcpy((uint8_t *)newKeys + hash * hashmap->keySize, (uint8_t *)hashmap->data + (hashmap->entriesCount - insertCount) * hashmap->valueSize + i * hashmap->keySize, hashmap->keySize);
            }

            occupiedIndices[hash] = 1;
        }
        if (successful)
        {
            break;
        }
        memset(occupiedIndices, 0, hashmap->entriesCount);
    }
    free(occupiedIndices);
    free(hashmap->data);
    hashmap->data = newValues;
}

void mapDelete(struct Hashmap *hashmap, void *key)
{
    size_t deletedValueIndex = hashmap->hash(key, hashmap->keySize, hashmap->seed) % hashmap->entriesCount;
    void *newValues = malloc((hashmap->valueSize + hashmap->keySize) * --hashmap->entriesCount);
    void *newKeys = (uint8_t *)newValues + hashmap->entriesCount * hashmap->valueSize;
    uint8_t *occupiedIndices = calloc(hashmap->entriesCount, 1);

    for (hashmap->seed = 0; hashmap->seed < UINT64_MAX; ++hashmap->seed)
    {
        uint8_t successful = 1;
        for (size_t i = 0; i < hashmap->entriesCount; ++i)
        {

            uint64_t hash = hashmap->hash((uint8_t *)hashmap->data + (hashmap->entriesCount + 1) * hashmap->valueSize + hashmap->keySize * (i + (i >= deletedValueIndex)), hashmap->keySize, hashmap->seed) % hashmap->entriesCount;
            if (occupiedIndices[hash])
            {
                successful = 0;
                break;
            }
            memcpy((uint8_t *)newValues + hash * hashmap->valueSize, (uint8_t *)hashmap->data + (i + (i >= deletedValueIndex)) * hashmap->valueSize, hashmap->valueSize);
            memcpy((uint8_t *)newKeys + hash * hashmap->keySize, (uint8_t *)hashmap->data + (hashmap->entriesCount + 1) * hashmap->valueSize + (i + (i >= deletedValueIndex)) * hashmap->keySize, hashmap->keySize);

            occupiedIndices[hash] = 1;
        }
        if (successful)
        {
            break;
        }
        memset(occupiedIndices, 0, hashmap->entriesCount);
    }
    free(occupiedIndices);
    free(hashmap->data);
    hashmap->data = newValues;
}

void *mapGet(struct Hashmap *hashmap, void *key)
{
    return (uint8_t *)hashmap->data + hashmap->valueSize * (hashmap->hash(key, hashmap->keySize, hashmap->seed) % hashmap->entriesCount);
}

void mapPrint(struct Hashmap *hashmap)
{
    for (size_t i = 0; i < hashmap->entriesCount; ++i)
    {
        printf("key: 0x");
        for (size_t j = 0; j < hashmap->keySize; ++j)
        {
            printf("%02X", ((uint8_t *)hashmap->data + (hashmap->entriesCount * hashmap->valueSize) + hashmap->keySize * i)[j]);
        }
        printf(", value: 0x");
        for (size_t j = 0; j < hashmap->valueSize; ++j)
        {
            printf("%02X", ((uint8_t *)hashmap->data + hashmap->valueSize * i)[j]);
        }
        printf("\n");
    }
}
