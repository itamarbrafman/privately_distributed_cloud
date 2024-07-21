#pragma once

#include <stddef.h>
#include <stdint.h>
#include <memory>

struct AtlasHeader
{
    size_t DataLength () const { return m_dataLen; }
    
    uint32_t    m_requestUid;   // not to be used by IoT; must be first field
    uint32_t    m_fragmentIdx;  // not to be used by IoT
    uint32_t    m_alarmUid;     // not to be used by IoT
    uint32_t    m_iotOffset;
    uint32_t    m_type;
    uint32_t    m_dataLen;      // how many bytes to write OR to read
};
