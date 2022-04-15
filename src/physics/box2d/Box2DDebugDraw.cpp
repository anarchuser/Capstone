#include "Box2DDebugDraw.h"
#include "oxygine-framework.h"

Box2DDraw::Box2DDraw(): _worldScale(1.0f), _world(nullptr)
{
    m_drawFlags = 0xffffffff;

    const char* vertexShaderData = "\
									uniform mediump mat4 projection;\
									attribute vec2 a_position;\
									void main() {\
									vec4 position = vec4(a_position, 0.0, 1.0);\
									gl_Position = projection * position;\
									}\
									";

    const char* fragmentShaderData = "\
									  uniform mediump vec4 color;\
									  void main() { \
									  gl_FragColor = color; \
									  } \
									  ";


    int vs = ShaderProgramGL::createShader(GL_VERTEX_SHADER, vertexShaderData, 0, 0);
    int fs = ShaderProgramGL::createShader(GL_FRAGMENT_SHADER, fragmentShaderData, 0, 0);

    int pr = ShaderProgramGL::createProgram(vs, fs, (VertexDeclarationGL*)IVideoDriver::instance->getVertexDeclaration(VERTEX_POSITION));
    _program = new ShaderProgramGL(pr);
}

Box2DDraw::~Box2DDraw()
{
    delete _program;
}

void Box2DDraw::doRender(const RenderState& rs)
{
    Material::null->apply();
    rsCache().reset();
    IVideoDriver* driver = IVideoDriver::instance;

    _world->SetDebugDraw(this);

    driver->setShaderProgram(_program);

    Matrix m = Matrix(rs.transform) * STDRenderer::getCurrent()->getViewProjection();
    driver->setUniform("projection", m);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    _world->DebugDraw();
//    _world->DrawDebugData();
    _world->SetDebugDraw(nullptr);

    rsCache().reset();

}

void Box2DDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    createPolygonVertices(vertices, vertexCount);
    drawPrimitives(false, true, vertexCount, color);
}

/// Draw a solid closed polygon provided in CCW order.
void Box2DDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    createPolygonVertices(vertices, vertexCount);
    drawPrimitives(true, true, vertexCount, color);
}

/// Draw a circle.
void Box2DDraw::DrawCircle(const b2Vec2& center, float aRadius, const b2Color& color)
{
    createCircleVertices(center, aRadius);
    drawPrimitives(false, true, CIRCLE_SEGMENTS, color);
}

/// Draw a solid circle.
void Box2DDraw::DrawSolidCircle(const b2Vec2& center, float aRadius, const b2Vec2& aAxis,
                                const b2Color& color)
{
    createCircleVertices(center, aRadius);
    drawPrimitives(true, true, CIRCLE_SEGMENTS, color);
    // Draw the axis line
    DrawSegment(center, center + aRadius * aAxis, color);
}

/// Draw a line segment.
void Box2DDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
    mVertices[0].x = p1.x * _worldScale;
    mVertices[0].y = p1.y * _worldScale;
    mVertices[1].x = p2.x * _worldScale;
    mVertices[1].y = p2.y * _worldScale;
    drawPrimitives(false, true, 2, color);
}

/// Draw a transform. Choose your own length scale.
/// @param xf a transform.
void Box2DDraw::DrawTransform(const b2Transform& xf)
{
    b2Vec2 p1 = xf.p, p2{};
    const float k_axisScale = 0.4f;

    p2 = p1 + k_axisScale * xf.q.GetXAxis();
    DrawSegment(p1, p2, b2Color(1, 0, 0));

    p2 = p1 + k_axisScale * xf.q.GetYAxis();
    DrawSegment(p1, p2, b2Color(0, 1, 0));
}

/// Draw a circle.
void Box2DDraw::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
{
    createCircleVertices (p, 0);
    drawPrimitives(false, true, CIRCLE_SEGMENTS, color);
}

void Box2DDraw::createCircleVertices(const b2Vec2& center, float aRadius)
{
    const float k_increment = 2.0f * b2_pi / CIRCLE_SEGMENTS;
    float theta = 0.0f;

    for (int32 i = 0; i < CIRCLE_SEGMENTS; ++i)
    {
        b2Vec2 v = center + aRadius * b2Vec2(scalar::cos(theta), scalar::sin(theta));
        mVertices[i].x = _worldScale * v.x;
        mVertices[i].y = _worldScale * v.y;
        theta += k_increment;
    }
}

void Box2DDraw::createPolygonVertices(const b2Vec2* vertices, int32 vertexCount)
{
    if (vertexCount > MAX_VERTICES)
    {
        logs::warning("need more vertices");
        return;
    }

    // convert vertices to screen resolution
    for (int i = 0; i < vertexCount; i++)
    {
        mVertices[i].x = _worldScale * vertices[i].x;
        mVertices[i].y = _worldScale * vertices[i].y;
    }
}


//------------------------------------------------------------------------
void Box2DDraw::drawPrimitives(bool drawTriangles, bool drawLines, int count, const b2Color& color)
{
    oxglEnableVertexAttribArray(0);
    oxglVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLfloat*)mVertices);

    if (drawTriangles)
    {
        Vector4 c(color.r, color.g, color.b, 0.5f);
        IVideoDriver::instance->setUniform("color", &c, 1);
        glDrawArrays(GL_TRIANGLE_FAN, 0, count);
    }

    if (drawLines)
    {
        Vector4 c(color.r, color.g, color.b, 1.0f);
        IVideoDriver::instance->setUniform("color", &c, 1);
        glDrawArrays(GL_LINE_LOOP, 0, count);
    }

    oxglDisableVertexAttribArray(0);
}
