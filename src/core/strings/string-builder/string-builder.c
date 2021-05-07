#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "string-builder.h"

// Assertion snippet (not abstracted away for piece of code portability)

/**
 * Asserts that the condition is true, otherwise sends a failure message
 * and stops the execution returning error code 1.
 *
 * @param condition the condition that is to be met
 * @param message the error message to be printed if the condition is not met
 */
void assert(int condition, char message[]) {
    if(condition != 1) {
        printf("%s\n", message);
        exit(1);
    }
}

/*
 * A constant increment resize string builder.
 *
 * ## Explanation:
 *
 * A string builder is based on the idea of creating a mutable sequence of
 * characters, and provide utility operations that can be used to mutate
 * the string that is being built.
 *
 * As we might know, we have a problem in computer science which is that
 * once we allocate a block of memory, if we want to extend that block to
 * a bigger size, we need to allocate a new block of the size and copy
 * all the contents from the old block to the new one, then finally we
 * free the old block and use the new one instead.
 *
 * Focusing more on implementation details, there is a need to mention that
 * this implementation has some problems. Let's suppose the following
 * scenario:
 * - I = 1000 = initial capacity
 * - K = 1000 = resize increment
 *
 * Whenever we run out of space we must reallocate the memory, so the
 * formula is the following: "new size = old size + resize increment".
 *
 * The idea behind the string builder is that we append a sequence of
 * characters but when we run out of space and append the 1001th char
 * before doing so me must resize the array from 1000 to 2000 chars,
 * because of the previous formula: new size = 1000 + 1000 = 2000.
 *
 * But each time we reallocate a new string of higher size is created
 * and all contents are copied from the old string to the new one,
 * character by character, that's how reallocation works.
 *
 * The first reallocation requires us to copy K chars, the second one
 * required 2K, the third 3K, the fourth 4K, the fifth 5K and so on.
 * The total time complexity will be O(K + 2K + 3K + ... + NK), this
 * ends up in a O(N^2) time complexity. How do we know? Because we
 * use the gauss series formula that tells us that "1 + 2 + ... + N"
 * is equal to "N * (N + 1) / 2", which turns out to be "O(N^2) by
 * simplying the big O notation. Note the multiplication because that's
 * the main point of why we get the N-squared, "N * (N + 1) / 2" can
 * be simplified to "(N^2 + N) / 2", which is basically "N^2".
 *
 * If we are curious and try to build a huge string (i.e. 1 million
 * chars) we'll dicover that this implementation wastes a lot of
 * memory and time.
 *
 * Supposing that the might end up with a 1 millions chars string,
 * we can compute the amount of wasted memory using K = resize
 * increment, the gauss series will be the following:
 *
 * "1000 + 2000 + 3000 + ... + 1000000" which sums to on the order
 * of 500 billion characters of wasted memory.
 *
 * We can compute the exact amount by using the gauss formula for
 * even numbers:
 *
 * "N (a1 + aN) / 2" = "1000 * (1000 + 1000000) / 2" = "500500000"
 * wasted space in characters, the good thing is that now we know
 * that the amount of wasted space is bounded to the K resize
 * increment value, which might be useful for some use cases.
 *
 * Also note that if we got a builder of size "999000" when we
 * increase its capacity to "1000000" we need to allocate a whole
 * new array of "1000000" chars so we might end up having at the
 * same time approximately 2 million characters in memory for a
 * short period of time (until reallocation is completed).
 *
 * ## Summary
 *
 * Basic implementation strategy:
 * - Create an initial array of characters (initial length of I chars).
 * - When you run out of space and need to add more chars, reallocate to
 *   a new array with K more chars (K = constant = resize increment).
 * - The reallocation copies all the old array contents to the new block
 *   and frees the old block.
 *
 * ## Use cases
 *
 * This implementation is really good for educational purposes, but it's
 * applications are limited, unless you know the exact size of the string
 * or the expected growth so you can prevent the reallocation complexity
 * overhead (n-squared problem).
 *
 * ### Advantages
 *
 * - Simple implementation (really easy to implement)
 * - Useful when you know approximately the expected final size or growth.
 *
 * ### Disadvantages
 *
 * - High time complexity and wasted memory.
 * - Not recommended for huge strings being built.
 *
 * ### Better implementation ideas
 *
 * - Usage of a double when full strategy instead of defining a constant
 *   resize increment.
 * - Usage of a growth factor instead of a constant resize increment, the
 *   factor value is often a bit smaller than the golden mean (~1.6), most
 *   implementations use the 1.5 value.
 * - Using a linked list of blocks (this implementation only prevents the
 *   amortized complexity of reallocation, but in the end you might need to
 *   turn the whole thing into a string which can be really expensive because
 *   of the poor locality).
 *
 * @see https://stackoverflow.com/questions/10196942/how-much-to-grow-buffer-in-a-stringbuilder-like-c-module
 * @see https://stackoverflow.com/questions/9252891/big-o-what-is-the-complexity-of-summing-a-series-of-n-numbers
 * @see https://math.stackexchange.com/questions/2844825/time-complexity-from-an-arithmetic-series/2844851#2844851
 * @see https://mathbitsnotebook.com/Algebra2/Sequences/SSGauss.html
 */

// Implementation of the string builder

struct string_builder {
    char * built_chain;
    size_t initial_capacity;
    size_t used_capacity;
    size_t max_capacity;
    size_t resize_increment;
};

// Default implementation values
static const int DEFAULT_INITIAL_CAPACITY = 128;
static const int DEFAULT_RESIZE_INCREMENT = 64;

// You can also specify your custom initial capacity and resize increment
StringBuilder * string_builder_create_default() {
    return string_builder_create(DEFAULT_INITIAL_CAPACITY, DEFAULT_RESIZE_INCREMENT);
}

StringBuilder * string_builder_create(size_t initial_capacity, size_t resize_increment) {
    if(initial_capacity < 0) {
        fprintf(stderr, "The 'initial_capacity' provided to 'string_builder_create' "
                        "must be a positive integer, bigger or equal to '1'\n");
        return NULL;
    }
    if(resize_increment < 1) {
        fprintf(stderr, "The 'resize_increment' provided to 'string_builder_create' "
                        "must be a positive integer, bigger or equal to '1'\n");
        return NULL;
    }
    // We use calloc to prevent valgrind screaming about garbage values
    // Anyway we won't be reading those garbage values because we keep control of the structure
    // Can be perfectly replaced with: malloc(sizeof(char) * initial_capacity);
    char * built_chain = calloc(initial_capacity, sizeof(char));
    if(built_chain == NULL) {
        fprintf(stderr, "Unable to allocate memory for 'built_chain' at 'string_builder_create'\n");
        return NULL;
    }
    StringBuilder * string_builder = malloc(sizeof(StringBuilder));
    if(string_builder == NULL) {
        fprintf(stderr, "Unable to allocate memory for 'string_builder' at 'string_builder_create'\n");
        free(built_chain);
        return NULL;
    }
    string_builder->built_chain = built_chain;
    string_builder->initial_capacity = initial_capacity;
    string_builder->used_capacity = 0;
    string_builder->max_capacity = initial_capacity;
    string_builder->resize_increment = resize_increment;
    return string_builder;
}

void string_builder_destroy(StringBuilder * string_builder) {
    if(string_builder == NULL) return;
    if(string_builder->built_chain != NULL) free(string_builder->built_chain);
    free(string_builder);
}

bool string_builder_ensure_capacity(StringBuilder * string_builder, size_t chars_amount) {
    if(string_builder == NULL) {
        fprintf(stderr, "Trying to ensure the capacity of a NULL builder at 'string_builder_ensure_capacity'\n");
        return false;
    }
    if(chars_amount < 1) {
        fprintf(stderr, "The 'chars_amount' capacity to be ensured at 'string_builder_create' "
                        "must be a positive integer, bigger or equal to '1'\n");
    }
    // Always ensure one spot for the string null terminator
    while(string_builder->used_capacity + chars_amount > string_builder->max_capacity - 1) {
        // Increment the builder's size (new size = old size + increment)
        size_t new_size = string_builder->max_capacity + string_builder->resize_increment;
        char * resized_chain = realloc(string_builder->built_chain, sizeof(char) * new_size);
        if(resized_chain == NULL) {
            fprintf(stderr, "Unable to reallocate memory for 'resized_chain' at 'string_builder_ensure_capacity'\n");
            return false;
        }
        string_builder->built_chain = resized_chain;
        string_builder->max_capacity = new_size;
    }
    return true;
}

bool string_builder_append(StringBuilder * string_builder, char character) {
    if(string_builder == NULL) {
        fprintf(stderr, "Trying to append a character to a NULL builder at 'string_builder_append'\n");
        return false;
    }
    // Make sure the builder's capacity allows one more character (if not resize the buffer)
    bool is_capacity_ensured = string_builder_ensure_capacity(string_builder, 1);
    if(is_capacity_ensured == false) return false;
    // Get the last unused character position to which the new character is to be appended
    char * current_position = string_builder->built_chain + string_builder->used_capacity;
    (* current_position) = character;
    string_builder->used_capacity++;
    return true;
}

bool string_builder_remove(StringBuilder * string_builder, size_t start_index, size_t stop_index) {
    if(string_builder == NULL) {
        fprintf(stderr, "Trying to remove characters from a NULL builder at 'string_builder_remove'\n");
        return false;
    }
    if(start_index < 0) {
        fprintf(stderr, "Trying to pass an invalid 'start_index' value with "
                        "value less than '1' at 'string_builder_remove'\n");
        return false;
    }
    if(stop_index < 0) {
        fprintf(stderr, "Trying to pass an invalid 'stop_index' value with "
                        "value less than '1' at 'string_builder_remove'\n");
        return false;
    }
    if(stop_index > string_builder->max_capacity - 1) {
        fprintf(stderr, "Trying to pass an invalid 'stop_index' value with value "
                        "bigger than the chain size at 'string_builder_remove'\n");
        return false;
    }
    char * start = string_builder->built_chain + start_index;
    char * next = string_builder->built_chain + stop_index + 1;
    size_t left_to_move = string_builder->used_capacity - (stop_index + 1);
    // Adjust the capacity of the builder to the new one
    string_builder->used_capacity -= (stop_index - start_index) + 1;
    // Move the all the characters after the stop index to the left
    while(left_to_move != 0) {
        (* start) = (* next);
        start++;
        next++;
        left_to_move--;
    }
    return true;
}

char * string_builder_result(StringBuilder * string_builder) {
    if(string_builder == NULL) {
        fprintf(stderr, "Trying to get the result of a NULL builder at 'string_builder_result'\n");
        return false;
    }
    // If the chain is not of the proper size
    if(string_builder->used_capacity != string_builder->max_capacity - 1) {
        size_t new_size = string_builder->used_capacity + 1;
        char * resized_chain = realloc(string_builder->built_chain, sizeof(char) * new_size);
        if (resized_chain == NULL) {
            fprintf(stderr, "Unable to reallocate memory for 'resized_chain' at 'string_builder_result'\n");
            return NULL;
        }
        string_builder->built_chain = resized_chain;
    }
    // Add a null terminator to our built string
    char * next_free_position = string_builder->built_chain + string_builder->used_capacity;
    (* next_free_position) = '\0';
    return string_builder->built_chain;
}

char * string_builder_result_as_copy(StringBuilder * string_builder) {
    if(string_builder == NULL) {
        fprintf(stderr, "Trying to get a copy of the result of a NULL builder at 'string_builder_result_as_copy'\n");
        return false;
    }
    char * copy = strdup(string_builder->built_chain);
    if(copy == NULL) {
        fprintf(stderr, "Failed to allocate memory for the 'built_chain' copy at 'string_builder_result_as_copy'\n");
        return NULL;
    }
    size_t new_size = string_builder->used_capacity + 1;
    char * resized_chain = realloc(copy, sizeof(char) * new_size);
    if(resized_chain == NULL) {
        fprintf(stderr, "Unable to reallocate memory for 'resized_chain' at 'string_builder_result'\n");
        return false;
    }
    copy = resized_chain;
    // Add a null terminator to our built string
    char * current_position = copy + string_builder->used_capacity;
    (* current_position) = '\0';
    return copy;
}

// Unit testing

void string_builder_create_default_test() {
    StringBuilder * string_builder = string_builder_create_default();
    assert(string_builder != NULL, "Expected 'string_builder' not to be NULL");
    assert(string_builder->built_chain != NULL, "Expected 'built_chain' not to be NULL");
    assert(string_builder->initial_capacity == DEFAULT_INITIAL_CAPACITY, "Expected 'initial_capacity' to be equal to '128'");
    assert(string_builder->used_capacity == 0, "Expected 'used_capacity' to be equal to '0'");
    assert(string_builder->max_capacity == DEFAULT_INITIAL_CAPACITY, "Expected 'max_capacity' to be equal to '128'");
    assert(string_builder->resize_increment == DEFAULT_RESIZE_INCREMENT, "Expected 'resize_increment' to be equal to '64'");
    string_builder_destroy(string_builder);
    printf("The test 'string_builder_create_default_test' passed successfully!\n");
}

void string_builder_create_using_invalid_resize_increment_test() {
    StringBuilder * string_builder = string_builder_create(10, 0);
    assert(string_builder == NULL, "Expected the builder to be NULL");
    string_builder_destroy(string_builder);
    printf("The test 'string_builder_create_using_invalid_resize_increment_test' passed successfully!\n");
}

void string_builder_remove_test() {
    StringBuilder * string_builder = string_builder_create(2, 5);
    string_builder_append(string_builder, 'A');
    string_builder_append(string_builder, 'B');
    string_builder_append(string_builder, 'C');
    string_builder_append(string_builder, 'D');
    string_builder_append(string_builder, 'E');
    string_builder_remove(string_builder, 1, 2);
    assert((* string_builder->built_chain) == 'A', "Expected first char equal to 'A'");
    assert((* (string_builder->built_chain + 1)) == 'D', "Expected second char equal to 'D'");
    assert((* (string_builder->built_chain + 2)) == 'E', "Expected third char equal to 'E'");
    assert(string_builder->used_capacity == 3, "Expected used capacity to be equal to '3'");
    string_builder_destroy(string_builder);
    printf("The test 'string_builder_remove_test' passed successfully!\n");
}

void string_builder_ensure_capacity_test() {
    StringBuilder * string_builder = string_builder_create(5, 10);
    for(int i = 0; i < 5; i++) {
        string_builder_append(string_builder, 'A');
    }
    assert(string_builder->used_capacity == 5, "Expected used capacity to be equal to '5'");
    assert(string_builder->max_capacity == 15, "Expected max capacity to be equal to '15'");
    string_builder_destroy(string_builder);
    printf("The test 'string_builder_ensure_capacity_test' passed successfully!\n");
}

void string_builder_append_test() {
    StringBuilder * string_builder = string_builder_create(2, 5);
    string_builder_append(string_builder, 'A');
    assert((* string_builder->built_chain) == 'A', "Expected first char to be equal to 'A'");
    string_builder_append(string_builder, 'B');
    assert((* (string_builder->built_chain + 1)) == 'B', "Expected second char to be equal to 'B'");
    string_builder_append(string_builder, 'C');
    assert((* (string_builder->built_chain + 2)) == 'C', "Expected third char to be equal to 'C'");
    string_builder_append(string_builder, 'D');
    assert((* (string_builder->built_chain + 3)) == 'D', "Expected fourth char to be equal to 'D'");
    string_builder_destroy(string_builder);
    printf("The test 'string_builder_append_test' passed successfully!\n");
}

void string_builder_result_test() {
    StringBuilder * string_builder = string_builder_create_default();
    string_builder_append(string_builder, 'A');
    string_builder_append(string_builder, 'B');
    string_builder_append(string_builder, 'C');
    string_builder_append(string_builder, 'D');
    string_builder_remove(string_builder, 0, 2);
    char * given_result = string_builder_result(string_builder);
    char * expected_result = strdup("D");
    assert(strcmp(given_result, expected_result) == 0, "Expected the results to be equal");
    free(expected_result);
    string_builder_destroy(string_builder);
    printf("The test 'string_builder_result_test' passed successfully!\n");
}

void string_builder_result_as_copy_test() {
    StringBuilder * string_builder = string_builder_create_default();
    string_builder_append(string_builder, 'A');
    string_builder_append(string_builder, 'B');
    string_builder_append(string_builder, 'C');
    string_builder_append(string_builder, 'D');
    string_builder_remove(string_builder, 0, 2);
    char * given_result = string_builder_result_as_copy(string_builder);
    char * internal_result = string_builder_result(string_builder);
    assert(given_result != internal_result, "Expected the result pointers to be different");
    char * expected_result = strdup("D");
    assert(strcmp(given_result, expected_result) == 0, "Expected the results to be equal");
    free(expected_result);
    free(given_result);
    string_builder_destroy(string_builder);
    printf("The test 'string_builder_result_as_copy_test' passed successfully!\n");
}

// Tests runner

int main() {
    fclose(stderr);
    string_builder_create_default_test();
    string_builder_create_using_invalid_resize_increment_test();
    string_builder_ensure_capacity_test();
    string_builder_append_test();
    string_builder_remove_test();
    string_builder_result_test();
    string_builder_result_as_copy_test();
}
