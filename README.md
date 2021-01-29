# CAN FD Sniffer
** Actively in development **

Updated CAN Sniffer for use on cars which make use of the CAN FD protocol rather than just CAN 2.0 with OBD 2.
Based on the CAN sniffer by Adam Varga.


Adam created some useful tutorials for the original project. These are still partly relevant and are linked here:
https://www.youtube.com/playlist?list=PLNiFaO8hU1z0o_6DSxk-jcVAM3UCUR-pY

# Content
- 00_canSnifferFD_Arduino: This code runs on your arduino sniffer device and creates an interface between the car and the GUI (For CAN FD).
- 01_canSniffer_Arduino: This code runs on your arduino sniffer device and creates an interface between the car and the GUI.
- 02_canSniffer_GUI: Powerful and easy-to-use graphical sniffer application used for reverse engineering CAN packets (Both CAN FD and CAN 2.0). 
# Description
##### 00_canSnifferFD_Arduino
This code creates the interface between the car and the canSniffer_GUI application. 
There is a mode setting in helpers.h that allows you to switch between different modes of operation. These modes are described here:
- STANDALONE: Standard mode of operation. Reads in packets from the CAN controller and sends them back over serial for the GUI to interpret
- FAKE_SERIAL: Test mode. Generates random packets that are sent back over serial to the GUI.
- CHECK_CONNECTION: Prints out extra diagnostic information while connecting to the vehicle, then enters normal standalone mode. Note that this extra diagnostic information will have to be read through the arduino serial monitor as the GUI does not support it.

 Required arduino packages: 
 - ACAN2517FD by Pierre Molinaro (https://github.com/pierremolinaro/acan2517FD)

##### 01_canSniffer_Arduino
This code creates the interface between the car and the canSniffer_GUI application. If the RANDOM_CAN define is set to 1, this code is generating random CAN packets in order to test the higher level code. The received packets will be echoed back. If the  RANDOM_CAN define is set to 0, the CAN_SPEED define  has to match the speed of the desired CAN channel in order to receive and transfer from and to the CAN bus.
 Required arduino packages: 
- CAN by Sandeep Mistry (https:github.com/sandeepmistry/arduino-CAN)

Required modifications: 
- MCP2515.h: 16e6 clock frequency reduced to 8e6 (depending on MCP2515 clock)
- MCP2515.cpp: extend CNF_MAPPER with your desired CAN speeds

##### 02_canSniffer_GUI
Python 3 is required for this project, 3.8 is preferred. This project contains Adam's decoded packets in save_old/decodedPackets.csv. 
Decoded packets from the current author are saved in save/decodedPackets.csv.

The required python packages can be installed with:
```sh
$ pip install -r requirements.txt
```
