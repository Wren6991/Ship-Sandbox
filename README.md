

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

### Features:
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

### Youtube Video
<a href="http://www.youtube.com/watch?v=LQ0XHRIoQe0">Link</a>.

----------
Some interesting background on the original project can be found at <a href="http://sinkingshipdev.tumblr.com/">Luke's blog</a>.

My goal with this playing pen is to learn the physics modeling from Luke and start messing with it. Of course I have great ambitions, so the very first step
is to make the current simulator as fast as possible, so to minimize the impact of my new features on the performance of the game.

After cleaning up the code a bit and making it CMake- and VS2017-friendly, the initial baseline (on my single-core laptop!) is:
- Debug: 1fps/2fps
- Release: 7fps/8fps

My plan is roughly as follows:
### Phase 1
- Revert Luke's code to use wxWidgets, as GLFW is too bare-bones for my liking (can't really make dialogs and common UI controls)
- Cleanup all compiler warnings and memory leaks
-- As of now the game has a ton of memory leaks, though not on the repeated steps so they don't hurt, but still they show up in Visual Studio and it'd be nice to get rid of them so to have better visibility on future accidentally-introduced leaks
- Cleanup naming conventions
- Add a logger to have better visibility into the effects of code changes
### Phase 2
- Better interaction between the UI and the game, splitting settings between physics-related settings and render-related settings
- Fix lifetime management of points, springs, and triangles - at this moment elements are removed from vectors while these are being iterated, and the entire points-to graph is a tad too complex IMHO
- Low-hanging speedups (e.g. inline functions)
### Phase 3
- Investigate feasibility of new features:
-- Add time-of-day (i.e. sink at night vs. during the day)
-- Add lights that would turn off on when generator is wet
-- Add ability to raise ship from bottom of the sea via ropes