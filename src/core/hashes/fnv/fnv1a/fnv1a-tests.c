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
