#pragma once

#include <GL/glew.h>

template<typename T> T glGet(GLenum what)
{
    assert(false); // invalid type
    return T();
}

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
    return v==GL_TRUE;
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
