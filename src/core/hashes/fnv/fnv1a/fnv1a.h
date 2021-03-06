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

#include <stdbool.h>        // For "true", "false" (boolean constants)
#include <stddef.h>         // For "size_t" (size type)
#include <stdint.h>         // For "uint32_t", "uint64_t" (more integer types)

/* string-builder.h */
#ifndef HASHES_FNV1A_H
#define HASHES_FNV1A_H

/**
 * Returns a 32 bit integer hash of the given bytes.
 *
 * It might return {@code NULL} if:
 * - The "bytes" pointer is null.
 * - The "length" is less or equal to zero.
 * - Could not allocate memory for the return value.
 *
 * @return the hash value, or {@code NULL} if an error occurred
 */
uint32_t * hashes_fnv1a_hash32_bytes(const char * bytes, const size_t length);

/**
 * Returns a 32 bit integer hash of the given text.
 *
 * It might return {@code NULL} if:
 * - The "text" pointer is null.
 * - The "text" length is less or equal to zero.
 * - Could not allocate memory for the return value.
 *
 * @return the hash value, or {@code NULL} if an error occurred
 */
uint32_t * hashes_fnv1a_hash32_str(const char * text);

/**
 * Returns a 64 bit integer hash of the given bytes.
 *
 * It might return {@code NULL} if:
 * - The "bytes" pointer is null.
 * - The "length" is less or equal to zero.
 * - Could not allocate memory for the return value.
 *
 * @return the hash value, or {@code NULL} if an error occurred
 */
uint64_t * hashes_fnv1a_hash64_bytes(const char * bytes, const size_t length);

/**
 * Returns a 64 bit integer hash of the given text.
 *
 * It might return {@code NULL} if:
 * - The "text" pointer is null.
 * - The "text" length is less or equal to zero.
 * - Could not allocate memory for the return value.
 *
 * @return the hash value, or {@code NULL} if an error occurred
 */
uint64_t * hashes_fnv1a_hash64_str(const char * text);

#endif /* HASHES_FNV1A_H */