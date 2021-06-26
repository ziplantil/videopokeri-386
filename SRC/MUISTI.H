
#ifndef _MUISTI_H
#define _MUISTI_H

void fast_memset(void *dst, char val, unsigned int sz);
void fast_memcpy(void *dst, const void *src, unsigned int sz);
/* dst must be word-aligned. sz must be divisible by 2 */
void fast_large_memset(char *dst, char val, unsigned int sz);
/* dst must be word-aligned. sz must be divisible by 2 */
void fast_large_memcpy(char *dst, const char *src, unsigned int sz);
void memxor(char *dst, unsigned int sz);

#pragma aux fast_memset = \
    "rep stosb" \
    parm [edi] [al] [ecx] modify exact [edi eax ecx]

#pragma aux fast_memcpy = \
    "rep movsb" \
    parm [edi] [esi] [ecx] modify exact [edi esi eax ecx]

#pragma aux fast_large_memset = \
    "mov ah,al" \
    "shr ecx,1" \
    "rep stosw" \
    parm [edi] [al] [ecx] modify exact [edi esi eax ecx]

#pragma aux fast_large_memcpy = \
    "inc ecx" \
    "shr ecx,1" \
    "rep movsw" \
    parm [edi] [esi] [ecx] modify exact [edi esi eax ecx]

#pragma aux memxor = \
    "and ecx,ecx" \
    "jz end1" \
    "loop1:" \
    "mov esi,edi" \
    "lodsb" \
    "not al" \
    "stosb" \
    "loop loop1" \
    "end1:" \
    parm [edi] [ecx] modify exact [edi esi eax ecx]

#endif
