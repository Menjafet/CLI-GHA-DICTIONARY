#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef char bool;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef struct
{
  const u32 lines;
  const char *idx;
  const char *tsv;
} Lang;

u32
swap_u32 (uint32_t v)
{ // 4 bytes 32 bits 1byte 4times 8+8+8+8
  // from little indian to big indian
  return ((v << 24) & 0xff000000) | ((v << 8) & 0x00ff0000)
         | ((v >> 8) & 0x0000ff00) | ((v >> 24) & 0x000000ff);
}

// GENERATE IDX FILES
void
PRINT_LINE_SIZES_AND_POSITIONS (Lang language, FILE *file_pointer,
                                FILE *idx_file)
{
  //  printf ("sda\n");
  u32 header[3] = { swap_u32 (0x00000C02), // magic number to validate it is
                                           // indeed your dicitonary index file
                    swap_u32 (language.lines), swap_u32 (2) };

  fwrite (header, sizeof (u32), 3, idx_file);
  fseek (file_pointer, 0, SEEK_SET);
  u32 str_e = 0;
  u32 end_e = 0;

  i64 c;
  while ((c = fgetc (file_pointer)) != EOF)
    {

      if ('\n' == c)
        {
          // get exact byte position
          end_e = ftell (file_pointer);
          // end position-start position -1\n
          u32 real_size = end_e - str_e - 1;

          u32 pos = swap_u32 (str_e);
          u32 siz = swap_u32 (real_size);

          fwrite (&pos, sizeof (u32), 1, idx_file);
          fwrite (&siz, sizeof (u32), 1, idx_file);

          str_e = end_e;
        }
    }
}

void
provide_entry (FILE *file_pointer, FILE *idx_file, int entry_index)
{ //  int ret_code = 0
  // 12 bytes header + index * 2elemts per row* 4bytes per elemt
  // long idx_offset = 12 + (entry_index * 2 * sizeof (u32));
  long idx_offset = 12 + (entry_index * 2 * sizeof (u32));

  if (fseek (idx_file, idx_offset, SEEK_SET) != 0)
    {

      printf ("fseek() failet in file %s at line #%d ", __FILE__,
              __LINE__ - 20);
      fclose (file_pointer);
      fclose (idx_file);
      exit (EXIT_FAILURE);
    }

  u32 raw_position, raw_size;
  fread (&raw_position, sizeof (u32), 1, idx_file);
  fread (&raw_size, sizeof (u32), 1, idx_file);
  // fseek
  u32 real_size = swap_u32 (raw_size);
  u32 real_position = swap_u32 (raw_position);
  if (fseek (file_pointer, real_position, SEEK_SET) != 0)
    {

      printf ("fseek() failet in file %s at line #%d ", __FILE__,
              __LINE__ - 20);
      fclose (file_pointer);
      fclose (idx_file);
      exit (EXIT_FAILURE);
    }

  // printf ("real size %d", real_size);
  char *buffer = (char *)malloc (real_size + 1);
  fread (buffer, sizeof (char), real_size, file_pointer);
  buffer[real_size] = '\0';
  printf ("%s", buffer);
}

typedef enum
{
  N, // NOT FOUND
  G, // GREEK
  H, // HEBREW
  A, // ARAMAIC
  R  // RE INDEX DICTIONARIES
} Command;

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

Command
getCommand (char *val)
{
  // reminder= strcmp 0 means equal
  assert (!(val == NULL));
  if (0 == strcmp (val, "-h"))
    return H;
  else if (0 == strcmp (val, "-a"))
    return A;
  else if (0 == strcmp (val, "-g"))
    return G;
  else if (0 == strcmp (val, "-r"))
    return R;

  return N;
}

bool
is_arg_val_number (char *arg_val)
{
  if (45 <= arg_val[0] && arg_val[0] <= 57)
    return 'a';
  return ' ';
}

int
main (int argc, char *argv[])
{
  const Lang ARAMAIC = { .lines = 8215,
                         .idx = "dictfull.idx",
                         .tsv = "./dicts/dictfull.tsv" };
  const Lang HEBREW = { .lines = 8676,
                        .idx = "hebrew_strong.idx",
                        .tsv = "strongHebrewG_c.tsv" };
  const Lang GREEK = { .lines = 5624,
                       .idx = "greek_strong.idx",
                       .tsv = "strongsgreek_c.tsv" };
  char *help = "commands available \n gha -g entry ->greek entry of strong "
               "dicitonary\n gha -h entry ->strong hebrew entry\n gha -a "
               "entry -> entry in aramaic\n gha -r reindex all the "
               "dictionaries from the  tsv's"
               "\n gha -s word ->search exact match";

  int entry = 0;
  if (argc >= 2)
    {
      FILE *file_pointer_G = fopen (GREEK.tsv, "r");
      FILE *file_pointer_H = fopen (HEBREW.tsv, "r");

      FILE *idx_file_G = fopen (GREEK.idx, "r+");
      FILE *idx_file_H = fopen (HEBREW.idx, "r+");
      FILE *file_pointer_AR = fopen (ARAMAIC.tsv, "r");
      FILE *idx_file_AR = fopen (ARAMAIC.idx, "r+");

      // set error guardrails
      assert (!(idx_file_AR == NULL));
      assert (!(file_pointer_AR == NULL));
      assert (!(idx_file_G == NULL));
      assert (!(idx_file_H == NULL));
      assert (!(file_pointer_H == NULL));
      assert (!(file_pointer_G == NULL));
      if (getCommand (argv[1]) == R)
        {
          // lets get the aramaic in

          PRINT_LINE_SIZES_AND_POSITIONS (ARAMAIC, file_pointer_AR,
                                          idx_file_AR);
          PRINT_LINE_SIZES_AND_POSITIONS (GREEK, file_pointer_G, idx_file_G);
          PRINT_LINE_SIZES_AND_POSITIONS (HEBREW, file_pointer_H, idx_file_H);
          fclose (file_pointer_AR);
          fclose (file_pointer_H);
          fclose (file_pointer_G);
          fclose (idx_file_AR);
          fclose (idx_file_H);
          fclose (idx_file_G);
          printf ("reindexing successful!");
          int GREAT_SUCCESS = 0;
          return GREAT_SUCCESS;
        }
      // validate it is a number
      if (NULL == argv[2])
        {
          printf ("argument not provided:see -> ");
          printf ("%s", help);
          exit (1);
        }

      char *arg_val = argv[2];
      if (is_arg_val_number (arg_val) == 'a')
        {
          entry = atoi (arg_val);
          switch (getCommand (argv[1]))
            {
            case H:
              provide_entry (file_pointer_H, idx_file_H, entry);
              break;
            case G:
              provide_entry (file_pointer_G, idx_file_G, entry);
              break;
            case A:
              provide_entry (file_pointer_AR, idx_file_AR, entry);
              break;
            case R:
            case N:

              fclose (file_pointer_H);
              fclose (file_pointer_G);
              fclose (idx_file_H);
              fclose (idx_file_G);
              perror ("UNKNOWN COMMAND");
              printf (":see ->%s ", help);
              return 1;
              break;
            }
        }
      else
        {
          printf ("so far is not a number");
        }
      fclose (file_pointer_AR);
      fclose (file_pointer_H);
      fclose (file_pointer_G);
      fclose (idx_file_AR);
      fclose (idx_file_H);
      fclose (idx_file_G);
      return 0;
    }

  printf ("Command not provided:see -> ");
  printf ("%s", help);
  return 0;
}
