from enum import IntEnum


class SegmentRights(IntEnum):
    R = 0x0   # Read-Only
    RW = 0x2  # Read/Write
    X = 0x8   # Execute-Only
    RX = 0xA  # Execute/Read


def build_segment_descriptor(base: int, limit: int, rights: SegmentRights, dpl: int = 0, d_b: int = 1) -> str:
    if not (0 <= dpl <= 3):
        raise ValueError("DPL must be in range 0-3")

    # Access Byte (byte 5): Type | S=1 | DPL | P=1
    access_byte = (rights.value & 0xF) | (1 << 4) | ((dpl & 0x3) << 5) | (1 << 7)

    # Limit & Granularity
    g_flag = 0
    if limit > 0xFFFFF:
        g_flag = 1
        limit >>= 12  # 4 KB pages
    if limit > 0xFFFFF:
        raise ValueError("Limit exceeds max 20-bit capacity (4 GB).")

    limit_low = limit & 0xFFFF
    limit_high = (limit >> 16) & 0xF

    base_low = base & 0xFFFF
    base_mid = (base >> 16) & 0xFF
    base_high = (base >> 24) & 0xFF

    # Byte 6: G | D/B | L | AVL | Limit[19:16]
    flags_byte = (g_flag << 7) | (d_b << 6) | (0 << 5) | (0 << 4) | limit_high

    descriptor = [
        limit_low & 0xFF,          # Byte 0
        (limit_low >> 8) & 0xFF,   # Byte 1
        base_low & 0xFF,           # Byte 2
        (base_low >> 8) & 0xFF,    # Byte 3
        base_mid,                  # Byte 4
        access_byte,               # Byte 5
        flags_byte,                # Byte 6
        base_high                  # Byte 7
    ]

    return " ".join(f"{b:02X}" for b in descriptor)


if __name__ == "__main__":
    print("Example 1:", build_segment_descriptor(0x00000000, 0xFFFFFFFF, SegmentRights.RW, dpl=0, d_b=0))
    print("Example 2:", build_segment_descriptor(0x00000000, 0x0000FFFF, SegmentRights.RW, dpl=0, d_b=0))
    print("Example 3:", build_segment_descriptor(0x12345678, 0x0000FFFF, SegmentRights.RX, dpl=0, d_b=0))
