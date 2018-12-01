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

#include <webAsmPlay/Util.h>
#include <webAsmPlay/shaders/ColorDistanceShader.h>
#include <webAsmPlay/shaders/ColorSymbology.h>
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/GUI/GUI.h>

using namespace std;
using namespace glm;

namespace
{
    // 0 - default
    // 1 - house
    // 2 - building
    // 3 - grass

    const ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_NoInputs;

    void addSymbologyColorControls(const size_t colorIndex, const string & name, vec4 * colors)
    {
        ColorDistanceShader * shader = ColorDistanceShader::getDefaultInstance();

        if(ImGui::ColorEdit4((name + " fill near")   .c_str(), value_ptr(colors[0]), flags)) { shader->setColor(colorIndex * 4 + 0, colors[0]) ;}
        if(ImGui::ColorEdit4((name + " fill far")    .c_str(), value_ptr(colors[1]), flags)) { shader->setColor(colorIndex * 4 + 1, colors[1]) ;}
        if(ImGui::ColorEdit4((name + " outline near").c_str(), value_ptr(colors[2]), flags)) { shader->setColor(colorIndex * 4 + 2, colors[2]) ;}
        if(ImGui::ColorEdit4((name + " outline far") .c_str(), value_ptr(colors[3]), flags)) { shader->setColor(colorIndex * 4 + 3, colors[3]) ;}

        ImGui::Separator();
    }

    void addMeshSymbologyColorControls(const size_t colorIndex, const string & name, vec4 * colors)
    {
        ColorSymbology * symbology = ColorSymbology::getDefaultInstance();

        if(ImGui::ColorEdit4((name + " roof near")        .c_str(), value_ptr(colors[0]), flags)) { symbology->setColor(colorIndex * 8 + 0, colors[0]) ;}
        if(ImGui::ColorEdit4((name + " roof outline near").c_str(), value_ptr(colors[1]), flags)) { symbology->setColor(colorIndex * 8 + 1, colors[1]) ;}
        if(ImGui::ColorEdit4((name + " roof far")         .c_str(), value_ptr(colors[2]), flags)) { symbology->setColor(colorIndex * 8 + 2, colors[2]) ;}
        if(ImGui::ColorEdit4((name + " roof outline far") .c_str(), value_ptr(colors[3]), flags)) { symbology->setColor(colorIndex * 8 + 3, colors[3]) ;}
        if(ImGui::ColorEdit4((name + " wall near")        .c_str(), value_ptr(colors[4]), flags)) { symbology->setColor(colorIndex * 8 + 4, colors[4]) ;}
        if(ImGui::ColorEdit4((name + " wall outline near").c_str(), value_ptr(colors[5]), flags)) { symbology->setColor(colorIndex * 8 + 5, colors[5]) ;}
        if(ImGui::ColorEdit4((name + " wall far")         .c_str(), value_ptr(colors[6]), flags)) { symbology->setColor(colorIndex * 8 + 6, colors[6]) ;}
        if(ImGui::ColorEdit4((name + " wall outline far") .c_str(), value_ptr(colors[7]), flags)) { symbology->setColor(colorIndex * 8 + 7, colors[7]) ;}
    }
}

void GUI::symbologyPanel()
{
    if(!showSymbologyPanel) { return ;}

    ImGui::Begin("Symbology", &showSymbologyPanel);

    if (ImGui::CollapsingHeader("Linear Features", ImGuiTreeNodeFlags_DefaultOpen))
    {
        
    }

    if (ImGui::CollapsingHeader("Polygonal features", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ColorDistanceShader * shader = ColorDistanceShader::getDefaultInstance();

        addSymbologyColorControls(0, "Default",  &shader->getColorRef(0 * 4 ));
        addSymbologyColorControls(1, "Grass",    &shader->getColorRef(1 * 4 ));
        addSymbologyColorControls(2, "water",    &shader->getColorRef(2 * 4 ));
    }

    if (ImGui::CollapsingHeader("Mesh features", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ColorSymbology * symbology = ColorSymbology::getDefaultInstance();

        addMeshSymbologyColorControls(0, "House",    &symbology->getColorRef(0 * 8));
        addMeshSymbologyColorControls(1, "Building", &symbology->getColorRef(1 * 8));
    }

    ImGui::End();
}