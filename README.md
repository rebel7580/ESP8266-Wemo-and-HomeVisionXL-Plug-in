# ESP8266-Wemo-and-HomeVisionXL-Plug-in

Code to control ESP8266 Wemo emulating devices via HomeVision. Also natively supported by Alexa.

<b>You must have a HomeVision Controller running HomeVisionXL software with the NetIO plug-in and an RCS TX-15B thermostat for this to work.</b>

This project is based on the work of Nassir Malik. See his Youtube tutorials at https://www.youtube.com/channel/UCn8v7OzXk7IqRdKZdf14yjA and code at https://github.com/nassir-malik

<b> This is a work-in-progress and is subject to change.</b>

In this project you will be able to control an ESP8266 device as a Belkin Wemo.
Edit the [ESP8266-Wemo-and-HomeVisionXL-Plug-in.ino]https://github.com/rebel7580/ESP8266-Wemo-and-HomeVisionXL-Plug-in/blob/master/ESP8266_Wemo_and_HomeVisionXL.ino code to include your WiFi SSID and passcode and a unique device name. This name will be used with Alexa.
Right now, the code turns on/off the built-in LED and GPIO02, to which the LED is connected in the ESP8266-01s.
You may need to change this for your particular hardware.
Set up your ESP8266 for downloading and download the midified code to it.

You should be able to discover the device with Alexa. It will appear as a "WeMo Switch".

HomeVision control of the devices is achieved via the [wemo.hap]https://github.com/rebel7580/ESP8266-Wemo-and-HomeVisionXL-Plug-in/blob/master/wemo.hap HomeVisionXL plug-in.  The plug-in goes with the ESP8266 code I have running on my "Wemo" emulator. So the two work together.  The http commands in the plug-in can be modified to do commands supported by other ESP8266 emulations.

The http command is like this:
<pre>
      http://192.168.xxx.yyy/switch?st=off
</pre>
Possible values for xx in "st=xx" are  0|off|1|on|2|toggle

Response is "{device name} is {On|Off}"
I haven't done anything w.r.t the response. Maybe it would set a flag to show state?

A HomeVision serial command to turn it on would be:
<pre>
     wemo: [device]  [0|off|1|on|2|toggle];
</pre>
Can also control it via the NetIO plug-in:
<pre>
     sends: netioaction wemo [device ]  [0|off|1|on|2|toggle]
</pre>  
See also this Project's [Wiki page](https://github.com/rebel7580/ESP8266-Wemo-and-HomeVisionXL-Plug-in/wiki/ESP8266-Wemo-and-HomeVisionXL-Plug-in) for more details.
