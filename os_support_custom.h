

#ifndef OS_SUPPORT_CUSTOM_H
#define OS_SUPPORT_CUSTOM_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define OVERRIDE_SPEEX_ALLOC
#define OVERRIDE_SPEEX_ALLOC_SCRATCH
#define OVERRIDE_SPEEX_REALLOC
#define OVERRIDE_SPEEX_FREE
#define OVERRIDE_SPEEX_FREE_SCRATCH

static inline void *speex_alloc (int size)
{
   void* p = malloc(size+4);  /* freertos malloc默认配置是8字节对齐,所以预留的4字节强制类型转换不会有对齐问题 */
   memset(p,0,size);
   *(uint32_t*)p = size;  /* 记录分配的大小 */
   return (uint8_t*)p+4;
}

static inline void *speex_alloc_scratch (int size)
{
   /* Scratch space doesn't need to be cleared */
   void* p = malloc(size+4);  /* freertos malloc默认配置是8字节对齐 */
   *(uint32_t*)p = size;  /* 记录分配的大小 */
   return (uint8_t*)p+4;
}

static inline void *speex_realloc (void *ptr, int size)
{
   uint32_t len;
   void* p = malloc(size+4);
   *(uint32_t*)p = size;  /* 记录分配的大小 */
   len = *(uint32_t*)((uint8_t*)ptr-4);
   memcpy((uint8_t*)p+4,ptr,len);
   free((uint8_t*)ptr-4);
   return (uint8_t*)p+4;
}

static inline void speex_free (void *ptr)
{
   free((uint8_t*)ptr-4);
}

static inline void speex_free_scratch (void *ptr)
{
   free((uint8_t*)ptr-4);
}

#define OVERRIDE_SPEEX_COPY
#define OVERRIDE_SPEEX_MOVE
#define OVERRIDE_SPEEX_MEMSET
#define SPEEX_COPY(dst, src, n) (memcpy((dst), (src), (n)*sizeof(*(dst)) + 0*((dst)-(src)) ))
#define SPEEX_MOVE(dst, src, n) (memmove((dst), (src), (n)*sizeof(*(dst)) + 0*((dst)-(src)) ))
#define SPEEX_MEMSET(dst, c, n) (memset((dst), (c), (n)*sizeof(*(dst))))

#define OVERRIDE_SPEEX_FATAL
static inline void _speex_fatal(const char *str, const char *file, int line)
{
   printf("Fatal (internal) error in %s, line %d: %s\n", file, line, str);
   while(1);
}

#define OVERRIDE_SPEEX_WARNING
static inline void speex_warning(const char *str)
{
#ifndef DISABLE_WARNINGS
   printf ("warning: %s\n", str);
#endif
}


#define OVERRIDE_SPEEX_WARNING_INT
static inline void speex_warning_int(const char *str, int val)
{
#ifndef DISABLE_WARNINGS
   printf ("warning: %s %d\n", str, val);
#endif
}

#define OVERRIDE_SPEEX_NOTIFY
static inline void speex_notify(const char *str)
{
#ifndef DISABLE_NOTIFICATIONS
   printf ("notification: %s\n", str);
#endif
}

#define OVERRIDE_SPEEX_PUTC
static inline void _speex_putc(int ch, void *file)
{
   (void)file;
   printf("%c", ch);
}

#define speex_fatal(str) _speex_fatal(str, __FILE__, __LINE__);
#define speex_assert(cond) {if (!(cond)) {speex_fatal("assertion failed: " #cond);}}

#ifndef RELEASE
static inline void print_vec(float *vec, int len, char *name)
{
   int i;
   printf ("%s ", name);
   for (i=0;i<len;i++)
      printf (" %f", vec[i]);
   printf ("\n");
}
#endif

#endif

