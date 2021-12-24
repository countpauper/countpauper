#include "stdafx.h"
#include "GLUtil.h"
#include <gl/GL.h> 
#include <utility>

template<> int glGet(GLenum what)
{
    GLint v;
    glGetIntegerv(what, &v);
    return v;
}

template<> bool glGet(GLenum what)
{
    GLboolean v;
    glGetBooleanv(what, &v);
    return v == GL_TRUE;
}

template<> double glGet(GLenum what)
{
    GLdouble v;
    glGetDoublev(what, &v);
    return v;
}

template<> float glGet(GLenum what)
{
    GLfloat v;
    glGetFloatv(what, &v);
    return v;
}

glTemporary::glTemporary(GLenum option, bool enable) :
    option(option),
    enabled(enable)
{
    if (option)
    {
        if (enable)
            glEnable(option);
        else
            glDisable(option);
    }
}

glTemporary glTemporary::Enable(GLenum option)
{
    if (glIsEnabled(option))
        return glTemporary( 0,true );
    return glTemporary{ option, true };
}

glTemporary glTemporary::Disable(GLenum option)
{
    if (!glIsEnabled(option))
        return glTemporary( 0,false );
    return glTemporary{ option, false };
}


glTemporary::glTemporary(glTemporary&& other) :
    option(other.option),
    enabled(other.enabled)
{
    other.option = 0;
}

glTemporary& glTemporary::operator=(glTemporary&& other)
{
    std::swap(option, other.option);
    std::swap(enabled, other.enabled);
    return *this;
}

glTemporary::~glTemporary()
{
    if (!option)
        return;
    else if (enabled)
        glDisable(option);
    else
        glEnable(option);
};
