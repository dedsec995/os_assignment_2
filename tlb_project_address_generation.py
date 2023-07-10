import random
import math
import numpy as np
import sys
if len(sys.argv) != 2:
	print("You must specify an output name!")
	quit()
NUM_VIRTUAL_ADDRESSES = 100
ADDRESS_SPACE_SIZE = 16
PAGE_SIZE = 8
TLB_SIZE = 12
bytes_needed = ADDRESS_SPACE_SIZE // 8 
with open(sys.argv[1], "wb") as fp:
	fp.write(bytes([PAGE_SIZE, TLB_SIZE]))
	FRAMES = 1 << (int) (ADDRESS_SPACE_SIZE - math.log2(PAGE_SIZE))
	PPN = [*(range(0,FRAMES))]
	np.random.shuffle(PPN)
	for num in PPN:
		fp.write(bytes([(num & 0x00FF), (num & 0xFF00) >> 8]))
	for i in range(NUM_VIRTUAL_ADDRESSES):
		for j in range(bytes_needed):
			fp.write(bytes([random.randint(0,255)]))