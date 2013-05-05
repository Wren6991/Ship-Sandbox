Ship Sandbox
============

Written for Francis Racicot

Ship Sandbox is a physics simulator for ships, written in C++. wxWidgets is used for windowing.

<img src="http://i.imgur.com/UYhVSHJ.png">

Features:
---------

- Super-stable physics simulation, written from the ground up in C++
	- 5000+ connected springs, stable simulation
	- Iterative, scaleable method - can make the simulation more accurate by throwing more processor power at it
- Imports and builds ships from PNG images
	- Draw a ship in paint:
	- <img src="http://i.imgur.com/Hzgsh1A.png">
	- Different colours represent different materials
- Customize stuff!
	- Can turn the waves into a storm, or make ships fall apart at a touch
- Fluid Simulation
	- Simulator models water pressure and the effects of gravity inside the ships
	- Water is forced in through breaches in the hull and pools at the bottom, dragging the ship down
- Interact!
	- If your ship's taking too long to sink, smash it to bits with your mouse

Youtube Video
-------------
<iframe width="560" height="315" src="http://www.youtube.com/embed/LQ0XHRIoQe0" frameborder="0" allowfullscreen></iframe>