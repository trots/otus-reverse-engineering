#pragma once
#include <cstdint>
#include <cstddef>

struct LicenseInfo
{
    bool valid;
    uint16_t productId;
    uint32_t expirationDate;
};

LicenseInfo ParseLicenseKey(const char* key, size_t len);
