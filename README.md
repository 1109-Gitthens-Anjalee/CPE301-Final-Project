# CPE301-Final-Project
Anjalee Gitthens
Shawn Ray
CPE 301
13 December 2022
Cooling System Design Document
The design of the cooling system consists of a water level sensor, a stepper motor, an LCD display, a real-time clock module, a temp/humidity sensor, a kit motor, a fan blade, buttons and a couple of LEDs. The cooling system is powered by 5V for all the components and the temperature needs to range from 0 degrees Celsius to 50 degrees Celsius. The LEDs are able to demonstrate the state that the system currently is in. The water sensor's current state in terms of water level will guide the cooling system's components.
Throughout each state that the water sensor goes through the real-time clock and it is displayed on the serial screen. The data from the humidity and temperature sensor will be displayed on the LCD display. The fan motor when off has a button that can toggle it. When the fan is off the system goes to a disabled mode where a yellow light should be on. The idle state on the other hand has the water level constantly monitored. The green light will also be on with the real-time clock also being displayed on the serial monitor. The error message is present when the water level is too low. The motor will be off and an error message will be displayed on the LCD display. The red light will be on for the error state. Finally, there is the running state that is present when the fan is on and the blue light is on. As the water level changes the state will change and transition to the idle state or the error state. Overall the vent direction as well as the state of the cooling system is controlled with buttons.

Resources:
Water Level Sensor:
https://create.arduino.cc/projecthub/sheekar/water-level-sensor-with-arduino-sheekar-banerjee-e02c05

Vent Direction:
https://www.tutorialspoint.com/arduino/arduino_stepper_motor.htm

LCD Display:
https://create.arduino.cc/projecthub/arduino-projects/arduino-tutorial-lcd-display-cb2834

Real-Time Clock Module:
https://www.circuitbasics.com/how-to-use-a-real-time-clock-module-with-the-arduino/

Temperature/Humidity Sensor
https://theiotprojects.com/interface-dht11-sensor-with-arduino-and-lcd/
