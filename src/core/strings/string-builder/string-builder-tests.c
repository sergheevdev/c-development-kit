#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "string-builder.h"

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
    assert(string_builder_result(string_builder) != NULL, "The 'string_builder' result must not be null");
    assert(string_builder_size(string_builder) == 0, "The 'string_builder' size must be equal to zero");
    assert(string_builder_capacity(string_builder) == 16, "The 'string_builder' capacity must be equal to '16'");
    string_builder_destroy(string_builder);
}

void string_builder_create_with_custom_capacity_test() {
    printf("*** Running test '%s'\n", __func__);
    StringBuilder * string_builder = string_builder_create(0);
    assert(string_builder != NULL, "The 'string_builder' must not be null");
    assert(string_builder_result(string_builder) != NULL, "The 'string_builder' result must not be null");
    assert(string_builder_size(string_builder) == 0, "The 'string_builder' size must be equal to zero");
    assert(string_builder_capacity(string_builder) == 0, "The 'string_builder' capacity must be equal to zero");
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
    assert(string_builder_size(string_builder) == strlen(given), "The 'string_builder->used_capacity' does not match expected length");
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
    assert(string_builder_size(string_builder) == strlen(given), "The 'string_builder->used_capacity' does not match expected length");
    string_builder_destroy(string_builder);
}

void string_builder_ensure_capacity_test() {
    printf("*** Running test '%s'\n", __func__);
    char input[] = "AAAAAAAAAAAAAAA";
    StringBuilder * string_builder = string_builder_create(1);
    string_builder_append_all(string_builder, input);
    char * given = string_builder_result(string_builder);
    assert(strcmp(given, input) == 0, "The 'string_builder->built_chain' does not match expected chain");
    assert(string_builder_size(string_builder) == strlen(input), "The 'string_builder->used_capacity' does not match expected length");
    assert(string_builder_capacity(string_builder) == 17, "The 'string_builder->max_capacity' does not match expected value"); // Resizes: 2, 4, 7, 11, [17], 26, 40, 61
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
