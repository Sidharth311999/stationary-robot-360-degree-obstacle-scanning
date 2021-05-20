# stationary-ultrasonic-360-degree-obstacle-scanning
## Project description of the code
  
There are two ultrasonic sensors connected back to back and they are rotated using a stepper motor 180 degrees clockwise and anti clockwise to scan for single object in 5m radius, 360 degree using arduino nano.

## working of code

* All variables, component pins are declared.
* Two classes are created for trigger and ecco pins for two sensors, for calculating distance of the obstacle.
* Half-stepping motor input seuqence byte array is made.
* All objects of the classes are declared globally.
* In void setup the serialmonitor and motor positioning(home();) declaration is done.
* The display and positioning functions are initialized.
* In void loop function for continuous excecution, the objects are invoking the member functions printing the distance value and angle, the motor is also rotated.
* In rotate function the motor is rotated clockwise and anti clockwise, and angle value is also calculated.
  
