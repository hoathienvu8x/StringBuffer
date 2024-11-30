#ifndef _STRING_BUFFER_H
#define _STRING_BUFFER_H

#include <sys/types.h>

typedef struct {
  size_t len;
  char * alloc;
  char * data;
} string_buffer_t;

string_buffer_t * string_buffer_new();
string_buffer_t * string_buffer_new_with_size(size_t n);
string_buffer_t * string_buffer_new_with_string(const char *str);
string_buffer_t * string_buffer_new_with_string_length(const char *str, size_t len);
string_buffer_t * string_buffer_new_with_copy(const char *str);
size_t string_buffer_size(const string_buffer_t *self);
size_t string_buffer_length(const string_buffer_t *self);
void string_buffer_free(string_buffer_t *self);
int string_buffer_prepend(string_buffer_t *self, const char *str);
int string_buffer_append(string_buffer_t *self, const char *str);
int string_buffer_appendf(string_buffer_t *self, const char *fmt, ...);
int string_buffer_append_n(string_buffer_t *self, const char *str, size_t len);
int string_buffer_equals(const string_buffer_t *self, const string_buffer_t *other);
ssize_t string_buffer_index(const string_buffer_t *self, const char * str);
string_buffer_t * string_buffer_slice(const string_buffer_t *self, size_t from, ssize_t to);
ssize_t string_buffer_compact(string_buffer_t *self);
void string_buffer_fill(string_buffer_t *self, int c);
void string_buffer_clear(string_buffer_t *self);
void string_buffer_ltrim(string_buffer_t *self);
void string_buffer_rtrim(string_buffer_t *self);
void string_buffer_trim(string_buffer_t *self);
#define string_buffer_final(self) (self->data)

#endif
