
#ifndef _POKERI_H
#define _POKERI_H

#define DEBUG 0
#define RANGECHECK 0
#define SLOMO 0
#define RECT_DUMP 0

extern int english;

#define NDEBUG !DEBUG

void debug_print_(const char *f, int line, const char *s, ...);
void debug_abort_(const char *f, int line, const char *s, ...);
#define debug_abort(s) debug_abort_(__FILE__, __LINE__, s)
#define debug_abort1(s,a) debug_abort_(__FILE__, __LINE__, s, a)
#define debug_abort2(s,a,b) debug_abort_(__FILE__, __LINE__, s, a, b)
#define debug_abort3(s,a,b,c) debug_abort_(__FILE__, __LINE__, s, a, b, c)
#define debug_abort4(s,a,b,c,d) debug_abort_(__FILE__, __LINE__, s, a, b, c, d)
#define debug_print(s) debug_print_(__FILE__, __LINE__, s)
#define debug_print1(s,a) debug_print_(__FILE__, __LINE__, s, a)
#define debug_print2(s,a,b) debug_print_(__FILE__, __LINE__, s, a, b)
#define debug_print3(s,a,b,c) debug_print_(__FILE__, __LINE__, s, a, b, c)
#define debug_print4(s,a,b,c,d) debug_print_(__FILE__, __LINE__, s, a, b, c, d)

#if RANGECHECK
#define RANGE_CHECK(p,a,b) do { if ((p) < (a) || (p) >= (b)) \
    debug_abort("range check failure"); \
    } while (0)
#define RANGE_TASO0(p) RANGE_CHECK(p, taso0, taso0 + PLANE_SIZE)
#else
#define RANGE_CHECK(p,a,b)
#define RANGE_TASO0(p)
#endif

#endif
