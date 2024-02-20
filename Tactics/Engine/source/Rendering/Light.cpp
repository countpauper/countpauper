#include "Engine/Rendering/Light.h"
#include <GL/gl.h>

namespace Engine
{
    Light::Light() :
        on(true),
        position(0,0,0),
        diffusePower(2.0f),
        diffuse(255,255,255,255),
        ambientPower(1.0f),
        ambient(255,255,255,255),
        specularPower(10.0f),
        specular(255, 255, 255, 255)
    {
    }
    void Light::Render() const
    {
        if (!on)
        {
            glDisable(GL_LIGHT0);   // TODO: multiple lights
        }
        else
        {
            glEnable(GL_LIGHT0);
            GLfloat light_position[] = { GLfloat(position.x), GLfloat(position.y), GLfloat(position.z) };
            glLightfv(GL_LIGHT0, GL_POSITION, light_position);

            GLfloat light_ambient[] = { ambientPower * float(ambient.r)/255.0f, ambientPower * float(ambient.a)/255.0f, ambientPower * float(ambient.b)/255.0f, float(ambient.a)/255.0f };
            glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

            GLfloat light_diffuse[] = { diffusePower * float(diffuse.r)/255.0f, diffusePower * float(diffuse.a)/255.0f, diffusePower * float(diffuse.b)/255.0f, float(diffuse.a)/255.0f };
            glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

            GLfloat light_specular[] = { specularPower * float(specular.r)/255.0f, specularPower * float(specular.a)/255.0f, specularPower * float(specular.b)/255.0f, float(specular.a)/255.0f };
            glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
        }
    }
    void Light::On()
    {
        on = true;
    }
    void Light::Off()
    {
        on = false;
    }

    void Light::Move(const Coordinate& newPosition)
    {
        position = newPosition;
    }


}   // ::Engine
