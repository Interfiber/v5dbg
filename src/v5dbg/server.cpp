#include "v5dbg/server.h"
#include "v5dbg/debugger.h"
#include "v5dbg/stack.h"
#include <iostream>
#include <mutex>

v5dbg_server_state_t V5Dbg_AllocateServerState()
{
  v5dbg_server_state_t state{};

  state.threadListLock = new pros::rtos::Mutex();
  state.messageQueueLock = new pros::rtos::Mutex();
  state.canRun = true;

  return state;
}

void V5Dbg_StartServer(v5dbg_server_state_t *pState)
{
  if (pState == nullptr)
  {
    return;
  }

  CURRENT_SERVER = pState;

  pros::rtos::Task *serverTask = new pros::rtos::Task([]() {
    V5Dbg_ServerMain();
  }, TASK_PRIORITY_MAX, TASK_STACK_DEPTH_DEFAULT, "v5dbg Server");

  pState->serverTask = serverTask;
}


v5dbg_thread_t* V5Dbg_Init()
{
  return V5Dbg_RemoteInit(pros::rtos::Task::current());
}

v5dbg_thread_t* V5Dbg_RemoteInit(pros::rtos::Task other)
{
  printf("RemoteInit\n");
  CURRENT_SERVER->threadListLock->lock();
  printf("RemoteInitLocked\n");

  v5dbg_thread_t thread = V5Dbg_AllocateServerThread();
  thread.task = other;
  thread.name = other.get_name();
  thread.threadLock = new pros::rtos::Mutex();
  thread.threadState = (other.get_state() & pros::E_TASK_STATE_SUSPENDED) ? THREAD_STATE_SUSPEND : THREAD_STATE_RUN;

  CURRENT_SERVER->threads.push_back(thread);

  CURRENT_SERVER->threadListLock->unlock();
  printf("RemoteInitDone\n");

  return nullptr;
}


v5dbg_thread_t V5Dbg_AllocateServerThread()
{
  v5dbg_thread_t thread{};
  thread.threadLock = new pros::rtos::Mutex();

  return thread;
}

void V5Dbg_ServerIOMain()
{
  while (CURRENT_SERVER->canRun)
  {
    std::string cmdBuffer;
    std::getline(std::cin, cmdBuffer);

    V5Dbg_PushMessage(V5Dbg_DeserializeMessage(cmdBuffer));
  }
}

void V5Dbg_ServerMain()
{
  CURRENT_SERVER->ioTask = new pros::rtos::Task([] () {
    V5Dbg_ServerIOMain();
  }, TASK_PRIORITY_MAX, TASK_STACK_DEPTH_DEFAULT, "v5dbg IO server");

  while (CURRENT_SERVER->canRun)
  {
    if (!V5Dbg_CanPump())
    {
      pros::delay(10);
      continue;
    }

    const v5dbg_message_t message = V5Dbg_NextMessage();

    if (message.type == DEBUGGER_MESSAGE_SUSPEND)
    {
      V5Dbg_SuspendProgram();
    }
    else if (message.type == DEBUGGER_MESSAGE_ALLOCATE_STRING)
    {
      printf("%s\n", V5Dbg_SerializeMessage(message).c_str());
    }

    // TODO:
    // Call V5Dbg_GetNextCommand() until nullptr is returned
    // Call stack generation is done by V5DbgStackCatcher

    // Debugger delay of 10ms
    pros::delay(10);
  }
}

bool V5Dbg_CanPump()
{
  return !CURRENT_SERVER->messageQueue.empty();
}


v5dbg_message_t V5Dbg_NextMessage()
{
  CURRENT_SERVER->messageQueueLock->lock();

  const v5dbg_message_t msg = CURRENT_SERVER->messageQueue.back();

  CURRENT_SERVER->messageQueue.pop();
  CURRENT_SERVER->messageQueueLock->unlock();

  return msg;
}

v5dbg_thread_t* V5Dbg_ThreadForTask()
{
  CURRENT_SERVER->threadListLock->lock();

  for (auto &task : CURRENT_SERVER->threads)
  {
    if (task.task.get_name() == pros::rtos::Task::current().get_name())
    {
      CURRENT_SERVER->threadListLock->unlock();
      return &task;
    }
  }

  CURRENT_SERVER->threadListLock->unlock();

  return nullptr;
}


void V5Dbg_PushMessage(const v5dbg_message_t &msg)
{
  std::lock_guard<pros::rtos::Mutex> _g(*CURRENT_SERVER->messageQueueLock);

  CURRENT_SERVER->messageQueue.push(msg);
}