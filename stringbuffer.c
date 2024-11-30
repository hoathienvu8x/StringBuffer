#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include "stringbuffer.h"

#ifndef STRING_BUFFER_DEFAULT_SIZE
  #define STRING_BUFFER_DEFAULT_SIZE 64
#endif

#define nearest_multiple_of(a, b) (((b) + ((a) - 1)) & ~((a) - 1))

string_buffer_t * string_buffer_new() {
  return string_buffer_new_with_size(STRING_BUFFER_DEFAULT_SIZE);
}
string_buffer_t * string_buffer_new_with_size(size_t n) {
  string_buffer_t *self = malloc(sizeof(string_buffer_t));
  if (!self) return NULL;
  self->len = n;
  self->data = self->alloc = calloc(n + 1, 1);
  if (self->alloc) return self;
  free(self);
  return NULL;
}
string_buffer_t * string_buffer_new_with_string(const char *str) {
  if (!str) return NULL;
  return string_buffer_new_with_string_length(str, strlen(str));
}
string_buffer_t * string_buffer_new_with_string_length(const char *str, size_t len) {
  string_buffer_t *self = NULL;
  if (len == 0) return self;
  self = string_buffer_new_with_size(len);
  if (!self) return NULL;
  if (memcpy(self->alloc, str, len)) {
    return self;
  }
  string_buffer_free(self);
  return NULL;
}
string_buffer_t * string_buffer_new_with_copy(const char *str) {
  string_buffer_t *self = NULL;
  size_t len;
  if (!str || (len = strlen(str)) == 0) return NULL;
  self = string_buffer_new_with_size(len);
  if (!self) return NULL;
  if (memcpy(self->alloc, str, len)) {
    return self;
  }
  string_buffer_free(self);
  return NULL;
}
size_t string_buffer_size(const string_buffer_t *self) {
  if (!self) return 0;
  return self->len;
}
size_t string_buffer_length(const string_buffer_t *self) {
  if (!self) return 0;
  return strlen(self->data);
}
void string_buffer_free(string_buffer_t *self) {
  if (!self) return;
  free(self->alloc);
  free(self);
}
static int string_buffer_resize(string_buffer_t *self, size_t n) {
  size_t len = nearest_multiple_of(1024, n);
  char * tmp = realloc(self->alloc, len + 1);
  if (!tmp) return -1;
  self->len = len;
  self->alloc = self->data = tmp;
  self->alloc[len] = '\0';
  return 0;
}
int string_buffer_prepend(string_buffer_t *self, const char *str) {
  size_t len, prev, needed;
  if (!self || !str || (len = strlen(str)) == 0) return -1;
  prev = strlen(self->data);
  needed = len + prev;
  if (self->len > needed) goto move;
  if (string_buffer_resize(self, needed)) return -1;
move:
  memmove(self->data + len, self->data, prev + 1);
  memcpy(self->data, str, len);
  return 0;
}
int string_buffer_append(string_buffer_t *self, const char *str) {
  if (!self || !str || strlen(str) == 0) return -1;
  return string_buffer_append_n(self, str, strlen(str));
}
int string_buffer_appendf(string_buffer_t *self, const char *fmt, ...) {
  va_list ap, tmpa;
  char *dst = NULL;
  int length = 0, required = 0, bytes = 0;
  if (!self || !fmt || strlen(fmt) == 0) return -1;
  va_start(ap, fmt);
  length = string_buffer_length(self);
  va_copy(tmpa, ap);
  required = vsnprintf(NULL, 0, fmt, tmpa);
  va_end(tmpa);
  if (string_buffer_resize(self, length + required)) {
    va_end(ap);
    return -1;
  }
  dst = self->data + length;
  bytes = vsnprintf(dst, 1 + required, fmt, ap);
  va_end(ap);
  return bytes < 0 ? -1 : 0;
}
int string_buffer_append_n(string_buffer_t *self, const char *str, size_t len) {
  size_t prev, needed;
  if (!self || !str || len == 0) return -1;
  prev = strlen(self->data);
  needed = len + prev;
  if (self->len > needed) {
    return strncat(self->data, str, len) ? 0 : -1;
  }
  if (string_buffer_resize(self, needed)) return -1;
  return strncat(self->data, str, len) ? 0 : -1;
}
int string_buffer_equals(const string_buffer_t *self, const string_buffer_t *other) {
  if (!self && !other) return 1;
  if (!self || !other) return 0;
  return strcmp(self->data, other->data) == 0;
}
ssize_t string_buffer_index(const string_buffer_t *self, const char * str) {
  char *sub = NULL;
  if (!self || !str || strlen(str) == 0) return -1;
  sub = strstr(self->data, str);
  if (!sub) return -1;
  return (sub - self->data);
}
string_buffer_t * string_buffer_slice(const string_buffer_t *self, size_t from, ssize_t to) {
  size_t len, n;
  if (!self) return NULL;
  len = strlen(self->data);

  if (to < 0) to = (ssize_t)len - ~to;
  if (to < (ssize_t)from) return NULL;

  if (to > (ssize_t)len) to = (ssize_t)len;
  n = (size_t)to - from;
  string_buffer_t *buf = string_buffer_new_with_size(n);
  if (!buf) return NULL;
  if (memcpy(buf->data, self->data + from, n)) {
    return buf;
  }
  string_buffer_free(buf);
  return NULL;
}
ssize_t string_buffer_compact(string_buffer_t *self) {
  size_t len, rem;
  char * buf;
  if (!self) return -1;
  len = string_buffer_length(self);
  rem = self->len - len;
  buf = calloc(len + 1, 1);
  if (memcpy(buf, self->data, len)) {
    free(self->alloc);
    self->len = len;
    self->data = self->alloc = buf;
    return rem;
  }
  free(buf);
  return -1;
}
void string_buffer_fill(string_buffer_t *self, int c) {
  if (!self) return;
  memset(self->data, c, self->len);
}
void string_buffer_clear(string_buffer_t *self) {
  if (!self) return;
  string_buffer_fill(self, 0);
}
static int is_delim(int ch, const char * chars) {
  if (!chars || strlen(chars) == 0) return isspace(ch);
  return strchr(chars, ch) != NULL;
}
void string_buffer_ltrim(string_buffer_t *self, const char * chars) {
  int c;
  if (!self) return;
  while ((c = *self->data) && is_delim(c, chars)) {
    ++self->data;
  }
}
void string_buffer_rtrim(string_buffer_t *self, const char * chars) {
  int c;
  size_t i;
  if (!self) return;
  i = string_buffer_length(self) - 1;
  while ((c = self->data[i]) && is_delim(c, chars)) {
    self->data[i--] = '\0';
  }
}
void string_buffer_trim(string_buffer_t *self, const char * chars) {
  string_buffer_ltrim(self, chars);
  string_buffer_rtrim(self, chars);
}
