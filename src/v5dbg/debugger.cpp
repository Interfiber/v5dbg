#include "v5dbg/debugger.h"
#include "v5dbg/server.h"
#include "v5dbg/stack.h"
#include "pros/rtos.hpp"

void V5Dbg_SuspendProgram()
{
  printf("%p\n", CURRENT_SERVER->threadListLock);
  CURRENT_SERVER->threadListLock->lock();
  printf("Suspending program\n");

  for (auto &thread : CURRENT_SERVER->threads)
  {
    thread.task.suspend();

    // Print out current virtual call stack

    int sLevel = 0;
    for (auto &sFrame : thread.stack)
    {
      printf("%i %s\n", sLevel, sFrame.funcName.c_str());

      ++sLevel;
    }
  }

  CURRENT_SERVER->threadListLock->unlock();
}