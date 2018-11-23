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

#include <glm/gtx/normal.hpp>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/VertexArrayObject.h>

using namespace std;
using namespace glm;

VertexArrayObject * VertexArrayObject::create(const Tessellations & tessellations)
{
    if(tessellations[0]->getHeight() != 0.0) { return _create<true>(tessellations) ;}

    return _create<false>(tessellations);
}

namespace
{
    void addVert(   FloatVec & verts,
                    const vec3 & v,
                    const vec3 & n,
                    const float c)
    {
        verts.push_back(v.x);
        verts.push_back(v.y);
        verts.push_back(v.z);

        verts.push_back(n.x);
        verts.push_back(n.y);
        verts.push_back(n.z);

        verts.push_back(c);
    }
}

template<bool IS_3D>
VertexArrayObject * VertexArrayObject::_create(const Tessellations & tessellations)
{
    if(!tessellations.size()) { return NULL ;}

    FloatVec  verts;
    Uint32Vec triangleIndices;
    Uint32Vec counterVertIndices;
    Uint32Vec lineIndices;

    size_t offset = 0;

    size_t symbologyID_Stride;

    if(IS_3D) { symbologyID_Stride = 8 ;}
    else      { symbologyID_Stride = 4 ;}

    for(const auto & tess : tessellations)
    {
        // TODO try to remove hard coded values.
        const float symbologyID_value = (float(tess->symbologyID * symbologyID_Stride) + 0.5) / 32.0;

        const float symbologyWallID_value = (float(tess->symbologyID * symbologyID_Stride) + 0.5) / 32.0 + 4.0 / 32.0;

        for(size_t i = 0; i < tess->numVerts; ++i)
        {
            verts.push_back(tess->verts[i * 2 + 0]);
            verts.push_back(tess->verts[i * 2 + 1]);

            if(IS_3D)
            {
                verts.push_back(tess->height);

                verts.push_back(0);
                verts.push_back(0);
                verts.push_back(1);
            }

            verts.push_back(symbologyID_value);
        }

        for(size_t i = 0; i < tess->numTriangles * 3; ++i)
        {
            triangleIndices.push_back(tess->triangleIndices[i] + offset);
        }

        for(size_t i = 0; i < tess->numVerts; ++i)
        {
            lineIndices.push_back(i + offset);
            lineIndices.push_back((i + 1) % tess->numVerts + offset);
        }

        offset += tess->numVerts;

        if(!IS_3D) { continue ;}

        for(size_t a = 0; a < tess->numVerts; ++a)
        {
            const size_t b = (a + 1) % tess->numVerts;

            /*
            A   B
            p1  p2

            p4  p3
            */
            const vec2 A(tess->verts[a * 2], tess->verts[a * 2 + 1]);
            const vec2 B(tess->verts[b * 2], tess->verts[b * 2 + 1]);

            const vec3 p1(A, tess->height);
            const vec3 p2(B, tess->height);
            const vec3 p3(B, 0);
            const vec3 p4(A, 0);

            const vec3 normal = normalize(triangleNormal(p1, p2, p3));

            addVert(verts, p1, normal, symbologyWallID_value);
            addVert(verts, p2, normal, symbologyWallID_value);
            addVert(verts, p3, normal, symbologyWallID_value);
            addVert(verts, p4, normal, symbologyWallID_value);

            triangleIndices.push_back(offset + 0);
            triangleIndices.push_back(offset + 1);
            triangleIndices.push_back(offset + 2);

            triangleIndices.push_back(offset + 2);
            triangleIndices.push_back(offset + 3);
            triangleIndices.push_back(offset);

            offset += 4;
        }

#ifdef WORKING
        size_t prevIndexA;
        size_t prevIndexB;

        lineIndices.push_back(prevIndexA = offset++);
        lineIndices.push_back(prevIndexB = offset++);

        vec3 prevA(tess->verts[0], tess->verts[1], 0);
        
        const vec3 prevB(tess->verts[0], tess->verts[1], tess->height);

        const vec3 C(tess->verts[2], tess->verts[3], 0);

        //const vec3 normal = normalize(triangleNormal(prevA, prevB, C)) * vec3(-1,-1,-1);
        const vec3 normal = normalize(triangleNormal(prevA, prevB, C));
        //const vec3 normal(0,0,0);

        verts.push_back(prevA.x);
        verts.push_back(prevA.y);
        verts.push_back(prevA.z);

        verts.push_back(normal.x);
        verts.push_back(normal.y);
        verts.push_back(normal.z);

        verts.push_back(symbologyWallID_value);

        verts.push_back(prevB.x);
        verts.push_back(prevB.y);
        verts.push_back(prevB.z);

        verts.push_back(normal.x);
        verts.push_back(normal.y);
        verts.push_back(normal.z);

        verts.push_back(symbologyWallID_value);

        for(size_t i = 1; i <= tess->numVerts; ++i)
        {
            const size_t vertIndex = i % tess->numVerts;

            size_t indexA;
            size_t indexB;

            lineIndices.push_back(indexA = offset++);
            lineIndices.push_back(indexB = offset++);

            triangleIndices.push_back(indexA);
            triangleIndices.push_back(indexB);
            triangleIndices.push_back(prevIndexA);
            
            triangleIndices.push_back(prevIndexB);
            triangleIndices.push_back(indexB);
            triangleIndices.push_back(prevIndexA);

            prevIndexA = indexA;
            prevIndexB = indexB;

            const vec3 A(tess->verts[vertIndex * 2 + 0], tess->verts[vertIndex * 2 + 1], 0);
            const vec3 B(tess->verts[vertIndex * 2 + 0], tess->verts[vertIndex * 2 + 1], tess->height);

            //const vec3 normal = normalize(triangleNormal(A, B, prevA)) * vec3(-1,-1,-1);
            const vec3 normal = normalize(triangleNormal(A, B, prevA));
            //const vec3 normal(0,0,0);

            prevA = A;

            verts.push_back(A.x);
            verts.push_back(A.y);
            verts.push_back(A.z);

            verts.push_back(normal.x);
            verts.push_back(normal.y);
            verts.push_back(normal.z);

            verts.push_back(symbologyWallID_value);

            verts.push_back(B.x);
            verts.push_back(B.y);
            verts.push_back(B.z);

            verts.push_back(normal.x);
            verts.push_back(normal.y);
            verts.push_back(normal.z);

            verts.push_back(symbologyWallID_value);
        }
#endif
    }

    GLuint vao  = 0;
    GLuint ebo  = 0;
    GLuint ebo2 = 0;
    GLuint vbo  = 0;
    
    GL_CHECK(glGenVertexArrays(1, &vao));
    GL_CHECK(glBindVertexArray(    vao));

    GL_CHECK(glGenBuffers(1, &vbo));
    GL_CHECK(glGenBuffers(1, &ebo));
    GL_CHECK(glGenBuffers(1, &ebo2));

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));

    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), &verts[0], GL_STATIC_DRAW));
    
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * triangleIndices.size(), &triangleIndices[0], GL_STATIC_DRAW));
    
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * lineIndices.size(), &lineIndices[0], GL_STATIC_DRAW));

    return new VertexArrayObject(vao,
                                 ebo,
                                 ebo2,
                                 vbo,
                                 triangleIndices.size(),
                                 counterVertIndices,
                                 lineIndices.size(),
                                 tessellations.size() > 1);
}

VertexArrayObject::VertexArrayObject(   const GLuint      vao,
                                        const GLuint      ebo,
                                        const GLuint      ebo2,
                                        const GLuint      vbo,
                                        const GLuint      numTrianglesIndices,
                                        const Uint32Vec & counterVertIndices,
                                        const size_t      numContourLines,
                                        const bool        isMulti) : vao                (vao),
                                                                     ebo                (ebo),
                                                                     ebo2               (ebo2),
                                                                     vbo                (vbo),
                                                                     numTrianglesIndices(numTrianglesIndices),
                                                                     counterVertIndices (counterVertIndices),
                                                                     numContourLines    (numContourLines),
                                                                     _isMulti           (isMulti)
{

}

VertexArrayObject::~VertexArrayObject()
{
    GL_CHECK(glDeleteVertexArrays(1, &vao));
    GL_CHECK(glDeleteBuffers     (1, &vbo));
    GL_CHECK(glDeleteBuffers     (1, &ebo));
    GL_CHECK(glDeleteBuffers     (1, &ebo2));
}

void VertexArrayObject::bind() const
{
    GL_CHECK(glBindVertexArray(vao));
    
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));
}

void VertexArrayObject::bindTriangles() const
{
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
}

void VertexArrayObject::bindLines() const
{
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2));
}

void VertexArrayObject::drawTriangles() const
{
    GL_CHECK(glDrawElements(GL_TRIANGLES, numTrianglesIndices, GL_UNSIGNED_INT, NULL));
}

void VertexArrayObject::drawLines() const
{
    GL_CHECK(glDrawElements(GL_LINES, numContourLines, GL_UNSIGNED_INT, NULL));
}

bool VertexArrayObject::isMulti() const { return _isMulti ;}