// copy - move semantics
#define DECLARE_DEFAULT_COPYABLE(Type) \
    Type(const Type&) = default; \
    Type& operator=(const Type&) = default;
#define DECLARE_DEFAULT_COPYABLE_NOEXCEPT(Type) \
    Type(const Type&) noexcept = default; \
    Type& operator=(const Type&) noexcept = default;
#define DECLARE_DEFAULT_MOVABLE(Type) \
    Type(Type&&) = default; \
    Type& operator=(Type&&) = default;
#define DECLARE_DEFAULT_MOVABLE_NOEXCEPT(Type) \
    Type(Type&&) noexcept = default; \
    Type& operator=(Type&&) noexcept = default;
#pragma once

#define DECLARE_NON_COPYABLE(Type) \
    Type(const Type&) = delete; \
    Type& operator=(const Type&) = delete;
#define DECLARE_NON_MOVABLE(Type) \
    Type(Type&&) = delete; \
    Type& operator=(Type&&) = delete;

#define DECLARE_PINNED(Type) \
    DECLARE_NON_COPYABLE(Type) \
    DECLARE_NON_MOVABLE(Type)
#define DECLARE_COPY_ONLY(Type) \
    DECLARE_DEFAULT_COPYABLE(Type) \
    DECLARE_NON_MOVABLE(Type)
#define DECLARE_COPY_ONLY_NOEXCEPT(Type) \
    DECLARE_DEFAULT_COPYABLE_NOEXCEPT(Type) \
    DECLARE_NON_MOVABLE(Type)
#define DECLARE_MOVE_ONLY(Type) \
    DECLARE_NON_COPYABLE(Type) \
    DECLARE_DEFAULT_MOVABLE(Type)
#define DECLARE_MOVE_ONLY_NOEXCEPT(Type) \
    DECLARE_NON_COPYABLE(Type) \
    DECLARE_DEFAULT_MOVABLE_NOEXCEPT(Type)
#define DECLARE_TRANSFERABLE(Type) \
    DECLARE_DEFAULT_COPYABLE(Type) \
    DECLARE_DEFAULT_MOVABLE(Type)
#define DECLARE_TRANSFERABLE_NOEXCEPT(Type) \
    DECLARE_DEFAULT_COPYABLE_NOEXCEPT(Type) \
    DECLARE_DEFAULT_MOVABLE_NOEXCEPT(Type)

#define DECLARE_NON_ASSIGNABLE(Type) \
    Type& operator=(const Type&) = delete; \
    Type& operator=(Type&&) = delete;
#define DECLARE_CONSTRUCT_ONLY(Type) \
    Type(const Type&) = default; \
    Type(Type&&) = default; \
    DECLARE_NON_ASSIGNABLE(Type)
#define DECLARE_CONSTRUCT_ONLY_NOEXCEPT(Type) \
    Type(const Type&) noexcept = default; \
    Type(Type&&) noexcept = default; \
    DECLARE_NON_ASSIGNABLE(Type)

// interface semantics
#define DECLARE_INTERFACE(Type) \
    Type() = default; \
    virtual ~Type() = default; \
    DECLARE_PINNED(Type)
#define DECLARE_INTERFACE_NOEXCEPT(Type) \
    Type() noexcept = default; \
    virtual ~Type() = default; \
    DECLARE_PINNED(Type)

