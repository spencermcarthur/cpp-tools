#pragma once

#define NO_COPY_OR_MOVE(CLASS_NAME)                     \
    CLASS_NAME(const CLASS_NAME &) = delete;            \
    CLASS_NAME &operator=(const CLASS_NAME &) = delete; \
    CLASS_NAME(CLASS_NAME &&) = delete;                 \
    CLASS_NAME &operator=(CLASS_NAME &&) = delete
