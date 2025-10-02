#pragma once
#include <string>

/*
 * Example packet:
 * [PACKET_TYPE:VERSION,CLOSE,OPEN,etc]:[DEST:INCOMING(0),OUTGOING(1)]:[PACKET_DATA]
*/

/// Server protocol version
#define V5DBG_SERVER_VERSION 1

#define V5DBG_MSG_SEPERATOR ':'

enum v5dbg_message_type_e
{
  /// @brief  Connection opened
  DEBUGGER_MESSAGE_OPEN = 0,

  /// @brief  Request program suspension, assume it has occured when a DEBUG_MESSAGE_RSUSPEND is parsed
  DEBUGGER_MESSAGE_SUSPEND = 1,

  /// @brief  Connection closed
  DEBUGGER_MESSAGE_CLOSE = 2,

  /// @brief  Allocate a string, can be processed by the debugger however it likes
  DEBUGGER_MESSAGE_ALLOCATE_STRING = 3,

  /// @brief  Max debugger message ID
  DEBUGGER_MESSAGE_MAX = 4
};

struct v5dbg_message_t
{
  /// @brief  Message type
  v5dbg_message_type_e type;

  /// @brief  Message parameter buffer
  std::string paramBuffer;
};

/// @brief  Serialize a message from a v5dbg_message_t object
std::string V5Dbg_SerializeMessage(v5dbg_message_t message);

/// @brief  Deserialize a message from a std::string
/// @param msg Input message data string
/// @return Deserialized message
v5dbg_message_t V5Dbg_DeserializeMessage(const std::string &msg);