#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "string-builder.h"

/*
 * A Non Thread-Safe String Builder Implementation (with golden ratio as resize strategy).
 *
 * ### Explanation ###
 *
 * A string builder DS based on the idea of creating a mutable sequence of characters and providing utility operations
 * that can be used to mutate the string that is being built. In reality, the underlying structure is static, but it
 * uses dynamic memory to reallocate the memory section to a bigger one if more space is necessary.
 *
 * This structure is used as an auxiliary structure on several string algorithms (i.e., 'replace all' algorithm).
 *
 * In computer science we have kind of a problem, because once we allocate a block of memory, if we want to extend that
 * block to a larger size (for more space), we can't, we need to allocate a new block of the new larger size and copy
 * all the contents from the old block to the new one, finally freeing the old block contents and using the new one.
 *
 * ### Strategies ###
 *
 * There are several implementation strategies for this data structure, when we talk about a "strategy" we mean the
 * "reallocation strategy", to what "new size" resize the "old size"? what "increment" to use?
 *
 * The simplest strategy one might think of may be use of this formula: "new size = old size + constant", where the
 * constant is simply a constant amount of space to be incremented each time we run out of space.
 *
 * #### Basic Strategy - Constant Resize Increment ####
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
 * Remember that each time we ensure the capacity, if no more chars fit, we create a new array of higher size, and
 * all contents are copied from the old char array to the new one, character by character.
 *
 * The first reallocation requires us to copy K chars, the second one required 2K, the third 3K, the fourth 4K, the
 * fifth 5K and so on. The total time complexity will be O(K + 2K + 3K + ... + NK), this ends up in an O(N^2) worst
 * time complexity. How do we know that? Because we use the gauss series formula that tells us that "1 + 2 + ... + N"
 * is equal to "N * (N + 1) / 2", which turns out to be "O(N^2)", and by simplifying the big O notation. Note the
 * multiplication, because that's the main reason why we get the N-squared time, "N * (N + 1) / 2" which can be
 * simplified to "(N^2 + N) / 2", which is basically "N^2".
 *
 * If we are curious and try to build a huge string (i.e. 1 million chars long string) we'll discover that this
 * implementation wastes a lot of time and space (memory). Please note that here the word "string" and "char array"
 * are used interchangeably.
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
 * The good thing is that now we know that the amount of wasted space is bounded to the K (resize increment) value,
 * which might be useful for some use cases. Also note that if we got a builder of size "999000" when we increase
 * its capacity to "1000000" we need to allocate a whole new array of "1000000" chars, so we might end up having at
 * the same time approximately 2 million characters in memory for a short period of time (until reallocation is
 * completed).
 *
 * As we have seen this basic strategy is really simple to implement, and it's useful when you know approximately
 * the expected final size or growth of the final string, but usually we don't know this metric.
 *
 * #### This Implementation Strategy - Golden Ratio ####
 *
 * This implementation uses the godel ratio as a growth factor, instead of a constant resize increment, this value is
 * often similar to the golden mean (~1.6) but a little bit smaller, most implementations use the 1.5 value and this
 * works great in practice.
 *
 * #### Other Strategies ####
 *
 * - Usage of a "double when full" strategy instead of defining a constant resize increment.
 * - Using a "linked list of blocks" (this implementation only prevents the amortized complexity of reallocation, but
 *   in the end you always need to turn the whole thing into a string which can be really expensive because of the poor
 *   locality).
 *
 * ### References ###
 *
 * @see https://stackoverflow.com/questions/10196942/how-much-to-grow-buffer-in-a-stringbuilder-like-c-module
 * @see https://stackoverflow.com/questions/9252891/big-o-what-is-the-complexity-of-summing-a-series-of-n-numbers
 * @see https://math.stackexchange.com/questions/2844825/time-complexity-from-an-arithmetic-series/2844851#2844851
 * @see https://mathbitsnotebook.com/Algebra2/Sequences/SSGauss.html
 * @see https://news.ycombinator.com/item?id=8555550
 */

// Declaration of internal methods (to be able to order them in top-bottom style)

bool string_builder_ensure_capacity(StringBuilder * string_builder, size_t chars_amount);
void string_builder_write_unsafe(StringBuilder * string_builder, char character);
size_t string_builder_compute_new_size(StringBuilder * string_builder, size_t chars_amount);

// Implementation of the string builder

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
    return string_builder;
}

bool string_builder_append_one(StringBuilder * string_builder, char character) {
    if (string_builder == NULL) {
        fprintf(stderr, "Trying to append a character to a 'NULL' builder at '%s'\n", __func__);
        return false;
    }
    // Ensure that the builder capacity allows one more character to be appended, otherwise resize the chain
    bool is_capacity_ensured = string_builder_ensure_capacity(string_builder, 1);
    // If ensuring the capacity wasn't possible, then the "append" operation failed
    if (!is_capacity_ensured) return false;
    // We can use the "write unsafe" operation because we've ensured the capacity and the builder non-nullability
    string_builder_write_unsafe(string_builder, character);
    // Return a successful append result
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
    // Ensure that the builder capacity allows for N more characters to be appended, otherwise resize the chain
    bool is_capacity_ensured = string_builder_ensure_capacity(string_builder, strlen(chain));
    // If ensuring the capacity wasn't possible, then the "append all" operation failed
    if (!is_capacity_ensured) return false;
    // Append all the chars
    for (char * current = chain; * current != '\0'; current++) {
        // We can use the "write unsafe" operation because we've ensured the capacity and the builder non-nullability
        string_builder_write_unsafe(string_builder, (*current));
    }
    // Return a successful append all result
    return true;
}

/**
 * @note the PRECONDITIONS to use this method are: to have a free slot to append one char and the builder non-nullability
 */
void string_builder_write_unsafe(StringBuilder * string_builder, char character) {
    // Get the last unused character position, which is where the new character is to be appended
    char * last_unused = string_builder->built_chain + string_builder->used_capacity;
    // Assign the character value
    (* last_unused) = character;
    // Increase the amount of used characters
    string_builder->used_capacity++;
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
    // If there is enough capacity for the requested characters, then there's no need to perform a reallocation
    if (string_builder->max_capacity - 1 >= string_builder->used_capacity + chars_amount) {
        return true;
    }
    // Otherwise, we pre-compute the new size for our chain according to our chosen strategy
    size_t new_size = string_builder_compute_new_size(string_builder, chars_amount);
    // Resize the chain using the previously pre-computed new size
    char * resized_chain = realloc(string_builder->built_chain, sizeof(char) * new_size);
    if (resized_chain == NULL) {
        fprintf(stderr, "Unable to reallocate memory for 'resized_chain' at '%s'\n", __func__);
        return false;
    }
    // Assign the new chain and new capacity values
    string_builder->built_chain = resized_chain;
    string_builder->max_capacity = new_size;
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
    while (amount_to_move != 0) {
        (* start) = (* next);
        start++;
        next++;
        amount_to_move--;
    }
    // Finally, adjust the used capacity
    string_builder->used_capacity -= (stop_index - start_index) + 1;
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
    size_t left_to_copy = string_builder->used_capacity;
    // Copy all the chars used in the "original chain" to the "copied chain"
    while (left_to_copy != 0) {
        (* copy) = (* original);
        copy++;
        original++;
        left_to_copy--;
    }
    // Append the 'NULL' terminator at the last extra character
    (* copy) = '\0';
    return copied_chain;
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

// Assertion snippet (not abstracted away for piece of code portability)
void assert(int condition, char message[]) {
    if (condition != 1) {
        printf("%s\n", message);
        exit(1);
    }
}

// Unit testing

void string_builder_create_default_test() {
    printf("*** Running test '%s'\n", __func__);
    StringBuilder * string_builder = string_builder_create_default();
    if (string_builder == NULL) exit(1);
    assert(string_builder != NULL, "The 'string_builder' must not be null");
    assert(string_builder->built_chain != NULL, "The 'string_builder->built_chain' must not be null");
    assert(string_builder->used_capacity == 0, "The 'string_builder->used_capacity' must be equal to zero");
    assert(string_builder->max_capacity == DEFAULT_INITIAL_CAPACITY, "The 'string_builder->max_capacity' must be equal to 'DEFAULT_INITIAL_CAPACITY'");
    string_builder_destroy(string_builder);
}

void string_builder_create_with_custom_capacity_test() {
    printf("*** Running test '%s'\n", __func__);
    StringBuilder * string_builder = string_builder_create(0);
    assert(string_builder != NULL, "The 'string_builder' must not be null");
    assert(string_builder->built_chain != NULL, "The 'string_builder->built_chain' must not be null");
    assert(string_builder->used_capacity == 0, "The 'string_builder->used_capacity' must be equal to zero");
    assert(string_builder->max_capacity == 0, "The 'string_builder->max_capacity' must be equal to '0'");
    string_builder_destroy(string_builder);
}

void string_builder_remove_test() {
    printf("*** Running test '%s'\n", __func__);
    char input[] = "Hello world, I am a fancy string builder";
    char expected[] = "I am a fancy string builder";
    StringBuilder * string_builder = string_builder_create(1);
    string_builder_append_all(string_builder, input);
    string_builder_remove(string_builder, 0, 12); // Delete piece "Hello world, "
    char * given = string_builder_result(string_builder);
    assert(strcmp(given, expected) == 0, "The 'string_builder->built_chain' does not match expected chain");
    assert(string_builder->used_capacity == strlen(given), "The 'string_builder->used_capacity' does not match expected length");
    string_builder_destroy(string_builder);
}

void string_builder_remove_from_empty_test() {
    printf("*** Running test '%s'\n", __func__);
    StringBuilder * string_builder = string_builder_create_default();
    bool success = string_builder_remove(string_builder, 0, 0);
    assert(success == false, "The remove operation must have thrown an error");
    string_builder_destroy(string_builder);
}

void string_builder_remove_edge_case_test() {
    printf("*** Running test '%s'\n", __func__);
    char expected[] = "H";
    StringBuilder * string_builder = string_builder_create_default();
    string_builder_append_all(string_builder, "H");
    string_builder_remove(string_builder, 0, 1);
    assert(strcmp(string_builder_result(string_builder), expected) == 0, "The 'string_builder->built_chain' does not match expected chain"); // Must remain the same
    string_builder_remove(string_builder, 0, 0);
    assert(strcmp(string_builder_result(string_builder), "") == 0, "The 'string_builder->built_chain' does not match expected chain"); // Must be empty
    string_builder_destroy(string_builder);
}

void string_builder_remove_multiple_times_test() {
    printf("*** Running test '%s'\n", __func__);
    char input[] = "Hello world, I am a fancy string builder";
    char expected[] = "I am fancy";
    StringBuilder * string_builder = string_builder_create(4);
    string_builder_append_all(string_builder, input);
    string_builder_remove(string_builder, 0, 12); // Delete piece "Hello world, "
    string_builder_remove(string_builder, 4, 5); // Delete piece " a"
    string_builder_remove(string_builder, 10, 24); // Delete piece " string builder"
    char * given = string_builder_result(string_builder);
    assert(strcmp(given, expected) == 0, "The 'string_builder->built_chain' does not match expected chain");
    assert(string_builder->used_capacity == strlen(given), "The 'string_builder->used_capacity' does not match expected length");
    string_builder_destroy(string_builder);
}

void string_builder_ensure_capacity_test() {
    printf("*** Running test '%s'\n", __func__);
    char input[] = "AAAAAAAAAAAAAAA";
    StringBuilder * string_builder = string_builder_create(1);
    string_builder_append_all(string_builder, input);
    char * given = string_builder_result(string_builder);
    assert(strcmp(given, input) == 0, "The 'string_builder->built_chain' does not match expected chain");
    assert(string_builder->used_capacity == strlen(input), "The 'string_builder->used_capacity' does not match expected length");
    assert(string_builder->max_capacity == 17, "The 'string_builder->max_capacity' does not match expected value"); // Resizes: 2, 4, 7, 11, [17], 26, 40, 61
    string_builder_destroy(string_builder);
}

void string_builder_append_test() {
    printf("*** Running test '%s'\n", __func__);
    char expected[] = "John Smith";
    StringBuilder * string_builder = string_builder_create(1);
    string_builder_append_all(string_builder, "John");
    string_builder_append_one(string_builder, ' ');
    string_builder_append_all(string_builder, "Smith");
    char * given = string_builder_result(string_builder);
    assert(strcmp(given, expected) == 0, "The 'string_builder->built_chain' does not match expected chain");
    string_builder_destroy(string_builder);
}

void string_builder_result_test() {
    printf("*** Running test '%s'\n", __func__);
    char input[] = "Spiderman";
    StringBuilder * string_builder = string_builder_create_default();
    string_builder_append_all(string_builder, input);
    char * given = string_builder_result(string_builder);
    assert(strcmp(given, input) == 0, "The 'string_builder->built_chain' does not match expected chain");
    string_builder_destroy(string_builder);
}

void string_builder_result_as_copy_test() {
    printf("*** Running test '%s'\n", __func__);
    char input[] = "Extra-Ordinary Men";
    StringBuilder * string_builder = string_builder_create_default();
    string_builder_append_all(string_builder, input);
    char * given = string_builder_result_as_copy(string_builder);
    assert(given != NULL, "The 'string_builder->built_chain' copy must not be null");
    assert(strcmp(given, input) == 0, "The 'string_builder->built_chain' does not match expected chain");
    string_builder_destroy(string_builder);
    free(given);
}

void string_builder_destroy_except_chain_test() {
    printf("*** Running test '%s'\n", __func__);
    char input[] = "Don't think you will forgive you";
    StringBuilder * string_builder = string_builder_create_default();
    string_builder_append_all(string_builder, input);
    char * given = string_builder_result(string_builder);
    assert(strcmp(given, input) == 0, "The 'string_builder->built_chain' does not match expected chain 1");
    string_builder_destroy_except_chain(string_builder);
    assert(strcmp(given, input) == 0, "The 'string_builder->built_chain' does not match expected chain 2");
    free(given); // We need to free it explicitly after usage (that's the drawback of this destroy method)
}

// Tests runner

int main() {
    fclose(stderr); // Prevent printing "expected" errors
    string_builder_create_default_test();
    string_builder_create_with_custom_capacity_test();
    string_builder_ensure_capacity_test();
    string_builder_append_test();
    string_builder_remove_test();
    string_builder_remove_from_empty_test();
    string_builder_remove_edge_case_test();
    string_builder_remove_multiple_times_test();
    string_builder_result_test();
    string_builder_result_as_copy_test();
    string_builder_destroy_except_chain_test();
}
