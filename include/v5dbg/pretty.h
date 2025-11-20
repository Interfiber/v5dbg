#pragma once
#include <typeinfo>
#include <unordered_map>
#include "v5dbg/memory.h"
#include "v5dbg/util.h"


/// @brief  Result from V5Dbg_PrettyPrint(...)
struct v5dbg_pretty_printed_t
{
  /// @brief  Human readable type name, ex: std::vector<int>
  std::string typeName;

  /// @brief  Name of the pretty-printed variable
  std::string varName;

  /// @brief  Pretty printed output for this variable
  std::string printBuffer;
};

/**
 * @brief  Quick constructor for v5dbg_pretty_printed_t
 * @note typeName is automatically filled in by the pretty printer function using variable information
 */
#define $pretty_print_result(name, buf)                                                                                \
  v5dbg_pretty_printed_t { .typeName = "void*", .varName = name, .printBuffer = buf }

/// @brief  Function pointer to pretty printer impl
typedef v5dbg_pretty_printed_t (*V5Dbg_PrettyPrintMemoryObj)(V5DbgMemoryObject* pMemory);

/// @brief  Return a heap-allocated buffer which contains the properly casted memory for the incoming data type
typedef void* (*V5Dbg_PrettyPrinterAllocateBuffer)(const std::string& buffer);

struct v5dbg_pretty_printer_state_t
{
  /// @brief  Collection of pretty printers for each memory object type
  std::unordered_map<v5dbg_memory_type_e, V5Dbg_PrettyPrintMemoryObj> printers;

  /// @brief  Collection of pretty printer memory allocators
  std::unordered_map<v5dbg_memory_type_e, V5Dbg_PrettyPrinterAllocateBuffer> allocators;

  /// @brief  Automatic type detection database where the key is the hash_code provided by std::type_info
  std::unordered_map<std::size_t, v5dbg_memory_type_e> typeDb;
};

/// @brief  Return the global pretty printer state object
inline v5dbg_pretty_printer_state_t*
V5Dbg_GetPrettyPrinterState()
{
  static v5dbg_pretty_printer_state_t state{};

  return &state;
}


/**
 * @brief  Detect the memory object type from the compilers provided std::type_info, def
 * @return Correct type, if no type is found we default to a raw pointer
 */
v5dbg_memory_type_e V5Dbg_MemoryTypeFromType(const std::type_info& typeInfo);

/**
 * @brief  Find the correct pretty printer to use for the given memory type
 * @return Correct pretty printer or nullptr if none was found
 */
V5Dbg_PrettyPrintMemoryObj V5Dbg_PrettyPrinterFromType(v5dbg_memory_type_e memType);

/**
 * @brief  Register a new pretty printer function
 * @param memType Memory type which this pretty printer should act on
 * @param func Pretty printer function to be called
 */
void V5Dbg_RegisterPrettyPrinter(v5dbg_memory_type_e memType, V5Dbg_PrettyPrintMemoryObj func);

/**
 * @brief  Register a new pretty printer allocation function
 * @param memType Memory type which this pretty printer should act on
 * @param func Allocation function to be called
 */
void V5Dbg_RegisterPrettyPrinter(v5dbg_memory_type_e memType, V5Dbg_PrettyPrinterAllocateBuffer func);

/**
 * @brief Return the pretty printed string for pMem
 * @param pMem Memory object to pretty-print, if not pretty printed is found we default back to a raw pointer(void*)
 * @return Pretty printed result object
 */
v5dbg_pretty_printed_t V5Dbg_PrettyPrint(V5DbgMemoryObject* pMem);

/**
 * Internal class used to link pretty printer functions
 */
class V5DbgPrettyPrinterLinker
{
public:
  V5DbgPrettyPrinterLinker(v5dbg_memory_type_e memType, V5Dbg_PrettyPrintMemoryObj func = nullptr,
                           V5Dbg_PrettyPrinterAllocateBuffer alloc = nullptr)
  {

    if (func != nullptr)
    {
      V5Dbg_RegisterPrettyPrinter(memType, func);

      info_pre("Pretty printer registered");
    }
    else if (alloc != nullptr)
    {
      V5Dbg_RegisterPrettyPrinter(memType, alloc);

      info_pre("Pretty printer buffer allocator registered");
    }
  }

  V5DbgPrettyPrinterLinker(const std::type_info& t, v5dbg_memory_type_e memType)
  {
    V5Dbg_GetPrettyPrinterState()->typeDb.insert({ t.hash_code(), memType });

    info_pre("Pretty printer type db %zu -> %i", t.hash_code(), memType);
  }
};

#ifdef V5DBG_DISABLE
#include "v5dbg/pretty.stub.h"
#else

/// @brief  Register a pretty printer with a memory type
// stub:macro=$pretty_printer,func,type
#define $pretty_printer(func, type) static V5DbgPrettyPrinterLinker _v5dbg_pretty_printer_##type(type, &func);

/// @brief  Register a pretty printer allocator with a memory type
// stub:macro=$pretty_printer_allocator,func,type
#define $pretty_printer_allocator(func, type) static V5DbgPrettyPrinterLinker _v5dbg_pretty_printer_buf_##type(type, nullptr, &func);

/// @brief  Link a memory type and C++ typename
// stub:macro=$link_type_db,cpptype,etype
#define $link_type_db(cpptype, etype) static V5DbgPrettyPrinterLinker _v5dbg_pretty_printer_typedb_##etype(typeid(cpptype), etype);

#endif