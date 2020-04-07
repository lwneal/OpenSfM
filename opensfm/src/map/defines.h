#pragma once
#include <memory>
#include <Eigen/Core>
namespace map
{
struct KeyCompare
{
    template<typename T>
    bool operator()(T* lhs, T* rhs) const { return lhs->id_ < rhs->id_; }
    template<typename T>
    bool operator()(T const* lhs, T const * rhs) const { return lhs->id_ < rhs->id_; }
};
using ShotId = size_t;
using LandmarkId = size_t;
using FeatureId = size_t;
using CameraId = size_t;
}

namespace internal {
template <typename Type>
struct aligned_delete {
  constexpr aligned_delete() noexcept = default;

  template <typename TypeUp,
            typename = typename std::enable_if<
                std::is_convertible<TypeUp*, Type*>::value>::type>
  aligned_delete(const aligned_delete<TypeUp>&) noexcept {}

  void operator()(Type* ptr) const {
    static_assert(sizeof(Type) > 0, "Can't delete pointer to incomplete type!");
    typedef typename std::remove_const<Type>::type TypeNonConst;
    Eigen::aligned_allocator<TypeNonConst> allocator;
    allocator.deallocate(ptr, 1u /*num*/);
  }
};
}  // namespace internal

template <typename Type>
struct AlignedUniquePtr {
  typedef typename std::remove_const<Type>::type TypeNonConst;
  typedef std::unique_ptr<Type, internal::aligned_delete<TypeNonConst>> type;
};

template <typename Type, typename... Arguments>
inline typename AlignedUniquePtr<Type>::type aligned_unique(
    Arguments&&... arguments) {
  typedef typename std::remove_const<Type>::type TypeNonConst;
  Eigen::aligned_allocator<TypeNonConst> allocator;
  TypeNonConst* obj = ::new (allocator.allocate(1u))  // NOLINT
      Type(std::forward<Arguments>(arguments)...);
  return std::move(typename AlignedUniquePtr<Type>::type(obj));
}