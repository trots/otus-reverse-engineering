#include "../license_core/license_core.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    ParseLicenseKey(reinterpret_cast<const char*>(data), size);
    return 0;
}
