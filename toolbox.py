from optparse import OptionParser, OptionGroup
from io import open
from sys import argv, exit
from os import stat

_64_ko = 2**17
one_mo = 2**20
two_mo = 2**21
three_mo = one_mo + two_mo
four_mo = 2**22
six_mo = two_mo + four_mo

def header_size(fs):
	return fs % _64_ko

def swap(i):
	#arrange a human readable address list
	a = list("{0:020b}".format(i)[::-1])
	#classic swapbin
	a[16], a[18] = a[18], a[16]
	a[17], a[19] = a[19], a[17]
	return int("".join(a[::-1]),2)

def swapRom(bytes):
	dest = [chr(0)] * one_mo
	pos = 0
	try:
		for byte in bytes:
			dest[swap(pos)] = byte
			pos +=1
	except:
		print "Error during swapbin!"
		exit(1)
	return dest

def expandRom(bytes,target):
	dest = [chr(0)] * target
	pos = 0
	try:
		for byte in bytes:
			dest[pos] = byte
			pos +=1
	except:
		print "Error during expand!"
		exit(1)
	return dest

def readRom(filename):
	bytes = []
	try: 
		with open(filename,'rb') as f:
			while True:
				byte=f.read(1)
				if not byte:
					break
				bytes.append(byte)
	except:
		print "Error during read!"
		exit(1)
	return bytes

def writeRom(filename, bytes, action = lambda x: x):
	try:
		with open(filename,'wb') as f:
			for byte in action(bytes):
				f.write(byte)
	except:
		print "Error during write!"
		exit(1)

usage = "usage: %prog [options] <romfile>"
version = "%prog - v1.0 - 2013-10-28"
parser = OptionParser(usage=usage, version=version)

parser.add_option("-v","--verbose", action="store_true", dest="verbose", default=False, help="Be verbose (default off)")

group = OptionGroup(parser, "Target EPROM Size")
group.add_option("-1","--27c801", action="store_const", const=one_mo, dest="target_size", default=one_mo, help="1Mo (default)")
group.add_option("-2","--27c160", action="store_const", const=two_mo, dest="target_size", help="2Mo")
group.add_option("-4","--27c322", action="store_const", const=four_mo, dest="target_size", help="4Mo")
parser.add_option_group(group)

group = OptionGroup(parser, "Data operation")
group.add_option("-s","--swapbin", action="store_true", dest="swapbin", default=False, help="27C801 swapbin")
parser.add_option_group(group)


# No args, print help
if len(argv) == 1:
	parser.print_help()
	exit(1)

# Parse command line
(options, args) = parser.parse_args(argv[1:])

# Start - Input error handling
# No ROM file
if len(args) == 0:
	parser.error("<romfile> is mandatory")

# Swapbin and target not compatible
if options.target_size <> one_mo and options.swapbin:
	parser.error("Swapbin is only possible if target size is 1Mo")

# Get  file size
file_size = stat(args[0]).st_size

if file_size < _64_ko:
	print "File is smaller than 64ko (not a SNES Rom?)"
	exit(2)

# Get header size and real data size
hs = header_size(file_size)
if hs <> 0:
	print "Header found of size: %i" % hs
ds = file_size - hs
print "Actual data size: %i" % ds

if ds > (six_mo):
	print "Cannot work with ROM larger than 6Mo"
	exit(2)

if ds > (four_mo) and options.target_size <> four_mo:
	print "For file bigger than 4Mo, choose 4Mo as target size"
	exit(2)
# End - Input error handling

# Get all byte from ROM File
file_byte = readRom(args[0])

# Switch action by size of actual data

# Expand to target and write. Swapbin if asked.
if ds <= one_mo or (ds <= two_mo and options.target_size >= two_mo) or (ds <= four_mo and options.target_size == four_mo):
	file_byte = expandRom(file_byte[hs:],options.target_size)
	if options.swapbin:
		writeRom(args[0]+".bin",file_byte, swapRom)
		print "Swapbin done."
	else:
		writeRom(args[0]+".bin",file_byte)
	print "File %s.bin of size %i has been written.\nDone!" % (args[0], options.target_size)
	exit(0)

# Large Rom, split at 4Mo
if options.target_size == four_mo and ds > four_mo:
	writeRom(args[0]+".1.bin",file_byte[hs:four_mo])
	writeRom(args[0]+".2.bin",file_byte[hs+four_mo:])
	print "Files %s.1.bin (%i) and .2.bin (%i) have been written.\nDone!" % (args[0], options.target_size, ds - options.target_size)
	exit(0)

if options.target_size == one_mo and ds <= two_mo:
	file_byte = expandRom(file_byte[hs:],two_mo)
	if options.swapbin:
		writeRom(args[0]+".1.bin",file_byte[0:one_mo],swapRom)
		writeRom(args[0]+".2.bin",file_byte[one_mo:],swapRom)
		print "Swapbin done."
	else:
		writeRom(args[0]+".1.bin",file_byte[0:one_mo])
		writeRom(args[0]+".2.bin",file_byte[one_mo:])
	print "Files %s.1.bin and .2.bin of size %i have been written.\nDone!" % (args[0], options.target_size)
	exit(0)

if options.target_size == one_mo and ds <= four_mo:
	file_byte = expandRom(file_byte[hs:],four_mo)
	if options.swapbin:
		writeRom(args[0]+".1.bin",file_byte[0:one_mo],swapRom)
		writeRom(args[0]+".2.bin",file_byte[one_mo:two_mo],swapRom)
		writeRom(args[0]+".3.bin",file_byte[two_mo:three_mo],swapRom)
		writeRom(args[0]+".4.bin",file_byte[three_mo:],swapRom)
		print "Swapbin done."
	else:
		writeRom(args[0]+".1.bin",file_byte[0:one_mo])
		writeRom(args[0]+".2.bin",file_byte[one_mo:two_mo])
		writeRom(args[0]+".3.bin",file_byte[two_mo:three_mo])
		writeRom(args[0]+".4.bin",file_byte[three_mo:])
	print "Files %s.1.bin, .2.bin, .3.bin and .4.bin of size %i have been written.\nDone!" % (args[0], options.target_size)
	exit(0)

if options.target_size == two_mo and ds <= four_mo:
	file_byte = expandRom(file_byte[hs:],four_mo)
	writeRom(args[0]+".1.bin",file_byte[0:two_mo])
	writeRom(args[0]+".2.bin",file_byte[two_mo:])
	print "Files %s.1.bin and .2.bin of size %i have been written.\nDone!" % (args[0], options.target_size)
	exit(0)

print "No reason for action found (bug?)"
exit(1)
