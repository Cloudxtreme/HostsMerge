CC = g++
CFLAGS = -g -O3 -Wall -fomit-frame-pointer -mcpu=arm926ej-s -std=c++0x
OBJECTS = Hosts.o HostsMergeCmd.o LoadHostsFile.o OutputHostsFile.o OutputWpad.o OutputBindZones.o PackedIPAddress.o stdafx.o

HostsMergeCmd.exe : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o HostsMergeCmd.exe

%.o : %.cpp
	$(CC) $(CFLAGS) -c $<

#Hosts.o: stdafx.h Hosts.h PackedIPAddress.h
#HostsMergeCmd.o: stdafx.h Hosts.h HostsMergeCmd.h PackedIPAddress.h LoadHostsFile.h OutputHostsFile.h OutputWpad.h PackedIPAddress.h
#LoadHostsFile.o: stdafx.h Hosts.h LoadHostsFile.h PackedIPAddress.h
#OutputHostsFile.o: stdafx.h Hosts.h OutputHostsFile.h PackedIPAddress.h
#OutputWpad.o: stdafx.h Hosts.h OutputWpad.h PackedIPAddress.h
#OutputBindZones.o: stdafx.h Hosts.h OutputBindZones.h PackedIPAddress.h
#PackedIPAddress.o: stdafx.h PackedIPAddress.h
#stdafx.o: stdafx.h 

# Hosts.h : PackedIPAddress.h
# LoadHostsFile.h : Hosts.h PackedIPAddress.h
# OutputHostsFile.h : Hosts.h PackedIPAddress.h
# OutputWpad.h : Hosts.h PackedIPAddress.h
# OutputBindZones.h : Hosts.h PackedIPAddress.h
