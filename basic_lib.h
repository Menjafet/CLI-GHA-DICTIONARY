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
// TODO
// MARKS REMOVER :so tildes and cantilations are gone while searching
#include <stdbool.h>

/**this lives in the stack */
#define MAX_STR_LEN 256
typedef struct
{
  char data[MAX_STR_LEN];
  u16 size;
  u16 bytes;
} String8;

int
utf8_char_len (unsigned char first_byte)
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

int
codepoint_to_utf8 (int cp, unsigned char *out)
{
  if (cp <= 0b1111111)
    { // Up to U+007F
      out[0] = (unsigned char)cp;
      return 1;
    }
  else if (cp <= 0b11111111111)
    { // Up to U+07FF
      out[0] = (unsigned char)(0b11000000 | ((cp >> 6) & 0b11111));
      out[1] = (unsigned char)(0b10000000 | (cp & 0b111111));
      return 2;
    }
  else if (cp <= 0b1111111111111111)
    { // Up to U+FFFF
      out[0] = (unsigned char)(0b11100000 | ((cp >> 12) & 0b1111));
      out[1] = (unsigned char)(0b10000000 | ((cp >> 6) & 0b111111));
      out[2] = (unsigned char)(0b10000000 | (cp & 0b111111));
      return 3;
    }
  else if (cp <= 0b100011111111111111111)
    { // Up to U+10FFFF
      out[0] = (unsigned char)(0b11110000 | ((cp >> 18) & 0b111));
      out[1] = (unsigned char)(0b10000000 | ((cp >> 12) & 0b111111));
      out[2] = (unsigned char)(0b10000000 | ((cp >> 6) & 0b111111));
      out[3] = (unsigned char)(0b10000000 | (cp & 0b111111));
      return 4;
    }
  return 0;
}

unsigned int
utf8_to_codepoint (const unsigned char *str, u32 len)
{
  if (len == 1)
    return str[0];
  // structural bits (0b00011111, 0b00001111, 0b00000111)
  // continuation bytes  are always masked with 0b0011 1111 to pull 6 data bits
  if (len == 2)
    {
      return ((str[0] & 0b00011111) << 6) | (str[1] & 0b00111111);
    }
  if (len == 3)
    {

      return ((str[0] & 0b00001111) << 12) | ((str[1] & 0b00111111) << 6)
             | (str[2] & 0b00111111);
    }
  if (len == 4)
    {

      return ((str[0] & 0b00000111) << 18) | ((str[1] & 0b00111111) << 12)
             | ((str[2] & 0b00111111) << 6) | (str[3] & 0b00111111);
    }
  return 0;
}

unsigned int
flatten_ancient_greek (unsigned int cp)
{
  // Target Block: Greek Extended (0b1111100000000 to 0b1111111111111 aka
  // U+1F00 to U+1FFF)
  // if (cp >= 0b1111100000000 && cp <= 0b1111111111111)
  // if (cp >= 0b1111100000000 && cp <= 0b1111111111111)
  if (cp >= 7936 && cp <= 8191)
    {
      // FIX: Expanded to a 16-bit wide mask to keep the leading bits intact
      unsigned int row = (cp & 0b1111111111110000) >> 4;
      unsigned int col = cp & 0b1111; // Column index (0 to 15)
      if (cp == 7957)
        {
          printf ("[INSIDE FUNC] Row calculated: %u, Col calculated: %u\n",
                  row, col);
        }
      // Rows 0x1F00 through 0x1F6F house perfectly sequenced vowel matrices:
      // (Alpha, Epsilon, Eta, Iota, Omicron, Upsilon, Omega)
      if (row >= 496 && row <= 502)
        {

          printf ("pasa2");
          bool is_capital = (col >= 0b1000);
          unsigned int base_vowel_index = row - 496;

         static const unsigned int lower_vowels[]
              = { 0x03B1, 0x03B5, 0x03B7, 0x03B9, 0x03BF, 0x03C5, 0x03C9 };
          static const unsigned int upper_vowels[]
              = { 0x0391, 0x0395, 0x0397, 0x0399, 0x039F, 0x03A5, 0x03A9 };
	  if (base_vowel_index < 7) {
                return is_capital ? upper_vowels[base_vowel_index] : lower_vowels[base_vowel_index];
            }
	  /*  if (is_capital)
            {
              switch (base_vowel_index)
                {
                case 0:
                  return 0x0391; // Α
                case 1:
                  return 0x0395; // Ε
                case 2:
                  return 0x0397; // Η
                case 3:
                  return 0x0399; // Ι
                case 4:
                  return 0x039F; // Ο
                case 5:
                  return 0x03A5; // Υ
                case 6:
                  return 0x03A9; // Ω
                }
            }
          else
            {
              switch (base_vowel_index)
                {
                case 0:
                  return 0x03B1; // α
                case 1:
                  return 0x03B5; // ε
                case 2:
                  return 0x03B7; // η
                case 3:
                  return 0x03B9; // ι
                case 4:
                  return 0x03BF; // ο
                case 5:
                  return 0x03C5; // υ
                case 6:
                  return 0x03C9; // ω
                }
		}*/
	  
        }

      // Rows 0x1F70 through 0x1FFF manage mixed vowel, sub-ioat, and rho
      // markings Manual override hooks for remaining isolated Ancient
      // variations:
      if (cp >= 0x1F70 && cp <= 0x1F71)
        return 0x03B1; // ὰ, ά -> α
      if (cp >= 0x1F72 && cp <= 0x1F73)
        {
          return 0x03B5; // ὲ, έ -> ε
        }
      if (cp >= 0x1F74 && cp <= 0x1F75)
        return 0x03B7; // ὴ, ή -> η
      if (cp >= 0x1F76 && cp <= 0x1F77)
        return 0x03B9; // ὶ, ί -> ι
      if (cp >= 0x1F78 && cp <= 0x1F79)
        return 0x03BF; // ὸ, ό -> ο
      if (cp >= 0x1F7A && cp <= 0x1F7B)
        return 0x03C5; // ὺ, ύ -> υ
      if (cp >= 0x1F7C && cp <= 0x1F7D)
        return 0x03C9; // ὼ, ώ -> ω

      if (cp >= 0x1FB0 && cp <= 0x1FB7)
        return 0x03B1; // ᾀ-ᾇ, ὰ, ᾶ, ᾷ variants -> α
      if (cp == 0x1FB8 || cp == 0x1FB9 || cp == 0x1FBA || cp == 0x1FBB)
        return 0x0391; // Capitals

      if (cp >= 0x1FC2 && cp <= 0x1FC7)
        return 0x03B7; // ᾐ-ᾗ variants -> η
      if (cp == 0x1FC8 || cp == 0x1FC9 || cp == 0x1FCA || cp == 0x1FCB)
        return 0x0397; // Capitals

      if (cp >= 0x1FD0 && cp <= 0x1FD7)
        return 0x03B9; // ῐ-ῗ variants -> ι
      if (cp == 0x1FD8 || cp == 0x1FD9 || cp == 0x1FDA || cp == 0x1FDB)
        return 0x0399; // Capitals

      if (cp >= 0x1FE0 && cp <= 0x1FE7)
        return 0x03C5; // ῠ-ῧ variants -> υ
      if (cp == 0x1FE8 || cp == 0x1FE9 || cp == 0x1FEA || cp == 0x1FEB)
        return 0x03A5; // Capitals
      if (cp == 0x1FE4 || cp == 0x1FE5)
        return 0x03C1; // ῤ, ῥ (Rho with breathing) -> ρ

      if (cp >= 0x1FF2 && cp <= 0x1FF7)
        return 0x03C9; // ᾠ-ᾧ variants -> ω
      if (cp == 0x1FF8 || cp == 0x1FF9 || cp == 0x1FFA || cp == 0x1FFB)
        return 0x03A9; // Capitals
    }
  return cp;
}

bool
is_diacritic_to_remove (unsigned int cp)
{
  // Hebrew Points/Nikud range: U+0591 to U+05C7
  // In binary: 0b10110010001 to 0b10111000111
  if (cp >= 0b10110010001 && cp <= 0b10111000111)
    return true;

  // Greek Combining Diacritics (Accents, Breathings) range: U+0300 to U+036F
  // In binary: 0b1100000000 to 0b1101101111
  if (cp >= 0b1100000000 && cp <= 0b1101101111)
    return true;

  // Greek Core Diacritics (tonos, dialytika) values: U+037A, U+0384, U+0385
  if (cp == 0b1101111010 || cp == 0b1110000100 || cp == 0b1110000101)
    return true;

  return false;
}

void
strip_diacritics (const char *input, char *output)
{
  u32 i = 0, j = 0;
  while (input[i] != '\0')
    {
      unsigned char leading_byte = (unsigned char)input[i];
      int len = utf8_char_len (leading_byte);

      unsigned int cp
          = utf8_to_codepoint ((const unsigned char *)&input[i], len);
      printf ("DEBUG Code Point: %u (Hex: %X)\n", cp, cp);
      cp = flatten_ancient_greek (cp);
      printf ("DEBUG Code Point: %u (Hex: %X)\n", cp, cp);

      if (is_diacritic_to_remove (cp))
        {
          i += len; // skip
        }
      else
        {
          unsigned char temp_bytes[5];
          int encoded_len = codepoint_to_utf8 (cp, temp_bytes);
          u8 k;
          for (k = 0; k < encoded_len; k++)
            {
              output[j++] = temp_bytes[k];
            }
          i += len;
        }
    }
  output[j] = '\0';
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
