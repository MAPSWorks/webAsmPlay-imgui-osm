//#include <geos/geom/Envelope.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/LineString.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Shader.h>
#include <webAsmPlay/RenderiableLineString2D.h>
#include <webAsmPlay/RenderiablePolygon2D.h>
#include <webAsmPlay/Renderiable.h>

using namespace std;
using namespace glm;
using namespace geos::geom;

Shader * Renderiable::defaultShader = NULL;

void Renderiable::ensureShader()
{
    if(defaultShader) { return ;}
    
    // Shader sources
    const GLchar* vertexSource = R"glsl(#version 330 core
        in vec2 position;
        out vec4 vertexColor;
        uniform mat4 MVP;
        uniform vec4 vertexColorIn;

        void main()
        {
            gl_Position = MVP * vec4(position.xy, 0, 1);
            vertexColor = vertexColorIn;
        }
    )glsl";

    const GLchar* fragmentSource = R"glsl(#version 330 core
        out vec4 outColor;
        in vec4 vertexColor;

        void main()
        {
            outColor = vertexColor;
        }
    )glsl";

    defaultShader = Shader::create(vertexSource, fragmentSource);
}

Renderiable * Renderiable::create(const Geometry * geom, const mat4 & trans)
{
    switch(geom->getGeometryTypeId())
    {
        case GEOS_POINT:                dmess("Implement me!"); return NULL;
        case GEOS_LINESTRING:           dmess("Implement me!"); return NULL;
        case GEOS_LINEARRING:           return RenderiableLineString2D  ::create(dynamic_cast<const LineString *>   (geom), trans);
        case GEOS_POLYGON:              return RenderiablePolygon2D     ::create(dynamic_cast<const Polygon *>      (geom), trans);
        case GEOS_MULTIPOINT:           dmess("Implement me!"); return NULL;
        case GEOS_MULTILINESTRING:      dmess("Implement me!"); return NULL;
        case GEOS_MULTIPOLYGON:         return RenderiablePolygon2D     ::create(dynamic_cast<const MultiPolygon *> (geom), trans);
        case GEOS_GEOMETRYCOLLECTION:   dmess("Implement me!"); return NULL;
        default:
            dmess("Error!");
            abort();
    }

    return NULL;
}

Renderiable::Renderiable()
{
}

Renderiable::~Renderiable()
{
}

vec4 Renderiable::setFillColor(const vec4 & fillColor)
{
    return this->fillColor = fillColor;
}

vec4 Renderiable::getFillColor() const
{
    return fillColor;
}

vec4 Renderiable::setOutlineColor(const vec4 & outlineColor)
{
    return this->outlineColor = outlineColor;
}

vec4 Renderiable::getOutlineColor() const
{
    return outlineColor;
}