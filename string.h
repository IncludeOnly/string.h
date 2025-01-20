#ifndef STRING_H
#define STRING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STRING_VERSION_MAJOR 0
#define STRING_VERSION_MINOR 0
#define STRING_VERSION_PATCH 2
#define STRING_VERSION "0.0.2"

#ifndef STRINGAPI
    #define STRINGAPI extern
#endif // STRINGAPI

#include <stddef.h> // for size_t


typedef struct {
    char* memory;
    size_t capacity;
    size_t offset;
} string_arena_t;
STRINGAPI void string_arena_init(string_arena_t* arena, size_t size);
STRINGAPI void string_arena_free(string_arena_t* arena);
STRINGAPI void string_arena_resize(string_arena_t* arena, size_t new_size);
STRINGAPI void string_arena_log(const string_arena_t* arena);

typedef struct {
    char* chars;
    size_t length;
} string_t;
STRINGAPI string_t string_init(char* str, string_arena_t* arena);
STRINGAPI string_t string_concat(string_t str1, string_t str2, string_arena_t* arena);
STRINGAPI void string_append(string_t* dest, string_t src, string_arena_t* arena);
STRINGAPI int string_compare(string_t str1, string_t str2);
STRINGAPI string_t string_substring(string_t str, size_t start, size_t length, string_arena_t* arena);
STRINGAPI string_t string_trim(string_t str, string_arena_t* arena);
STRINGAPI void string_clear(string_t* str);
STRINGAPI char string_char_at(string_t str, size_t index);

typedef struct {
    string_t* strings;
    size_t count;
} string_array_t;
STRINGAPI string_array_t string_split(string_t str, char delimiter, string_arena_t* arena);


#define ArenaInit(size) string_arena_init(&global_arena, size)
#define ArenaFree() string_arena_free(&global_arena)
#define String(str) string_init(str, &global_arena)
#define Concat(str1, str2) string_concat(str1, str2, &global_arena)
#define Append(dest, src) string_append(dest, src, &global_arena)
#define Compare string_compare
#define Substring(src, start, len) string_substring(src, start, len, &global_arena)
#define Trim(src) string_trim(src, &global_arena)
#define Clear string_clear
#define CharAt string_char_at
#define Split(src, delim) string_split(src, delim, &global_arena)

#endif // STRING_H


#define STRING_IMPLEMENTATION
#ifdef STRING_IMPLEMENTATION

#include <assert.h>

static string_arena_t global_arena = {0};
static int global_arena_initialized = 0;


static size_t string_len(char* string)
{
    size_t count = 0;
    while (string[count++] != '\0');
    return count-1; // Don't count '\0'
}

STRINGAPI void string_arena_init(string_arena_t* arena, size_t size)
{
    arena->memory = (char*)malloc(size);
    assert(arena->memory && "StringArena allocation failed");
    arena->capacity = size;
    arena->offset = 0;
}

STRINGAPI void string_arena_free(string_arena_t* arena)
{
    free(arena->memory);
    arena->memory = NULL;
    arena->capacity = 0;
    arena->offset = 0;
}
STRINGAPI void string_arena_log(const string_arena_t* arena)
{
    printf("String Arena Allocator: \n");
    printf("  Capacity: %zu\n", arena->capacity);
    printf("  Offset: %zu\n", arena->offset);
}
STRINGAPI void string_arena_resize(string_arena_t* arena, size_t new_size)
{
    assert(new_size > arena->capacity && "New size must be greater than current capacity");

    char* new_memory = (char*)realloc(arena->memory, new_size);
    assert(new_memory && "Failed to resize arena");

    arena->memory = new_memory;
    arena->capacity = new_size;
}

static void* string_arena_alloc(string_arena_t* arena, size_t size)
{
    // Check if the arena has enough space
    if (arena->offset + size > arena->capacity) {
        size_t new_capacity = arena->capacity * 2;
        string_arena_resize(arena, new_capacity);
    }

    // Allocate the requested size
    void* ptr = arena->memory + arena->offset;
    arena->offset += size;

    return ptr;
}
#define stralloc(arena, size) \
    string_arena_alloc(arena, size)


STRINGAPI string_t string_init(char* str, string_arena_t* arena)
{
    string_t result = {0};
    result.length = string_len(str);
    result.chars = (char*) stralloc(arena, sizeof(char) * result.length);
    memcpy(result.chars, str, result.length);
    return result;
}

STRINGAPI string_t string_concat(string_t str1, string_t str2, string_arena_t* arena)
{
    string_t result = {0};

    result.length = str1.length + str2.length;

    result.chars = (char*) stralloc(arena, result.length + 1);

    if (result.chars == NULL) {
        return result;
    }

    memcpy(result.chars, str1.chars, str1.length);

    memcpy(result.chars + str1.length, str2.chars, str2.length);

    result.chars[result.length] = '\0';

    return result;
}
STRINGAPI void string_append(string_t* dest, string_t src, string_arena_t* arena)
{
    size_t new_length = dest->length + src.length;

    char* new_chars = (char*)stralloc(arena, new_length + 1); // +1 for null terminator

    if (new_chars == NULL) {
        return;
    }

    memcpy(new_chars, dest->chars, dest->length);

    memcpy(new_chars + dest->length, src.chars, src.length);

    new_chars[new_length] = '\0';

    dest->chars = new_chars;
    dest->length = new_length;
}

STRINGAPI int string_compare(string_t str1, string_t str2)
{
    size_t min_length = str1.length < str2.length ? str1.length : str2.length;
    int cmp = memcmp(str1.chars, str2.chars, min_length);

    if (cmp == 0) {
        if (str1.length < str2.length) return -1;
        if (str1.length > str2.length) return 1;
    }
    return cmp;
}

STRINGAPI string_t string_substring(string_t str, size_t start, size_t length, string_arena_t* arena)
{
    assert(start + length <= str.length && "Substring out of bounds");
    
    string_t result = {0};
    result.length = length;
    result.chars = (char*)stralloc(arena, length + 1);
    
    if (result.chars == NULL) {
        return result;
    }
    
    memcpy(result.chars, str.chars + start, length);
    result.chars[length] = '\0';
    return result;
}

STRINGAPI string_t string_trim(string_t str, string_arena_t* arena)
{
    size_t start = 0;
    size_t end = str.length;

    while (start < end && (str.chars[start] == ' ' || str.chars[start] == '\t' || str.chars[start] == '\n')) {
        start++;
    }
    while (end > start && (str.chars[end - 1] == ' ' || str.chars[end - 1] == '\t' || str.chars[end - 1] == '\n')) {
        end--;
    }

    return string_substring(str, start, end - start, arena);
}

STRINGAPI string_array_t string_split(string_t str, char delimiter, string_arena_t* arena)
{
    string_array_t result = {0};
    result.strings = (string_t*)stralloc(arena, sizeof(string_t) * (str.length + 1)); // Worst case: every char is a delimiter
    
    if (result.strings == NULL) {
        return result;
    }

    size_t start = 0;
    result.count = 0;

    for (size_t i = 0; i < str.length; i++) {
        if (str.chars[i] == delimiter) {
            result.strings[result.count++] = string_substring(str, start, i - start, arena);
            start = i + 1;
        }
    }

    if (start < str.length) {
        result.strings[result.count++] = string_substring(str, start, str.length - start, arena);
    }

    return result;
}

STRINGAPI void string_clear(string_t* str)
{
    str->chars = NULL;
    str->length = 0;
}

STRINGAPI char string_char_at(string_t str, size_t index)
{
    if(index > str.length) return '\0';
    return str.chars[index];
}


#undef stralloc

#endif // STRING_IMPLEMENTATION
