/**
 ╭━━━━╮╱╱╱╱╱╱╱╱╱╭╮╱╭━━━╮╱╱╱╱╱╱╭╮
 ┃╭╮╭╮┃╱╱╱╱╱╱╱╱╱┃┃╱┃╭━╮┃╱╱╱╱╱╱┃┃
 ╰╯┃┃╰╯╭━╮╭━━╮╭╮┃┃╱┃┃╱╰╯╭━━╮╭━╯┃╭━━╮
 ╱╱┃┃╱╱┃╭╯┃╭╮┃┣┫┃┃╱┃┃╱╭╮┃╭╮┃┃╭╮┃┃┃━┫
 ╱╱┃┃╱╱┃┃╱┃╭╮┃┃┃┃╰╮┃╰━╯┃┃╰╯┃┃╰╯┃┃┃━┫
 ╱╱╰╯╱╱╰╯╱╰╯╰╯╰╯╰━╯╰━━━╯╰━━╯╰━━╯╰━━╯
 // This software is provided 'as-is', without any express or implied
 // warranty.  In no event will the authors be held liable for any damages
 // arising from the use of this software.
 // Permission is granted to anyone to use this software for any purpose,
 // including commercial applications, and to alter it and redistribute it
 // freely, subject to the following restrictions:
 // 1. The origin of this software must not be misrepresented; you must not
 //    claim that you wrote the original software. If you use this software
 //    in a product, an acknowledgment in the product documentation would be
 //    appreciated but is not required.
 // 2. Altered source versions must be plainly marked as such, and must not be
 //    misrepresented as being the original software.
 // 3. This notice may not be removed or altered from any source distribution.

  \author Matthew Tang
  \email trailcode@gmail.com
  \copyright 2018
*/

#include <unordered_set>
#include <thread>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <OpenSteer/PolylineSegmentedPathwaySingleRadius.h>
#include <OpenSteer/OpenSteerDemo.h>
#include <OpenSteer/Draw.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Camera.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/ZombiePlugin.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <webAsmPlay/renderables/DeferredRenderable.h>
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/OpenSteerGlue.h>

#ifdef max
#undef max
#endif

using namespace std;
using namespace glm;
using namespace OpenSteer;
using namespace geosUtil;

void openSteerDisplayFunc();

thread * openSteerThread = NULL; // TODO make a thread waiting collection.

namespace
{
    const double scaleValue = 1.0 / (60.0 * 2.0);

    const dmat4 geomTrans(scale(dmat4(1.0), dvec3(scaleValue, scaleValue, scaleValue)));

    const dmat4 geomInverseTrans(inverse(geomTrans));

    unique_ptr<Renderable> openSteerGeom;

    mutex openSteerMutex;
}

extern bool gotoNextZombie; // TODO Un-globalize

namespace
{
    vec4 lookat;
    vec4 pos;   
    vec4 up;

    void updateOpenSteerCamera()
    {
        const vec3 scale(0.1, 0.1, 0.1);
            
        const dmat4 rotate = glm::rotate(radians(-90.0), dvec3(1, 0, 0));

        lookat = rotate * geomTrans * vec4(__(OpenSteerDemo::camera.target)     * scale, 1);
        pos    = rotate * geomTrans * vec4(__(OpenSteerDemo::camera.position()) * scale, 1);
        pos.z *= -1;
        up     = rotate * vec4(__(OpenSteer::OpenSteerDemo::camera.up()), 1);
        up.z *= -1;

        pos = vec4(vec3(lookat) + normalize(vec3(pos) - vec3(lookat)) * GUI::openSteerCameraDist, 1);

        pos.z = glm::max(float(GUI::openSteerCameraDist * 0.5f), float(pos.z)); // When switching camera to next Zombie, keep camera above ground.
    }

    void updateOpenSteer()
    {
		//return;

        if(gotoNextZombie)
        {
            dmess("gotoNextZombie");

            gotoNextZombie = false;

            OpenSteerDemo::selectNextVehicle();
        }

        OpenSteerDemo::updateSimulation();

		//lock_guard<mutex> _(openSteerMutex);

		//updateOpenSteerCamera();

		//OpenSteerDemo::redraw();

		/*
        lock_guard<mutex> _(openSteerMutex);

        updateOpenSteerCamera();

        OpenSteerDemo::redraw();

        openSteerGeom = unique_ptr<Renderable>(DeferredRenderable::createFromQueued(geomTrans));
		*/
    }
}

void OpenSteerGlue::init(Canvas * canvas, Network * network)
{
    dmess("OpenSteerGlue::init");

    ZombiePlugin::setNetwork(network);

	//return;

#ifndef __EMSCRIPTEN__
//#if 0

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

	GLFWwindow* threadWin = glfwCreateWindow(1, 1, "Thread Window", NULL, GUI::getMainWindow());

	OpenSteerDemo::initialize();

    openSteerThread = new thread([threadWin]()
    {
		//return;

        glfwMakeContextCurrent(threadWin);

		/*
		if (glewInit())
		{
			dmess("failed to initialize OpenGL\n");

			exit(-1);
		}
		*/

        //gl3wInit();
		/*
		if (gl3wInit())
		{
			dmess("failed to initialize OpenGL\n");

			exit(-1);
		}
		//*/
		//gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

        dmess("Start OpenSteerDemo::initialize()");

        //OpenSteerDemo::initialize();

        dmess("Done OpenSteerDemo::initialize()");

        for(; !GUI::isShuttingDown() ;) { updateOpenSteer() ;}

        // TODO cleanup threadWin!
    });

#else

    OpenSteerDemo::initialize();

#endif

	//OpenSteerDemo::initialize();


    dmess("Done OpenSteer::OpenSteerDemo::initialize();");

    GUI::addUpdatable([canvas]()
    {
		//return;

        lock_guard<mutex> _(openSteerMutex);

#ifdef __EMSCRIPTEN__
//#if 1

        updateOpenSteer();

#endif

		//lock_guard<mutex> _(openSteerMutex);

		updateOpenSteerCamera();

		OpenSteerDemo::redraw();

		//openSteerGeom = unique_ptr<Renderable>(DeferredRenderable::createFromQueued(geomTrans));

		//*
		if (!openSteerGeom) { openSteerGeom = unique_ptr<Renderable>(DeferredRenderable::createFromQueued(geomTrans)); }
		else
		{
			((DeferredRenderable*)openSteerGeom.get())->setFromQueued(geomTrans);
		}
		//*/

        if(GUI::getCameraMode() == GUI::CAMERA_FOLLOW_ENTITY)
        {
            // TODO this will be one frame behind!
            canvas->getCamera()->setCenter(lookat);
            canvas->getCamera()->setEye   (pos);
            canvas->getCamera()->setUp    (up);

            canvas->getCamera()->update();
        }

        if(openSteerGeom) { openSteerGeom->render(canvas) ;}
    });
}

const dmat4 & OpenSteerGlue::getGeomTrans()         { return geomTrans ;}
const dmat4 & OpenSteerGlue::getGeomInverseTrans()  { return geomInverseTrans ;}

PolylineSegmentedPathwaySingleRadius * OpenSteerGlue::getPath(const vector<dvec2> & path)
{
	if (path.size() < 3)
	{
		dmess("Error!!!");
	}

    //const float pathRadius = 2.0;
    const float pathRadius = 1.0;

    vector<Vec3> points;

    points.reserve(path.size());

    unordered_set<dvec3> seen;

    for(const auto & i : path)
    {
        const dvec3 pos(geomInverseTrans * dvec4(i, 0, 1));
        
        if(seen.find(pos) != seen.end())
        {
            //dmess("Seen!");

            continue;
        }

        seen.insert(pos);

        // TODO Where is this * 10 coming from?
        points.push_back(Vec3(pos.x * 10.0, 0, pos.y * 10.0));
    }

	if (points.size() < 3)
	{
		dmess("Error!!!");
	}

    return new PolylineSegmentedPathwaySingleRadius(points.size(),
                                                    &points[0],
                                                    pathRadius,
                                                    false);
}