
from block import Block

blockchain = []

genesis_block = Block("first block", ["transaction + 1", "transaction + 6", "transaction + 3"])

second_block = Block(genesis_block.block_hash, ["tr2", "tr3", "tr4"])

print(second_block.block_hash)