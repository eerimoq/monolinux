import os
import sys
from hashlib import sha256


class BlockReader:
    """Reads one block at a time from the end of the file towards the
    beginning.

    """

    def __init__(self, fin, offset, size):
        self._fin = fin
        self._offset = offset
        self._size = size
        self._left = size

    def __len__(self):
        return self._size // 4096

    def __iter__(self):
        while self._left > 0:
            self._left -= 4096
            self._fin.seek(self._offset + self._left, os.SEEK_SET)

            yield self._fin.read(4096)


class HashTree:
    """Writes hashes (or padding) from the end of the file towards the
    beginning.

    """

    def __init__(self, ftree, size):
        self.ftree = ftree
        self._size = size
        self._left = size

    def write(self, data):
        self._left -= len(data)
        self.ftree.seek(self._left, os.SEEK_SET)

        return self.ftree.write(data)

    def tell(self):
        return self._left

    def read_first_block(self):
        self.ftree.seek(0)

        return self.ftree.read(4096)


def padding_size(size):
    rest = (size % 4096)

    if rest > 0:
        rest = (4096 - rest)

    return rest


def round_up(number):
    return number + padding_size(number)


def calc_hash_tree_size(size):
    """Calculate the hash tree size, not including the top/root hash.

    """

    number_of_hashes = (size // 4096)
    tree_size = 0

    while number_of_hashes > 1:
        size = round_up(32 * number_of_hashes)
        tree_size += size
        number_of_hashes = (size // 4096)

    return tree_size


def create_hash_tree(block_reader, hash_tree, salt):
    """Create a hash tree, not including the top/root hash.

    """

    offset_end = hash_tree.tell()
    hash_tree.write(b'\x00' * padding_size(32 * len(block_reader)))

    for block in block_reader:
        hash_tree.write(sha256(salt + block).digest())

    offset = hash_tree.tell()

    if offset > 0:
        create_hash_tree(BlockReader(hash_tree.ftree,
                                     offset,
                                     offset_end - offset),
                         hash_tree,
                         salt)


def main():
    image_path = sys.argv[1]
    tree_path = sys.argv[2]
    salt = bytes.fromhex(sys.argv[3])

    image_size = os.stat(image_path).st_size

    if padding_size(image_size) != 0:
        sys.exit('Input size must be a multiple of 4096 bytes.')

    tree_size = calc_hash_tree_size(image_size)

    with open(image_path, 'rb') as fin:
        with open(tree_path, 'w+b') as ftree:
            ftree.truncate(tree_size)
            hash_tree = HashTree(ftree, tree_size)
            create_hash_tree(BlockReader(fin, 0, image_size),
                             hash_tree,
                             salt)
            root_hash = sha256(salt + hash_tree.read_first_block()).digest()

    print('Salt:     ', salt.hex())
    print('Root hash:', root_hash.hex())


if __name__ == '__main__':
    main()
