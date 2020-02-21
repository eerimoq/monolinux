from hashlib import sha256
from binascii import hexlify
from io import BytesIO


def bin_to_hex(data):
    return hexlify(data).decode('ascii')


def padding_size(size):
    rest = (size % 4096)

    if rest > 0:
        rest = (4096 - rest)

    return rest


def create_hash_tree(fin, salt):
    """Create a hash tree, not including the top/root hash.

    """

    fout = BytesIO()

    while True:
        block = fin.read(4096)

        if len(block) == 0:
            break
        elif len(block) != 4096:
            raise Exception('Input file must be a multiple of 4096 bytes.')

        fout.write(sha256(salt + block).digest())

    fout.write(b'\x00' * padding_size(fout.tell()))

    if fout.tell() == 4096:
        return fout.getvalue()
    else:
        fout.seek(0)

        return create_hash_tree(fout, salt) + fout.getvalue()


def main():
    salt = (
        b'\x78\x91\x23\x48\x71\x26\x39\x71\x62\x57\x89\x62\x34\x97\x58\x62'
        b'\x39\x87\x56\x98\x27\x34\x65\x98\x72\x34\x65\x87\x92\x36\x45\x98')

    with open('fs.img', 'rb') as fin:
        tree = create_hash_tree(fin, salt)

    with open('table.img', 'wb') as fout:
        fout.write(tree)

    root_hash = sha256(salt + tree[:4096]).digest()

    print('Salt:     ', bin_to_hex(salt))
    print('Root hash:', bin_to_hex(root_hash))


if __name__ == '__main__':
    main()
