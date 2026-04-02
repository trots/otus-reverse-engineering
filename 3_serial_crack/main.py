import zlib
import string


def calculate_login_hash(login):
    # CRC32
    crc = zlib.crc32(login.encode('utf-8')) & 0xffffffff
    return crc & 0xFF


def find_password(target_hash):
    # sum((byte ^ 0x99)) & 0xFF == target_hash

    # Finding first symbol
    for c1 in string.ascii_letters + string.digits:
        val = ord(c1) ^ 0x99
        if (val & 0xFF) == target_hash:
            return c1

    # Finding second symbol
    for c1 in string.ascii_letters + string.digits:
        for c2 in string.ascii_letters + string.digits:
            val = (ord(c1) ^ 0x99) + (ord(c2) ^ 0x99)
            if (val & 0xFF) == target_hash:
                return c1 + c2

    return None


if __name__ == "__main__":
    login = input("Enter login: ")
    target = calculate_login_hash(login)
    password = find_password(target)
    if password:
        print(f"Password: {password}")
    else:
        print("Unable to find the password.")
