# Hashmap
A hashmap that guarantees a constant time complexity implemented in c. The hashing function can be easily changed. The hashmap supports any datatype given that the padding is consistent.

To guarantee a constant time complexity all values are rehashed with a different seed until no hash collisions are present. 

Currently implemented hash functions:
- CRC64
- blake2b
