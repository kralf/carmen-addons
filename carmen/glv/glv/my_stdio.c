#include <carmen/global.h>

#include "my_stdio.h"

my_FILE *my_fopen(const char *filename, const char *mode)
{
  my_FILE *fp;
  
  /* allocate a new file pointer */
  fp = (my_FILE *)calloc(1, sizeof(my_FILE));
  carmen_test_alloc(fp);

  /* look at filename extension to determine if file is compressed */
  if(strcmp(filename + strlen(filename) - 3, ".gz") == 0)
    fp->compressed = 1;
  else
    fp->compressed = 0;

  if(!fp->compressed) {
    fp->fp = fopen(filename, mode);
    if(fp->fp == NULL) {
      free(fp);
      return NULL;
    }
  }
  else {
    fp->fp = fopen(filename, mode);
    if(fp->fp == NULL) {
      free(fp);
      return NULL;
    }
    fp->comp_fp = gzdopen(fileno(fp->fp), mode);
    if(fp->comp_fp == NULL) {
      fclose(fp->fp);
      free(fp);
      return NULL;
    }
  }
  return fp;
}

int my_fgetc(my_FILE *fp)
{
  if(!fp->compressed)
    return fgetc(fp->fp);
  else
    return gzgetc(fp->comp_fp);
}

int my_feof(my_FILE *fp)
{
  if(!fp->compressed)
    return feof(fp->fp);
  else
    return gzeof(fp->comp_fp);
}

int my_fseek(my_FILE *fp, long offset, int whence)
{
  if(!fp->compressed)
    return fseek(fp->fp, offset, whence);
  else
    return gzseek(fp->comp_fp, offset, whence);
}

long my_ftell(my_FILE *fp)
{
  if(!fp->compressed)
    return ftell(fp->fp);
  else
    return gztell(fp->comp_fp);
}

int my_fclose(my_FILE *fp)
{
  if(!fp->compressed)
    return fclose(fp->fp);
  else
    return gzclose(fp->comp_fp);
}

size_t my_fread(void *ptr, size_t size, size_t nmemb, my_FILE *fp)
{
  if(!fp->compressed)
    return fread(ptr, size, nmemb, fp->fp);
  else
    return gzread(fp->comp_fp, ptr, size * nmemb) / size;
}

size_t my_fwrite(const void *ptr, size_t size, size_t nmemb, my_FILE *fp)
{
  if(!fp->compressed)
    return fwrite(ptr, size, nmemb, fp->fp);
  else
    return gzwrite(fp->comp_fp, (void *)ptr, size * nmemb) / size;
}

char *my_fgets(char *s, int size, my_FILE *fp)
{
  if(!fp->compressed)
    return fgets(s, size, fp->fp);
  else
    return gzgets(fp->comp_fp, s, size);
}

int my_fputc(int c, my_FILE *fp)
{
  if(!fp->compressed)
    return fputc(c, fp->fp);
  else
    return gzputc(fp->comp_fp, c);
}

void my_fprintf(my_FILE *fp, const char *fmt, ...)
{
  /* Guess we need no more than 100 bytes. */
  int n, size = 100;
  char *p;
  va_list ap;

  if((p = (char *)malloc(size)) == NULL)
    return;
  while(1) {
    /* Try to print in the allocated space. */
    va_start(ap, fmt);
    n = vsnprintf(p, size, fmt, ap);
    va_end(ap);
    /* If that worked, return the string. */
    if(n > -1 && n < size) {
      my_fwrite(p, strlen(p), 1, fp);
      free(p);
      return;
    }
    /* Else try again with more space. */
    if(n > -1)    /* glibc 2.1 */
      size = n + 1; /* precisely what is needed */
    else           /* glibc 2.0 */
      size *= 2;  /* twice the old size */
    if((p = (char *)realloc(p, size)) == NULL)
      return;
  }
}

