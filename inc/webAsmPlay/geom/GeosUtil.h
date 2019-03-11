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

#include <vector>
#include <memory>
#include <initializer_list>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <geos/geom/Point.h>
#include <geos/geom/LineString.h>

namespace geos
{
    namespace geom
    {
        class Geometry;
        class Polygon;
    }
}

#define _SCOPED_GEOS_GEOMETRY_COMBINE1(X, Y) X##Y
#define _SCOPED_GEOS_GEOMETRY_COMBINE(X, Y) _SCOPED_GEOS_GEOMETRY_COMBINE1(X, Y)

#define scopedGeosGeometry(geom) geom; _ScopedGeosGeometry _SCOPED_GEOS_GEOMETRY_COMBINE(scopedGeosGeom, __LINE__)(geom);

class _ScopedGeosGeometry
{
public:

    _ScopedGeosGeometry(geos::geom::Geometry * geom);
    
    ~_ScopedGeosGeometry();
    
private:

    geos::geom::Geometry * geom;
};

namespace geosUtil
{
    // TODO Inline a these!

    geos::geom::Geometry::Ptr makeBox(const double xmin, const double ymin, const double xmax, const double ymax);
    
    geos::geom::Geometry::Ptr makeBox(const glm::dvec2 & min, const glm::dvec2 & max);

    geos::geom::Geometry::Ptr unionPolygons(const std::initializer_list<geos::geom::Geometry::Ptr> & polys);

    std::vector<const geos::geom::LineString *> getExternalRings(const geos::geom::Geometry * geom);

    std::vector<const geos::geom::LineString *> getExternalRings(const geos::geom::Geometry::Ptr & geom);

    void getExternalRings(std::vector<const geos::geom::LineString *> & rings, const geos::geom::Geometry * geom);

    std::vector<geos::geom::Geometry::Ptr> __(const std::vector<const geos::geom::LineString *> & lineStrings);

    geos::geom::Point * __(const glm::dvec2 & pos);

    inline geos::geom::Coordinate ___(const glm::dvec2 & pos) { return geos::geom::Coordinate(pos.x, pos.y) ;}

    glm::dvec2 __(const geos::geom::Coordinate & point);

    std::vector<glm::dvec2> __(const std::vector<geos::geom::Coordinate> & coords);

    std::vector<glm::dvec2> __(const std::vector<geos::geom::Coordinate> * coords);

    std::vector<glm::dvec2> __(const std::unique_ptr<std::vector<geos::geom::Coordinate> > & coords);

    geos::geom::Coordinate ___(const glm::dvec2 & point);

    inline glm::dvec4 __(const geos::geom::Coordinate & v, const double z, const double w)
    {
        return glm::dvec4(v.x, v.y, z, w);
    }

    bool contains(const std::unique_ptr<geos::geom::Geometry> & A, const std::unique_ptr<geos::geom::Geometry> & B);

    bool contains(const geos::geom::Geometry * A, const geos::geom::Geometry * B);

    std::unique_ptr<geos::geom::Geometry> difference(const std::unique_ptr<geos::geom::Geometry> & A, const std::unique_ptr<geos::geom::Geometry> & B);
    
    bool difference(const std::unique_ptr<geos::geom::Geometry> & A,
                    const std::unique_ptr<geos::geom::Geometry> & B,
                    std::unique_ptr<geos::geom::Geometry>       & out);

    bool subtract(  geos::geom::Geometry       *& A,
                    const geos::geom::Geometry * B);

    bool subtract(  std::unique_ptr<geos::geom::Geometry>       & A,
                    const std::unique_ptr<geos::geom::Geometry> & B);

    glm::dvec2 getStartPoint(const geos::geom::LineString * ls);
    glm::dvec2 getEndPoint  (const geos::geom::LineString * ls);

    void transformInPlace(std::vector<geos::geom::Coordinate> & points, const glm::dmat4 & trans);

    geos::geom::LineString * getLineString(const std::vector<glm::dvec2> & verts);

    inline geos::geom::Coordinate append2f(float *& dataStream, const geos::geom::Coordinate & v)
    {
        *dataStream = (float)v.x; ++dataStream;
        *dataStream = (float)v.y; ++dataStream;

        return v;
    }

    inline geos::geom::Coordinate append2d(std::vector<double> & data, const geos::geom::Coordinate & v)
    {
        data.push_back(v.x);
        data.push_back(v.y);

        return v;
    }
};
