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

#include <webAsmPlay/Debug.h>
#include <webAsmPlay/ColorSymbology.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/Shader.h>

using namespace std;
using namespace glm;

vector<function<void()>> Shader::s_shadersToRegister;

Shader::Shader(const string  & shaderName) :    m_shaderName     (shaderName),
                                                m_colorSymbology (ColorSymbology::getInstance("defaultPolygon")) {}

string Shader::getName() const { return m_shaderName ;}

size_t Shader::getNumRenderingStages() const { return 1 ;}

bool Shader::shouldRender(const bool isOutline, const size_t renderingStage) const { return renderingStage == 0 ;}

ColorSymbology * Shader::setColorSymbology(ColorSymbology * colorSymbology) { return m_colorSymbology = colorSymbology	;}
ColorSymbology * Shader::getColorSymbology() const							{ return m_colorSymbology					;}

RegisterShader::RegisterShader(const function<void()> & registerFunction)
{
	dmess("RegisterShader");

	Shader::s_shadersToRegister.push_back(registerFunction);
}

void Shader::ensureShaders()
{
	for (const auto& i : s_shadersToRegister) { i(); }
}