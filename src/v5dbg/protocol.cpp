#include "v5dbg/protocol.h"
#include <sstream>
#include <vector>

std::string V5Dbg_SerializeMessage(v5dbg_message_t message)
{
  std::stringstream ss;

  ss << "] "; // All debugger messages start with %
  ss << V5DBG_SERVER_VERSION;
  ss << ":";
  ss << message.type;
  ss << ":";
  ss << message.paramBuffer;

  return ss.str();
}

v5dbg_message_t V5Dbg_DeserializeMessage(const std::string &msg)
{
  const int tChars = msg.size();
  int collectedArguments = 0;

  std::string allocatedMessage;

  std::vector<std::string> parameters;
  parameters.reserve(3); // Protcol version, Message type, Message buffer

  for (int i = 0; i < tChars; i++)
  {
    char c = msg[i];

    if (c == ':' && collectedArguments < 2)
    {
      ++collectedArguments;
      parameters.push_back(allocatedMessage);

      allocatedMessage.clear();
    }
    else
    {
    allocatedMessage += c;
    }
  }

  if (collectedArguments < 2)
  {
    printf("Invalid message! Got: %i parameters\n", collectedArguments);
    return {};
  }

  parameters.push_back(allocatedMessage);

  v5dbg_message_t message{};

  printf("%s %s %s\n", parameters[0].c_str(), parameters[1].c_str(), parameters[2].c_str());

  int msgType = std::stoi(parameters[1]);

  if (msgType > DEBUGGER_MESSAGE_MAX-1)
  {
    printf("Invalid message! Unknown message type!");
  } else if (std::stoi(parameters[0]) != V5DBG_SERVER_VERSION)
  {
    printf("Invalid message! Protocol mismatch!");
  }

  message.type = (v5dbg_message_type_e) msgType;
  message.paramBuffer = parameters[2];

  return message;
}