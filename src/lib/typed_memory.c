/* SPDX-License-Identifier: BSD-2-Clause */
#include "config.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "pkcs11.h"
#include "typed_memory.h"

void *type_calloc(size_t nmemb, size_t size, CK_BYTE type) {

    assert(size != 0);

    // overflow safety here...
    size_t total = nmemb * size;
    total += 1;

    CK_BYTE_PTR ptr = (CK_BYTE_PTR)calloc(1, total);
    if (!ptr) {
        return NULL;
    }

    ptr[total - 1] = type;

    return ptr;
}

void *type_realloc(void *orig, size_t size, CK_BYTE type) {

    assert(size != 0);

    // overflow safety here...
    size_t total = size;
    total += 1;

    CK_BYTE_PTR ptr = (CK_BYTE_PTR)realloc(orig, total);
    if (!ptr) {
        return NULL;
    }

    ptr[total - 1] = type;

    return ptr;
}

CK_BYTE type_from_ptr(void *ptr, size_t len) {

    if (!len || !ptr) {
        return TYPE_BYTE_HEX_STR;
    }

    CK_BYTE_PTR b = (CK_BYTE_PTR)ptr;
    return b[len];
}

CK_RV type_mem_dup(void *in, size_t len, void **dup) {

    CK_BYTE type = in ?
            type_from_ptr(in, len) : TYPE_BYTE_HEX_STR;

    assert(type != 0);

    void *buf = type_calloc(1, len, type);
    if (!buf) {
        return CKR_HOST_MEMORY;
    }

    if (in) {
        memcpy(buf, in, len);
    }

    *dup = buf;

#ifndef NDEBUG
    CK_BYTE check = type_from_ptr(buf, len);
    assert(check == type);
#endif
    return CKR_OK;
}

void type_mem_cpy(void *dest, void *in, size_t size) {
    assert(in);
    assert(dest);
    assert(size);
    memcpy(dest, in, size + 1);
#ifndef NDEBUG
    CK_BYTE check = type_from_ptr(in, size);
    CK_BYTE got = type_from_ptr(dest, size);
    assert(check == got);
#endif
}
