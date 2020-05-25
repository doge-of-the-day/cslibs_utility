#ifndef CSLIBS_UTILITY_SYNCHRONIZED_PRIORITY_QUEUE_HPP
#define CSLIBS_UTILITY_SYNCHRONIZED_PRIORITY_QUEUE_HPP

#include <ext/pb_ds/priority_queue.hpp>
#include <mutex>
#include <queue>

namespace cslibs_utility {
namespace synchronized {
/**
 * @param _Tp       - comparable data types
 * @param _Compare  - comparing data types, please make sure to enter a sort
 * function which results in a unique asssociation of elements, otherwise, the
 * order within the queue is not guaranteed.
 */
template <typename _Tp, typename _Compare = std::less<_Tp>,
          typename _Alloctor = std::allocator<char>>
class priority_queue {
 public:
  using mutex_t = std::mutex;
  using lock_t = std::unique_lock<mutex_t>;
  using queue_t =
      __gnu_pbds::priority_queue<_Tp, _Compare,
                                 __gnu_pbds::rc_binomial_heap_tag, _Alloctor>;

  inline priority_queue() = default;
  inline ~priority_queue() { q_.clear(); }

  inline bool empty() const {
    lock_t l(mutex_);
    return q_.empty();
  }

  inline bool hasElements() const { return !empty(); }

  inline std::size_t size() const {
    lock_t l(mutex_);
    return q_.size();
  }

  inline _Tp pop() {
    lock_t l(mutex_);
    _Tp t = q_.top();
    q_.pop();
    return t;
  }

  inline _Tp const &top() const {
    lock_t l(mutex_);
    return q_.top();
  }

  inline void emplace(const _Tp &t) {
    lock_t l(mutex_);
    q_.push(t);
  }

  inline void clear() {
    lock_t l(mutex_);
    while (!q_.empty()) {
      q_.pop();
    }
  }

 private:
  mutable mutex_t mutex_;
  queue_t q_;
};
}  // namespace synchronized
}  // namespace cslibs_utility

#endif  // SYNCHRONIZED_PRIORITY_QUEUE_HPP
