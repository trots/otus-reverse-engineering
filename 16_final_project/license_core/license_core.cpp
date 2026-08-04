#include "license_core.h"
#include <cstring>

namespace
{
int CharValue(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'Z')
        return c - 'A' + 10;
    return -1;
}

bool DecodeGroup(const char* group, size_t len, uint32_t* out)
{
    char buf[5];
    memcpy(buf, group, len);
    buf[len] = '\0';

    uint32_t value = 0;
    for (size_t i = 0; i < len; ++i)
    {
        int v = CharValue(buf[i]);
        if (v < 0) return false;
        value = value * 36 + v;
    }
    *out = value;
    return true;
}

uint8_t Checksum(const char* data, size_t len)
{
    uint8_t sum = 0;
    for (size_t i = 0; i < len; ++i)
        sum ^= static_cast<uint8_t>(data[i]);
    return sum;
}
}

LicenseInfo ParseLicenseKey(const char* key, size_t len)
{
    LicenseInfo info{false, 0, 0};

    if (len < 19)
        return info;
    if (key[4] != '-' || key[9] != '-' || key[14] != '-')
        return info;

    const char* dash4 = static_cast<const char*>(memchr(key + 15, '-', len - 15));
    size_t group4Len = dash4 ? static_cast<size_t>(dash4 - (key + 15)) : (len - 15);

    uint32_t productId = 0;
    uint32_t expHigh = 0, expLow = 0;
    uint32_t checksumValue = 0;

    if (!DecodeGroup(key, 4, &productId))
        return info;
    if (!DecodeGroup(key + 5, 4, &expHigh))
        return info;
    if (!DecodeGroup(key + 10, 4, &expLow))
        return info;
    if (!DecodeGroup(key + 15, group4Len, &checksumValue))
        return info;

    uint8_t expected = Checksum(key, 14);
    if ((checksumValue & 0xFF) != expected)
        return info;

    info.valid = true;
    info.productId = static_cast<uint16_t>(productId);
    info.expirationDate = (expHigh << 16) | (expLow & 0xFFFF);
    return info;
}
