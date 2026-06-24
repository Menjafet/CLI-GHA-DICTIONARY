#include <stdint.h>
#include <stdio.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
void test ();

#ifndef STRING_UTF8
#define STRING_UTF8
#include <stddef.h>

// Returns how many bytes a specific UTF-8 character starts with (1 to 4)
int utf8_char_len(char first_byte);

// Counts the actual visual characters in a string, not the raw bytes
size_t utf8_strlen(const char *str);

// Safely prints a single multi-byte character given its starting pointer
void utf8_print_char(const char *str_ptr);

#endif

#if defined(BASIC_TYPES)
void
test ()
{
  printf ("test is great");
}
#endif
#if defined(POLLY)
typedef struct
{
  Command type;
  union
  {
    char str[234];
    char byte;
    int number;
  };
} languino;

void
polimorph_print (languino *lan)
{
  switch (lan->type)
    {
    case N:
      printf ("%d", lan->number);
    case R:
    case A:

      printf ("%c", lan->byte);
    case G:
      printf ("%c", lan->byte);
      break;
    case H:
      printf ("%s", lan->str);
      break;
    }
}

#endif

#if defined(STRING_UTF8)
/**
purpose of this custom string, we are dealing with utf8 non ascii characters such as hebrew
and greek so to make all more simple is made this 
========UTF8 KEYPOINTS=============================
first character can help you deduce the size
0.......-> 1 byte standar ascii
110.....-> 2 bytes greek arabic
1110....-> 3 bytes chinese,jap,emojis
11110...-> 4 rare symbols
10......-> continuation byte
 */

/**this lives in the stack */
#define MAX_STR_LEN 256
// Fixed String size max len 256 bytes
typedef struct
{
    char data[MAX_STR_LEN];
    u16 size;
} fstr;

fstr str_new(const char *initial_str)
{
    fstr s;
    s.size = strlen(initial_str);

    s.size = (s.size >= MAX_STR_LEN) ? MAX_STR_LEN - 1 : s.size;
    // strncpy(s.data, initial_str, s.size);

    u16 i;
    for (i = 0; i < s.size && initial_str[i] != '\0'; i++)
    {
        s.data[i] = initial_str[i];
    }

    s.data[s.size] = '\0'; // Ensure null-termination
    return s;
}

#endif
