# cppBD
This is C++ Back door send information with WMI queries &amp; bypassing Firewall with Reverse TCP

-------------------------------------------------------------------------------------------------------------------------------
bd-1\bd-1.cpp
    This is the main application source file.
    
-------------------------------------------------------------------------------------------------------------------------------
with the change of line 138 in bd-1.cpp you can change WMI query and send other information, now this sample code send current Processes on victim computer.
 By Default this code listenig TCP connection on Port 8888, but in line 223 you can change this Port.
