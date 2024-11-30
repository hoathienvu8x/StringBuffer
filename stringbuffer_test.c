#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stringbuffer.h"

#ifndef STRING_BUFFER_DEFAULT_SIZE
  #define STRING_BUFFER_DEFAULT_SIZE 64
#endif

void
equal(char *a, char *b) {
  if (strcmp(a, b)) {
    printf("\n");
    printf("  expected: '%s'\n", a);
    printf("    actual: '%s'\n", b);
    printf("\n");
    exit(1);
  }
}

void
test_string_buffer_new() {
  printf("test_string_buffer_new() -> ");
  string_buffer_t *buf = string_buffer_new();
  assert(STRING_BUFFER_DEFAULT_SIZE == string_buffer_size(buf));
  assert(0 == string_buffer_length(buf));
  string_buffer_free(buf);
  printf("OK\n");
}

void
test_string_buffer_new_with_size() {
  printf("test_string_buffer_new_with_size() -> ");
  string_buffer_t *buf = string_buffer_new_with_size(1024);
  assert(1024 == string_buffer_size(buf));
  assert(0 == string_buffer_length(buf));
  string_buffer_free(buf);
  printf("OK\n");
}

void
test_string_buffer_append() {
  printf("test_string_buffer_append() -> ");
  string_buffer_t *buf = string_buffer_new();
  assert(0 == string_buffer_append(buf, "Hello"));
  assert(0 == string_buffer_append(buf, " World"));
  assert(strlen("Hello World") == string_buffer_length(buf));
  equal("Hello World", string_buffer_final(buf));
  string_buffer_free(buf);
  printf("OK\n");
}

void
test_string_buffer_append_n() {
  printf("test_string_buffer_append_n() -> ");
  string_buffer_t *buf = string_buffer_new();
  assert(0 == string_buffer_append_n(buf, "subway", 3));
  assert(0 == string_buffer_append_n(buf, "marines", 6));
  assert(strlen("submarine") == string_buffer_length(buf));
  equal("submarine", string_buffer_final(buf));
  string_buffer_free(buf);
  printf("OK\n");
}

void
test_string_buffer_append__grow() {
  printf("test_string_buffer_append__grow() -> ");
  string_buffer_t *buf = string_buffer_new_with_size(10);
  assert(0 == string_buffer_append(buf, "Hello"));
  assert(0 == string_buffer_append(buf, " tobi"));
  assert(0 == string_buffer_append(buf, " was"));
  assert(0 == string_buffer_append(buf, " here"));

  char *str = "Hello tobi was here";
  equal(str, string_buffer_final(buf));
  assert(1024 == string_buffer_size(buf));
  assert(strlen(str) == string_buffer_length(buf));
  string_buffer_free(buf);
  printf("OK\n");
}

void
test_string_buffer_prepend() {
  printf("test_string_buffer_prepend() -> ");
  string_buffer_t *buf = string_buffer_new();
  assert(0 == string_buffer_append(buf, " World"));
  assert(0 == string_buffer_prepend(buf, "Hello"));
  assert(strlen("Hello World") == string_buffer_length(buf));
  equal("Hello World", string_buffer_final(buf));
  string_buffer_free(buf);
  printf("OK\n");
}

void
test_string_buffer_slice() {
  printf("test_string_buffer_slice() -> ");
  string_buffer_t *buf = string_buffer_new();
  string_buffer_append(buf, "Tobi Ferret");

  string_buffer_t *a = string_buffer_slice(buf, 2, 8);
  equal("Tobi Ferret", string_buffer_final(buf));
  equal("bi Fer", string_buffer_final(a));

  string_buffer_free(buf);
  string_buffer_free(a);
  printf("OK\n");
}

void
test_string_buffer_slice__range_error() {
  printf("test_string_buffer_slice__range_error() -> ");
  string_buffer_t *buf = string_buffer_new_with_copy("Tobi Ferret");
  string_buffer_t *a = string_buffer_slice(buf, 10, 2);
  assert(NULL == a);
  string_buffer_free(buf);
  printf("OK\n");
}

void
test_string_buffer_slice__end() {
  printf("test_string_buffer_slice__end() -> ");
  string_buffer_t *buf = string_buffer_new_with_copy("Tobi Ferret");

  string_buffer_t *a = string_buffer_slice(buf, 5, -1);
  equal("Tobi Ferret", string_buffer_final(buf));
  equal("Ferret", string_buffer_final(a));

  string_buffer_t *b = string_buffer_slice(buf, 5, -3);
  equal("Ferr", string_buffer_final(b));

  string_buffer_t *c = string_buffer_slice(buf, 8, -1);
  equal("ret", string_buffer_final(c));

  string_buffer_free(buf);
  string_buffer_free(a);
  string_buffer_free(b);
  string_buffer_free(c);
  printf("OK\n");
}

void
test_string_buffer_slice__end_overflow() {
  printf("test_string_buffer_slice__end_overflow() -> ");
  string_buffer_t *buf = string_buffer_new_with_copy("Tobi Ferret");
  string_buffer_t *a = string_buffer_slice(buf, 5, 1000);
  equal("Tobi Ferret", string_buffer_final(buf));
  equal("Ferret", string_buffer_final(a));
  string_buffer_free(a);
  string_buffer_free(buf);
  printf("OK\n");
}

void
test_string_buffer_equals() {
  printf("test_string_buffer_equals() -> ");
  string_buffer_t *a = string_buffer_new_with_copy("Hello");
  string_buffer_t *b = string_buffer_new_with_copy("Hello");

  assert(1 == string_buffer_equals(a, b));

  string_buffer_append(b, " World");
  assert(0 == string_buffer_equals(a, b));

  string_buffer_free(a);
  string_buffer_free(b);
  printf("OK\n");
}

void test_string_buffer_formatting() {
  printf("test_string_buffer_formatting() -> ");
  string_buffer_t *buf = string_buffer_new();
  int result = string_buffer_appendf(buf, "%d %s", 3, "cow");
  assert(0 == result);
  equal("3 cow", string_buffer_final(buf));
  result = string_buffer_appendf(buf, " - 0x%08X", 0xdeadbeef);
  assert(0 == result);
  equal("3 cow - 0xDEADBEEF", string_buffer_final(buf));
  string_buffer_free(buf);
  printf("OK\n");
}

void
test_string_buffer_index() {
  printf("test_string_buffer_index() -> ");
  string_buffer_t *buf = string_buffer_new_with_copy("Tobi is a ferret");

  ssize_t i = string_buffer_index(buf, "is");
  assert(5 == i);

  i = string_buffer_index(buf, "a");
  assert(8 == i);

  i = string_buffer_index(buf, "something");
  assert(-1 == i);

  string_buffer_free(buf);
  printf("OK\n");
}

void
test_string_buffer_fill() {
  printf("test_string_buffer_fill() -> ");
  string_buffer_t *buf = string_buffer_new_with_copy("Hello");
  assert(5 == string_buffer_length(buf));

  string_buffer_fill(buf, 0);
  assert(0 == string_buffer_length(buf));
  string_buffer_free(buf);
  printf("OK\n");
}

void
test_string_buffer_clear() {
  printf("test_string_buffer_clear() -> ");
  string_buffer_t *buf = string_buffer_new_with_copy("Hello");
  assert(5 == string_buffer_length(buf));

  string_buffer_clear(buf);
  assert(0 == string_buffer_length(buf));
  string_buffer_free(buf);
  printf("OK\n");
}

void
test_string_buffer_trim() {
  printf("test_string_buffer_trim()\n");
  string_buffer_t *buf = string_buffer_new_with_copy("  Hello\n\n ");
  string_buffer_trim(buf);
  printf("1. string_buffer_trim() -> ");
  equal("Hello", string_buffer_final(buf));
  string_buffer_free(buf);
  printf("OK\n");

  buf = string_buffer_new_with_copy("  Hello\n\n ");
  string_buffer_ltrim(buf);
  printf("2. string_buffer_ltrim() -> ");
  equal("Hello\n\n ", string_buffer_final(buf));
  string_buffer_free(buf);
  printf("OK\n");

  buf = string_buffer_new_with_copy("  Hello\n\n ");
  string_buffer_rtrim(buf);
  printf("3. string_buffer_rtrim() -> ");
  equal("  Hello", string_buffer_final(buf));
  string_buffer_free(buf);
  printf("OK\n");
}

void
test_string_buffer_compact() {
  printf("test_string_buffer_compact() -> ");
  string_buffer_t *buf = string_buffer_new_with_copy("  Hello\n\n ");
  string_buffer_trim(buf);
  assert(5 == string_buffer_length(buf));
  assert(10 == string_buffer_size(buf));

  ssize_t removed = string_buffer_compact(buf);
  assert(5 == removed);
  assert(5 == string_buffer_length(buf));
  assert(5 == string_buffer_size(buf));
  equal("Hello", string_buffer_final(buf));

  string_buffer_free(buf);
  printf("OK\n");
}

int
main(){
  test_string_buffer_new();
  test_string_buffer_new_with_size();
  test_string_buffer_append();
  test_string_buffer_append__grow();
  test_string_buffer_append_n();
  test_string_buffer_prepend();
  test_string_buffer_slice();
  test_string_buffer_slice__range_error();
  test_string_buffer_slice__end();
  test_string_buffer_slice__end_overflow();
  test_string_buffer_equals();
  test_string_buffer_formatting();
  test_string_buffer_index();
  test_string_buffer_fill();
  test_string_buffer_clear();
  test_string_buffer_trim();
  test_string_buffer_compact();
  printf("\n  \033[32m\u2713 \033[90mok\033[0m\n\n");
  return 0;
}
