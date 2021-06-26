
#ifndef _AIKA_H
#define _AIKA_H

int microsleep(int us);

#pragma aux microsleep = \
    "mov ecx,edx" \
    "shr ecx,16" \
    "mov ax,8600h" \
    "int 15h" \
    parm [edx] modify exact [eax ecx edx]

#endif
