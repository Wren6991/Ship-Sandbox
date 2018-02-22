Ship Sandbox
============
This game was originally written in 2013 by Luke Wren. After becoming fascinated by it, I (Gabriele Giuseppini) forked the GitHub repo
and started playing with the source code.

My background is in C++ development, mostly of low-latency ETL pipelines for ingesting multi-terabyte data in real time. 
After stumbling upon Luke's simulator, I became fascinated with the idea of messing up with a virtual world
modeled in terms of elementary physics. More on this later.

Some interesting background on the original project can be found at <a href="http://sinkingshipdev.tumblr.com/">Luke's blog</a>. Following is the original README.md:

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

---
Back to Gabe's version now.

My goal with this playing pen is to learn the physics modeling from Luke and start messing with it. Of course I have great ambitions, so the very first step
is to make the current simulator as fast as possible, so to minimize the impact of my new features on the performance of the game.

After cleaning up the code a bit and making it CMake- and VS2017-friendly, the initial baseline (on my single-core laptop!) was:
- Debug: 1fps/2fps
- Release: 7fps/8fps

Here's a list of the major changes I've been doing:
- Reverted Luke's code to use wxWidgets, as GLFW was too bare-bones for my liking (can't really make dialogs and common UI controls)
- Upgraded to C++17
- Restructured interactions between the UI and the game, splitting settings between physics-related settings and render-related settings
- Rearchitected lifetime management of elements - originally elements were removed from vectors while these are being iterated, and the entire "points-to" graph was a tad too complex 
- Completely re-written the OpenGL interactions, targeting 2.0 "core profile" (i.e. no compatibility API) with custom shaders and adding texture mapping
-- At this moment, the fps rate on my laptop reached 16fps
- Inlined critical functions
- Added unit tests

..and here's a rought list of the major changes I want to do:
- Texture mapping for ships
- Leverage vectorized instructions for dynamics calculations 
- Add sounds and cued music
- Add lights that would turn off (after flickering) when generator is wet or when electrical cables break
- Add time-of-day (i.e. day light change during the game)

I tried to do my best to craft the CMake files in a platform-independent way, but I'm working on this exclusively in Visual Studio, hence I'm sure some unportable features have slipped in. Feel free to send pull requests for CMake edits for other platforms.
You'll need the following libraries in order to build the game:
- <a href="https://www.wxwidgets.org/">WxWidgets</a> (cross-platform GUI library) (on Windows, has to be built with statically-linked CRT libraries)
- <a href="http://openil.sourceforge.net/">DevIL</a> (cross-platform image library) (on Windows, has to be built with statically-linked CRT libraries)
-- I've actually built my own DevIL, as the DevIL DLL's come only with a dynamically-linked CRT
- <a href="https://github.com/kazuho/picojson">picojson</a> (header-only JSON parser and serializer)
The top of the main CMakeFiles.txt contains a section with hardcoded paths to these three libraries; you'll have to edit your CMakeFiles to match your environment.