import serial, glob

def scan():
   # scan for available ports. return a list of device names.
   return glob.glob('/dev/ttyS*') + glob.glob('/dev/ttyUSB*')

print "Found ports:"
for name in scan(): print name
print "------------------Port list over-----------------"