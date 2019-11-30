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
#pragma once

#include <glm/vec4.hpp>
#include <JSONValue.h>
#include <nlohmann/json.hpp>
#include <webAsmPlay/OpenGL_Util.h>

class ColorSymbology
{
public:

    static ColorSymbology * getInstance(const std::string & name);

    GLuint getTextureID();

    glm::vec4   setColor	(const size_t index, const glm::vec4 & color);
    glm::vec4   getColor	(const size_t index);
    glm::vec4 & getColorRef	(const size_t index);

    void loadState(const JSONObject & dataStore);

    void saveState(JSONObject & dataStore);

	void loadState(const nlohmann::json & state);

    void saveState(nlohmann::json & state) const;

private:

    ColorSymbology(const std::string & name);
    ~ColorSymbology();

    const std::string m_name;

    GLuint m_colorTexture = 0;

    glm::vec4 m_colors[32];

    bool m_colorTextureDirty = true;
};
