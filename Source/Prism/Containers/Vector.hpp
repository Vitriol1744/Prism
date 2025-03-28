/*
 * Created by v1tr10l7 on 28.02.2025.
 * Copyright (c) 2024-2025, Szymon Zemke <v1tr10l7@proton.me>
 *
 * SPDX-License-Identifier: GPL-3
 */
#pragma once

#include <Prism/Core/Compiler.hpp>
#include <Prism/Core/Types.hpp>

#include <cassert>
#include <limits>
#include <new>

void operator delete(void* ptr, Prism::usize) noexcept;

namespace Prism
{
    template <typename T>
    class Vector
    {
      public:
        using ValueType            = T;
        using SizeType             = usize;
        using DifferenceType       = std::ptrdiff_t;
        using ReferenceType        = ValueType&;
        using ConstReferenceType   = const ValueType&;
        using PointerType          = ValueType*;
        using ConstPointerType     = const ValueType*;
        using Iterator             = T*;
        using ConstIterator        = const T*;
        using ReverseIterator      = std::reverse_iterator<Iterator>;
        using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

        Vector() { Reserve(2); }
        Vector(std::initializer_list<T> init)
        {
            Reserve(2);
            for (const auto& value : init) PushBack(value);
        }

        constexpr ReferenceType At(SizeType pos)
        {
            assert(pos < m_Size);

            return m_Data[pos];
        }
        constexpr ConstReferenceType At(SizeType pos) const
        {
            assert(pos < m_Size);

            return m_Data[pos];
        }
        constexpr ReferenceType operator[](SizeType pos) { return At(pos); }
        constexpr ConstReferenceType operator[](SizeType pos) const
        {
            return At(pos);
        }
        constexpr ReferenceType      Front() { return m_Data[0]; }
        constexpr ConstReferenceType Front() const { return m_Data[0]; }
        constexpr ReferenceType      Back() { return m_Data[m_Size - 1]; }
        constexpr ConstReferenceType Back() const { return m_Data[m_Size - 1]; }
        constexpr T*                 Raw() noexcept { return m_Data; }
        constexpr const T*           Raw() const noexcept { return m_Data; }

        constexpr Iterator           begin() noexcept { return m_Data; }
        constexpr ConstIterator      begin() const noexcept { return m_Data; }
        constexpr ConstIterator      cbegin() const noexcept { return m_Data; }
        constexpr Iterator           end() noexcept { return m_Data + m_Size; }
        constexpr ConstIterator      end() const noexcept { return m_Data; }
        constexpr ConstIterator      cend() const noexcept { return m_Data; }

        constexpr ReverseIterator    rbegin() noexcept
        {
            return ReverseIterator(m_Data + m_Size);
        }
        constexpr ConstReverseIterator rbegin() const noexcept
        {
            return ConstReverseIterator(m_Data + m_Size);
        }
        constexpr ConstReverseIterator crbegin() const noexcept
        {
            return ConstReverseIterator(m_Data + m_Size);
        }
        constexpr ReverseIterator rend() noexcept
        {
            return ReverseIterator(m_Data);
        }
        constexpr ConstReverseIterator rend() const noexcept
        {
            return ConstReverseIterator(m_Data);
        }
        constexpr ConstReverseIterator crend() const noexcept
        {
            return ConstReverseIterator(m_Data);
        }

        [[nodiscard]] constexpr bool Empty() const noexcept
        {
            return m_Size == 0;
        }
        constexpr SizeType Size() const noexcept { return m_Size; }
        constexpr SizeType MaxSize() const noexcept
        {
            return std::numeric_limits<DifferenceType>::max();
        }
        constexpr void Reserve(SizeType newCapacity)
        {
            if (newCapacity < m_Size) return;

            T* newData = new T[newCapacity];
            for (SizeType i = 0; i < m_Size; i++) newData[i] = m_Data[i];

            m_Capacity = newCapacity;
            if (m_Size > 0) delete[] m_Data;
            m_Data = newData;
        }
        constexpr SizeType Capacity() const noexcept { return m_Capacity; }
        constexpr void     ShrinkToFit()
        {
            if (m_Size <= m_Capacity) return;
            PointerType newData = reinterpret_cast<PointerType>(
                ::operator new(Size() * sizeof(ValueType)));

            for (SizeType i = 0; i < m_Size; i++)
            {
                new (&newData[i]) ValueType(std::move(m_Data[i]));
                m_Data[i].~ValueType();
            }

            ::operator delete(m_Data, m_Capacity * sizeof(ValueType));
            m_Data     = newData;
            m_Capacity = m_Size;
        }
        constexpr Iterator Erase(Iterator pos)
        {
            size_t i = pos - m_Data;
            for (; i < (m_Size - 1); i++) m_Data[i] = std::move(m_Data[i + 1]);

            --m_Size;

            Iterator next = ++pos;
            return next;
        }

        constexpr void Clear() noexcept
        {
            for (SizeType i = 0; i < m_Size; i++) m_Data[i].~ValueType();
            m_Size = 0;
        }
        void PushBack(T& value)
        {
            if (m_Size >= m_Capacity) Reserve(m_Capacity ? m_Capacity * 2 : 1);
            m_Data[m_Size] = value;
            ++m_Size;
        }
        void PushBack(const T& value)
        {
            if (m_Size >= m_Capacity) Reserve(m_Capacity ? m_Capacity * 2 : 1);
            m_Data[m_Size] = value;
            ++m_Size;
        }
        void PushBack(T&& value)
        {
            if (m_Size >= m_Capacity) Reserve(m_Capacity ? m_Capacity * 2 : 1);

            m_Data[m_Size] = std::move(value);
            ++m_Size;
        }
        constexpr void PopBack()
        {
            if (m_Size == 0) return;

            m_Data[m_Size - 1].~ValueType();
            m_Size--;
        }
        constexpr void Resize(SizeType count)
        {
            Reserve(count);
            for (SizeType i = m_Size; i < m_Capacity; i++)
                new (&m_Data[i]) ValueType();
            m_Size = count;
        }
        constexpr void Resize(SizeType count, const ValueType& value)
        {
            Reserve(count);
            for (SizeType i = m_Size; i < m_Capacity; i++)
                new (&m_Data[i]) ValueType(value);
            m_Size = count;
        }

      private:
        PointerType m_Data     = nullptr;
        SizeType    m_Size     = 0;
        SizeType    m_Capacity = 0;
    };
} // namespace Prism
#if PRISM_TARGET_CRYPTIX == 1
using Prism::Vector;
#endif
