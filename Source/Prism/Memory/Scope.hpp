/*
 * Created by v1tr10l7 on 27.02.2025.
 * Copyright (c) 2024-2025, Szymon Zemke <v1tr10l7@proton.me>
 *
 * SPDX-License-Identifier: GPL-3
 */
#pragma once

#include <Prism/Core/Core.hpp>
#include <Prism/Memory/Ref.hpp>
#include <Prism/Memory/WeakRef.hpp>

namespace Prism
{
    template <typename T>
    using ScopeDeleter = std::default_delete<T>;

    template <typename T, typename Deleter = ScopeDeleter<T>()>
    class Scope
    {
      public:
        Scope()
            : m_Instance(nullptr)
        {
        }
        Scope(std::nullptr_t)
            : m_Instance(nullptr)
        {
        }
        Scope(Scope&& other)
            : m_Instance(other.m_Instance)
        {
            other.m_Instance = nullptr;
        }

        template <typename U>
        Scope(Scope<U>&& other)
            : m_Instance(other.Leak())
        {
        }

        Scope(Scope const&) = delete;
        template <typename U>
        Scope(Scope<U> const&)         = delete;
        Scope& operator=(Scope const&) = delete;
        template <typename U>
        Scope& operator=(Scope<U> const&) = delete;

        template <typename U>
        Scope(Ref<U> const&) = delete;
        template <typename U>
        Scope(WeakRef<U> const&) = delete;

        ~Scope() { Reset(); }

        T*   Release() PM_NOEXCEPT { return Exchange(m_Instance, nullptr); }
        void Reset(T* instance = nullptr) PM_NOEXCEPT
        {
            auto ptr = Exchange(m_Instance, instance);
            Deleter{}(ptr);
        }

        void Swap(Scope& other) PM_NOEXCEPT
        {
            Swap(m_Instance, other.m_Instance);
        }
        template <typename U>
        void Swap(Scope<U>& other)
        {
            Swap(m_Instance, other.m_Instance);
        }

        T*                 Raw() const PM_NOEXCEPT { return m_Instance; }

        constexpr Deleter& GetDeleter() PM_NOEXCEPT { return Deleter(); }
        constexpr Deleter& GetDeleter() const PM_NOEXCEPT { return Deleter(); }

        explicit           operator bool() const PM_NOEXCEPT
        {
            return m_Instance != nullptr;
        }

        AddLValueReferenceType<T> operator*() const
            PM_NOEXCEPT(PM_NOEXCEPT(*DeclVal<T*>()))
        {
            return *m_Instance;
        }

        T* operator->() const PM_NOEXCEPT
        {
            Assert(m_Instance);
            return m_Instance;
        }

        T& operator[](usize i) const { return m_Instance[i]; }

        template <typename U>
        Scope& operator=(Ref<U> const&) = delete;
        template <typename U>
        Scope& operator=(WeakRef<U> const&) = delete;

        Scope& operator=(Scope&& other)
        {
            Scope ptr(Move(other));
            swap(ptr);
            return *this;
        }

        template <typename U>
        Scope& operator=(Scope<U>&& other)
        {
            Scope ptr(Move(other));
            Swap(ptr);
            return *this;
        }

        Scope& operator=(T* ptr) = delete;

        Scope& operator=(std::nullptr_t)
        {
            Reset();
            return *this;
        }

        [[nodiscard]]
        T* Leak()
        {
            T* leaked  = m_Instance;
            m_Instance = nullptr;
            return leaked;
        }

        static Scope Lift(T* other) { return Scope{other}; }

      private:
        T* m_Instance = nullptr;
    };
}; // namespace Prism

#ifdef PRISM_TARGET_CRYPTIX
using Prism::Scope;
#endif
