import serial
import UartLinux
import struct
import binascii
from ctypes import *

#defines
BAUDRATE = 57600
CLOCK_RATE = 24000000

def float_to_hex(f):
    str1 = hex(struct.unpack('I', struct.pack('<f', f))[0])
    str1 = reversehex32b(str1[2:])
    return str1


def hex_to_float(s):
    i = int(s, 16)                   # convert from hex to a Python int
    cp = pointer(c_int(i))           # make this into a c integer
    fp = cast(cp, POINTER(c_float))  # cast the int pointer to a float pointer
    return str(fp.contents.value)         # dereference the pointer, get the float


def hexU32str(strBuf,ref):
    str3 = hexStrB(strBuf,ref+3) +hexStrB(strBuf,ref+2) +hexStrB(strBuf,ref+1) +hexStrB(strBuf,ref)
    str1 = (int("0x" +str3,0))
    return str1
        

def reversehex32b(st):
    rev = ""
    for i in range(0 ,len(st)/2):
        rev = st[2*i:(2*i+2)] + rev
    return rev

def hexStrB(str1,byteNum):
    return str1[2*byteNum:2*byteNum+2]

def hexStrF(str1,byteNum):
    str2 = hexStrB(str1,byteNum + 3) + hexStrB(str1,byteNum + 2) +hexStrB(str1,byteNum+1)  + hexStrB(str1,byteNum)
    return hex_to_float(str2)

def hex2Str(str1,NumS,len1):
    tempS = str1[2*NumS:2*NumS+2+len1*2]
    return tempS.decode('hex')


def GetBoardVersion():
   ports = UartLinux.ListPortsDialog()
   if len(ports.getPortsList()) == 0:
       print "NO AVAILABLE SERIAL PORTS"
       return None
   portname = UartLinux.full_port_name(ports.getPortsList()[-1])
   #print "Using " + portname
   ser = serial.Serial(portname, BAUDRATE)



#internal stuff
scale = 16 ## equals to hexadecimal
num_of_bits = 8


def hextobinary(hexstr):
    binval = hexstr.decode("hex")
    #print(' '.join(format(ord(ch), 'b') for ch in binval))
    #binstr = bin(int(hexstr, scale))[2:].zfill(num_of_bits)  
    #print("Sending: "+binstr+"\n")
    return binval


def hextomodule(hexstr):
    sendstr = hextobinary(hexstr)
    serialport.write(sendstr)
    str1 =""
    for x in range(0, len(hexstr)/2):
        str1 = str1 + hexstr[2*x:2*x+2] + " "
    print str1
#--------------------------------------------------------------
#--------------------------------------------------------------

def ResetCommand():
    hexStr = "7E0000FF04"
    userinp = raw_input("What type of Reset \n(1) Reset Now\n(2) Reset Eeprom / Reset Now\nOption[1 or 2]")
    if userinp == '1':
        hexStr += "55"
    elif userinp == '2':
        hexStr += "75"
    
    hexStr += "000000"
    hexStr += "7D"
    hextomodule(hexStr)

def GetInfo():
    serialport.flushInput()
    hextomodule("7E0100007D")
    buf = serialport.read(size=100)
    strBuf = binascii.hexlify(buf)
    if( len(strBuf) >= 32):
        print("Data returned: 0x" + strBuf)
        print("------------------------------------------------------");
        print("Board Type: 0x" +  hexStrB(strBuf,5))
        print("hardware version: " + str(int("0x" +hexStrB(strBuf,9),0))+ "." + str(int("0x" +hexStrB(strBuf,10),0))  + "." + str(int("0x" +hexStrB(strBuf,12) +hexStrB(strBuf,11),0)))
        print("firmware version: " + str(int("0x" +hexStrB(strBuf,13),0))+ "." + str(int("0x" +hexStrB(strBuf,14),0))  + "." + str(int("0x" +hexStrB(strBuf,16) +hexStrB(strBuf,15),0)))
        print("------------------------------------------------------");
    else:
        print("ERROR: No Data Returned")
                                                                                                               
def GetAssert():
    serialport.flushInput()
    hextomodule("7E0100107D")
    buf = serialport.read(size=100)
    strBuf = binascii.hexlify(buf)
    if( len(strBuf) >= 32):
        print("Data returned: " + strBuf)
        print("------------------------------------------------------");
        print("Assert Line: " + str(int("0x"+hexStrB(strBuf,5),0)))
        print("Assert file: " + hex2Str(strBuf,9,48))
        print("Time Since Reset: " + str(hexU32str(strBuf,57)) + "s")
        print("Total time: " + str(hexU32str(strBuf,61)) + "s")
        print("Total Resets: " + str(hexU32str(strBuf,65)))
        print("------------------------------------------------------");
    else:
        print("ERROR: No Data Returned")

def SetLEDPPF():
    hexStr = "7E00020014"
    hexStr += "FFFF0000" # group mask
    
    for x in range(0, 3):
        var = (float(raw_input("CH" +str(x)+ " PPF(uMol/s): ")))
        if(var != 0):
            hexStr += (float_to_hex(var))
        else:
            hexStr += "00000000"
    hexStr += "00000000"
    hexStr += "7D"
    hextomodule(hexStr)

def SetLEDPPF_GroupMask():
    hexStr = "7E00020014"
    str1 =(raw_input("Destination Group Mask in hex i.e. 0xffff: 0x"))# group mask
    hexStr += str1[2:4] + str1[0:2]
    hexStr += "0000" 
    for x in range(0, 3):
        var = (float(raw_input("CH" +str(x)+ " PPF(uMol/s): ")))
        if(var != 0):
            hexStr += (float_to_hex(var))
        else:
            hexStr += "00000000"
    hexStr += "00000000"
    hexStr += "7D"
    hextomodule(hexStr)


def SetLEDCurrent():
    hexStr = "7E00021010"

    for x in range(0, 3):
        var = (float(raw_input("Ch" +str(x)+ " Current(mA): ")))
        if(var != 0):
            hexStr += (float_to_hex(var))
        else:
            hexStr += "00000000"
    hexStr += "00000000"
    hexStr += "7D"
    hextomodule(hexStr)
    
def GetLEDError():
    serialport.flushInput()
    hextomodule("7E0102017D")
    buf = serialport.read(size=52)
    strBuf = binascii.hexlify(buf)
    if( len(strBuf) >= 16):
        print("Data returned: 0x" + strBuf)
        print("------------------------------------------------------");
        print("Board Temp (C):" + (hexStrF(strBuf,5)))
        print("Board Voltage (V):" + hexStrF(strBuf,5+4))
        print("Board Current (A):" + hexStrF(strBuf,5+8))
        offset = 5+12
        for x in range(0, 3):
            print("\nChannel " + str(x) + ":");
            print("------------------------------------------------------");
            print("General Error:" + hexStrB(strBuf,offset +8*x))
            print("Led Error Mask:" + hexStrB(strBuf,offset+3+8*x)+ hexStrB(strBuf,offset+2+8*x))
            print("Range Error:" + hexStrB(strBuf,offset+7+8*x) + hexStrB(strBuf,offset+6+8*x))
        print("------------------------------------------------------");
    else:
        print("ERROR: No Data Returned")


def GetLEDInfo():
    serialport.flushInput()
    hextomodule("7E0102027D")
    buf = serialport.read(size=220)
    strBuf = binascii.hexlify(buf)
    if( len(strBuf) >= 100):
        print("Data returned: 0x" + strBuf)
        print("------------------------------------------------------");
        print("Group Mask:" + hexStrB(strBuf,6)+ hexStrB(strBuf,5) )
        offset = 5+4
        for x in range(0, 3):
            print("\nChannel " + str(x) + ":");
            print("------------------------------------------------------");
            print("Number of LED's:" + str(int("0x"+hexStrB(strBuf,offset),0 ) ) )
            print("Type of LED's: 0x" + hexStrB(strBuf,offset +1) )
            
            print("PPF (umol/s):" + hexStrF(strBuf,offset+4))
            print("PPF2C (umol/s/mA):" + hexStrF(strBuf,offset+8))
            print("Set Current (mA):" + hexStrF(strBuf,offset+12))
            print("Minimum Current (mA):" + hexStrF(strBuf,offset+16))
            print("Full Scale Current (mA):" + hexStrF(strBuf,offset+20))
            offset = offset + 24
        print("------------------------------------------------------");
    else:
        print("ERROR: No Data Returned")


def SetLEDGroupMask():
    hexStr = "7E00024004"

    str1 =(raw_input("Group Mask in hex i.e. 0xffff: 0x"))
    hexStr += str1[2:4] + str1[0:2]
    hexStr += "0000"
    hexStr += "7D"
    hextomodule(hexStr)

def LEDManualControl():
    hexStr = "7E0002"
    ch = (raw_input("Which Channel(0-3): "))
    hexStr += "9"+ch[0]
    hexStr += "08"
    if( (ch[0] == '2') | (ch[0] == '3')):
        str1 =(raw_input("LED On Mask in hex i.e. for all on 0xffff for led 1 on 0x0001 \n: 0x"))
        hexStr += str1[2:4] + str1[0:2]
    else:
        hexStr += "ffff"
    f1 =float(raw_input("PWM Setting (0-100%)" ))
    str1 = "{:04X}".format(int((f1*0xffff)/100))
    hexStr += str1[2:4] + str1[0:2]
    var = (float(raw_input("Ch" +str(ch)+ " Current(mA): ")))
    if(var != 0):
        hexStr += (float_to_hex(var))
    else:
        hexStr += "00000000"
    hexStr += "7D"
    hextomodule(hexStr)

def SetLEDCalibrate():
    hexStr = "7E0002"
    ch = (raw_input("Which Channel(0-3): "))
    hexStr += "A"+ch[0]
    hexStr += "48"
    hexStr += "0000" # res
    f1 =float(raw_input("PWM Setting (0-100%)" ))
    str1 = "{:04X}".format(int((f1*0xffff)/100))
    hexStr += str1[2:4] + str1[0:2]
    var = (float(raw_input("Ch" +str(ch)+ " Current(mA): ")))
    if(var != 0):
        hexStr += (float_to_hex(var))
    else:
        hexStr += "00000000"

    for x in range(0, 16):
        var = (float(raw_input("Ch" +str(ch)+ " Led Num:" + str(x) + " Measured PPF(mA): ")))
        if(var != 0):
            hexStr += (float_to_hex(var))
        else:
            hexStr += "00000000"
    hexStr += "7D"
    hextomodule(hexStr)

def TestCommand():
    str1= "7E00029208ffffFF7F000048427D"
    hextomodule(str1)

#--------------------------------------------------------------
#--------------------------------------------------------------    

ports = "/dev/ttyUSB0"
if len(ports) == 0:
   print "NO AVAILABLE SERIAL PORTS"
   exit

#portname = __name__
#portname = UartLinux.s()
#print "Using " + portname
serialport = serial.Serial(ports, BAUDRATE,timeout = 0.6)

exitflag = 0
while exitflag == 0:
    print("General Commands")
    print("1) Send Reset Command")
    print("2) Get Board Info")
    print("3) Get Assert Info")
    print("LED Commands")
    print("4) Send SetLEDPPF ")
    print("5) Send SetLEDCurrent")
    print("6) Get GetLEDError")
    print("7) Get GetLEDInfo ")
    print("8) Send SetLEDGroupMask")
    print("9) Send LEDManualControl")
    print("10) Send SetLEDCalibrate")
    print("11) Send SetLEDPPF with group Mask")
    print("X) to exit")
    userinp = raw_input("\nEnter Command:")
    if userinp in ['X','x']:
        exitflag = 1
    elif userinp == '0':
        TestCommand()
    elif userinp == '1':
        ResetCommand()
    elif userinp == '2':
        GetInfo()
    elif userinp == '3':
        GetAssert()
        
    elif userinp == '4':
        SetLEDPPF()
    elif userinp == '5':
        SetLEDCurrent()
    elif userinp == '6':
        GetLEDError()
    elif userinp == '7':
        GetLEDInfo()
    elif userinp == '8':
        SetLEDGroupMask()
    elif userinp == '9':
        LEDManualControl()
    elif userinp == '10':
        SetLEDCalibrate()
    elif userinp == '11':
        SetLEDPPF_GroupMask()
		
serialport.close()
