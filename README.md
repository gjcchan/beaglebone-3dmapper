# beaglebone-3dmapper

This project was created for CMPT433 at SFU during Fall 2015 Semester. Powered by a BeagleBone Black, the hardware setup uses a pan and tilt PWM servo kit with a Sanyo GP2Y0A21YK infrared
proximity sensor to map out the immediate area around the hardware in 3d space.

It contains a routine that measures room temperate, as well as an on-demand routine that scans the room and display the interpreted 3d space to web-based control panel  
using webGL (wireframe only.)

The code is a mixture of HTML5, various WebGL libraries, jQuery (for front-end), nodeJS for web-API, and C for all lower level routines.
