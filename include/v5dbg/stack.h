#pragma once
#include <string>
#include <cstdint>
#include "v5dbg/server.h"

/// @brief  Stack frame information from $function
struct v5dbg_stack_frame_t
{
  /// @brief  Function name
  std::string funcName;

  /// @brief  Jump address of the function
  void *jmpAddress;
};

/**
 * Injected variable created with $function, allows for the creation of stack traces with pretty function names.
 * On construction we push a new v5dbg_stack_frame_t to the debugger, on destruction we remove it
 */
class V5DbgStackCatcher
{
public:
  V5DbgStackCatcher(const std::string &name, void *pAddress)
  {
    m_name = name;

    v5dbg_thread_t* thread = V5Dbg_ThreadForTask();
    if (thread == nullptr)
    {
      return;
    }

    v5dbg_stack_frame_t frame{};
    frame.funcName = name;
    frame.jmpAddress = pAddress;

    thread->threadLock->lock();
    thread->stack.push_back(frame);

    thread->threadLock->unlock();
  }

  ~V5DbgStackCatcher()
  {
    v5dbg_thread_t* thread = V5Dbg_ThreadForTask();
    if (thread == nullptr)
    {
      return;
    }

    thread->stack.pop_back();

    // When the program is paused before a stack pop we can inspect each threads virtual stack in order to determine the current execution location down to the function
  }

private:
  std::string m_name;
};

/// @brief  Collect a stack frame
#define $function V5DbgStackCatcher _fStackCatch(__PRETTY_FUNCTION__, nullptr);