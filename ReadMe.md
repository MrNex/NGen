# NGen

###### By: [Mr. Nex (A. K. A. Nicholas Gallagher)](http://mrnex.github.io)

## Description

A 3D Game / Simulation API for the C programming language.

Rapidly create complex 3D graphical games and simulations. Working similarly to XNA, NGen strives to be a quickstart to advanced 3D games. By planning to provide engine components ready to manage input, rendering, collisions, physics, menus, game objects, and states you can rapidly create games and simulations without any of the performance drawbacks of easy to learn languages or libraries. However, the NGen has a mission that is unlike many other engines out there.

## The Mission

**Free and Open Features**. Many other engines out there today try to hide and abstract the underlying structure. The NGen *wants* you to view and take advantage of these features! Sure, it's dangerous- you could even mess up & get a few errors, but in the end it opens the door for possibilities. As an example, here are some things I was able to play around with out of sheer curiosity:

On the first day I decided it would be cool if the Y axis of an objects local coordinate system were to slowly rotate around the X axis, skewing the object until it's model had fallen flat, and it was good.

On the second day, I thought it neat to take a pointer to the row of the Camera's transformation matrix which signifies the Z axis of the camera, and sent it to the physics manager as a global acceleration vector (With a magnitude of 9.81), and it was good.

On the third day, I dared to take the rotation matrix of the camera & sent it to the fragment shader to transform the texture sample of the ground, and it was *fucking awesome*.

On the fourth day I manipulated the perspective matrix to make things get BIGGER as they got further away, and it turns out that was not such a good idea..

Play with my NGen, satiate your curiosity, break things and then fix them, create your own world that defys the laws of all things we know to be simulation, and lastly-- contact me if you have ANY questions.

## Dependencies

OpenGL
- https://www.opengl.org/

Freeglut
- http://freeglut.sourceforge.net/

## Build

NGen is compiled using MinGw's gcc compiler. After installing freeglut you can compile the program from the top-level project directory by entering the following command:

Windows:
> mingw32-make

Linux/Mac:
> make

The Makefile will take care of the rest. You can then run the project with:
> NGen

## Installation

Simply download the project, extract to it's own folder if necessary, and then follow the instructions in the Build section. No other installation steps are needed.

## Using the NGen

As engine components are added I will be updating this section with examples of how you can use them.

### Math

ToDo: Provide an example of how to utilize the NGen's Matrix and Vector mathematics library.
