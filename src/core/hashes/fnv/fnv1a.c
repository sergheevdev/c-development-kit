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
#include <string.h>         // For "memcpy", "strlen" (better memory copy and utils)
#include <stdint.h>         // For "uint32_t", "uint64_t" (more integer types)
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

uint32_t hashes_fnv1a_hash32_bytes(const char * bytes, const size_t length) {
    uint32_t hash = INIT_32;
    for (int i = 0; i < length; i++) {
        hash ^= (bytes[i] & 0xff);
        hash *= PRIME_32;
    }
    return hash;
}

uint32_t hashes_fnv1a_hash32_str(const char * text) {
    return hashes_fnv1a_hash32_bytes(text, strlen(text));
}

// Constants for 64 bits hash

static const uint64_t INIT_64 = 0xcbf29ce484222325;
static const uint64_t PRIME_64 = 1099511628211;

uint64_t hashes_fnv1a_hash64_bytes(const char * bytes, const size_t length) {
    uint64_t hash = INIT_64;
    for (int i = 0; i < length; i++) {
        hash ^= (bytes[i] & 0xff);
        hash *= PRIME_64;
    }
    return hash;
}

uint64_t hashes_fnv1a_hash64_str(const char * text) {
    return hashes_fnv1a_hash64_bytes(text, strlen(text));
}

// Assertion snippet (not abstracted away for piece of code portability)
void assert(int condition, char message[]) {
    if (condition != 1) {
        printf("%s\n", message);
        exit(1);
    }
}

// Unit testing

void hashes_fnv1a_hash32_str_test() {
    printf("*** Running test '%s'\n", __func__);
    // Testing 32-bit hashes match expected
    assert(hashes_fnv1a_hash32_str("Hello there!") == 2037575912, "Hash (32-bit) 1 does not match expected!");
    assert(hashes_fnv1a_hash32_str("Hello where?") == 1369641681, "Hash (32-bit) 2 does not match expected!");
    assert(hashes_fnv1a_hash32_str("AAAAA") == 3552656040, "Hash (32-bit) 3 does not match expected!");
    assert(hashes_fnv1a_hash32_str("AAAAA ") == 3777963032, "Hash (32-bit) 4 does not match expected!");
    assert(hashes_fnv1a_hash32_str("Yo, Whats up!") == 1109325136, "Hash (32-bit) 5 does not match expected!");
}

void hashes_fnv1a_hash64_str_test() {
    printf("*** Running test '%s'\n", __func__);
    // Testing 64-bit hashes match expected
    assert(hashes_fnv1a_hash64_str("Welcome home!") == 6875887167340965921, "Hash (64-bit) 1 does not match expected!");
    assert(hashes_fnv1a_hash64_str("Minecraft") == 2767293019749932152, "Hash (64-bit) 2 does not match expected!");
    assert(hashes_fnv1a_hash64_str("Yo, it's a plane!") == 5942718437609282930, "Hash (64-bit) 3 does not match expected!");
    assert(hashes_fnv1a_hash64_str("Pen Pineapple Apple Pen!") == 3085370648541523016, "Hash (64-bit) 4 does not match expected!");
    assert(hashes_fnv1a_hash64_str("RFC-2616 for HTTP!") == 3530592443485884302, "Hash (64-bit) 5 does not match expected!");
}

int main() {
    hashes_fnv1a_hash32_str_test();
    hashes_fnv1a_hash64_str_test();
}
