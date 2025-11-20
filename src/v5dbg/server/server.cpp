#include "v5dbg/server.h"
#include "v5dbg/debinfo.h"
#include "pros/rtos.h"
#include "pros/rtos.hpp"
#include "v5dbg/state.h"
#include "v5dbg/stack.h" // We need this otherwise v5dbg_server_state_t objects cannot be created due to them containing stack frame objects
#include "v5dbg/util.h"

using namespace pros::c;

v5dbg_server_state_t
V5Dbg_AllocateServerState()
{
  v5dbg_server_state_t state{};

  state.threadListLock = new pros::rtos::Mutex();
  state.messageQueueLock = new pros::rtos::Mutex();
  state.canRun = true;

  return state;
}

void
V5Dbg_StartServer(v5dbg_server_state_t* pState)
{
  #ifdef V5DBG_DISABLE
    info("Debugger disabled, refusing to start server. See v5dbg.mk for more information");
    return;
  #endif

  if (pState == nullptr)
  {
    info("Allocated state is nullptr");
    return;
  }

  info("ServerInit");

  CURRENT_SERVER = pState;

  // Default to serial IO
  if (V5Dbg_GetCurrentServer()->serial == nullptr)
  {
    V5Dbg_SetWriteMode(pState, WRITE_MODE_SERIAL);
  }

  pros::rtos::Task* serverTask
    = new pros::rtos::Task([pState]() { V5Dbg_ServerMain(pState); }, TASK_PRIORITY_MAX, TASK_STACK_DEPTH_DEFAULT, "v5dbg Server");

  pState->serverTask = serverTask;
}


v5dbg_thread_t
V5Dbg_AllocateServerThread()
{
  v5dbg_thread_t thread{};
  thread.threadLock = new pros::rtos::Mutex();

  return thread;
}

v5dbg_server_state_t*
V5Dbg_GetCurrentServer()
{
  return CURRENT_SERVER;
}