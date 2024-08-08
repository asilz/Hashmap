#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <blake.h>

static const uint64_t initializationVectors[8] = {0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1, 0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179};

static const uint8_t sigma[10][16] =
    {
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
        {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3},
        {11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4},
        {7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8},
        {9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13},
        {2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9},
        {12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11},
        {13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10},
        {6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5},
        {10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13, 0}};

#define Rotate0 (32)
#define Rotate1 (24)
#define Rotate2 (16)
#define Rotate3 (63)

static inline void mix(uint64_t *restrict Va, uint64_t *restrict Vb, uint64_t *restrict Vc, uint64_t *restrict Vd, uint64_t x, uint64_t y)
{
    *Va = *Va + *Vb + x;
    *Vd = ((*Vd ^ *Va) >> Rotate0) | ((*Vd ^ *Va) << (sizeof(*Vd) * 8 - Rotate0));

    *Vc = *Vc + *Vd;
    *Vb = ((*Vb ^ *Vc) >> Rotate1) | ((*Vb ^ *Vc) << (sizeof(*Vb) * 8 - Rotate1));

    *Va = *Va + *Vb + y;
    *Vd = ((*Vd ^ *Va) >> Rotate2) | ((*Vd ^ *Va) << (sizeof(*Vd) * 8 - Rotate2));

    *Vc = *Vc + *Vd;
    *Vb = ((*Vb ^ *Vc) >> Rotate3) | ((*Vb ^ *Vc) << (sizeof(*Vb) * 8 - Rotate3));
}

static inline void compress(uint64_t h[8], uint8_t chunk[128], size_t byteCount, uint8_t isLastBlock)
{
    uint64_t V[16];
    memcpy(V, h, sizeof(uint64_t) * 8);
    memcpy(V + 8, initializationVectors, sizeof(initializationVectors));

    V[12] = V[12] ^ byteCount;
    // TODO: Add missing line

    if (isLastBlock)
    {
        V[14] = ~V[14];
    }

    for (size_t i = 0; i < 12; ++i)
    {
        uint64_t *chunk64 = (uint64_t *)chunk;
        uint8_t S[16];
        memcpy(S, sigma[i % 10], sizeof(S));

        mix(V + 0, V + 4, V + 8, V + 12, chunk64[S[0]], chunk64[S[1]]);
        mix(V + 1, V + 5, V + 9, V + 13, chunk64[S[2]], chunk64[S[3]]);
        mix(V + 2, V + 6, V + 10, V + 14, chunk64[S[4]], chunk64[S[5]]);
        mix(V + 3, V + 7, V + 11, V + 15, chunk64[S[6]], chunk64[S[7]]);

        mix(V + 0, V + 5, V + 10, V + 15, chunk64[S[8]], chunk64[S[9]]);
        mix(V + 1, V + 6, V + 11, V + 12, chunk64[S[10]], chunk64[S[11]]);
        mix(V + 2, V + 7, V + 8, V + 13, chunk64[S[12]], chunk64[S[13]]);
        mix(V + 3, V + 4, V + 9, V + 14, chunk64[S[14]], chunk64[S[15]]);
    }

    for (size_t i = 0; i < 8; ++i)
    {
        h[i] = h[i] ^ V[i] ^ V[i + 8];
    }
}

uint8_t *blake2b(const uint8_t *data, size_t dataSize, const uint8_t *key, size_t keySize, size_t hashSize)
{
    assert(keySize <= 64 && hashSize > 0 && hashSize <= 64);
    uint64_t h[8];
    memcpy(h, initializationVectors, sizeof(h));

    h[0] ^= 0x01010000 | (keySize << 8) | hashSize;

    size_t bytesCompressed = 0;
    size_t bytesRemaining = dataSize;

    uint8_t *dataAppended;

    if (keySize > 0)
    {
        bytesRemaining += 128;
        dataAppended = malloc(bytesRemaining);
        memcpy(dataAppended, key, keySize);
        memset(dataAppended + keySize, 0, 128 - keySize);
        memcpy(dataAppended + 128, data, dataSize);
    }
    else
    {
        dataAppended = malloc(bytesRemaining);
        memcpy(dataAppended, data, dataSize);
    }

    while (bytesRemaining > 128)
    {
        uint8_t *chunk = dataAppended + bytesCompressed;
        bytesCompressed += 128;
        bytesRemaining -= 128;

        compress(h, chunk, bytesCompressed, 0);
    }

    uint8_t lastChunk[128];
    memcpy(lastChunk, dataAppended + bytesCompressed, bytesRemaining);
    memset(lastChunk + bytesRemaining, 0, sizeof(lastChunk) - bytesRemaining);
    bytesCompressed = bytesCompressed + bytesRemaining;
    compress(h, lastChunk, bytesCompressed, 1);

    free(dataAppended);

    uint8_t *hash = malloc(hashSize);
    memcpy(hash, h, hashSize);
    return hash;
}

static size_t blake2bMap(const uint8_t *inputData, size_t dataSize, uint64_t seed)
{
    uint8_t *hashBuffer = blake2b(inputData, dataSize, (uint8_t *)&seed, sizeof(seed), sizeof(size_t));
    size_t hash = *(size_t *)hashBuffer;
    free(hashBuffer);
    return hash;
}

struct Hashmap initHashmapBlake2b(size_t valueSize, size_t keySize)
{
    struct Hashmap map = {0, valueSize, keySize, NULL, blake2bMap, 0};
    return map;
}