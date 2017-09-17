# ESP8266-Wemo-Sonoff-and-HomeVisionXL-Plug-in

Code to control ESP8266 Wemo emulating devices via HomeVision. Also natively supported by Alexa.

<b>You must have a HomeVision Controller running HomeVisionXL software with the NetIO plug-in and an RCS TX-15B thermostat for this to work.</b>

The ESP8266 part of this project is based on the work of Nassir Malik. See his Youtube tutorials at https://www.youtube.com/channel/UCn8v7OzXk7IqRdKZdf14yjA and code at https://github.com/nassir-malik

The HomeVisionXL plug-in is based in part on the work of Schelte Bron.

<b> This is a work-in-progress and is subject to change.</b>

In this project you will be able to control an ESP8266 Sonoff device as a Belkin Wemo.
Note: Other ESP8266 device types can be used, with appropriate relay and/or LED pin changes.
1. Edit the [ESP8266-Wemo-Sonoff-and-HomeVisionXL-Plug-in.ino](https://github.com/rebel7580/ESP8266-Wemo-and-HomeVisionXL-Plug-in/blob/master/ESP8266_Wemo_Sonoff_and_HomeVisionXL/ESP8266_Wemo_Sonoff_and_HomeVisionXL.ino) code to include your WiFi SSID and passcode and a unique device name. This name will be used with Alexa.
Set up your ESP8266 for downloading and download the modified code to it.
See Nassir Malik's tutorials for details if you need help doing this.
1. Once downloaded, you should be able to discover the device with Alexa. It will appear as a "WeMo Switch" with the name you set in the code.
1. HomeVision control of the devices is achieved via the [wemo.hap](https://github.com/rebel7580/ESP8266-Wemo-and-HomeVisionXL-Plug-in/blob/master/wemo.hap) HomeVisionXL plug-in. Download the plug-in to your HomeVisionXL's plugin directory and enable it via the Plugin Manager.
1. The plug-in's configuration screen allows you to enter multiple device names and their corresponding IP addresses, or discover them.
1. Test the plug-in my using the HomeVisionXL's Serial Command Test screen.
1. Once you have it working via the Serial Command Test screen. you can include appropriate serial commands into your schedule.
1. You can use NetIO to control the devices as well.

See this Project's [Wiki page](https://github.com/rebel7580/ESP8266-Wemo-and-HomeVisionXL-Plug-in/wiki/ESP8266-Wemo-and-HomeVisionXL-Plug-in) for more details.
