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

#include <stdio.h>          // For "printf", "stderr" (printing errors)
#include <stdlib.h>         // For "exit"
#include "fnv1a.h"

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
    // Testing hashes values match expected
    uint32_t * first = hashes_fnv1a_hash32_str("Hello there!");
    assert((* first) == 2037575912, "First hash result does not match expected!");
    free(first);
    uint32_t * second = hashes_fnv1a_hash32_str("AAAAA");
    assert((* second) == 3552656040, "Second hash result does not match expected!");
    free(second);
    uint32_t * third = hashes_fnv1a_hash32_str("AAAAA ");
    assert((* third) == 3777963032, "Third hash result does not match expected!");
    free(third);
    // Testing hashes pointers return 'NULL' if parameters validation fails
    uint32_t * fourth = hashes_fnv1a_hash32_bytes(NULL, 10);
    assert(fourth == NULL, "Fourth hash result must be NULL!");
    free(fourth);
    uint32_t * fifth = hashes_fnv1a_hash32_bytes("sample text", 0);
    assert(fifth == NULL, "Fifth hash result must be NULL!");
    free(fifth);
    uint32_t * sixth = hashes_fnv1a_hash32_bytes(NULL, 0);
    assert(sixth == NULL, "Sixth hash result must be NULL!");
    free(sixth);
}

void hashes_fnv1a_hash64_str_test() {
    printf("*** Running test '%s'\n", __func__);
    // Testing hashes values match expected
    uint64_t * first = hashes_fnv1a_hash64_str("Welcome home!");
    assert((* first) == 6875887167340965921, "First hash result does not match expected!");
    free(first);
    uint64_t * second = hashes_fnv1a_hash64_str("RFC-2616 for HTTP!");
    assert((* second) == 3530592443485884302, "Second hash result does not match expected!");
    free(second);
    uint64_t * third = hashes_fnv1a_hash64_str("Pen Pineapple Apple Pen!");
    assert((* third) == 3085370648541523016, "Third hash result does not match expected!");
    free(third);
    // Testing hashes pointers return 'NULL' if parameters validation fails
    uint64_t * fourth = hashes_fnv1a_hash64_bytes(NULL, 10);
    assert(fourth == NULL, "Fourth hash result must be NULL!");
    free(fourth);
    uint64_t * fifth = hashes_fnv1a_hash64_bytes("sample text", 0);
    assert(fifth == NULL, "Fifth hash result must be NULL!");
    free(fifth);
    uint64_t * sixth = hashes_fnv1a_hash64_bytes(NULL, 0);
    assert(sixth == NULL, "Sixth hash result must be NULL!");
    free(sixth);
}

int main() {
    fclose(stderr); // Prevent printing "expected" errors
    hashes_fnv1a_hash32_str_test();
    hashes_fnv1a_hash64_str_test();
}
