#ifndef DGC_MY_STDIO_H
#define DGC_MY_STDIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zlib.h>

typedef struct {
  int compressed;
  FILE *fp;
  gzFile *comp_fp;
} my_FILE;

my_FILE *my_fopen(const char *filename, const char *mode);

int my_fgetc(my_FILE *fp);

int my_feof(my_FILE *fp);

int my_fseek(my_FILE *fp, long offset, int whence);

long my_ftell(my_FILE *fp);

int my_fclose(my_FILE *fp);

size_t my_fread(void *ptr, size_t size, size_t nmemb, my_FILE *fp);

size_t my_fwrite(const void *ptr, size_t size, size_t nmemb, my_FILE *fp);

char *my_fgets(char *s, int size, my_FILE *fp);

int my_fputc(int c, my_FILE *fp);

void my_fprintf(my_FILE *fp, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
