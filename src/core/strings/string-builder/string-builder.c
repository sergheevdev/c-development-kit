/*
 * A Non Thread-Safe String Builder Implementation (with golden ratio as resize strategy).
 *
 * ### Explanation ###
 *
 * The "string builder" data structure based on the idea of creating a mutable sequence of characters and providing
 * utility operations that can be used to mutate the string that is being built.
 *
 * For example, when concatenating strings in C, suppose the need to concat two strings A and B, to do so, first we
 * need to allocate a new chain of size len(A)+len(B), then copy all contents from A and B to C, but we can delegate
 * all this work to a builder, so we don't need to worrying about low-level "strings" memory management.
 *
 * This structure is used as an auxiliary structure on several string algorithms (i.e., 'replace all' algorithm).
 *
 * ### Memory Blocks Can't Be Resized ###
 *
 * In computer science we have kind of a problem, because once we allocate a block of memory, if we want to extend that
 * block to a larger size (for more space), we can't, we need to allocate a new block of the new larger size, and copy
 * all the contents from the old block to the new one, finally freeing the old block contents and using the new one.
 *
 * More info about this limitation: https://stackoverflow.com/a/26836095
 *
 * ### Strategies ###
 *
 * A "reallocation strategy" is said to be the new size to which we need to resize the array of characters, when we
 * run out of space (because no more chars fit in the current array).
 *
 * Now the raised question is: to what "new size" resize the "old size"?
 *
 * The simplest strategy one might think of may be use of this formula: "new size = old size + constant", where the
 * constant is simply a constant amount of space to be incremented each time we run out of space.
 *
 * ### Constant Resize Increment Strategy ###
 *
 * Let's suppose we implemented the previously mentioned simplest strategy with the following parameters:
 * - I = initial capacity = 1000
 * - K = constant (resize increment) = 1000
 * - F = resize formula -> new size = old size + constant
 *
 * So when we run out of space and try to append the 1001 th char, before doing so, we must ensure the capacity is
 * enough to fit one more char, if not, we resize the char array from 1000 to 2000 chars, and then append 1001 th
 * char when the reallocation had been performed.
 *
 * The first reallocation requires us to copy K chars, the second one requires 2K, the third 3K, the fourth 4K, the
 * fifth 5K and so on. The total time complexity will be O(K + 2K + 3K + ... + NK), this ends up in an O(N^2) worst
 * time complexity. Why? Because of the gauss series "1 + 2 + ... + N" ≈ "N * (N + 1) / 2" ≈ "(N^2 + N) / 2" ≈ "N^2".
 *
 * Supposing that we might end up with a 1 million chars string, we can compute the total amount of wasted memory by
 * using K = resize increment, the applied gauss series will be the following:
 *
 * "1000 + 2000 + 3000 + ... + 1000000" which sums to on the order of 500 billion characters of wasted memory.
 *
 * We can compute the exact amount by using the gauss formula for even numbers:
 *
 * "N (a1 + aN) / 2" = "1000 * (1000 + 1000000) / 2" = "500500000" wasted chars (in space)
 *
 * We might end up having at the same time 2 million characters in memory for a short period of time.
 *
 * ### Golden Ratio/Mean Strategy (!!!used in this implementation!!!) ###
 *
 * This implementation uses the godel ratio as a growth factor, instead of a constant resize increment, this value is
 * often similar to the golden mean (~1.6) but a little bit smaller, most implementations use the 1.5 value and this
 * works great in practice.
 *
 * ### Other Strategies ###
 *
 * - Usage of a "double when full" strategy instead of defining a constant resize increment.
 * - Using a "linked list of blocks" (prevents the amortized complexity of reallocation, but poor locality & construction).
 *
 * ### References ###
 *
 * - https://stackoverflow.com/questions/10196942/how-much-to-grow-buffer-in-a-stringbuilder-like-c-module
 * - https://stackoverflow.com/questions/9252891/big-o-what-is-the-complexity-of-summing-a-series-of-n-numbers
 * - https://math.stackexchange.com/questions/2844825/time-complexity-from-an-arithmetic-series/2844851#2844851
 * - https://mathbitsnotebook.com/Algebra2/Sequences/SSGauss.html
 * - https://news.ycombinator.com/item?id=8555550
 */

// Imports & Headers

#include <stdlib.h>         // For "malloc", "realloc", "free" (memory management)
#include <stdio.h>          // For "printf", "stderr" (printing errors)
#include <string.h>         // For "memcpy", "strlen" (better memory copy and utils)
#include "string-builder.h"

// Definition (to be able to use methods before declaring them, to follow up the top-bottom code style)

bool string_builder_ensure_capacity(StringBuilder * string_builder, size_t chars_amount);
size_t string_builder_compute_new_size(StringBuilder * string_builder, size_t chars_amount);

// Structures

struct string_builder {
    char * built_chain;
    size_t used_capacity;
    size_t max_capacity;
};

// Default implementation values
static const int DEFAULT_INITIAL_CAPACITY = 16;

StringBuilder * string_builder_create_default() {
    return string_builder_create(DEFAULT_INITIAL_CAPACITY);
}

StringBuilder * string_builder_create(size_t initial_capacity) {
    if (initial_capacity < 0) {
        fprintf(stderr, "The 'initial_capacity' must be an integer bigger or equal to '0' at '%s'\n", __func__);
        return NULL;
    }
    char * built_chain = malloc(sizeof(char) * initial_capacity);
    if (built_chain == NULL) {
        fprintf(stderr, "Unable to allocate memory for 'built_chain' at '%s'\n", __func__);
        return NULL;
    }
    StringBuilder * string_builder = malloc(sizeof(StringBuilder));
    if (string_builder == NULL) {
        free(built_chain);
        fprintf(stderr, "Unable to allocate memory for 'string_builder' at '%s'\n", __func__);
        return NULL;
    }
    string_builder->built_chain = built_chain;
    string_builder->used_capacity = 0;
    string_builder->max_capacity = initial_capacity;
    // Return the new builder
    return string_builder;
}

bool string_builder_append_one(StringBuilder * string_builder, char character) {
    if (string_builder == NULL) {
        fprintf(stderr, "Trying to append a character to a 'NULL' builder at '%s'\n", __func__);
        return false;
    }
    // Ensure there is size for one more character to be appended, otherwise resize the chain
    bool is_capacity_ensured = string_builder_ensure_capacity(string_builder, 1);
    // If ensuring the capacity wasn't possible, then the "append operation" failed
    if (!is_capacity_ensured) return false;
    // Get the last unused character position, which is where the new character is to be appended
    char * last_unused = string_builder->built_chain + string_builder->used_capacity;
    // Assign the character value
    (* last_unused) = character;
    // Increase the amount of used characters
    string_builder->used_capacity++;
    // Return a successful result
    return true;
}

/**
 * @note that we pre-compute the necessary slots and then we reallocate only ONCE, instead of performing M reallocations
 */
bool string_builder_append_all(StringBuilder * string_builder, char * chain) {
    if (string_builder == NULL) {
        fprintf(stderr, "Trying to append a character to a 'NULL' builder at '%s'\n", __func__);
        return false;
    }
    if (chain == NULL) {
        fprintf(stderr, "Trying to append a 'NULL' chain to a builder at '%s'\n", __func__);
        return false;
    }
    // Obtain the size of the chain to be appended
    size_t chain_size = strlen(chain);
    // Ensure there is size for N more character to be appended, otherwise resize the chain
    bool is_capacity_ensured = string_builder_ensure_capacity(string_builder, chain_size);
    // If ensuring the capacity wasn't possible, then the "append all operation" failed
    if (!is_capacity_ensured) return false;
    // Append all the chars to the builder
    char * from = chain;
    char * to = string_builder->built_chain + string_builder->used_capacity;
    memcpy(to, from, chain_size);
    // Increase the amount of used characters
    string_builder->used_capacity += chain_size;
    // Return a successful result
    return true;
}

bool string_builder_ensure_capacity(StringBuilder * string_builder, size_t chars_amount) {
    if (string_builder == NULL) {
        fprintf(stderr, "Trying to ensure the capacity of a 'NULL' builder at '%s'\n", __func__);
        return false;
    }
    if (chars_amount < 1) {
        fprintf(stderr, "The 'chars_amount' must be an integer bigger or equal to '1' at '%s'\n", __func__);
        return false;
    }
    // If there is enough capacity for N more chars, then no need to resize the chain
    if (string_builder->max_capacity - 1 >= string_builder->used_capacity + chars_amount) {
        return true;
    }
    // Otherwise, we pre-compute the new size for our chain according to our chosen strategy
    size_t new_size = string_builder_compute_new_size(string_builder, chars_amount);
    // Resize the chain according to the previously pre-computed new size
    char * resized_chain = realloc(string_builder->built_chain, sizeof(char) * new_size);
    if (resized_chain == NULL) {
        fprintf(stderr, "Unable to reallocate memory for 'resized_chain' at '%s'\n", __func__);
        return false;
    }
    // Assign the new chain and new capacity values
    string_builder->built_chain = resized_chain;
    string_builder->max_capacity = new_size;
    // Return a successful result
    return true;
}

size_t string_builder_compute_new_size(StringBuilder * string_builder, size_t chars_amount) {
    // Declare the new size
    size_t new_size = string_builder->max_capacity;
    // Always ensure one extra spot for the string 'NULL' terminator
    // Pre-compute the required size before performing a "reallocation" to prevent "overhead"
    while (new_size - 1 < string_builder->used_capacity + chars_amount) {
        // Same as "new_size = old_size * 1.5" or "new_size = (old_size * 3) / 2", where "1.5" is approximately ~ the golden ratio
        // See that we add an extra "+1" to prevent getting stuck at size = "1" (because the bit operation truncates decimals)
        new_size = ((new_size + (new_size << 1)) >> 1) + 1;
    }
    // Return the new size
    return new_size;
}

bool string_builder_remove(StringBuilder * string_builder, size_t start_index, size_t stop_index) {
    if (string_builder == NULL) {
        fprintf(stderr, "Trying to remove characters from a 'NULL' builder at '%s'\n", __func__);
        return false;
    }
    if (string_builder->used_capacity == 0) {
        fprintf(stderr, "Trying to remove characters from a empty builder at '%s'\n", __func__);
        return false;
    }
    if (start_index < 0) {
        fprintf(stderr, "The 'start_index' must not be less than '1' at '%s'\n", __func__);
        return false;
    }
    if (stop_index < 0) {
        fprintf(stderr, "The 'stop_index' must not be less than '1' at '%s'\n", __func__);
        return false;
    }
    if (stop_index >= string_builder->used_capacity) {
        fprintf(stderr, "The 'stop_index' must not be greater than the chain size at '%s'\n", __func__);
        return false;
    }
    // Start index (inclusive)
    char * start = string_builder->built_chain + start_index;
    // Stop index (inclusive)
    char * next = string_builder->built_chain + stop_index + 1;
    // Compute the amount of right side characters to shift to the left
    size_t amount_to_move = string_builder->used_capacity - (stop_index + 1);
    // Shift all the right side characters to the left
    memcpy(start, next, amount_to_move);
    // Finally, adjust the used capacity
    string_builder->used_capacity -= (stop_index - start_index) + 1;
    // Return a successful result
    return true;
}

char * string_builder_result(StringBuilder * string_builder) {
    if (string_builder == NULL) {
        fprintf(stderr, "Trying to get the result of a 'NULL' builder at '%s'\n", __func__);
        return false;
    }
    // If the chain has extra (garbage/unused characters), then resize the buffer to match the exact required size
    if (string_builder->used_capacity != string_builder->max_capacity - 1) {
        // The size with the extra 'NULL' terminator
        size_t new_size = string_builder->used_capacity + 1;
        // Attempt to resize to the used capacity
        char * resized_chain = realloc(string_builder->built_chain, sizeof(char) * new_size);
        if (resized_chain == NULL) {
            fprintf(stderr, "Unable to reallocate memory for 'resized_chain' at '%s'\n", __func__);
            return NULL;
        }
        string_builder->built_chain = resized_chain;
    }
    char * last_free_position = string_builder->built_chain + string_builder->used_capacity;
    // Append the 'NULL' terminator at the last extra character of our buffer
    (* last_free_position) = '\0';
    // Return the pointer to the built string
    return string_builder->built_chain;
}

char * string_builder_result_as_copy(StringBuilder * string_builder) {
    if (string_builder == NULL) {
        fprintf(stderr, "Trying to get a copy of the result of a 'NULL' builder at '%s'\n", __func__);
        return false;
    }
    char * copied_chain = malloc(sizeof(char) * (string_builder->used_capacity + 1));
    if (copied_chain == NULL) {
        fprintf(stderr, "Unable to allocate memory for 'copied_chain' at '%s'\n", __func__);
        return NULL;
    }
    char * copy = copied_chain;
    char * original = string_builder->built_chain;
    size_t amount_to_copy = string_builder->used_capacity;
    // Copy all the chars used in the "original chain" to the "copied chain"
    memcpy(copy, original, amount_to_copy);
    // Append the 'NULL' terminator at the last extra character
    (* (copy + amount_to_copy)) = '\0';
    return copied_chain;
}

size_t string_builder_size(StringBuilder * string_builder) {
    return string_builder->used_capacity;
}

size_t string_builder_capacity(StringBuilder * string_builder) {
    return string_builder->max_capacity;
}

void string_builder_destroy(StringBuilder * string_builder) {
    if (string_builder != NULL) {
        if (string_builder->built_chain != NULL) {
            free(string_builder->built_chain);
        }
        free(string_builder);
    }
}

void string_builder_destroy_except_chain(StringBuilder * string_builder) {
    if (string_builder != NULL) {
        free(string_builder);
    }
}
