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
 * ### Explanation:
 *
 * A string builder is based on the idea of creating a mutable sequence of
 * characters, and providing utility operations that can be used to mutate
 * the string that is being built.
 *
 * This structure is used as an auxiliary structure on several string
 * manipulation algorithms (i.e. replace all ocurrences of a string to
 * another).
 *
 * As we might know, we have a problem in computer science which is that
 * once we allocate a block of memory, if we want to extend that block to
 * a bigger size, we need to allocate a new block of the new size and copy
 * all the contents from the old block to the new one, then finally we
 * free the old block and use the new one instead.
 *
 * Focusing more on the implementation details, there is a need to mention
 * that this implementation has some problems. Let's suppose the following
 * scenario:
 *
 * - I = 1000 = initial capacity
 * - K = 1000 = resize increment
 *
 * Whenever we run out of space we must reallocate the array for more memory,
 * so the formula is the following: "new size = old size + resize increment".
 *
 * The idea behind the string builder is that we can append sequences of
 * characters, but when we run out of space and append the 1001th char
 * before doing so, we must ensure that the array has capacity for another
 * character to be added, if not we resize the array from 1000 to 2000 chars,
 * because of the previous formula: new size = 1000 + 1000 = 2000.
 *
 * But each time we reallocate a new string of higher size, a new array is
 * created and all contents are copied from the old char array to the new
 * one, character by character, that's how reallocation works.
 *
 * The first reallocation requires us to copy K chars, the second one
 * required 2K, the third 3K, the fourth 4K, the fifth 5K and so on.
 * The total time complexity will be O(K + 2K + 3K + ... + NK), this
 * ends up in an O(N^2) time complexity. How do we know that? Because we
 * use the gauss series formula that tells us that "1 + 2 + ... + N"
 * is equal to "N * (N + 1) / 2", which turns out to be "O(N^2)" by
 * simplying the big O notation. Note the multiplication, because
 * that's the main reason why we get the N-squared time, "N * (N + 1) / 2"
 * which can be simplified to "(N^2 + N) / 2", which is basically "N^2".
 *
 * If we are curious and try to build a huge string (i.e. 1 million
 * chars long string) we'll dicover that this implementation wastes a
 * lot of time and space (memory).
 *
 * Please note the word "string" and "char array" are used interchangeably.
 *
 * Supposing that the we might end up with a 1 million chars string,
 * we can compute the total amount of wasted memory by using K = resize
 * increment, the applied gauss series will be the following:
 *
 * "1000 + 2000 + 3000 + ... + 1000000" which sums to on the order
 * of 500 billion characters of wasted memory.
 *
 * We can compute the exact amount by using the gauss formula for
 * even numbers:
 *
 * "N (a1 + aN) / 2" = "1000 * (1000 + 1000000) / 2" = "500500000"
 * characters of wasted space, the good thing is that now we know
 * that the amount of wasted space is bounded to the K (resize
 * increment) value, which might be useful for some use cases.
 *
 * Also note that if we got a builder of size "999000" when we
 * increase its capacity to "1000000" we need to allocate a whole
 * new array of "1000000" chars, so we might end up having at the
 * same time approximately 2 million characters in memory for a
 * short period of time (until reallocation is completed).
 *
 * ### Summary
 *
 * Basic implementation strategy:
 * - Create an initial array of characters (initial length of I chars).
 * - When you run out of space and need to add more chars, reallocate
 *   a new array with K more chars (K = constant = resize increment).
 * - The reallocation copies all the old array contents to the new
 *   array and finally frees the old array memory.
 *
 * ### Use cases
 *
 * This implementation is really good for educational purposes, but
 * its applications are limited, unless you know the exact size of
 * the string or the expected growth, if so, you can prevent the
 * reallocation complexity overhead (n-squared problem), and then this
 * implementation is perfectly reasonable.
 *
 * ### Advantages
 *
 * - Simple implementation (really easy to implement)
 * - Useful when you know approximately the expected final size or
 *   growth of the final string length.
 *
 * ### Disadvantages
 *
 * - High time complexity and wasted memory (not the same as time complexity).
 * - Not recommended for huge strings being built because complexity get to
 *   be O(N^2) time.
 *
 * ### Better implementation ideas
 *
 * - Usage of a "double when full" strategy instead of defining a constant
 *   resize increment.
 * - Usage of a growth factor instead of a constant resize increment,
 *   this value is often similar to the golden mean (~1.6) but a little bit,
 *   smaller, most implementations use the 1.5 value.
 * - Using a "linked list of blocks" (this implementation only prevents the
 *   amortized complexity of reallocation, but in the end you always need
 *   to turn the whole thing into a string which can be really expensive
 *   because of the poor locality) but with the previous implementations you
 *   can just get the pointer and that's it.
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
    assert(string_builder != NULL, "Expected 'string_builder' not to be NULL at 'string_builder_create_default_test'");
    assert(string_builder->built_chain != NULL, "Expected builder's 'built_chain' not to be NULL at 'string_builder_create_default_test'");
    assert(string_builder->initial_capacity == DEFAULT_INITIAL_CAPACITY, "Expected builder's 'initial_capacity' to be equal to 'DEFAULT_INITIAL_CAPACITY' at 'string_builder_create_default_test'");
    assert(string_builder->used_capacity == 0, "Expected builder's 'used_capacity' to be equal to '0' at 'string_builder_create_default_test'");
    assert(string_builder->max_capacity == DEFAULT_INITIAL_CAPACITY, "Expected builder's 'max_capacity' to be equal to 'DEFAULT_INITIAL_CAPACITY' at 'string_builder_create_default_test'");
    assert(string_builder->resize_increment == DEFAULT_RESIZE_INCREMENT, "Expected builder's 'resize_increment' to be equal to 'DEFAULT_RESIZE_INCREMENT' at 'string_builder_create_default_test'");
    string_builder_destroy(string_builder);
    printf("The test 'string_builder_create_default_test' passed successfully!\n");
}

void string_builder_create_with_custom_values_test() {
    size_t initial_capacity = 16;
    size_t resize_increment = 32;
    StringBuilder * string_builder = string_builder_create(initial_capacity, resize_increment);
    assert(string_builder != NULL, "Expected 'string_builder' not to be NULL at 'string_builder_create_with_custom_values_test'");
    assert(string_builder->built_chain != NULL, "Expected builder's 'built_chain' not to be NULL at 'string_builder_create_with_custom_values_test'");
    assert(string_builder->initial_capacity == initial_capacity, "Expected builder's 'initial_capacity' to be equal to 'initial_capacity' at 'string_builder_create_with_custom_values_test'");
    assert(string_builder->used_capacity == 0, "Expected builder's 'used_capacity' to be equal to '0' at 'string_builder_create_with_custom_values_test'");
    assert(string_builder->max_capacity == initial_capacity, "Expected builder's 'max_capacity' to be equal to 'initial_capacity' at 'string_builder_create_with_custom_values_test'");
    assert(string_builder->resize_increment == resize_increment, "Expected builder's 'resize_increment' to be equal to 'resize_increment' at 'string_builder_create_with_custom_values_test'");
    string_builder_destroy(string_builder);
    printf("The test 'string_builder_create_with_custom_values_test' passed successfully!\n");
}

void string_builder_create_with_zero_resize_increment_test() {
    size_t initial_capacity = 16;
    size_t resize_increment = 0;
    StringBuilder * string_builder = string_builder_create(initial_capacity, resize_increment);
    assert(string_builder == NULL, "Expected 'string_builder' to be NULL at 'string_builder_create_with_zero_resize_increment_test'");
    printf("The test 'string_builder_create_with_zero_resize_increment_test' passed successfully!\n");
}

void string_builder_remove_test() {
    StringBuilder * string_builder = string_builder_create(1, 5);
    // Append the example sampleText character by character to the builder
    char sampleText[] = "Hello world, I am a fancy string builder";
    char * charTraverser;
    for(charTraverser = sampleText; * charTraverser != '\0'; charTraverser++) {
        string_builder_append(string_builder, (*charTraverser));
    }
    // Remove the "Hello world, " part
    string_builder_remove(string_builder, 0, 12);
    // The internal result must be equal to "I am a fancy string builder"
    char * internal_result = string_builder_result(string_builder);
    assert(strcmp(internal_result, "I am a fancy string builder") == 0, "Expected builder's 'internal_result' to be equal to 'I am a fancy string builder' at 'string_builder_remove_test'");
    // The length of the result string "I am a fancy string builder" must be equal to 27
    assert(string_builder->used_capacity == 27, "Expected builder's 'used_capacity' to be equal to '27' at 'string_builder_remove_test'");
    string_builder_destroy(string_builder);
    printf("The test 'string_builder_remove_test' passed successfully!\n");
}

void string_builder_remove_multiple_times_test() {
    StringBuilder * string_builder = string_builder_create(4, 8);
    // Append the example sampleText character by character to the builder
    char sampleText[] = "Hello world, I am a fancy string builder";
    char * charTraverser;
    for(charTraverser = sampleText; * charTraverser != '\0'; charTraverser++) {
        string_builder_append(string_builder, (*charTraverser));
    }
    // Remove "Hello world, " and "I am a fancy string builder" is left
    string_builder_remove(string_builder, 0, 12);
    // Remove " a" and "I am fancy string builder" is left
    string_builder_remove(string_builder, 4, 5);
    // Remove " string builder" and "I am fancy" is left
    string_builder_remove(string_builder, 10, 24);
    // The internal result must be equal to "I am fancy"
    char * internal_result = string_builder_result(string_builder);
    assert(strcmp(internal_result, "I am fancy") == 0, "Expected builder's 'internal_result' to be equal to 'I am fancy' at 'string_builder_remove_multiple_times_test'");
    // The length of the result string "I am fancy" must be equal to 10
    assert(string_builder->used_capacity == 10, "Expected builder's 'used_capacity' to be equal to '10' at 'string_builder_remove_multiple_times_test'");
    string_builder_destroy(string_builder);
    printf("The test 'string_builder_remove_multiple_times_test' passed successfully!\n");
}

void string_builder_ensure_capacity_test() {
    StringBuilder * string_builder = string_builder_create(5, 10);
    for(int i = 0; i < 15; i++) {
        string_builder_append(string_builder, 'A');
    }
    assert(string_builder->used_capacity == 15, "Expected builder's 'used_capacity' to be equal to '15' at 'string_builder_ensure_capacity_test'");
    assert(string_builder->max_capacity == 25, "Expected builder's 'max_capacity' to be equal to '25' at 'string_builder_ensure_capacity_test'");
    string_builder_destroy(string_builder);
    printf("The test 'string_builder_ensure_capacity_test' passed successfully!\n");
}

void string_builder_append_test() {
    StringBuilder * string_builder = string_builder_create(2, 5);
    // Append the example name character by character to the builder
    char name[] = "John";
    char * charTraverser;
    for(charTraverser = name; * charTraverser != '\0'; charTraverser++) {
        string_builder_append(string_builder, (*charTraverser));
    }
    // Append a space separator
    string_builder_append(string_builder, ' ');
    // Append the example surname character by character to the builder
    char surname[] = "Smith";
    for(charTraverser = surname; * charTraverser != '\0'; charTraverser++) {
        string_builder_append(string_builder, (*charTraverser));
    }
    // The internal result must be equal to "John Smith"
    char * internal_result = string_builder_result(string_builder);
    assert(strcmp(internal_result, "John Smith") == 0, "Expected builder's 'internal_result' to be equal to 'John Smith' at 'string_builder_append_test'");
    string_builder_destroy(string_builder);
    printf("The test 'string_builder_append_test' passed successfully!\n");
}

void string_builder_result_test() {
    StringBuilder * string_builder = string_builder_create_default();
    // Append the example sampleText character by character to the builder
    char sampleText[] = "Spiderman";
    char * charTraverser;
    for(charTraverser = sampleText; * charTraverser != '\0'; charTraverser++) {
        string_builder_append(string_builder, (*charTraverser));
    }
    // The internal result must be equal to "Spiderman"
    char * internal_result = string_builder_result(string_builder);
    assert(strcmp(internal_result, "Spiderman") == 0, "Expected builder's 'internal_result' to be equal to 'Spiderman' at 'string_builder_result_test'");
    string_builder_destroy(string_builder);
    printf("The test 'string_builder_result_test' passed successfully!\n");
}

void string_builder_result_as_copy_test() {
    StringBuilder * string_builder = string_builder_create_default();
    // Append the example sampleText character by character to the builder
    char sampleText[] = "Extra-Ordinary Men";
    char * charTraverser;
    for(charTraverser = sampleText; * charTraverser != '\0'; charTraverser++) {
        string_builder_append(string_builder, (*charTraverser));
    }
    // The internal result must be equal to "Extra-Ordinary Men"
    char * result_as_copy = string_builder_result_as_copy(string_builder);
    assert(result_as_copy != NULL, "Expected 'result_as_copy' not to be NULL at 'string_builder_result_as_copy_test'");
    assert(strcmp(result_as_copy, "Extra-Ordinary Men") == 0, "Expected 'result_as_copy' to be equal to 'Extra-Ordinary Men' at 'string_builder_result_as_copy_test'");
    free(result_as_copy);
    string_builder_destroy(string_builder);
    printf("The test 'string_builder_result_as_copy_test' passed successfully!\n");
}

// Tests runner

int main() {
    fclose(stderr);
    string_builder_create_default_test();
    string_builder_create_with_custom_values_test();
    string_builder_create_with_zero_resize_increment_test();
    string_builder_ensure_capacity_test();
    string_builder_append_test();
    string_builder_remove_test();
    string_builder_remove_multiple_times_test();
    string_builder_result_test();
    string_builder_result_as_copy_test();
}
