#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BASIC_TYPES
#include "basic_lib.h"
#include <stdbool.h>

#define  GREAT_SUCCESS  0;
typedef struct
{
  const u32 lines;
  const char *idx;
  const char *tsv;
} Lang;

u32
swap_u32 (u32 v)
{ // 4 bytes 32 bits 1byte 4times 8+8+8+8
  // from little indian to big indian
  return ((v << 24) & 0xff000000) | ((v << 8) & 0x00ff0000)
         | ((v >> 8) & 0x0000ff00) | ((v >> 24) & 0x000000ff);
}

// IDX CONTAINS LINE_SIZES AND POSITIONS
void
GENERATE_IDX_FILE (Lang language, FILE *file_pointer,
                                FILE *idx_file)
{
  u32 header[3] = { swap_u32 (0x00000C02), // magic number to validate it is
                    swap_u32 (language.lines), swap_u32 (2) };

  fwrite (header, sizeof (u32), 3, idx_file);
  fseek (file_pointer, 0, SEEK_SET);
  u32 str_e = 0, end_e = 0;

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
provide_entry (FILE *file_pointer, FILE *idx_file, u32 entry_index)
{ 
  // 12 bytes header + index * 2elemts per row* 4bytes per elemt
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
is_number (char *arg_val)
{
  if (45 <= arg_val[0] && arg_val[0] <= 57)
    return true;
  return false;
}

void str_demo(){

  exit(0);
}

int
main (int argc, char *argv[])
{
  str_demo();
  const Lang ARAMAIC = { .lines = 8215,
                         .idx = "./DATA/dictfull.idx",
                         .tsv = "./DATA/dictfull.tsv" };
  const Lang HEBREW = { .lines = 8676,
                        .idx = "./DATA/hebrew_strong.idx",
                        .tsv = "./DATA/hebrew_strong.tsv" };
  const Lang GREEK = { .lines = 5624,
                       .idx = "./DATA/greek_strong.idx",
                       .tsv = "./DATA/greek_strong.tsv" };
  char *help = "commands available \n gha -g entry ->greek entry of strong "
               "dicitonary\n gha -h entry ->strong hebrew entry\n gha -a "
               "entry -> entry in aramaic of the CAL\n gha -r reindex all the "
               "dictionaries from the  tsv's"
               "\n gha -s word ->search exact match";

  i64 entry = 0;
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

          GENERATE_IDX_FILE (ARAMAIC, file_pointer_AR,
                                          idx_file_AR);
          GENERATE_IDX_FILE (GREEK, file_pointer_G, idx_file_G);
          GENERATE_IDX_FILE (HEBREW, file_pointer_H, idx_file_H);
          fclose (file_pointer_AR);
          fclose (file_pointer_H);
          fclose (file_pointer_G);
          fclose (idx_file_AR);
          fclose (idx_file_H);
          fclose (idx_file_G);
          printf ("reindexing successful!");
          return GREAT_SUCCESS;
        }
      // validate it is a number
      if (NULL == argv[2])
        {
          printf ("argument not provided:see ->%s", help);
          exit (1);
        }

      char *arg_val = argv[2];
      if (is_number (arg_val) )
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
  return GREAT_SUCCESS;
  
}
