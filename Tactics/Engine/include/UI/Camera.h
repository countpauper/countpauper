#pragma once
#include <cstdint>
#include "Geometry/Coordinate.h"
#include "Geometry/Vector.h"

namespace Engine
{
    class Camera
    {
    public:
        Camera();

        virtual void Render() const = 0;
        void Move(const Coordinate& newPosition);
        void Move(const Vector& offset);
        void Zoom(double factor);
        virtual bool Key(std::uint16_t key, unsigned modifiers) = 0;
        void Face(Coordinate target);
    protected:
        Vector vertical{0, 0, 1};
        Coordinate position{0,0,0};
        Coordinate rotation{0,0,0};
        double zoom = 1.0;
    };


    class PerspectiveCamera : public Camera
    {
    public:
        PerspectiveCamera();
        void Render() const override;
        float fov;
    };

    class TopCamera : public Camera
    {
    public:
        TopCamera();
        void Render() const override;
    };


    // TODO: this (and TrackingCmaera) is more like a cameraman and the camera should be injected
    // for now assume perspective
    class FreeCamera : public PerspectiveCamera
    {
    public:
        // Key is a GLUT special key if the top byte is 0 and ascii if the bottom byte is 0
        // modifiers is a bit mask of GLUT_ACTIVE_
        // returns true if handled
        bool Key(std::uint16_t key, unsigned modifiers) override;
    };

    class TrackingCamera : public PerspectiveCamera
    {
    public:
        void Track(const Coordinate& newPosition);
        bool Key(std::uint16_t key, unsigned modifiers);
    private:
        Coordinate target{0,0,0};
    };

}   // ::Engine
