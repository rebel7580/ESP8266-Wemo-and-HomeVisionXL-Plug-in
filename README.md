# ESP8266-Wemo-and-HomeVisionXL-Plug-in

Code to control ESP8266 Wemo emulating devices via HomeVision. Also natively supported by Alexa.

<b>You must have a HomeVision Controller running HomeVisionXL software with the NetIO plug-in and an RCS TX-15B thermostat for this to work.</b>

This project is based on the work of Nassir Malik. See his Youtube tutorials at https://www.youtube.com/channel/UCn8v7OzXk7IqRdKZdf14yjA and code at https://github.com/nassir-malik

TCP code used to talk to the HomeVision system is a modified version from Big Dan the Blogging Man at https://bigdanzblog.wordpress.com/2016/02/09/esp8266arduino-ide-communicating-with-tcp/

In This project you will be able to control an ESP8266 device as a Belkin Wemo. HomeVision control of the devices is achieved via a HomeVisionXl plug-in.

The plug-in goes with the ESP8266 code I have running on my "Wemo" emulator. So the two work together.
The http commands in the plug-in can be modified to do commands supported by other ESP8266 emulations.

The http command is like this:
      http://192.168.xxx.yyy/switch?st=off
Possible values for xx in "st=xx" are  0|off|1|on|2|toggle

Response is "{device name} is {On|Off}"
I haven't done anything w.r.t the response. Maybe it would set a flag to show state?

A HomeVision serial command to turn it on would be:
   wemo: [device]  [0|off|1|on|2|toggle];

Can also control it via the NetIO plug-in:
   sends: netioaction wemo [device ]  [0|off|1|on|2|toggle]
   
See also this Project's [Wiki page](https://github.com/rebel7580/ESP8266-Wemo-and-HomeVisionXL-Plug-in/wiki) for more details.
