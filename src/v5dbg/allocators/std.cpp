#include "v5dbg/memory.h"
#include "v5dbg/pretty.h"

void*
V5Dbg_AllocateString(const std::string &value)
{
    std::string *buffer = new std::string;

    try {
        buffer->assign(value.c_str());
    } catch (std::exception &e)
    {
        delete buffer; return nullptr;
    }

    return buffer;
}

$pretty_printer_allocator(V5Dbg_AllocateString, MEMORY_TYPE_STDSTR);