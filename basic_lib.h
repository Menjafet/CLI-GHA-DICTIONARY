/**
purpose of this custom string, we are dealing with utf8 non ascii characters
such as hebrew and greek so to make all more simple is made this
========UTF8 KEYPOINTS=============================
first character can help you deduce the size
0.......-> 1 byte standar ascii
110.....-> 2 bytes greek arabic
1110....-> 3 bytes chinese,jap,emojis
11110...-> 4 rare symbols
10......-> continuation byte

*/
//-------- SECTION 1 :API DECLARATIONS
// ifndef INCLDUDE GUARD job is to prevent compilation errors caused by
// duplicate definitions

#ifndef BASIC_TYPES
#define BASIC_TYPES
typedef void u0;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef _Float16 f16; // 2bytes 16-bit half precision.
typedef float f32;    // 4bytes 32-bit width (matches IEEE-754 binary32)
typedef double f64;   //: 64-bit width (matches IEEE-754 binary64)

void test ();
#endif

#ifndef STRING_UTF8 // if this falg is not defined yet. then->
#define STRING_UTF8
#include <string.h>
u8 utf8_char_len (char first_byte);
size_t utf8_strlen (char *str);
void utf8_print_chars (const char *str_ptr);

#endif // include STRING_UTF8

//======================================================
//------------- SECTION 2: THE CODE SECTION-------------
//======================================================

#if defined(BASIC_TYPES)

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h> //INCLUDE LIBRARIES IN THE IMPLEMENTATION BLOCK
typedef void u0;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef _Float16 f16; // 2bytes 16-bit half precision.
typedef float f32;    // 4bytes 32-bit width (matches IEEE-754 binary32)
typedef double f64;   //: 64-bit width (matches IEEE-754 binary64)
// typedef _Float128 f28;// 128-bit width (matches IEEE-754 binary128)
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
/**this lives in the stack */
#define MAX_STR_LEN 256
typedef struct
{
  char data[MAX_STR_LEN];
  u16 size;
  u16 bytes;
} String8;

u8
utf8_char_len (char first_byte)
{
  // Returns how many bytes a specific UTF-8 character starts with (1 to 4)
  unsigned char b = (unsigned char)first_byte;

  if ((b & 0b10000000) == 0b00000000)
    return 1;
  if ((b & 0b11100000) == 0b11000000)
    return 2;
  if ((b & 0b11110000) == 0b11100000)
    return 3;
  if ((b & 0b11111000) == 0b11110000)
    return 4;
  return 1;
}

size_t
utf8_strlen (const char *str)
{
  // Counts the actual visual characters in a string, not the raw bytes
  size_t length = 0;
  size_t i = 0;
  while (str[i] != '\0')
    {
      u8 bytes = utf8_char_len (str[i]);
      i += bytes;
      length++;
    }

  return length;
}
void
utf8_print_chars (const char *str)
{ // %.*s print precise number of characters without needing
  //\0 to stop  reg %s reads until \0 stops it
  // but the latter allows to stop where the len says
  // . specify strict precision limit
  // * dont hardcode the precision limit here,
  // i will pass it to you as an argument
  // right before the string .ejemplo 0.4f
  // printf("%.2s", &str[i]); Prints ebactly 2 bytes and stops

  size_t i = 0;
  while (str[i] != '\0')
    {
      u8 len = utf8_char_len ((unsigned char)str[i]);
      printf ("%.*s", len, &str[i]);
      i += len;
    }
}

void
str8_stat (String8 sr8)
{
  utf8_print_chars (sr8.data);
  printf (" size:%hu bytes:%hu \n", sr8.size, sr8.bytes);
}

String8
new_str8 (const char *str)
{
  String8 s;
  s.size = strlen (str);
  s.bytes = 0;
  s.size = (s.size >= MAX_STR_LEN) ? MAX_STR_LEN - 1 : s.size;
  // strncpy(s.data, initial_str, s.size);
  u16 i = 0;
  while (str[i] != '\0')
    {
      s.data[i] = str[i];
      i++;
    }
  s.bytes = i;
  s.data[s.size] = '\0'; // Ensure null-termination
  return s;
}

#endif
