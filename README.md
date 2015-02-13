This is my game engine.

It is written to be useful from the ground-up, to avoid unpleasant abstractions, and to put the programmer in control. Focus is on Mobile ES 3 games while using a modern unixy programmer workstation. Structured carefully so you only have to write shaders, assets, and game logic once.

Code consists mostly of glue and is rather self-documenting; mostly C99 and C++11.

Will feature iOS and Android platform targets out-of-box. Currently focusing on Android. Will eventually support asset management, content creation, unified logging, and control over a network of various compilers. May evolve into a platform.

Still in very early stages.

This is a serious, focused project. Expect progress, but please keep in mind that my time budget is very strict: I cannot help you on anything out-of-scope, nor do I have enough time to integrate a feature unless this framework truly needs it. However, if you like this project and want to contribute, feel free to code a module and submit a pull request. We'll both benefit. :)

IMPORTANT NOTICE: This repository is subject to rebasing, renaming, intense refactoring, non-functional and even outright broken builds, library additions, library removals, total lack of friendly documentation.


ROADMAP:

```
Figure out requirements and general structure
Rudimentary buildscripts
Integrate Android/EGL
Integrate GLFW
---------> WE ARE HERE <-----------
Virtual filesystem
Integrate multimedia libraries
 Nanovg
 ImGUI (?)
 Enet, libcurl
 libpd
Integrate physics and modeling libs
 Newton
 Tokamak (?)
 Bullet
 ODE
 polyvox
Assimp
Touch gestures via $1 or $P
Add shaders, basic assets, virtual camera control
Skeletal animation
Gambit Scheme REPL
Asset Managememnt
Level editor, libspnav for ndof 
iOS deployment target
Project deemed mature and my own game development starts.
```



Future updates (on an as-needed basis)

```
Ad network and IAP integration
bgfx
More automations
Authentication
Music creation/scripting
Editors for Modeling/Animation
Unified testing framework
Unify math libs
ShapeOp (?)
Castor (?)
```
