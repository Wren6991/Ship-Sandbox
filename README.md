Ship Sandbox
============

This game was originally written by Luke Wren. After becoming fascinated by it, I (Gabriele Giuseppini) forked the GitHub repo
and started playing with the source code.

My background is in C++ development, mostly of low-latency ETL pipelines for ingesting multi-terabyte data in real time. 
After stumbling upon Luke's simulator, I became fascinated with the idea of messing up with a virtual world
modeled in terms of elementary physics. More on this later.

Following is the original README.md:
---
Written for Francis Racicot

Ship Sandbox is a physics simulator for ships, written in C++. wxWidgets is used for windowing.

<img src="http://i.imgur.com/UYhVSHJ.png">

.h2 Features:

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

.h2 Youtube Video

<a href="http://www.youtube.com/watch?v=LQ0XHRIoQe0">Link.</a>
---

Some interesting background on the original project at Luke's blog for this project can be found at <a href="http://sinkingshipdev.tumblr.com/">this page.</a>

My goal with this playing pen is to learn the physics modeling from Luke and start messing with it. Of course I have great ambitions, so the very first step
is to make the current simulator as fast as possible, so to minimize the impact of my new features on the performance of the game.

After cleaning up the code a bit and making it CMake- and VS2017-friendly, the initial baseline (on my single-core laptop!) is:
- Debug: 1fps/2fps
- Release: 7fps/8fps

And here's the list of changes:
- 2018-01-19: Reverted Luke's code to use wxWidgets, as GLFW was too bare-bones for my liking (couldn't really make dialogs and UI controls)
- 2018-01-21: Added logging and a logging window (to make it easier to play with things and get direct feedback)