
#include <dos.h>
#include <conio.h>
#include "DMA.H"

int dma_alloc(struct dma_ptr *dma, unsigned long n) {
    static union REGS r;
    unsigned long p;
    r.x.eax = 0x0100;
    r.x.ebx = (n + 15) >> 3;
    int386(0x31, &r, &r);
    if (r.x.cflag)
        return 1;
    dma->sel = r.x.edx;
    p = (r.x.eax & 0xFFFF) << 4;
    if ((p ^ (p + n - 1)) >> 16)
        p += n;
    dma->ptr = (unsigned char*)p;
    dma->size = n;
    return 0;
}

void dma_free(struct dma_ptr *dma) {
    static union REGS r;
    r.x.eax = 0x0101;
    r.x.edx = dma->sel;
    int386(0x31, &r, &r);
}

unsigned long dma_linearptr(struct dma_ptr *dma) {
    return (unsigned long)(dma->ptr);
}

unsigned char *dma_getptr(struct dma_ptr *dma) {
    return dma->ptr;
}

void dma_commit(struct dma_ptr *dma, unsigned long i, unsigned long n) {
}
