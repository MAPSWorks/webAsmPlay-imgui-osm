----------------------
WebAsmPlay
----------------------

WebAsmPlay was initially developed to evaluate web application development using Emscripten.

Currently a handful of C++ libraries have been compiled with Emscripten in this project to run in the browser
and run natively. The idea is to test a cross platform development environment where an application
is developed and debugged both as a web application and a native application.
Another goal is to discover the limitations and capabilities of Emscripten web application development.
Cross platform development is achieved using cmake.

![Screenshot](docs/webAsmPlay.jpg)

The current goal of this project is morphing into a Real-Time Strategy game using
OpenSteer (http://opensteer.sourceforge.net/) and OpenStreetMap (https://www.openstreetmap.org)
To provide a starting point for non player character(NPC) zombie bots who roam Open Street Map cities.
NPC positions will be calculated on a server and the client application running in the browser or natively
can query bot positions and states for the given viewport. The server will also be responsible for geometry
generalization, bot AI, navigation graphs generated from Open Street Map data and state persistence.
In the near future this project is going to be renamed to ZombieGeoSim. 

### Features

* Ability to run the client in a supported web browser or as a native client. 
    * Object Picking where the picked object under the mouse cursor is highlighted and it's attributes are displayed.
    * Skybox Renderer.
    * Path finding over linear features.
    * Autonomous zombies wonder on paths while steering to avoid each other.
    * Multiple camera modes: TrackBall and zombie tracking camera mode.
    * User definable symbology color rendering supporting transparency for both fill and outlines.
        * Camera near and far symbology can be defined for each attribute allowing for interpolated symbology blending proportional to object to camera distance.
    * Ability to render 3D buildings. 

* Geometry Server
    * ESRI Shapefile and OpenStreetMap XML GIS formats with linestring and polygon ingestion along with attribution.
    * Fast custom attributed transport protocol.
    * Ensures linear feature topology correctness. Breaks linestrings at intersections and connections. Removes overlapping linestrings.

### Dependencies

Unfortunately during development notes on build depends were not tracked. Development is currently
being done on OSX. Macports (https://www.macports.org/) and Brew (https://brew.sh/) is being used to
install third party dependencies. C++ 14 is being used. Currently emscripten is using clang 5.0 which
does not fully support c++ 17. On windows it is recommened to use scoop (https://scoop.sh) to install third party requirements

* Cmake: https://cmake.org
* Emscripten: http://kripken.github.io/emscripten-site
* Glfw3: https://www.glfw.org/
* Glew: http://glew.sourceforge.net/
* Boost: https://www.boost.org/
* Websocketpp: https://www.zaphoyd.com/websocketpp
* Gdal: https://www.gdal.org/ [[Must be built with Geos]]  For Windows: http://www.gisinternals.com
* Geos https://trac.osgeo.org/geos
* SDL2_image: https://www.libsdl.org/
* Intel TBB: https://www.threadingbuildingblocks.org/

### Sources

Some third party dependencies are built into the WebAsmPlay source tree to make emscripten compilation easer
and allow C++ 17 compilation:  
* Geos: https://trac.osgeo.org/geos
* OpenSteer: http://opensteer.sourceforge.net
* Ctpl: https://github.com/vit-vit/CTPL
* Glm: https://glm.g-truc.net/0.9.9/index.html
* GLUTessellator: https://github.com/mbebenita/GLUTesselator
* Native Dear Imgui: https://github.com/ocornut/imgui [Docking branch]
* Imgui-addons: https://github.com/wflohry/imgui-addons
* Emscripten Dear Imgui: https://github.com/greggman/doodles/tree/master/glfw-imgui
* Websocketpp: https://github.com/zaphoyd/websocketpp
* Histogram: https://github.com/HDembinski/histogram
* SimpleJSON: https://github.com/MJPA/SimpleJSON

### Art and Data
* https://www.iconfinder.com/iconsets/32x32-free-design-icons

#### Building


The web client:

``` Bash
$ git clone https://github.com/trailcode/webAsmPlay.git
$ cd webAsmPlay
$ mkdir buildEmscripten
$ cd buildEmscripten
$ emconfigure cmake ..
$ emmake make -j8
```

The native client:

``` Base
$ cd ..
$ mkdir buildNative
$ cd buildNative
$ cmake ..
$ make -j8
```

The server:

``` Base
cd ..
mkdir buildServer
cd buildServer
cmake ../GeoServer
make -j8
```

#### Running

Start the server:

``` Bash
cd ..
cd buildServer
./geoServer &
```

Run the web client:

``` Bash
cd ..
cd buildEmscripten
emrun --browser chrome index.html
```

// TODO The instructions are out of date!
You will need to move the the matrix panels out of the way and resize the
scene window panel. With the mouse in the scene window scrolling with the mouse
zooms in and out. Holding down the left shift key and moving the mouse rotates
the camera around the object. Holding down the left alt or option key pans the
camera on the XY plane.

Run the native client:

``` Bash
cd ..
cd buildNative
./webAsmPlay
```

### Notes

No notes.

### Ideas

* Create geometry summary for the current viewport. Pie charts representing the area of land use, surface type, surface color, etc
  Also include roads, paths, and other linear features in this.

* Building to other surface ratios, roads, paths, grass, water, etc

* Try to classify the viewport to different attributes such as social economics, political distribution, city type, etc.
  Will require training.

* Split data into different layers. Possible layers could be transportation, linear feature infrastructure, rivers, etc. The objective
  is to separate linear features from features with area. The layers will be rendered to different offscreen buffers allowing for image
  processing.
  
    * ~~One filter to test would be setting the transparency depending on the distance from the camera.~~
    * Another filter would be to set the transparency based on the density of the features of similar color local to the current screen pixel.
      This also might be a start at generalization.
    * Transparency shader based on feature area or length.
      
* ~~Interpolate line string color in relation to the distance to the camera.~~

* Auto parse shader source to determine uniforms and attributes.
    * ~~Or better, pass into shader name and location reference pairs.~~

* Determine topological relationships of the polygonal features in the GeoServer.
  This will allow associating OSM building parts and extruding them on top of their parent geometry.

* Implement movable clip planes.

### TODO

* Make geometric terminology consistent with Geos.
* Fix dmess for GeoServer
* ~~Bring Emscripten ImGUI up to date with native version~~.
* Present error if cannot connect to server
* Move to a newer geos. 3.8.0Dev seems to have more functions.
* ~~Make a third party directory.~~
* Make a project Talent Stack Document/Panel.
* ~~Remove fill and outline color arguments in Renderable, RenderablePolygon, etc.~~
* Make tessellation templated.
* Generate OpenGL vertex buffers in the GeoServer.
* Assign default values of class member variables in header file. Must be a C++ 11 or C++ 14 feature.
* All renderables should use the VertexArrayObject class. This will be a step in using multiple GPU interfaces such as Vulkan, Direct3D, or Metal
* Zombie bots need to avoid walking through building walls.
* Make a unified camera class.
* Move OpenSteer Vec3 objects to glm::dvec objects.
* Detect OSM building polygons which have the same base but different heights. Use only the one with the smallest height.
    * Investigate this some more.

### Bugs

* Holding down shift and moving outside the Geos test panel keeps it as if the shift key is still held down when the mouse is brought back in the panel.
* Camera still has a bug with window resizing and trackball. Seems to only happen in emscripten build.
* Upgraded OSX, glfw is having problems now :( Happened after upgrading to xcode 10
    * https://github.com/glfw/glfw/issues/1337
    * https://www.syntaxbomb.com/index.php?topic=4927.0



