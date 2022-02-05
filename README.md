# circumcenter-map

Repository for the publication "Exploring the Circumcenter Map" by Ronaldo Garcia, Nicholas McDonald and Dan Reznik.

This allows for C / C++ based visualization of the iterative circumcenter map on local devices for fast computation, and also provides a webGL viewer.

This problem is embarassingly parallel, so that is why this repo exists to accelerate the visualization using GPU programming.

## C / C++ Visualizations

A number of GPU accelerated, interactive visualizations exist, written in C/C++ using [TinyEngine](https://github.com/weigert/TinyEngine).

`visualizations/iteration-map`: Regular Planar Visualization of the Iterative Circumcenter Map
`visualizations/stereographic`: Stereographic projection with adjustable scale
`visualizations/sweephull`: Raymarched sweephull of the 2D planar iterative circumcenter map, parameterized by an affine skew operation.

### Building

To setup the program, install the only dependency (`TinyEngine`)

1. Install TinyEngine (On Linux)
2. Use the Makefile (`make all`)

Execute using

    ./main [windowsizex] [windowsizey]

Default Window Size: (800, 800)

### Usage

Controls:

    WASD: Move
    Scroll: Zoom
    ESC: Toggle Menu

## Webviewer
