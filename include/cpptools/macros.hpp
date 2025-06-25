#pragma once

#define CPPTOOLS_NO_COPY_OR_MOVE(CLASS)       \
    CLASS(const CLASS &) = delete;            \
    CLASS &operator=(const CLASS &) = delete; \
    CLASS(CLASS &&) = delete;                 \
    CLASS &operator=(CLASS &&) = delete

#define CPPTOOLS_NO_COPY(CLASS)    \
    CLASS(const CLASS &) = delete; \
    CLASS &operator=(const CLASS &) = delete

#ifndef CPPTOOLS_CACHELINE_SIZE
#define CPPTOOLS_CACHELINE_SIZE 64
#endif
