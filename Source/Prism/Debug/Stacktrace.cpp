/*
 * Created by v1tr10l7 on 17.11.2024.
 * Copyright (c) 2024-2024, Szymon Zemke <v1tr10l7@proton.me>
 *
 * SPDX-License-Identifier: GPL-3
 */
#include <Prism/Debug/Stacktrace.hpp>
#include <Prism/Utility/Math.hpp>

#include <demangler/Demangle.h>
#include <vector>

namespace Prism
{

    const std::string Stacktrace::Symbol::Demangle() const
    {
        return llvm::demangle(Name);
    }

    Stacktrace::Stacktrace(Pointer frameAddress, usize skipFrames,
                           usize maxDepth)
    {
        auto stackFrame = frameAddress.As<StackFrame>();

        for (usize i = 0; stackFrame && i < maxDepth; i++)
        {
            while (i < skipFrames) continue;

            u64 rip = stackFrame->InstructionPointer;
            if (!rip) break;
            stackFrame = stackFrame->PreviousFrame;

            m_Frames.PushBack(stackFrame);
        }
    }

    const Stacktrace::Symbol* Stacktrace::GetSymbol(PhysAddr address) const
    {
        if (address < m_LowestSymbolAddress || address > m_HighestSymbolAddress)
            return nullptr;
        const Symbol* ret = &m_Symbols[0];

        for (auto symbol : m_Symbols)
        {
            if ((&symbol + 1) == m_Symbols.end()) break;
            if (address < (&symbol + 1)->Address.Raw())
            {
                ret = &symbol;
                break;
            }
        }

        return ret;
    }

    Stacktrace Stacktrace::GetCurrent()
    {
        return Stacktrace(PrismGetFrameAddress(0));
    }
}; // namespace Prism
