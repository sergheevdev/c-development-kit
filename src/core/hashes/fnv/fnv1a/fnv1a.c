/*
 * Copyright 2022 Serghei Sergheev
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and limitations under the License.
 */

// Imports & Headers

#include <stdio.h>          // For "printf", "stderr" (printing errors)
#include <stdint.h>         // For "uint32_t", "uint64_t" (more integer types)
#include <string.h>         // For "memcpy", "strlen" (better memory copy and utils)
#include <stdlib.h>         // For "exit"

/*
 * ### Introduction ###
 *
 * Fowler-Noll-Vo (or FNV) is a non-cryptographic fast-computable hash function with a low collision rate.
 *
 * ### Implementation ###
 *
 * This implementation uses the FNV-1a version of FNV, variation of the FNV-1, but with better avalanche characteristics.
 *
 * ### References ###
 *
 * - https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
 * - https://softwareengineering.stackexchange.com/a/145633
 * - http://www.isthe.com/chongo/src/fnv/hash_32a.c
 * - http://www.isthe.com/chongo/src/fnv/hash_64a.c
 */

// Constants for 32 bits hash

static const uint32_t INIT_32 = 0x811c9dc5;
static const uint32_t PRIME_32 = 16777619;

uint32_t * hashes_fnv1a_hash32_bytes(const char * bytes, const size_t length) {
    if (bytes == NULL) {
        fprintf(stderr, "Trying to hash 'NULL' bytes at '%s'\n", __func__);
        return NULL;
    }
    if (length <= 0) {
        fprintf(stderr, "The 'length' must be greater than zero at '%s'\n", __func__);
        return NULL;
    }
    uint32_t * hash = malloc(sizeof(uint32_t));
    if (hash == NULL) {
        fprintf(stderr, "Unable to allocate memory for 'hash' at '%s'\n", __func__);
        return NULL;
    }
    (* hash) = INIT_32;
    for (size_t i = 0; i < length; i++) {
        (* hash) ^= (bytes[i] & 0xff);
        (* hash) *= PRIME_32;
    }
    return hash;
}

uint32_t * hashes_fnv1a_hash32_str(const char * text) {
    return hashes_fnv1a_hash32_bytes(text, strlen(text));
}

// Constants for 64 bits hash

static const uint64_t INIT_64 = 0xcbf29ce484222325;
static const uint64_t PRIME_64 = 1099511628211;

uint64_t * hashes_fnv1a_hash64_bytes(const char * bytes, const size_t length) {
    if (bytes == NULL) {
        fprintf(stderr, "Trying to hash 'NULL' bytes at '%s'\n", __func__);
        return NULL;
    }
    if (length <= 0) {
        fprintf(stderr, "The 'length' must be greater than zero at '%s'\n", __func__);
        return NULL;
    }
    uint64_t * hash = malloc(sizeof(uint64_t));
    if (hash == NULL) {
        fprintf(stderr, "Unable to allocate memory for 'hash' at '%s'\n", __func__);
        return NULL;
    }
    (* hash) = INIT_64;
    for (size_t i = 0; i < length; i++) {
        (* hash) ^= (bytes[i] & 0xff);
        (* hash) *= PRIME_64;
    }
    return hash;
}

uint64_t * hashes_fnv1a_hash64_str(const char * text) {
    return hashes_fnv1a_hash64_bytes(text, strlen(text));
}
