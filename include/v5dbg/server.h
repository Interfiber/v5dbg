#pragma once
#include <vector>
#include <queue>
#include "pros/rtos.hpp"
#include "v5dbg/protocol.h"

struct v5dbg_stack_frame_t;

enum v5dbg_thread_state_e {
  THREAD_STATE_RUN = 0 << 1,
  THREAD_STATE_SUSPEND = 0 << 2
};

/// @brief Thread with a stack trace
struct v5dbg_thread_t {
  /// @brief  Name of the thread, by default this is the task name
  std::string name;

  /// @brief  Underlying task object for this thread
  pros::rtos::Task task = pros::rtos::Task::current();

  /// @brief Bitmask of the thread state
  v5dbg_thread_state_e threadState = THREAD_STATE_RUN;

  /// @brief  Locked when modifiying any debugger stored data for this thread
  pros::rtos::Mutex* threadLock;

  /// @brief  Allocate thread stack
  std::vector<v5dbg_stack_frame_t> stack;

  /// @brief  Number of popped items from the virtual callstack
};

/// @brief  Server control state
struct v5dbg_server_state_t {
  /// @brief  Used to lock the list of managed threads
  pros::rtos::Mutex* threadListLock;

  /// @brief List of managed threads which have called V5Dbg_Init()
  std::vector<v5dbg_thread_t> threads;

  /// @brief  Lock the message queue during event pumping
  pros::rtos::Mutex* messageQueueLock;

  /// @brief  Message queue
  std::queue<v5dbg_message_t> messageQueue;

  /// @brief  Server task object
  pros::rtos::Task *serverTask;

  /// @brief  Server IO task
  pros::rtos::Task *ioTask;

  /// @brief  Turned to false when the server needs to be suspended
  bool canRun = true;
};

static v5dbg_server_state_t* CURRENT_SERVER = nullptr;

/**
 * Should be called as early as possible to start the debugger server
 * @brief Start the local debugger server
 * @param pState Allocated state object to use 
 */
void V5Dbg_StartServer(v5dbg_server_state_t *pState);

/// @brief  Should be called as the first line of code in a new task, allows the debugger to manage this task
v5dbg_thread_t* V5Dbg_Init();

/// @brief  Can be called to add another task object to the debugger
v5dbg_thread_t* V5Dbg_RemoteInit(pros::rtos::Task other);

/// @brief  Allocate a new server state object
v5dbg_server_state_t V5Dbg_AllocateServerState();

/// @brief  Allocate a new server thread object
v5dbg_thread_t V5Dbg_AllocateServerThread();

/// @brief  Return the v5dbg_thread_t* for the current task
v5dbg_thread_t* V5Dbg_ThreadForTask();

/// @brief  Internal server main function, should never be called manually
void V5Dbg_ServerMain();

/// @brief  Debug server IO thread
void V5Dbg_ServerIOMain();

/// @brief  Can we pump another message from the debugger message queue
bool V5Dbg_CanPump();

/// @brief  Return the next message in the queue
v5dbg_message_t V5Dbg_NextMessage();

/// @brief  Push a message into the outbound message queue
void V5Dbg_PushMessage(const v5dbg_message_t &message);