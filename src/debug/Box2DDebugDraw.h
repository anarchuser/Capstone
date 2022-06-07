#pragma once
#include "oxygine-framework.h"
#include "box2d.h"

using namespace oxygine;
namespace oxygine
{
    /// Shader applied on top of rendered primitives
    class ShaderProgramGL;
}

DECLARE_SMART(Box2DDraw, spBox2DDraw);

/// Class used to render a box2d world, regardless of any oxygine sprites. NOTE: taken from oxygine's example project
class Box2DDraw: public Actor, public b2Draw {
public:
    /// Create a new debug draw instance
    Box2DDraw();
    /// Destroy this debug draw instance
    ~Box2DDraw() override;

    /// Configure the world to debug-draw
    void setWorld(float worldScale, b2World* world) {_worldScale = worldScale; _world = world;}

    /// Draw a closed polygon provided in CCW order.
    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

    /// Draw a solid closed polygon provided in CCW order.
    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

    /// Draw a circle.
    void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override;

    /// Draw a solid circle.
    void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override;

    /// Draw a line segment.
    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;

    /// Draw a transform. Choose your own length scale.
    /// @param xf a transform.
    void DrawTransform(const b2Transform& xf) override;

    /// Draw a circle.
    void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override;

protected:
    /// The world to debug-draw
    b2World* _world;
    /// The graphical:physical scale of the world
    float _worldScale;

    /// Render the given world
    void doRender(const RenderState& rs) override;
    /// Maximum number of points allowed in a polygon to render it
    static const int MAX_VERTICES = 64;
    /// Maximum number of points allowed to render a circle
    static const int CIRCLE_SEGMENTS = 16;

    /// Points defining polygons and circles
    Vector2 mVertices[MAX_VERTICES];
    /// Fill vertices with points along the given circle
    void createCircleVertices(const b2Vec2& aCenter, float aRadius);
    /// Fill vertices with corners of the given polygon
    void createPolygonVertices(const b2Vec2* aVertices, int32 aVertexCount);
    /// Draw the vertices based on these settings
    void drawPrimitives(bool drawTriangles, bool drawLines, int aCount, const b2Color& aColor);

    /// Shader applied onto the rendered primitives
    ShaderProgramGL* _program;
};
