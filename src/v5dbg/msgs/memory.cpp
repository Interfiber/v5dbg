#include "v5dbg/debinfo.h"
#include "v5dbg/msg.h"
#include "v5dbg/pretty.h"
#include "v5dbg/protocol.h"
#include "v5dbg/server.h"
#include "v5dbg/set.h"
#include "v5dbg/stack.h"
#include "v5dbg/state.h"
#include "v5dbg/subargs.h"
#include "v5dbg/util.h"

void
V5Dbg_LMemForHandle(v5dbg_server_state_t* pState, const v5dbg_message_t& msg)
{
  int stackFrame = -1;
  int threadID = -1;

  int x = 0;
  std::string collect;

  for (char c : msg.paramBuffer)
  {
    if (c == ',' && x == 0)
    {
      stackFrame = std::stoi(collect);
      collect.clear();
      x = 1;

      continue;
    }
    else if (c == ',')
    {
      continue;
    }

    collect += c;
  }

  if (x == 1)
  {
    threadID = std::stoi(collect);
  }

  v5dbg_thread_t* thread = V5Dbg_ThreadWithID(pState, threadID);
  if (thread == nullptr)
  {
    info("NoThread");
    return;
  }

  v5dbg_message_t result{};
  result.type = DEBUGGER_MESSAGE_RLMEM;

  int stackID = thread->stack.size() - 1;
  for (int i = 0; i < thread->stack.size(); i++)
  {
    if (stackID == stackFrame)
    {
      for (auto& obj : thread->stack[i].pMemory->local)
      {
        v5dbg_code_point_t cPoint = obj->getVariable().allocationPoint;

        v5dbg_pretty_printed_t prettyPrint = V5Dbg_PrettyPrint(obj.get());

        // type name, variable name, pretty printed string buffer
        result.paramBuffer = V5Dbg_FormatPrint("[%s]:%s", prettyPrint.typeName.c_str(), prettyPrint.varName.c_str());

        // debug info location

        result.paramBuffer += V5Dbg_FormatPrint(":[%s]:%i:[%s]", cPoint.filePath.c_str(), cPoint.lineNumber,
                                                prettyPrint.printBuffer.c_str());

        V5Dbg_WriteToOut(V5Dbg_SerializeMessage(result));
      }

      break;
    }

    stackID--;
  }

  result.paramBuffer = "ENDSTACKMEM";
  result.type = DEBUGGER_MESSAGE_LMEM_END;

  V5Dbg_WriteToOut(V5Dbg_SerializeMessage(result));
}

void
V5Dbg_SetMemoryHandle(v5dbg_server_state_t* pState, const v5dbg_message_t& msg)
{
  // Get parameters

  // 0: Variable name
  // 1: Buffer contents
  // 2: Stack frame
  // 3: Thread ID
  // 4: Set mode converted from v5dbg_variable_set_mode_e

  const std::vector<std::string> arguments = V5Dbg_ParseSubargs(msg.paramBuffer);

  if (arguments.size() != 5)
  {
    info("Invalid argument count of: %zu, expected 5!", arguments.size());
    return;
  }

  // Pull out info

  std::string name = arguments[0];
  std::string buf = arguments[1];

  int stackFrame = std::stoi(arguments[2]);
  int threadID = std::stoi(arguments[3]);
  v5dbg_variable_set_mode_e setMode = VARIABLE_SET_MODE_SINGLE;

  // enum can be converted from an int, v5dbg_variable_set_mode_e is included in the spec document
  setMode = (v5dbg_variable_set_mode_e)std::stoi(arguments[4]);

  // Set memory

  v5dbg_thread_t* thread = V5Dbg_ThreadWithID(pState, threadID);
  if (thread == nullptr)
  {
    info("NoThread");
    return;
  }

  int stackID = thread->stack.size() - 1;
  for (int i = 0; i < thread->stack.size(); i++)
  {
    if (stackID == stackFrame)
    {
      for (auto& obj : thread->stack[i].pMemory->local)
      {
        if (obj->getVariable().name == name)
        {
          if (v5dbg_variable_set_result_e result = V5Dbg_SetVariable(obj, buf, setMode); result != MEMORY_SET_COMPLETE)
          {
            v5dbg_message_t msg{};
            msg.type = DEBUGGER_MESSAGE_RMEMORY_SET;

            // Convert error enum to string
            // TODO: Just send the enum as an integer to save space
            if (result == MEMORY_SET_NO_ALLOCATOR || result == MEMORY_SET_ALLOCATOR_FAILURE)
            {
              msg.paramBuffer = V5DBG_MEMORY_ALLOCATOR_FAILURE;
            }
            else if (result == MEMORY_SET_CONVERSION_FAILURE)
            {
              msg.paramBuffer = V5DBG_MEMORY_CONVERSION_FAILURE;
            }
            else if (result == MEMORY_SET_RESET_COPY_BUFFER)
            {
              msg.paramBuffer = V5DBG_MEMORY_RESET_COPY_BUFFER;
            }

            V5Dbg_WriteToOut(V5Dbg_SerializeMessage(msg));

            return;
          }
          else
          {
            // Tell the debugger that we've set the variables memory

            v5dbg_message_t msg{};
            msg.type = DEBUGGER_MESSAGE_RMEMORY_SET;
            msg.paramBuffer = V5DBG_MEMORY_SET;

            V5Dbg_WriteToOut(V5Dbg_SerializeMessage(msg));
          }
        }
      }

      break;
    }

    stackID--;
  }

  v5dbg_message_t result{};
  result.type = DEBUGGER_MESSAGE_RMEMORY_SET;
  result.paramBuffer = V5DBG_MEMORY_NO_VARIABLE;

  V5Dbg_WriteToOut(V5Dbg_SerializeMessage(result));
}
