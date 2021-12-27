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
 * @return a new string builder
 */
StringBuilder * string_builder_create_default();

/**
 * Creates a string builder with a concrete initial capacity and resize increment.
 *
 * The returned builder must be freed by the client after its usage.
 *
 * @param initial_capacity initial amount of dynamically allocated characters
 *
 * @return a new string builder
 */
StringBuilder * string_builder_create(size_t initial_capacity);

/**
 * Frees the string builder structure and all its dynamically allocated contents.
 *
 * @param string_builder the string builder that is about to be freed
 */
void string_builder_destroy(StringBuilder * string_builder);

/**
 * Frees the string builder structure but does not free the internal built chain.
 *
 * @param string_builder the string builder that is about to be freed
 *
 * @note instead of obtaining the result as copy we can directly access the chain pointer,
 *       and then freeing only the structure, that way we don't need extra "re-allocations"
 *       to obtain the result (i.e., the built chain)
 */
void string_builder_destroy_except_chain(StringBuilder * string_builder);

/**
 * Appends a character to the given string builder.
 *
 * @param string_builder the string builder to whom the character must be appended to
 * @param character the character to be appended
 *
 * @return true if the append operation completed successfully, false otherwise
 */
bool string_builder_append(StringBuilder * string_builder, char character);

/**
 * Appends an array of characters to the given string builder.
 *
 * @param string_builder the string builder to whom the array of characters must be appended to
 * @param character the array of characters to be appended
 *
 * @note if the append operation fails, the string builder is rolled back to its old contents.
 *
 * @return true if the append operation completed successfully, false otherwise
 */
bool string_builder_append_all(StringBuilder * string_builder, char * chain);

/**
 * Removes the characters between the start and stop indexes (inclusive both) from the given builder.
 *
 * @param string_builder the string builder from whom the chain in between the start and stop indexes is to be removed
 * @param start_index the start inclusive index from where to start removing
 * @param stop_index the stop inclusive index that indicates the last position to be removed
 *
 * @return true if the remove operation completed successfully, false otherwise
 */
bool string_builder_remove(StringBuilder * string_builder, size_t start_index, size_t stop_index);

/**
 * Returns the builder's internal pointer to the constructed string.
 *
 * This string is freed when you free the builder structure (use with caution).
 *
 * @param string_builder the string builder from whom the built chain is extracted
 *
 * @return the built chain by the given string builder
 */
char * string_builder_result(StringBuilder * string_builder);

/**
 * Returns a copy of the constructed string.
 *
 * The returned string must be freed by the client after its usage.
 *
 * @param string_builder the string builder from whom the built chain copy is extracted
 *
 * @return a copy of the string builder's built chain
 */
char * string_builder_result_as_copy(StringBuilder * string_builder);

#endif /* STRING_BUILDER_H */