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

#include <cmath>
//#include <imgui.h>

#ifdef __EMSCRIPTEN__
    // GLEW
    #define GLEW_STATIC
    #include <GL/glew.h>
    #define IMGUI_API
    #include <imgui.h>
    #include <imgui_impl_glfw_gl3.h>
    #include <emscripten/emscripten.h>
    #include <emscripten/bind.h>
#else
    #include <GL/gl3w.h>    // Initialize with gl3wInit()
    #define IMGUI_IMPL_API
    #include <imgui/imgui.h>
    #include <imgui_impl_opengl3.h>
    #include <imgui_impl_glfw.h>
    
#endif // __EMSCRIPTEN__

#include <GLFW/glfw3.h>
#include <imgui_internal.h>
#include <iostream>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Point.h>
#include <geos/simplify/TopologyPreservingSimplifier.h>
#include <geos/simplify/DouglasPeuckerSimplifier.h>
#include "../GLUTesselator/include/GLU/tessellate.h"
#include <webAsmPlay/TrackBallInteractor.h>
#include <webAsmPlay/Camera.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Renderable.h>
#include <webAsmPlay/Attributes.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imguitoolbar.h>
#include <tceGeom/vec2.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/RenderableCollection.h>
#include <webAsmPlay/RenderablePolygon.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/SkyBox.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/GridPlane.h>
#include <webAsmPlay/GeoClient.h>
#include <webAsmPlay/GeosTestCanvas.h>
#include <webAsmPlay/GeosUtil.h>

using namespace std;
using namespace geos::geom;
using namespace geos::simplify;
using namespace rsmz;
using namespace glm;
using namespace tce::geom;

#define ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

static ImVec4 clear_color = ImColor(114, 144, 154);
static int mouse_buttons_down = 0;

static bool mouse_buttons[GLFW_MOUSE_BUTTON_LAST + 1] = { false, };

GeosTestCanvas  * geosTestCanvas = NULL;
Canvas          * canvas         = NULL;
SkyBox          * skyBox         = NULL;

bool showViewMatrixPanel     = false;
bool showMVP_MatrixPanel     = false;
bool showSceneViewPanel      = false;
bool showPerformancePanel    = false;
bool showRenderSettingsPanel = false;
bool showLogPanel            = false;
bool showAttributePanel      = false;

bool isFirst = true;

FrameBuffer * frameBuffer = NULL;

void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

void refresh(GLFWwindow* window)
{
#ifdef __EMSCRIPTEN__
    glfwPollEvents();

    glfwMarkWindowForRefresh(window);
#else

    glfwPostEmptyEvent();

#endif
}

int counter = 0;

void cback(char* data, int size, void* arg) {
    std::cout << "Callback " << data << " " << size << std::endl;
    counter++;
}

ImGuiTextBuffer * Buf = NULL;

uint32_t infoIcon = 0;

struct AppLog
{
    //ImGuiTextBuffer     Buf;
    bool                ScrollToBottom;

    void    Clear()     { Buf->clear(); }

    void    AddLog(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        //Buf.appendv(fmt, args);
        //Buf.appendf("%s", "fdasdfasd");
        va_end(args);
        ScrollToBottom = true;
    }

    void    Draw(const char* title, bool* p_opened = NULL)
    {
        ImGui::SetNextWindowSize(ImVec2(500,400), ImGuiSetCond_FirstUseEver);
        ImGui::Begin(title, p_opened);
        if (ImGui::Button("Clear")) Clear();
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::Separator();
        ImGui::BeginChild("scrolling");
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,1));
        if (copy) ImGui::LogToClipboard();
        ImGui::TextUnformatted(Buf->begin());
        if (ScrollToBottom)
            ImGui::SetScrollHere(1.0f);
        ScrollToBottom = false;
        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::End();
    }
};

static void showCursorPositionOverlay(bool* p_open, const dvec4 & cursorPos)
{
    const float DISTANCE = 10.0f;
    static int corner = 2;
    ImVec2 window_pos = ImVec2((corner & 1) ? ImGui::GetIO().DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? ImGui::GetIO().DisplaySize.y - DISTANCE : DISTANCE);
    ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
    if (corner != -1)
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
    if (ImGui::Begin("Cursor Position", p_open, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
    {
        /*
        if (ImGui::IsMousePosValid())
            ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
        else
            ImGui::Text("Mouse Position: <invalid>");
        */
        ImGui::Text("Pos: (%.5f,%.5f)", cursorPos.x, cursorPos.y);

        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Custom",       NULL, corner == -1)) corner = -1;
            if (ImGui::MenuItem("Top-left",     NULL, corner == 0)) corner = 0;
            if (ImGui::MenuItem("Top-right",    NULL, corner == 1)) corner = 1;
            if (ImGui::MenuItem("Bottom-left",  NULL, corner == 2)) corner = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
            if (p_open && ImGui::MenuItem("Close")) *p_open = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

AppLog logPanel;

void dmessCallback(const string & file, const size_t line, const string & message)
{
    cout << file << " " << line << " " << message;
    /*
#ifdef __EMSCRIPTEN__
    if(Buf) { Buf->append("%s %i %s", file.c_str(), (int)line, message.c_str()) ;}
#else
    if(Buf) { Buf->appendf("%s %i %s", file.c_str(), (int)line, message.c_str()) ;}
#endif
     */
}

//vector<Renderable *> pickedRenderiables;

//extern void (*debugLoggerFunc)(const std::string & file, const std::string & line, const std::string & message);

GeoClient * client = NULL;

enum
{
    NORMAL_MODE = 0,
    PICK_MODE_SINGLE,
    PICK_MODE_MULTIPLE,
};

static char mode = NORMAL_MODE;

void mainLoop(GLFWwindow * window)
{
    if(!Buf) {  Buf = new ImGuiTextBuffer() ;}
    // Game loop
    
#ifdef __EMSCRIPTEN__
    // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
    glfwPollEvents();

    ImGui_ImplGlfwGL3_NewFrame();
#endif


    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New")) {}
                if (ImGui::MenuItem("Open", "Ctrl+O")) {

                }
                if(ImGui::MenuItem("Test Web Worker"))
                {
                    #ifdef __EMSCRIPTEN__
                        worker_handle worker = emscripten_create_worker("worker.js");
                        emscripten_call_worker(worker, "one", 0, 0, cback, (void*)42);
                    #else
                        dmess("Implement me!");
                    #endif
                }

                if(ImGui::MenuItem("Load Geometry"))
                {
                    //GeoClient::getInstance()->loadGeometry(canvas);

                    if(!client) { client = new GeoClient(window) ;}

                    client->loadAllGeometry(canvas);
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
                if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
                ImGui::Separator();
                if (ImGui::MenuItem("Cut", "CTRL+X")) {}
                if (ImGui::MenuItem("Copy", "CTRL+C")) {}
                if (ImGui::MenuItem("Paste", "CTRL+V")) {}
                ImGui::EndMenu();
            }

            if(ImGui::BeginMenu("View"))
            {
                if(ImGui::MenuItem("View Matrix"))     { showViewMatrixPanel     = !showViewMatrixPanel     ;}
                if(ImGui::MenuItem("MVP Matrix"))      { showMVP_MatrixPanel     = !showMVP_MatrixPanel     ;}
                if(ImGui::MenuItem("Geos Tests"))      { showSceneViewPanel      = !showSceneViewPanel      ;}
                if(ImGui::MenuItem("Performance"))     { showPerformancePanel    = !showPerformancePanel    ;}
                if(ImGui::MenuItem("Render Settings")) { showRenderSettingsPanel = !showRenderSettingsPanel ;}
                if(ImGui::MenuItem("Log"))             { showLogPanel            = !showLogPanel            ;}
                if(ImGui::MenuItem("Attributes"))      { showAttributePanel      = !showAttributePanel      ;}

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }

    {
        static ImGui::Toolbar toolbar("myFirstToolbar##foo");
        if (toolbar.getNumButtons()==0)
        {
            ImVec2 uv0(0,0);
            ImVec2 uv1(1,1);
            //ImVec2 size(16,16);
            ImVec2 size(32,32);
            toolbar.addButton(ImGui::Toolbutton("Normal Mode",(void*)infoIcon,uv0,uv1,size));
            toolbar.addButton(ImGui::Toolbutton("Get Info Mode",(void*)infoIcon,uv0,uv1,size));
            toolbar.addButton(ImGui::Toolbutton("Get Info Mode Multiple",(void*)infoIcon,uv0,uv1,size));

            toolbar.setProperties(false,false,true,ImVec2(0.5f,0.f));

            //toolbar.setScaling(0.5f,0.5f);
        }
        
        const int pressed = toolbar.render();
        if (pressed>=0) fprintf(stderr,"Toolbar1: pressed:%d\n",pressed);
        switch(pressed)
        {
            case 0: mode = NORMAL_MODE; break;
            case 1: mode = PICK_MODE_SINGLE; break;
            case 2: mode = PICK_MODE_MULTIPLE; break;
        }
        //dmess("infoIcon " << infoIcon);
    }

    // Rendering
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    glViewport(0, 0, screenWidth, screenHeight);
    
    static float time = 0.f;
    
    time += ImGui::GetIO().DeltaTime;

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    if(showPerformancePanel)
    {
        ImGui::Begin("Performance", &showPerformancePanel);

            static float f = 0.0f;
            static float frameTimes[100] = {0.f};
            memcpy(&frameTimes[0], &frameTimes[1], sizeof(frameTimes) - sizeof(frameTimes[0]));
            frameTimes[ARRAYSIZE(frameTimes) - 1] = ImGui::GetIO().Framerate;
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::PlotLines("Frame History", frameTimes, ARRAYSIZE(frameTimes), 0, "", 0.0f, 100.0f, ImVec2(0, 50));

        ImGui::End();
    }

    if(showViewMatrixPanel)
    {
        ImGui::Begin("View Matrix", &showViewMatrixPanel);

            ImGui::Text(mat4ToStr(canvas->getViewRef()).c_str());

        ImGui::End();
    }

    if(isFirst) { ImGui::SetNextWindowPos(ImVec2(0,0)) ;}

    if(showMVP_MatrixPanel)
    {
        ImGui::Begin("MVP Matrix", &showMVP_MatrixPanel);

            ImGui::Text(mat4ToStr(canvas->getMVP_Ref()).c_str());

        ImGui::End();
    }

    isFirst = false;

    if(showRenderSettingsPanel)
    {
        ImGui::Begin("Render Settings", &showRenderSettingsPanel);

            static bool fillPolygons            = true;
            static bool renderPolygonOutlines   = true;
            static bool renderSkyBox            = true;

            static bool _fillPolygons           = true;
            static bool _renderPolygonOutlines  = true;
            static bool _renderSkyBox           = true;

            ImGui::Checkbox("Fill Polygons",    &_fillPolygons);
            ImGui::Checkbox("Polygon Outlines", &_renderPolygonOutlines);
            ImGui::Checkbox("SkyBox",           &_renderSkyBox);

            if(fillPolygons != _fillPolygons)
            {
                fillPolygons = _fillPolygons;

                for(Renderable * r : canvas->getRenderiablesRef()) { r->setRenderFill(fillPolygons) ;}

                for(Renderable * r : geosTestCanvas->getRenderiablesRef()) { r->setRenderFill(fillPolygons) ;}

                Renderable::setDefaultRenderFill(fillPolygons);
            }

            if(renderPolygonOutlines != _renderPolygonOutlines)
            {
                renderPolygonOutlines = _renderPolygonOutlines;

                for(Renderable * r : canvas->getRenderiablesRef()) { r->setRenderOutline(renderPolygonOutlines) ;}

                for(Renderable * r : geosTestCanvas->getRenderiablesRef()) { r->setRenderOutline(renderPolygonOutlines) ;}

                Renderable::setDefaultRenderOutline(renderPolygonOutlines);
            }

            if(renderSkyBox != _renderSkyBox)
            {
                renderSkyBox = _renderSkyBox;

                if(renderSkyBox) { canvas->setSkyBox(skyBox) ;}
                else             { canvas->setSkyBox(NULL)   ;}
            }

            static vec4 fillColor(Renderable::getDefaultFillColor());
            static vec4 outlineColor(Renderable::getDefaultOutlineColor());
            
            ImGui::ColorEdit4("Fill", (float*)&fillColor, true);
            ImGui::ColorEdit4("Outline", (float*)&outlineColor, true);

            /*
            static vec4 fillColor(Renderable::getDefaultFillColor());

            ImGui::ColorPicker4("##picker",
                                (float*)&fillColor,
                                //ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
                                0);
                                */

        ImGui::End();
    }
    
    if(showLogPanel) { logPanel.Draw("Log", &showLogPanel) ;}

    canvas->render();
    
    string attrsStr;

    if(client)
    { // TODO move this code to GeoClient

        const dvec4 pos(canvas->getCursorPosWC(), 1.0);

        showCursorPositionOverlay(NULL, client->getInverseTrans() * pos);

        if(mode == PICK_MODE_SINGLE)
        {
            Renderable * renderiable;
            Attributes * attrs;

            tie(renderiable, attrs) = client->pickRenderable(canvas->getCursorPosWC());

            if(renderiable)
            {
                renderiable->render(canvas->getMVP_Ref());

                attrsStr = attrs->toString();
            }
        }
        else if(mode == PICK_MODE_MULTIPLE)
        {
            vector<pair<Renderable *, Attributes *> > picked = client->pickRenderables(canvas->getCursorPosWC());

            if(picked.size())
            {
                Renderable * renderiable;
                Attributes * attrs;

                tie(renderiable, attrs) = picked[0];

                renderiable->render(canvas->getMVP_Ref());

                attrsStr = attrs->toString();

                for(size_t i = 1; i < picked.size(); ++i)
                {
                    attrsStr += "\n";

                    attrsStr += get<1>(picked[i])->toString();
                }
            }
        }
    }

    if(showAttributePanel)
    {
        ImGui::Begin("Attributes", &showAttributePanel);

            ImGui::Text(attrsStr.c_str());

        ImGui::End();
    }

    geosTestCanvas->setEnabled(showSceneViewPanel);

    if(showSceneViewPanel)
    {
        ImGui::Begin("Geos Tests", &showSceneViewPanel);

            const ImVec2 pos = ImGui::GetCursorScreenPos();

            const ImVec2 sceneWindowSize = ImGui::GetWindowSize();

            geosTestCanvas->setArea(__(pos), __(sceneWindowSize));

            geosTestCanvas->setWantMouseCapture(GImGui->IO.WantCaptureMouse);

            ImGui::GetWindowDrawList()->AddImage(   (void *)geosTestCanvas->render(),
                                                    pos,
                                                    ImVec2(pos.x + sceneWindowSize.x, pos.y + sceneWindowSize.y),
                                                    ImVec2(0, 1),
                                                    ImVec2(1, 0));
            
            static float buffer1 = 0.1;
            static float buffer2 = 0.02;
            static float buffer3 = 0.22;

            ImGui::SliderFloat("buffer1", &buffer1, 0.0f, 0.3f, "buffer1 = %.3f");
            ImGui::SliderFloat("buffer2", &buffer2, 0.0f, 0.3f, "buffer2 = %.3f");
            ImGui::SliderFloat("buffer3", &buffer3, 0.0f, 0.3f, "buffer3 = %.3f");

            geosTestCanvas->setGeomParameters(buffer1, buffer2, buffer3);

        ImGui::End();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ImGui::Render();

#ifndef __EMSCRIPTEN__

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

#endif
    glfwMakeContextCurrent(window);

    glfwSwapBuffers(window);
}

void mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
    //dmess("mouseButtonCallback button: " << button << " action " << action << " mods " << mods);
    
    geosTestCanvas->onMouseButton(window, button, action, mods);

    if(!GImGui->IO.WantCaptureMouse)
    {
        canvas->onMouseButton(window, button, action, mods);
    }

    if (mouse_buttons[button] != action) {
      mouse_buttons[button] = action;
      mouse_buttons_down += action == GLFW_PRESS ? 1 : -1;
    }

#ifndef __EMSCRIPTEN__
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
#else
    ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
#endif

    refresh(window);
}

Vec2i lastShiftKeyDownMousePos;

void cursorPosCallback(GLFWwindow * window, double xpos, double ypos)
{
    //dmess("x " << xpos << " y " << ypos);

    geosTestCanvas->onMousePosition(window, Vec2d(xpos, ypos));

    canvas->onMousePosition(window, Vec2d(xpos, ypos));

    refresh(window);
}

void scrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{
    //dmess("ScrollCallback " << xoffset << " " << yoffset);

    geosTestCanvas->onMouseScroll(window, Vec2d(xoffset, yoffset));

    if(!GImGui->IO.WantCaptureMouse)
    {
        canvas->onMouseScroll(window, Vec2d(xoffset, yoffset));
    }

#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfwGL3_ScrollCallback(window, xoffset, yoffset);
#else
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
#endif
    refresh(window);
}

void keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    geosTestCanvas->onKey(window, key, scancode, action, mods);

    if(!GImGui->IO.WantCaptureKeyboard) { canvas->onKey(window, key, scancode, action, mods) ;}
 
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
#else
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
#endif

    refresh(window);
}

void charCallback(GLFWwindow * window, unsigned int c)
{
    geosTestCanvas->onChar(window, c);

    canvas->onChar(window, c);

#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfwGL3_CharCallback(window, c);
#else
    ImGui_ImplGlfw_CharCallback(window, c);
#endif
    refresh(window);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Need to use this to get true size because of retina displays.
    glfwGetWindowSize(window, &width, &height);

    canvas->setArea(Vec2i(0,0), Vec2i(width, height));

    refresh(window);
}

void windowFocusCallback(GLFWwindow* window, int focused)
{
    if(focused)
    {
        refresh(window);
    }
}

void cursorEnterCallback(GLFWwindow * window, int /* entered */)
{
    refresh(window);
}

// Is called whenever a key is pressed/released via GLFW
void keyCallback1(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) { glfwSetWindowShouldClose(window, GL_TRUE) ;}
}

void initOpenGL(GLFWwindow* window)
{
    debugLoggerFunc = &dmessCallback;

    infoIcon = Textures::load("if_Info_131908.png");
    //if(!infoIcon) { infoIcon = Textures::load("if_Info_131908.png") ;}
    //if(!infoIcon) { infoIcon = Textures::load("xpos.png") ;}

    // Define the viewport dimensions
    static int width, height;
    //glfwGetFramebufferSize(window, &width, &height); 
    glfwGetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);

    Renderable::ensureShader();
    //GridPlane  ::ensureShader();
    RenderablePolygon::ensureShaders();

    canvas = new Canvas(false);

    dmess("width " << width << " height " << height);

    //canvas->setArea(Vec2i(0,0), Vec2i(width / 2, height / 2));
    canvas->setArea(Vec2i(0,0), Vec2i(width, height));

    geosTestCanvas = new GeosTestCanvas();

    skyBox = new SkyBox();

    canvas->setSkyBox(skyBox);

    //GridPlane * gridPlane = new GridPlane();

    //canvas->addRenderiable(gridPlane);
}

void initGeometry()
{
    #ifdef WORKING

    Geometry * pp = scopedGeosGeometry(GeosUtil::makeBox(-0.1,-0.1,0.1,0.1));

    Geometry * p = scopedGeosGeometry(GeosUtil::makeBox(-0.5,-0.5,0.5,0.5));

    Geometry * ppp = scopedGeosGeometry(GeosUtil::makeBox(-0.05,-0.6,0.05,0.6));

    Geometry * pppp = scopedGeosGeometry(GeosUtil::makeBox(-0.6,-0.05,0.6,0.05));

    p = scopedGeosGeometry(p->buffer(0.1));
    //*

    p = scopedGeosGeometry(p->difference(pp));

    p = scopedGeosGeometry(p->difference(ppp));

    p = scopedGeosGeometry(p->difference(pppp));
    //*/

    const mat4 trans = scale(mat4(1.0), vec3(0.6, 0.6, 0.6));

    Renderable * r = Renderable::create(p, trans);
    //Renderable * r = Renderable::create(dynamic_cast<Polygon *>(p)->getExteriorRing());

    r->setFillColor(vec4(0.3,0.3,0,1));
        
    r->setOutlineColor(vec4(1,0,0,1));

    //geosTestCanvas->addRenderiable(r);
    canvas->addRenderiable(r);

    /*
    p = scopedGeosGeometry(GeosUtil::makeBox(-0.5,-0.5,-4,0.4));

    //r = Renderable::create(p, trans);
    r = Renderable::create(p);

    r->setFillColor(vec4(0.3,0.0,0.3,1));
        
    r->setOutlineColor(vec4(1,0,0,1));

    canvas->addRenderiable(r);
    //*/

    #endif
}
