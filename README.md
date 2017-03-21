# Natura

Collaborators:
* [JBouron](https://github.com/jbouron)
* [charlesthiebaut](https://github.com/charlesthiebaut)

### Table of Contents :
  * [Introduction](#introduction)
  * [Setup](#setup)
  * [Configuration](#configuration)
  * [Running](#running)
  * [Preview](#preview)
  * [Features](#features)
  * [Award](#award)



### Introduction
  As part of the Introduction to Computer Graphics class given at EPFL, for our class project we created an infinite procedural world.
This repository gathers all the code used for this project.

### Setup
  In order to run this code you will be needing GLFW. You should be able to install it through you package manager, for Arch it's available as either `glfw-x11` or `glfw-wayland` depending on your xServer.
You might be required to install other librairies depending on your graphics card / environment setup.

### Configuration
  Most of the configurable vars are in `config.h`, however for more in depth modifications you might have to modify more code.
Shaders should be plug-and-play if you do not mess with the paths.

### Running 
  Simply enter your favourite terminal emulator, and enter the following commands:
```bash
mkdir build
cd build
cmake ..
make
cd natura
./natura
```

### Preview
The image below links to a YouTube video illustrating the final result of this project. The video framerate and resolution is not representative of the actual software.
[![Video of project results](http://img.youtube.com/vi/yrVUSoXkI08/0.jpg)](http://www.youtube.com/watch?v=yrVUSoXkI08)

### Features
  Here is a non-exhaustive list of the implemented features:
Infinite procedual terrain (Perlin noise + Voronoi), water reflections (framebuffer), real-time shadow mapping, bezier curves (camera movement), physics simulation, grass moving (wind effect).

Our goal was to achieve perofmance before realistic/beautifull results. Most of the features mentionned above can be altered in real-time without any performance loss.

### Award
  This project won the second prize for the "Best course project" award.
