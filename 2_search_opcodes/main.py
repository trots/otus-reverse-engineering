from capstone import Cs, CS_ARCH_X86, CS_MODE_64
from collections import defaultdict
import json


if __name__ == "__main__":
    md = Cs(CS_ARCH_X86, CS_MODE_64)

    opcode_map = defaultdict(list)

    # 1 byte: 0x00 - 0xFF
    for b1 in range(0x100):
        opcode = bytes([b1])
        for inst in md.disasm(opcode, 0x1000):
            mnemonic = f"{inst.mnemonic} {inst.op_str}".strip()
            hex_str = f"0x{''.join(f'{b:02x}' for b in opcode)}"
            opcode_map[mnemonic].append(hex_str)
            break

    # 2 bytes: 0x0000 - 0xFFFF
    for b1 in range(0x100):
        for b2 in range(0x100):
            opcode = bytes([b1, b2])
            for inst in md.disasm(opcode, 0x1000):
                mnemonic = f"{inst.mnemonic} {inst.op_str}".strip()
                hex_str = f"0x{''.join(f'{b:02x}' for b in opcode)}"
                opcode_map[mnemonic].append(hex_str)
                break

    result = {}
    for mnemonic, opcodes in opcode_map.items():
        if len(opcodes) >= 2:
            result[mnemonic] = opcodes

    with open('opcode_variants.json', 'w', encoding='utf-8') as f:
        json.dump(result, f, indent=2)

    print(f"{len(result)} opcodes are saved into opcode_variants.json")
