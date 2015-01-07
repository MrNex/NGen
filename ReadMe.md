# NGen

A 3D Game / Simulation API for the C programming language.

Rapidly create complex 3D graphical games and simulations. Working similarly to XNA, NGen strives to be a quickstart to advanced 3D games. By planning to provide engine components ready to manage input, rendering, collisions, physics, menus, game objects, and states you can rapidly create games and simulations without any of the performance drawbacks of easy to learn languages or libraries.

## Dependencies

OpenGL
- https://www.opengl.org/

Freeglut
- http://freeglut.sourceforge.net/

## Build

NGen is compiled using MinGw's gcc compiler. After installing freeglut you can compile the program with
> gcc main.c -o NGen -lFreeglut -lOpenGL32

Coming soon: A makefile... Sorry for now guys.

## Installation

Currently, after compilation, you can run the engine directly from the output file.
In the future, when the other engine components are added, this will change.
