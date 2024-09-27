#ifndef MCS_LOCK_H_
#define MCS_LOCK_H_
#pragma once

#include <atomic>

struct MCSLock {
  struct QNode {
    std::atomic<QNode*> next{nullptr};
    std::atomic<bool> locked{false};
    };

  std::atomic<QNode*> tail{nullptr};
  static thread_local QNode qnode;
  void lock();

  void unlock();
};


#endif // MCS_LOCK_H_
