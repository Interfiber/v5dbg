#include "v5dbg/memory.h"
#include "v5dbg/pretty.h"

void*
V5Dbg_AllocateInt(const std::string& value)
{
  int* buffer = new int;

  try
  {
    *buffer = std::stoi(value);
  }
  catch (std::exception& e)
  {
    delete buffer;
    return nullptr;
  }

  return buffer;
}

$pretty_printer_allocator(V5Dbg_AllocateInt, MEMORY_TYPE_BASE_INT);

void*
V5Dbg_AllocateFloat(const std::string& value)
{
  float* buffer = new float;

  try
  {
    *buffer = std::stof(value);
  }
  catch (std::exception& e)
  {
    delete buffer;
    return nullptr;
  }

  return buffer;
}

$pretty_printer_allocator(V5Dbg_AllocateFloat, MEMORY_TYPE_FLOAT);

void*
V5Dbg_AllocateDouble(const std::string &value)
{
    double* buffer = new double;

    try {
        *buffer = std::stod(value);
    }
    catch (std::exception &e)
    {
        delete buffer;
        return nullptr;
    }

    return buffer;
}

$pretty_printer_allocator(V5Dbg_AllocateDouble, MEMORY_TYPE_DOUBLE);