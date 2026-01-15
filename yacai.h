// This file is part of YACAI.
// Copyright (C) 2026  Michele Esposito Marzino
//


#ifndef YACAI_H_
#define YACAI_H_


#ifdef __cplusplus
extern "C" {
#endif


#define YArray_static_len(a) (sizeof a / sizeof *a)


#if defined(YACAI_IMPLEMENTATION)

# include <assert.h>
# include <stdlib.h>
# include <string.h>


# ifndef ____YARRAY_GROWTH_FCT
#  define ____YARRAY_GROWTH_FCT 2
# endif // ! ____YARRAY_GROWTH_FCT


struct __YHdr {
    size_t size;
    size_t capacity;
    size_t item_size;
    size_t hdr_size;
};


typedef void *YArray;


//


# define ____YArray_hdr_ptr_offset(h, s) (YArray)((char *)(h) + (s))
# define ____YArray_at(a, i, s)          ((char *)(a) + ((i) * (s)))


//


static inline size_t ____YArray_hdr_size(const size_t alignment)
{
    const size_t hdr_size = sizeof(struct __YHdr) + sizeof(void *);
    return alignment > hdr_size ? alignment : hdr_size;
}

static inline struct __YHdr *____YArray_hdr(const YArray a)
{
    // TODO: Shall we raise error?
    if (!a) {
        return NULL;
    }
    return *((void **)a - 1);
}


static inline size_t ____YArray_new_size(const size_t s)
{
    // c
    return s * ____YARRAY_GROWTH_FCT;
}


static inline YArray ____YArray_new_aligned(const size_t item_size, const size_t count,
                                            const size_t hdr_size)
{
    YArray       ptr   = NULL;
    const size_t bytes = (item_size * count) + hdr_size;

    struct __YHdr *hdr = malloc(bytes);
    if (hdr) {
        hdr->size           = 0;
        hdr->capacity       = count;
        hdr->item_size      = item_size;
        hdr->hdr_size       = hdr_size;
        ptr                 = ____YArray_hdr_ptr_offset(hdr, hdr_size);
        *((void **)ptr - 1) = (void *)hdr;
    }

    return ptr;
}


static inline YArray __YArray_new_aligned(const size_t item_size, const size_t count,
                                          const size_t alignment)
{
    // For now, support only arrays of items of at least 4 bytes (`int`s) (DWORD).
    assert(alignment % 4 == 0);

    const size_t hdr_size = ____YArray_hdr_size(alignment);
    return ____YArray_new_aligned(item_size, count, hdr_size);
}


static inline void **__YArray_at(YArray a, const size_t index)
{
    struct __YHdr *restrict hdr = ____YArray_hdr(a);
    if (hdr) {
        return (void **)____YArray_at(a, index, hdr->item_size);
    }
    return NULL;
}


static inline void ____YArray_grow_size(YArray a, const size_t size)
{
    struct __YHdr *__restrict hdr = ____YArray_hdr(a);
    if (hdr) {
        hdr->size += size;
    }
    return;
}


static inline void __YArray_set(YArray a, int value)
{
    const struct __YHdr *hdr = ____YArray_hdr(a);
    if (hdr) {
        memset(a, value, hdr->item_size * hdr->capacity); // TODO: size??
    }
    return;
}


static inline size_t __YArray_size(YArray a)
{
    const struct __YHdr *const restrict hdr = ____YArray_hdr(a);
    if (hdr) {
        return hdr->size;
    }
    return 0;
}


static inline size_t __YArray_space(const YArray a)
{
    const struct __YHdr *__restrict hdr = ____YArray_hdr(a);
    if (hdr) {
        return hdr->capacity - hdr->size;
    }
    return 0;
}


static inline size_t __YArray_capacity(YArray a)
{
    const struct __YHdr *const restrict hdr = ____YArray_hdr(a);
    if (hdr) {
        return hdr->capacity;
    }
    return 0;
}


static inline void ____YArray_resize_inpl(YArray *a, const size_t new_capacity)
{
    YArray arr                  = *a;
    struct __YHdr *restrict hdr = ____YArray_hdr(arr);

    // Compute realloc capacity
    const size_t hdr_size = hdr->hdr_size;
    const YArray new_arr  = ____YArray_new_aligned(hdr->item_size, new_capacity, hdr_size);
    const size_t cp_bytes = hdr->item_size * hdr->capacity;
    memcpy(new_arr, arr, cp_bytes);

    // Don't forget to bring array size
    struct __YHdr *new_hdr = ____YArray_hdr(new_arr);
    new_hdr->size          = hdr->size;

    *a = new_arr;
    free(hdr);
    return;
}


static inline void __YArray_resize(YArray *a, const size_t new_capacity)
{
    YArray arr = *a;
    if (!arr) {
        return;
    }
    struct __YHdr *restrict hdr = ____YArray_hdr(arr);
    if (!hdr) {
        return;
    }

    if (new_capacity <= hdr->capacity) {
        return;
    }
    ____YArray_resize_inpl(a, new_capacity);
    return;
}


static inline void __YArray_push_back(YArray *arr, void **item)
{
    YArray a = *arr;

    struct __YHdr *restrict hdr = ____YArray_hdr(a);
    if (hdr) {
        const size_t item_size = hdr->item_size;
        const size_t curr_size = hdr->size;
        if (curr_size == hdr->capacity) {
            ____YArray_resize_inpl(arr, ____YArray_new_size(hdr->size));
            a   = *arr;
            hdr = ____YArray_hdr(a);
        }
        char *pos = ____YArray_at(a, curr_size, item_size);
        memcpy(pos, item, item_size);
        ++hdr->size;
    }
    return;
}


static inline void __YArray_merge(YArray *root, YArray child)
{
    // c
    struct __YHdr       *r_hdr   = ____YArray_hdr(*root);
    const struct __YHdr *c_hdr   = ____YArray_hdr(child);
    const size_t         r_space = r_hdr->capacity - r_hdr->size;
    const size_t         c_size  = __YArray_size(child);
    if (c_size > r_space) {
        ____YArray_resize_inpl(root, ____YArray_new_size(__YArray_size(*root)));
        r_hdr = ____YArray_hdr(*root);
    }
    for (size_t i = 0; i < c_size; ++i) {
        __YArray_push_back(root, __YArray_at(child, i));
    }
    return;
}


static inline void __YArray_empty(YArray a)
{
    struct __YHdr *hdr = ____YArray_hdr(a);
    if (hdr) {
        hdr->size = 0;
    }
    return;
}


static inline void __YArray_free(YArray a)
{
    struct __YHdr *hdr = ____YArray_hdr(a);
    if (hdr) {
        free((void *)hdr);
    }
    return;
}


/*------------------------------------------------------------*/
/*--- YArray APIs                                          ---*/
/*------------------------------------------------------------*/

# define YArray()                            (YArray)0
# define YArray_new(T, N)                    (T *)__YArray_new_aligned(sizeof(T), N, sizeof(T))
# define YArray_new_aligned(T, N, alignment) (T *)__YArray_new_aligned(sizeof(T), N, alignment)
# define YArray_at(arr, index)               __YArray_at((YArray)arr, index)
# define YArray_set(arr, val)                __YArray_set((YArray)arr, val)
# define YArray_size(arr)                    __YArray_size((YArray)arr)
# define YArray_space(arr)                   __YArray_space((YArray)arr)
# define YArray_capacity(arr)                __YArray_capacity((YArray)arr)
# define YArray_resize(arr, N)               __YArray_resize((YArray *)&arr, N)
# define YArray_push_back(arr, item)         __YArray_push_back((YArray *)&arr, (void **)item)
# define YArray_merge(root, child)           __YArray_merge((YArray *)&root, (YArray)child)
# define YArray_empty(a)                     __YArray_empty(a)
# define YArray_free(arr)                    __YArray_free((YArray)arr)


#endif // YACAI_IMPLEMENTATION


#ifdef __cplusplus
}
#endif


#endif // YACAI_H_
