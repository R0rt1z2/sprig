#!/usr/bin/env python3
import sys
from liblk.image import LkImage

def main():
    if len(sys.argv) != 4:
        print("Usage: %s <lk_image> <payload.bin> <output>" % sys.argv[0])
        return 1
    
    lk_path = sys.argv[1]
    pl_path = sys.argv[2]
    out_path = sys.argv[3]
    
    print("\nLoading: %s" % lk_path)
    img = LkImage(lk_path)
    
    if 'bl2_ext' not in img.partitions:
        print("Error: No bl2_ext partition")
        return 1
    
    with open(pl_path, 'rb') as f:
        pl = f.read()
    
    part = img.partitions['bl2_ext']
    
    print("Original bl2_ext size: 0x%x bytes" % len(part.data))
    print("New payload size: %d bytes" % len(pl))
    
    old_header = part.header
    
    part.data = pl
    
    print("New bl2_ext size: 0x%x bytes" % part.header.data_size)
    print("Load address: 0x%x" % old_header.memory_address)
    
    img._rebuild_contents()
    
    print("Saving: %s" % out_path)
    img.save(out_path)
    print("Done!\n")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())