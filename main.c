#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
uint32_t
swap_uint32 (uint32_t v)
{
  return ((v << 24) & 0xff000000) | ((v << 8) & 0x00ff0000)
         | ((v >> 8) & 0x0000ff00) | ((v >> 24) & 0x000000ff);
}

// GENERATE IDX FILES
void
PRINT_LINE_SIZES_AND_POSITIONS (FILE *file_pointer, FILE *idx_file)
{
  //  printf ("sda\n");
  uint32_t header[3]
      = { swap_uint32 (0x00000C02), // magic number signed 32bit int,2d
          swap_uint32 (8675), swap_uint32 (2) };

  fwrite (header, sizeof (uint32_t), 3, idx_file);
  fseek (file_pointer, 0, SEEK_SET);
  uint32_t str_e = 0;
  uint32_t end_e = 0;

  int c;
  while ((c = fgetc (file_pointer)) != EOF)
    {

      if ('\n' == c)
        {
          // get exact byte position
          end_e = ftell (file_pointer);
          // end position-start position -1\n
          uint32_t real_size = end_e - str_e - 1;

          uint32_t pos = swap_uint32 (str_e);
          uint32_t siz = swap_uint32 (real_size);

          fwrite (&pos, sizeof (uint32_t), 1, idx_file);
          fwrite (&siz, sizeof (uint32_t), 1, idx_file);

          str_e = end_e;
        }
    }
}

void
provide_entry (FILE *file_pointer, FILE *idx_file, int entry_index)
{ //  int ret_code = 0
  // 12 bytes header + index * 2elemts per row* 4bytes per elemt
  long idx_offset = 12 + (entry_index * 2 * sizeof (uint32_t));

  if (fseek (idx_file, idx_offset, SEEK_SET) != 0)
    {

      printf ("fseek() failet in file %s at line #%d ", __FILE__,
              __LINE__ - 20);
      fclose (file_pointer);
      fclose (idx_file);
      exit (EXIT_FAILURE);
    }

  uint32_t raw_position, raw_size;
  fread (&raw_position, sizeof (uint32_t), 1, idx_file);
  fread (&raw_size, sizeof (uint32_t), 1, idx_file);
  // fseek
  uint32_t real_size = swap_uint32 (raw_size);
  uint32_t real_position = swap_uint32 (raw_position);
  if (fseek (file_pointer, real_position, SEEK_SET) != 0)
    {

      printf ("fseek() failet in file %s at line #%d ", __FILE__,
              __LINE__ - 20);
      fclose (file_pointer);
      fclose (idx_file);
      exit (EXIT_FAILURE);
    }

  //printf ("real size %d", real_size);
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
  R  // RE INDEX DICTIONARIES
} Command;

Command
getCommand (char *val)
{
  assert (!(val == NULL));
  if (0 == strcmp (val, "-h"))
    return H;
  else if (0 == strcmp (val, "-g"))
    return G;
  else if (0 == strcmp (val, "-r"))
    return R;

  return N;
}

typedef struct
{
  const char *idx;
  const char *tsv;
} Lang;

int
main (int argc, char *argv[])
{
  const Lang HEBREW
      = { .idx = "hebrew_strong.idx", .tsv = "strongHebrewG_c.tsv" };
  const Lang GREEK
      = { .idx = "greek_strong.idx", .tsv = "strongsgreek_c.tsv" };
  // main -g entry
  // main -h entry
  // main -r

  int entry = 0;
  if (argc >= 2)
    {
      FILE *file_pointer_G = fopen (GREEK.tsv, "r");
      FILE *file_pointer_H = fopen (HEBREW.tsv, "r");

      FILE *idx_file_G = fopen (GREEK.idx, "r+");
      FILE *idx_file_H = fopen (HEBREW.idx, "r+");

      // set error guardrails
      assert (!(idx_file_G == NULL));
      assert (!(idx_file_H == NULL));
      assert (!(file_pointer_H == NULL));
      assert (!(file_pointer_G == NULL));
      if (getCommand (argv[1]) == R)
        {

          PRINT_LINE_SIZES_AND_POSITIONS (file_pointer_G, idx_file_G);
          PRINT_LINE_SIZES_AND_POSITIONS (file_pointer_H, idx_file_H);
          fclose (file_pointer_H);
          fclose (file_pointer_G);
          fclose (idx_file_H);
          fclose (idx_file_G);
          return 0;
        }
      // validate it is a number
      if (NULL == argv[2])
        {
          printf ("entry not provided: ");
          exit (1);
        }
      char *s = argv[2];

      entry = atoi (s);
      switch (getCommand (argv[1]))
        {
        case H:
          provide_entry (file_pointer_H, idx_file_H, entry);
          break;
        case G:
          provide_entry (file_pointer_G, idx_file_G, entry);
          break;
        case R:
        case N:

          fclose (file_pointer_H);
          fclose (file_pointer_G);
          fclose (idx_file_H);
          fclose (idx_file_G);
          perror ("UNKNOWN COMMAND");
          return 1;
          break;
        }

      fclose (file_pointer_H);
      fclose (file_pointer_G);
      fclose (idx_file_H);
      fclose (idx_file_G);
    }

  return 0;
}
