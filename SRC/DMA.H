
#ifndef _DMA_H
#define _DMA_H

typedef struct dma_ptr {
    void *ptr;
    unsigned long sel;
    unsigned long size;
};

int dma_alloc(struct dma_ptr *dma, unsigned long n);
void dma_free(struct dma_ptr *dma);
unsigned long dma_linearptr(struct dma_ptr *dma);
unsigned char *dma_getptr(struct dma_ptr *dma);
void dma_commit(struct dma_ptr *dma, unsigned long i, unsigned long n);

#endif
