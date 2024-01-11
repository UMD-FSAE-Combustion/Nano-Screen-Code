# Nano-Screen-Code
Code for Driver's Display

Screen with gear position, engine rpm, and shift bar

CHANGELOG:

 - v1.0:
	• Created display driver to show gear psoition, RPM, and shift light.

- v2.0:
	• Removed RPM data from display, deleted functions to calculate shift points
	• Moved shift point calculation to motec ECU, created flags for shift points 
	  to send to display.  This allows for better optimization on display driver.
	• Added 'shift messages' to communicate upshift or downshift for the driver 
	  in addition to shift lights.
	• Removed canvases from display due to bitmap issues, added alternate method
	  for flicker free display driving.
