Setup:

You need the latest godot headers in the base of this repository
Easiest way to do this is

>git clone https://github.com/GodotNativeTools/godot_headers

You also need some C compiler to run the scons build, but run

>scons

in the root directory

This only works on Godot master branch at the moment, so you need to clone and build 
follow instructions at http://docs.godotengine.org/en/latest/development/compiling/index.html

lastly you'll need to connect the gdlib to your compiled library.

In the godot file explorer, go to res://bin/testlib.gdnlib, double click on it, and point it to the library built from running scons in the first part.
should be a *.so file or a *.dylib on windows

