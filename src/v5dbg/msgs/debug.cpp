#include "v5dbg/debinfo.h"
#include "v5dbg/msg.h"
#include "v5dbg/server.h"
#include "v5dbg/stack.h"

// Thread and stack manipulation

void
V5Dbg_ThreadListHandle(v5dbg_server_state_t* pState, const v5dbg_message_t& msg)
{
  v5dbg_message_t rVal{};
  rVal.type = DEBUGGER_MESSAGE_RTHREADS;

  int i = 0;
  for (auto& thread : pState->threads)
  {
    rVal.paramBuffer += thread.name + "," + std::to_string(i) + (i + 1 < (int)pState->threads.size() ? "," : "");

    i++;
  }

  V5Dbg_WriteToOut(V5Dbg_SerializeMessage(rVal));
}

void
V5Dbg_VStackForHandle(v5dbg_server_state_t* pState, const v5dbg_message_t& msg)
{
  v5dbg_message_t rVal{};
  rVal.type = DEBUGGER_MESSAGE_RVSTACK;

  if (msg.paramBuffer.empty())
    return;

  int threadID = std::stoi(msg.paramBuffer);

  int x = 0;
  for (auto& thread : pState->threads)
  {
    // Thread ID match
    if (x == threadID)
    {
      rVal.type = DEBUGGER_MESSAGE_RVSTACK;

      // Generate each line of stack trace;
      int iframe = 0;
      for (int i = thread.stack.size() - 1; i >= 0; i--)
      {
        v5dbg_stack_frame_t frame = thread.stack[i];

        rVal.paramBuffer = V5Dbg_FormatPrint("%i:[%s]:[%s]:%i", iframe, frame.funcName.c_str(), frame.stackBegin.filePath.c_str(), frame.stackBegin.lineNumber);

        V5Dbg_WriteToOut(V5Dbg_SerializeMessage(rVal));

        iframe++;
      }

      // End of stack

      rVal.paramBuffer = "ENDSTACK";
      rVal.type = DEBUGGER_MESSAGE_VSTACK_END;

      V5Dbg_WriteToOut(V5Dbg_SerializeMessage(rVal));

      break;
    }

    x++;
  }
}
