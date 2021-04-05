# Bouncers

This is a simulation of evolving agents trying to win a one-on-one duels within
a two-dimensional physical environment.

## Building

You can probably just run `make`, which produces an optimized build by default.
You need a C++14 compiler and SDL2. The Makefile probably requires Unix.

## User Interface

When starting the simulation (by running `./bouncers`,) you can provide a
numeric seed as the first argument. If you don't provide one, the program will
choose a seed for itself.

While the simulation is running, you will see an increasing number which is how
many generations have passed. You can press SPACE to pause/unpause. Pressing
ENTER animates one round for you. The agents are the moving circles, and the
time remaining is shown in the top left.

## Customization

I've decided not to bother writing a configuration parser. To customize the
simulation, you can edit the files [src/conf.hpp](src/conf.hpp) and
[src/score.cpp](src/score.cpp). The former contains many numeric parameters, and
the latter lets you decide how to score the agents.
