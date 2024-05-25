# kit_robo_webserver

The project "Kit_Robo_Webserver" on GitHub is a repository for a robot controlled remotely via a web server. 
It integrates an HTML and JavaScript-based joystick for control and uses an ESP8266 microcontroller with the AsyncWebServer library for managing Wi-Fi connectivity. 
The repository includes code and documentation for setting up the hardware and software components necessary for the robot's operation.

To start developing the Kit_Robo_Webserver project, follow these steps:

1. **Clone the Repository**: Clone the GitHub repository to your local machine.
   ```bash
   git clone https://github.com/pahagon/Kit_Robo_Webserver.git
   ```

2. **Set Up Hardware**: Assemble the robot hardware as per the instructions in the repository. This includes connecting the ESP8266 microcontroller.

3. **Install Dependencies**: Install necessary libraries, such as AsyncWebServer, for the ESP8266.

4. **Upload Code**: Use the Arduino IDE to upload the provided code to the ESP8266.

5. **Configure Wi-Fi**: Set up your Wi-Fi credentials in the code to enable remote control.

6. **Run the Web Server**: Access the web interface via the robot's IP address to control it.

## TODO
[] Create makefile task in order to copy files to ESP8266 using [esptool](://github.com/espressif/esptool).

## Links
* [Arduino - Wrong Characters on Serial Monitor - How to Fix](https://www.youtube.com/watch?v=4Z8f5MyS_EY)
* [How to use the L298N Motor Driver with Arduino - Quick Tutorial](https://www.youtube.com/watch?v=E2sTbpFsvXI)
* [ESP8266 NodeMCU: Create a Wi-Fi Manager (AsyncWebServer library)](https://randomnerdtutorials.com/esp8266-nodemcu-wi-fi-manager-asyncwebserver/)
* [L298N H Bridge DC Motor Controller without any Microcontroller](https://www.youtube.com/watch?v=0FCA6n-8AXE)
* [Domine os segredos para programar o ESP01, o mini Arduino com WiFi](https://www.youtube.com/watch?v=9dxEhlgkE3o)
* [Joystick based on this project](https://www.instructables.com/Making-a-Joystick-With-HTML-pure-JavaScript/)
* [Wifi Manager](https://randomnerdtutorials.com/esp8266-nodemcu-wi-fi-manager-asyncwebserver/)

