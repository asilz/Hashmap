#include <crc64.h>
#include <string.h>
#include <blake.h>

struct Key
{
    uint64_t data1;
    uint64_t data2;
};

struct Value
{
    uint64_t data2;
    uint64_t data1;
};

int main()
{

    struct Hashmap map = initHashmapCRC64(sizeof(struct Value), sizeof(struct Key));
    struct Value values[2];
    values[0].data1 = 0xABAB;
    values[0].data2 = 0xDEADBEEFDEADF00D;
    values[1].data1 = 0xCDCD;
    values[1].data2 = 0xBEEFF00DF00DBEEF;
    struct Key keys[2];
    keys[0].data1 = 0xCAFEF00DF;
    keys[0].data2 = 0;
    keys[1].data1 = 0xF00DCAFECAE00D;
    keys[1].data2 = 0xBBAABBAAAACC1;
    mapInsert(&map, values, keys, 2);
    mapPrint(&map);
    struct Value *value = mapGet(&map, keys);
    if (memcmp(values, value, sizeof(struct Value)) == 0)
    {
        printf("correct!\n");
    }

    return 0;
}