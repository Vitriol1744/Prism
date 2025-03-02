/*
 * Created by v1tr10l7 on 01.03.2025.
 * Copyright (c) 2024-2025, Szymon Zemke <v1tr10l7@proton.me>
 *
 * SPDX-License-Identifier: GPL-3
 */
#pragma once

#ifdef __i386__
    #define PRISM_ARCH_IA32
    #define PRISM_ARCH_STRING "ia32"
#elifdef __x86_64__
    #define PRISM_ARCH_X86_64
    #define PRISM_ARCH_STRING "x86_64"
#elifdef __aarch64__
    #define PRISM_ARCH_AARCH64
    #define PRISM_ARCH_STRING "aarch64"
#elif defined(__riscv) && __riscv_xlen == 64
    #define PRISM_ARCH_RISCV64
    #define PRISM_ARCH_STRING "riscv64"
#elifdef __loongarch64
    #define PRISM_ARCH_LOONGARCH64
    #define PRISM_ARCH_STRING "loongarch64"
#endif
