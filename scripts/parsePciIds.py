#!python3

'''
Parse a pci.id list from 
https://pci-ids.ucw.cz/
into a header file used in cfg.exe
located in src\pciids.h
'''

import argparse
from pathlib import Path
import sys
import re
import string


PROGRAM_DESCRIPTION = 'Parse pciid.txt list into array, into switch function.'
PROGRAM_NAME = 'parsePciIds'
PROGRAM_VERSION = '1.0.0'



def extract(src_file):
    '''
    iterating all lines of src_file
    adding ids to a nested array of vendor.device.subdevice ids
    '''
    print("extract ids...");

    vendors = {}
    lines = src_file.readlines()
    count = 0
    act_devices = None
    act_sub_devices = None
    
    # find vendors
    for line in lines:
        count += 1
        # print("line %d: %s" % (count, line));

# # some comment line
# 001c  PEAK-System Technik GmbH
#	 0001  PCAN-PCI CAN-Bus controller
#		 001c 0004  2 Channel CAN Bus SJC1000
#		 001c 0005  2 Channel CAN Bus SJC1000 (Optically Isolated)
        
        # skip too short lines
        if len(line) < 4:
            continue
            
        # skip comments
        if line[0] == 0x23:
            continue
        
        if isVendorLine(line):
            # vendorId = line[0:4]
            vendorId = int(line[0:4], 16) 
            vendorName = line[6:-1].decode("utf-8")
            
            # print("  id: 0x%x" % (vendorId));
            # print("  name: %s" % (vendorName));
                
            vendors[vendorId] = { "name":vendorName, "devices":{} }
            act_devices = vendors[vendorId]["devices"]
            
            
        if not act_devices is None and isDeviceLine(line):
            deviceId = int(line[1:5], 16) 
            deviceName = line[7:-1].decode("utf-8")
            
            # print("  id: 0x%x" % (deviceId));
            # print("  name: %s" % (deviceName));
            
            act_devices[deviceId] = { "name":deviceName, "sub_devices":{} }
            act_sub_devices = act_devices[deviceId]["sub_devices"]
            
            
        if not act_sub_devices is None and isSubDeviceLine(line):
            subDeviceId0 = int(line[2:6], 16) 
            subDeviceId1 = int(line[7:11], 16) 
            subDeviceName = line[12:-1].decode("utf-8")
            
            # print("  id0: 0x%x" % (subDeviceId0));
            # print("  id1: 0x%x" % (subDeviceId1));
            # print("  name: %s" % (subDeviceName));
            
            if not subDeviceId0 in act_sub_devices:
                act_sub_devices[subDeviceId0] = { }
                act_sub_devices[subDeviceId0][subDeviceId1] = subDeviceName
            else:
                act_sub_devices[subDeviceId0][subDeviceId1] = subDeviceName
    
    # print("#vendors: 0x%x" % len(vendors))
    # print("vendor[0x8086]: %s" % vendors[0x8086]["name"])
    
    # print("  devices[0xa379]: %s" % vendors[0x8086]["devices"][0xa379])
    # print("    sub_devices[0xa379]: %s" % vendors[0x8086]["devices"][0xa379]["sub_devices"])
    
    # print("  devices[0xb555]: %s" % vendors[0x8086]["devices"][0xb555])
    # print("    sub_devices[0xb555]: %s" % vendors[0x8086]["devices"][0xb555]["sub_devices"])
    # print("      sub_devices[0xb555][0x12c7]: %s" % vendors[0x8086]["devices"][0xb555]["sub_devices"][0x12c7])
    # print("      sub_devices[0xb555][0x12c7][0x5005]: %s" % vendors[0x8086]["devices"][0xb555]["sub_devices"][0x12c7][0x5005])
    # print("      sub_devices[0xb555][0x12c7][0x5006]: %s" % vendors[0x8086]["devices"][0xb555]["sub_devices"][0x12c7][0x5006])
    
    print("  [x] done")
    
    return vendors
    
def isVendorLine(line):
    """
    vendor id line format:
    <vid>  <name>
    """
    
    # vendor ids start a line, not tab
    if line[0] == 0x9:
        return False
        
    # 4 digit vendor id with space following
    if line[4] != 0x20:
        return False
    
    # print("line: %s" % line)
    # print("  0:4: %s" % str(line[0:4]))
    
    if not isValidX16(line[0:4]):
        print("[e] Not a valid vendor id: %s" % line)
        return False
    # print("  valid")
    return True
    
def isDeviceLine(line):
    """
    device id line format:
    \t<did>  <name>
    """
    
    if line[0] != 0x9:
        return False
    if line[1] == 0x9:
        return False
        
    # 4 digit id with space following
    if line[5] != 0x20:
        return False
    
    # print("line: %s" % line)
    # print("  1:5: %s" % str(line[0:5]))
    
    if not isValidX16(line[1:5]):
        print("[e] Not a valid device id: %s" % line)
        return False
    # print("  valid")
    return True
    
def isSubDeviceLine(line):
    """
    sub device id line format:
    \t<svid> <sdid>  <name>
    """
    
    # check for min line size
    if len(line) < 12:
        return False
    # check for tabs
    if line[0] != 0x9 or line[1] != 0x9:
        return False
        
    # check for separating spaces after each 4 digit id
    if line[6] != 0x20 and line[11] != 0x20:
        return False
    
    # print("line: %s" % line)
    # print("  2:5: %s" % str(line[2:6]))
    # print("  7:11: %s" % str(line[7:11]))
    # print("  13:-1: %s" % str(line[13:-1]))
    
    if not isValidX16(line[2:6]) or not isValidX16(line[7:11]):
        print("[e] Not a valid sub device id: %s" % line)
        return False
    # print("  valid")
    return True

def isValidX16(input):
    """
    check 4 digit input for being in hex range
    """
    
    if len(input) > 4:
        return False
        
    s = input.decode("utf-8")
    return all(c in string.hexdigits for c in s)


def parse(src_path, out_path):
    
    src_file = open(src_path, 'rb')
    out_file = open(out_path, 'wb')
    
    print("parsing pci ids form %s into %s" % (src_path, out_path));

    try:
        vendors = extract(src_file)
        
        print("#vendors: 0x%x" % len(vendors))
        
        if len(vendors) == 0:
            raise ValueError("[e] No vendors found!")
        
        createHeaderStub(out_file)
        
        createVendorNameSwitch(vendors, out_file)
        
        createVendorDeviceIdSwitches(vendors, out_file)
        
        # after device id switches
        createVendorDeviceSwitch(vendors, out_file)
        
    finally:
        src_file.close()
        out_file.close()


def createHeaderStub(out_file):
    """
    Create a header stub file
    """
    print("createHeaderStub()")
    
    out_file.write(b"#pragma once\r\n")
    out_file.write(b"\r\n")
    
    return 0

def createVendorNameSwitch(vendors, out_file):
    """
    Create a function switching on vendor ids and returning its name
    """
    print("createVendorNameSwitch()")
    
    out_file.write(b"PCHAR vendorIdStr(UINT16 VendorId)\r\n")
    out_file.write(b"{\r\n")
    out_file.write(b"    switch ( VendorId )\r\n")
    out_file.write(b"    {\r\n")
    
    for v in vendors:
        id_str = hex(v)
        name = vendors[v]["name"].translate(str.maketrans({"\"":  r"\""}))
        
        content = f'        case {id_str}: return "{name}";\r\n'
        # print("%s" % (content))
        out_file.write(content.encode("utf-8"))
        
    out_file.write(b"        default: return \"Unknown\";\r\n")
    out_file.write(b"    }\r\n")
    out_file.write(b"}\r\n")
    out_file.write(b"\r\n")
    
    print("  [x] done")
    
    return 0

def createVendorDeviceSwitch(vendors, out_file):
    """
    Create a function switching on vendor ids and calling its corresponding device id switch
    """
    print("createVendorDeviceSwitch()")
    
    out_file.write(b"PCHAR deviceIdStr(UINT16 VendorId, UINT16 DeviceId)\r\n")
    out_file.write(b"{\r\n")
    out_file.write(b"    switch ( VendorId )\r\n")
    out_file.write(b"    {\r\n")
    
    for v in vendors:
        f_name = f'v{v:04X}_deviceIdStr'
        
        if len(vendors[v]["devices"]) > 0:
            content = f'        case 0x{v:04X}: return {f_name}(DeviceId);\r\n'
            # print("%s" % (content))
            out_file.write(content.encode("utf-8"))
        
    out_file.write(b"        default: return \"Unknown\";\r\n")
    out_file.write(b"    }\r\n")
    out_file.write(b"}\r\n")
    out_file.write(b"\r\n")
    
    print("  [x] done")
    
    return 0

def createVendorDeviceIdSwitches(vendors, out_file):
    """
    Create a device id switching function for each vendor
    """
    print("createVendorDeviceIdSwitches()")
    
    for v in vendors:
        if len(vendors[v]["devices"]) > 0:
            createVendorDeviceIdSwitch(v, vendors[v], out_file)
    
    print("  [x] done")
    
    return 0

def createVendorDeviceIdSwitch(vendor_id, vendorDict, out_file):
    """
    Create a header file with a function switching on device ids and returning its name
    """
    print("createVendorDeviceIdSwitch(0x%x)" % (vendor_id))
    
    vendor_id_str = f'{vendor_id:04X}'
        
    content = f'PCHAR v{vendor_id_str}_deviceIdStr(UINT16 DeviceId)\r\n'
    out_file.write(content.encode("utf-8"))
    out_file.write(b"{\r\n")
    out_file.write(b"    switch ( DeviceId )\r\n")
    out_file.write(b"    {\r\n")
    
    for d in vendorDict["devices"]:
        id_str = hex(d)
        name = vendorDict["devices"][d]["name"].translate(str.maketrans({"\"":  r"\""}))
        
        content = f'        case {id_str}: return "{name}";\r\n'
        # print("%s" % (content))
        out_file.write(content.encode("utf-8"))
        
    out_file.write(b"        default: return \"Unknown\";\r\n")
    out_file.write(b"    }\r\n")
    out_file.write(b"}\r\n")
    out_file.write(b"\r\n")
    
    print("  [x] done")
    
    return 0



def check_arguments():
    parser = argparse.ArgumentParser()
    parser.description = PROGRAM_DESCRIPTION
    parser.usage = 'python %(prog)s [-s <dir>] [-t <dir>]'
    parser.add_argument('-s', '--source', help='Source file with the pci id list.', default="pci.ids", type=str, required=False)
    parser.add_argument('-t', '--target', help='Target file to store the extracted ids.', default="pciids.h", type=str, required=False)
    parser.add_argument('-v', '--version', action='version', version='{} {}'.format(PROGRAM_NAME, PROGRAM_VERSION))

    return parser.parse_args()

if __name__ == '__main__':
    args = check_arguments()
    if args is None:
        sys.exit()

    if args.source:
        src_path = Path(args.source).expanduser()
    else:
        src_path = args.source.default

    if args.target:
        out_path = Path(args.target).expanduser()
    else:
        out_path = args.target.default
    
    print("src_path: %s" % src_path);
    print("out_path: %s" % out_path);
        
    if not src_path.is_file():
        raise IOError('IOError: Source file does not exist: %s' % src_path)
    
    if out_path.is_file():
        print("[!] Target path %s exists!" % (out_path))
        answer = input("Continue? [y/n] ")
        if not answer.lower() in ["y","yes"]:
             sys.exit()
        
    parse(src_path, out_path)
    

    sys.exit()
