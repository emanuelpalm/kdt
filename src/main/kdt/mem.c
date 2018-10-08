#include "mem.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <kdt/endian.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

inline
mem_t mem_FromBuffer(uint8_t *buffer, size_t size) {
    assert(!(buffer == NULL && size > 0));

    return (mem_t) {
        .begin = buffer,
        .offset = buffer,
        .end = buffer == NULL ? buffer : &buffer[size],
    };
}

inline
mem_t mem_FromString(char *string) {
    if (string == NULL) {
        string = "";
    }
    size_t length = strlen(string);
    return (mem_t) {
        .begin = (uint8_t *) string,
        .offset = (uint8_t *) string,
        .end = (uint8_t *) &string[length],
    };
}

inline
size_t mem_Capacity(mem_t *mem) {
    assert(mem != NULL);

    return mem->end - mem->begin;
}

inline
size_t mem_Read(mem_t *mem, size_t size, uint8_t *out) {
    const size_t space = mem_Space(mem);
    if (size > space) {
        size = space;
    }
    memcpy(out, mem->offset, size);
    return size;
}

#define _GEN_MEM_READ_WORD(NAME, TYPE, TRANSFORMER)        \
    inline                                                 \
    bool NAME(mem_t *mem, TYPE *out) {                     \
        if (mem_Space(mem) < sizeof(TYPE)) {               \
            return false;                                  \
        }                                                  \
        *out = TRANSFORMER(((TYPE *) &mem->offset[0])[0]); \
        mem->offset = &mem->offset[sizeof(TYPE)];          \
        return true;                                       \
    }

_GEN_MEM_READ_WORD(mem_ReadU16BE, uint16_t, endian_IntoBE16);
_GEN_MEM_READ_WORD(mem_ReadU32BE, uint32_t, endian_IntoBE32);

#undef _GEN_MEM_READ_WORD

inline
void mem_Reset(mem_t *mem) {
    assert(mem != NULL);

    mem->offset = mem->begin;
}

inline
size_t mem_Size(mem_t *mem) {
    assert(mem != NULL);

    return mem->offset - mem->begin;
}

inline
bool mem_Skip(mem_t *mem, size_t size) {
    assert(mem != NULL);

    uint8_t *offset = &mem->offset[size];
    if (offset > mem->end) {
        return false;
    }
    mem->offset = offset;
    return true;
}

inline
bool mem_SkipCharacter(mem_t *mem, char character) {
    assert(mem != NULL);

    if (mem_Space(mem) == 0) {
        return false;
    }
    if (mem->offset[0] == character) {
        mem->offset = &mem->offset[1];
        return true;
    }
    return false;
}

inline
void mem_SkipSpace(mem_t *mem) {
    assert(mem != NULL);

    while (mem->offset < mem->end && isspace(mem->offset[0])) {
        mem->offset = &mem->offset[1];
    }
}

inline
size_t mem_Space(mem_t *mem) {
    assert(mem != NULL);

    return mem->end - mem->offset;
}

inline
size_t mem_Write(mem_t *mem, void *buffer, size_t size) {
    assert(mem != NULL);
    assert(buffer != NULL || size == 0);

    const size_t space = mem_Space(mem);
    if (size > space) {
        size = space;
    }
    memcpy(mem->offset, buffer, size);
    mem->offset = &mem->offset[size];
    return space;
}

#define _GEN_MEM_WRITE_WORD(NAME, TYPE, TRANSFORMER)       \
    inline                                                 \
    bool NAME(mem_t *mem, TYPE half) {                     \
        if (mem_Space(mem) < sizeof(TYPE)) {               \
            return false;                                  \
        }                                                  \
        ((TYPE *) &mem->offset[0])[0] = TRANSFORMER(half); \
        mem->offset = &mem->offset[sizeof(TYPE)];          \
        return true;                                       \
    }

_GEN_MEM_WRITE_WORD(mem_Write8, uint8_t, )
_GEN_MEM_WRITE_WORD(mem_WriteU16BE, uint16_t, endian_FromBE16)
_GEN_MEM_WRITE_WORD(mem_WriteU32BE, uint32_t, endian_FromBE32)

#undef _GEN_MEM_WRITE_WORD

inline
err_t mem_WriteF(mem_t *mem, const char *format, ...) {
    assert(mem != NULL);
    assert(format != NULL);

    va_list args;
    va_start(args, format);

    const size_t size = mem->end - mem->offset;
    int len = vsnprintf((char *) mem->offset, size, format, args);
    if (len < 0) {
        return (err_t) errno;
    }
    mem->offset = &mem->offset[len];

    va_end(args);

    return ERR_NONE;
}

inline
void mem_WriteX(mem_t *mem, const uint8_t *buffer, size_t size) {
    assert(mem != NULL);
    assert(buffer != NULL || size == 0);

    const size_t space = mem_Space(mem);
    if (size > space) {
        size = space;
    }

    const uint8_t map[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    for (int i = 0; i < size; ++i) {
        uint8_t byte = buffer[i];
        mem->offset[0] = map[(byte & 0xF0u) >> 4u];
        mem->offset[1] = map[(byte & 0x0Fu)];
        mem->offset = &mem->offset[2];
    }

    if (mem_Space(mem) > 0) {
        mem->offset[0] = '\0';
    }
}