# Generates the file rom.c from a Super Mario Bros rom

import hashlib, sys

SMB1_SHA256 = 'f61548fdf1670cffefcc4f0b7bdcdd9eaba0c226e3b74f8666071496988248de'

if (len(sys.argv) != 3):
    print('\x1b[31merror: 2 arguments must be supplied!\x1b[m', file=sys.stderr)
    exit(1)

try:
    # check hash
    # https://docs.python.org/3/library/hashlib.html
    rom = open(sys.argv[1], 'rb')
    m = hashlib.sha256()
    m.update(rom.read())
    rom.close()
    if m.hexdigest() != SMB1_SHA256:
        print('\x1b[31merror: hash doesn\'t match!\x1b[m', file=sys.stderr)
        print('\x1b[31m  (is this the correct rom?)\x1b[m', file=sys.stderr)
        exit(1)

    rom = open(sys.argv[1], 'rb')

    rom_c = open(sys.argv[2], 'w')
    
    # write the header
    rom_c.write('#include "../src/rom.h"\n\n')

    # skip rom header
    rom.read(0x10)

    # write prg data
    rom_c.write('const uint8_t prg[] = {\n    ')
    rom_c.write(hex(rom.read(0x1)[0]))
    for n in rom.read(0x7fff):
        rom_c.write(', ' + hex(n))
    rom_c.write('\n};\n\n')
    
    # write chr data
    rom_c.write('const uint8_t chr[] = {\n    ')
    rom_c.write(hex(rom.read(0x1)[0]))
    for n in rom.read(0x1fff):
        rom_c.write(', ' + hex(n))
    
    rom_c.write('\n};\n')

    rom.close()
except FileNotFoundError:
    print('\x1b[31merror: could not find \'' + sys.argv[1] + '\'!\x1b[m', file=sys.stderr)
    exit(1)
