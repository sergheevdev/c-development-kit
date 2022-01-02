/*
 * Copyright 2021 Serghei Sergheev
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

#include <stdbool.h>

/* string-builder.h */
#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

typedef struct string_builder StringBuilder;

/**
 * Creates a string builder with the default initial capacity and resize increment.
 *
 * The returned builder must be freed by the client after its usage.
 *
 * @return a new string builder, or {@code NULL} if an allocation error occurred
 */
StringBuilder * string_builder_create_default();

/**
 * Creates a string builder with the provided initial capacity.
 *
 * The returned builder must be freed by the client after its usage.
 *
 * @param initial_capacity the initial amount of dynamically allocated characters
 *
 * @return a new string builder, or {@code NULL} if an allocation error occurred
 */
StringBuilder * string_builder_create(size_t initial_capacity);

/**
 * Frees the string builder structure and the dynamically allocated chain.
 *
 * @param string_builder the string builder that is about to be freed
 */
void string_builder_destroy(StringBuilder * string_builder);

/**
 * Frees the string builder structure but does NOT free the pointer to the built-in chain.
 *
 * @param string_builder the string builder that is about to be freed
 *
 * @note useful when you don't want to have two copies of the same string in memory at the same time
 */
void string_builder_destroy_except_chain(StringBuilder * string_builder);

/**
 * Appends one character to the given string builder.
 *
 * @param string_builder the string builder to whom the character must be appended to
 * @param character the character to be appended
 *
 * @note the append operation can only fail if there was a reallocation error
 *
 * @return {@code true} if the append operation was successful, {@code false} otherwise
 */
bool string_builder_append_one(StringBuilder * string_builder, char character);

/**
 * Appends an array of characters to the given string builder.
 *
 * @param string_builder the string builder to whom the array of characters must be appended to
 * @param character the array of characters to be appended
 *
 * @note the append operation can only fail if there was a reallocation error
 *
 * @return {@code true} if the append operation was successful, {@code false} otherwise
 */
bool string_builder_append_all(StringBuilder * string_builder, char * chain);

/**
 * Removes all the characters between the start index (inclusive) and stop index (inclusive) from the given builder.
 *
 * @param string_builder the string builder from whom the chain in the given range is to be removed
 * @param start_index the start inclusive index
 * @param stop_index the stop inclusive index
 *
 * @return {@code true} if the remove operation was successful, {@code false} otherwise
 */
bool string_builder_remove(StringBuilder * string_builder, size_t start_index, size_t stop_index);

/**
 * Returns the amount of characters present in the given string builder.
 *
 * @return the size of the builder
 */
size_t string_builder_size(StringBuilder * string_builder);

/**
 * Returns the current maximum capacity of the given string builder.
 *
 * @return the current max capacity of the builder
 */
size_t string_builder_capacity(StringBuilder * string_builder);

/**
 * Returns a pointer to the original built chain (which the builder uses internally).
 *
 * This pointer is freed (and therefore deallocated) when you use the default destroy method (use with caution).
 *
 * @param string_builder the string builder from whom the built chain pointer is to be returned
 *
 * @return a pointer to the original builder built in chain, or {@code NULL} if an allocation error occurred
 */
char * string_builder_result(StringBuilder * string_builder);

/**
 * Returns a copy of the given builder's constructed string.
 *
 * The returned chain must be freed by the client after its usage.
 *
 * @param string_builder the string builder from whom a built chain copy is to be created
 *
 * @return a copy of the constructed string, or {@code NULL} if an allocation error occurred
 */
char * string_builder_result_as_copy(StringBuilder * string_builder);

#endif /* STRING_BUILDER_H */