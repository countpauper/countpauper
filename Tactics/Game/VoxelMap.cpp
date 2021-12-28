#include "stdafx.h"
#include <numeric>
#include "VoxelMap.h"
#include <gl/GL.h>
#include "Engine/Image.h"
#include "Engine/Error.h"
#include "Engine/Coordinate.h"
#include "Engine/Geometry.h"
#include "Engine/Line.h"
#include "Engine/Timer.h"
#include "Engine/Maths.h"
#include "Engine/Mesh.h"
#include "Engine/Matrix.h"
#include "Engine/Plane.h"
#include "Engine/Utils.h"
#include "Engine/from_string.h"
#include "Engine/Debug.h"
#include "Engine/AxisAlignedBoundingBox.h"
#include "Physics/DiscreteGrid.h"
#include "Physics/BoxIterator.h"
#include "Physics/Direction.h"
#include "Engine//Effect.h"
#include "El.h"
#include <string>

namespace Game
{

    VoxelMap::VoxelMap() :
        VoxelMap(0, 0, 0)
    {

    }

    VoxelMap::VoxelMap(unsigned longitude, unsigned latitude, unsigned altitude) :
        grid(HorizontalGrid, HorizontalGrid, VerticalGrid),
        size(longitude, latitude, altitude),
        physical(),
        time(0),
        gravity(-10.0),
        planetRadius(6.371e6),  // assume earth sized planet
        atmosphereRadius(2e5),  // assume earth atmosphere
        atmosphericTemperature(273.15f),
        wind(0, 0, 0)
    {
        World(planetRadius);
        Space(grid.Meters(size));
    }

    VoxelMap::~VoxelMap() = default;

    Element DetermineElement(const Physics::Material& material, double temperature)
    {
        if (temperature >= 500.0f)
        {
            return Element::Fire;
        }
        else
        {
            static const std::map<const Physics::Material*, Element> elementMap = {
                { &Physics::Material::vacuum, Element::None },
                { &Physics::Material::air, Element::Air },
                { &Physics::Material::soil, Element::Nature },
                { &Physics::Material::stone, Element::Stone },
                { &Physics::Material::water, Element::Water }
            };
            return elementMap.at(&material);
        }
    }

    Element VoxelMap::Get(const Position& p) const
    {
        auto c = Center(p);
        auto material = physical->GetMaterial(c);
        if (!material)
            return Element::None;

        auto temperature = physical->Temperature(Engine::Point(c));
        return DetermineElement(*material, temperature);
    }

    Physics::Box VoxelMap::Bounds() const
    {
        return Physics::Box(Physics::Position(0, 0, 0), size);
    }

    void VoxelMap::Space(const Physics::Size& size)
    {
        Space(grid.Meters(size));
    }

    void VoxelMap::Space(const Engine::Vector& size)
    {
        physical = std::make_unique<Physics::DiscreteGrid>(size, Engine::Vector(HorizontalGrid, HorizontalGrid, VerticalGrid));
        this->size = Physics::Size(int(std::round(size.x / grid.x)),
            int(std::round(size.y / grid.y)),
            int(std::round(size.z / grid.z)));
        physical->ConnectChange([this](const Engine::AABB&) {
            mesh.reset();
        });
    }

    void VoxelMap::World(double radius)
    {
        planetRadius = radius;
        constexpr double coreDensity = 5000; // g/L approximate density of earth. liquid iron is 7000 
        constexpr double G = 6.67430e-11 * 0.001;  // m^3 / g * s^2
        double worldMass = float(Engine::SphereVolume(planetRadius) * 1000.0 * coreDensity);
        // surface gravity: https://en.wikipedia.org/wiki/Surface_gravity
        gravity = G * worldMass / (planetRadius*planetRadius);
    }

    float VoxelMap::AtmosphericTemperature(double elevation) const
    {
        double atmosphericLapse = atmosphericTemperature / atmosphereRadius;
        return float(atmosphericTemperature + atmosphericLapse * elevation);
    }

    float VoxelMap::AtmosphericPressure(double elevation) const
    {
        return float(Physics::PascalPerAtmosphere * (1.0 - elevation / atmosphereRadius));
    }

    void VoxelMap::Air(double temperature,  double meters)
    {
        //    atmosphere.mass = float((Engine::SphereVolume(meters + planetRadius) - Engine::SphereVolume(planetRadius))*
        //        1000.0 * atmosphere.material->normalDensity * 0.5);    // TODO: density isn't even over the whole atmosphere. assumed average
        atmosphericTemperature = float(temperature);
        atmosphereRadius = meters;
        double atmorphericLapse = float(-temperature / atmosphereRadius);
        size_t dbgCount = 0;
        constexpr double layerThickness = 1.0;
        meters = grid.Meters(size).z;
        for (double elevation = 0; elevation < meters; elevation += layerThickness)
        {
            Engine::AABB layer(
                Engine::Range<double>::infinity(),
                Engine::Range<double>::infinity(),
                Engine::Range<double>(elevation, elevation + layerThickness));
            auto temperature = AtmosphericTemperature(elevation + layerThickness * 0.5);
            dbgCount += physical->Fill(layer, Physics::fillAll, Physics::Material::air, temperature);
        }
        Engine::Debug::Log(L"Air =" + std::to_wstring(dbgCount) + L" voxels\n");
    }

    void VoxelMap::Wind(const Engine::Vector& speed)
    {
        wind = speed;
        // TODO: set all air flux and boundary
    }

    void VoxelMap::Sea(double level, double temperature)
    {
        Engine::AABB sea(
            Engine::Range<double>::infinity(),
            Engine::Range<double>::infinity(),
            Engine::Range<double>(0, level));
        auto dbgCount = physical->Fill(sea, Physics::fillAll, Physics::Material::water, temperature);
        Engine::Debug::Log(std::wstring(L"Sea level ") + Engine::ToWString(level) + L"=" + std::to_wstring(dbgCount) + L" voxels\n");
        
        int gridLevel = int(std::round(level / grid.z));
        Physics::Box surface(Engine::Range<int>(0,size.x), Engine::Range<int>(0,size.y), Engine::Range<int>(gridLevel-1, gridLevel));
        for (auto side : Physics::Directions() | Physics::Direction::north | Physics::Direction::south |Physics::Direction::east | Physics::Direction::west)
        {
            physical->Constrain(Engine::AABB(grid.BoundingBox(Edge(side) & surface)),
                                Physics::Material::water, temperature, [](double time) { 
                                    return (0.7 + 0.3*sin(time*0.2))*Physics::Material::water.normalDensity; 
                                });
        }
    }

    class Hill : public Engine::IVolume
    {
    public:
        Hill(const Engine::Line& line, double stddev) :
            m_line(line),
            m_stddev(stddev)
        {
        }
        Engine::AABB GetBoundingBox() const override
        {
            constexpr double treshold = 0.1;    // should be grid/z / 2 but don't know what grid
            double aHorRange = 0, bHorRange = 0;
            if (m_line.a.z >= treshold)
                aHorRange = Engine::InvGaussian(treshold / m_line.a.z, m_stddev);
            if (m_line.b.z >= treshold)
                bHorRange = Engine::InvGaussian(treshold / m_line.b.z, m_stddev);
            Engine::AABB aBounds(Engine::Range(m_line.a.x - aHorRange, m_line.a.x + aHorRange),
                Engine::Range<double>(m_line.a.y - aHorRange, m_line.a.y + aHorRange),
                Engine::Range<double>(0, m_line.a.z));

            Engine::AABB bBounds(Engine::Range(m_line.b.x - bHorRange, m_line.b.x + bHorRange),
                Engine::Range<double>(m_line.b.y - bHorRange, m_line.b.y + bHorRange),
                Engine::Range<double>(0, m_line.b.z));

            return aBounds | bBounds;
        }
        double Distance(const Engine::Coordinate& p) const
        {
            Engine::Line bottomLine = Engine::Plane::xy.Project(m_line);
            // compute c: the height of the hill at p
            Engine::Coordinate c(p.x, p.y, 0);
            if (bottomLine.Length() > std::numeric_limits<double>::epsilon())
            {
                auto bottomProjection = bottomLine.Project(c);
                double distance = (bottomProjection - c).Length();
                double interpolation_factor = std::min(1.0, (c - bottomLine.a).Length() / bottomLine.Length());
                c.z = Engine::Lerp(double(m_line.a.z), double(m_line.b.z), interpolation_factor) * Engine::Gaussian(distance, m_stddev);
            }
            else
            {
                double distance = (bottomLine.a - c).Length();
                c.z = m_line.a.z * Engine::Gaussian(distance, m_stddev);
            }
            return p.z - c.z;
        }
        double Volume() const 
        {
            throw std::runtime_error("Volume of a hill is unimplemented");
        }
    private:
        Engine::Line m_line;
        double m_stddev;

    };
    void VoxelMap::Hill(const Engine::Line& ridgeLine, double stddev)
    {
        Game::Hill hill(ridgeLine, stddev);

        auto dbgCount = physical->Fill(hill, Physics::fillAll, Physics::Material::stone, atmosphericTemperature);
        Engine::Debug::Log(std::wstring(L"Hill at ") + Engine::ToWString(ridgeLine) + L"=" + std::to_wstring(dbgCount) + L" voxels\n");
    }

    void VoxelMap::Wall(const Engine::Line& bottomLine, double height, double thickness)
    {
        Engine::Box box(Engine::Vector(bottomLine.Length(), thickness, height));
        auto xAxis = Engine::Vector(bottomLine).Normal();
        Engine::Vector zAxis(0, 0, 1);
        // translate the box up so the bottomline starts at 0,0,0 first
        box *= Engine::Matrix::Translation(Engine::Vector(bottomLine.Length()*0.5, 0, height*0.5));

        box *= Engine::Matrix(
            xAxis,  // the bottom line was in xAxis, now rotated towards the line
            zAxis.Cross(xAxis),  // the yAxis is perpendicular to the x and z simply
            zAxis,  // the zAxis remains aligned with the world to not rotate the wall, but skew it along the line 
            Engine::Vector(bottomLine.a));

        auto dbgCount = physical->Fill(box, Physics::fillAll, Physics::Material::stone, atmosphericTemperature);
        Engine::Debug::Log(std::wstring(L"Wall at ") + Engine::ToWString(bottomLine) + L"=" + std::to_wstring(dbgCount) + L" voxels\n");

    }


    void VoxelMap::River(const Engine::Line& axis, double width, double depth, double flow)
    {
        Engine::Cylinder cylinder(axis, width, depth);
        Engine::Vector flowVector(axis);
        Engine::Vector reverseGravity(0, 0, 1); // prefer water belly hangs down in the gravity direction
        Engine::Vector surfaceVector = flowVector.Cross(reverseGravity);
        Engine::Cylinder source = cylinder.Slice(Engine::Range<double>(0, 1));

        // TODO: flow is in percentage delta of normal density of water per source block so ~333 kg of water 
        //   should be mass per second, which should be implemented also by sources as they tick over time instead of constraining to a fixed density
        if (!surfaceVector)
        {   // completely vertical, make it a well, TODO: there could be a slope angle for nearly vertical rivers 
            auto dbgCount = physical->Fill(cylinder, Physics::fillAll, Physics::Material::water, atmosphericTemperature);
            Engine::Debug::Log(std::wstring(L"Well at ") + Engine::ToWString(flow) + L"=" + std::to_wstring(dbgCount) + L" voxels\n");
            if (flow)
            {
                // assert(physical->Measure(&Physics::Material::water, source) > 0);
                physical->Constrain(source, Physics::Material::water, atmosphericTemperature, [flow](double) {return Physics::Material::water.normalDensity*(1.0 + flow); });
            }
        }
        else
        {
            Engine::Plane surface(axis.a, axis.a + surfaceVector, axis.b);
            Engine::Intersection intersection({ cylinder, surface });

            auto dbgCount = physical->Fill(intersection, Physics::fillAll, Physics::Material::water, atmosphericTemperature);
            Engine::Debug::Log(std::wstring(L"River at ") + Engine::ToWString(axis) + L"=" + std::to_wstring(dbgCount) + L" voxels\n");
            if (flow)
            {
                Engine::Intersection constraintIntsection({ source, surface });
                assert(physical->Measure(&Physics::Material::water, constraintIntsection) > 0);
                physical->Constrain(constraintIntsection, Physics::Material::water, atmosphericTemperature, [flow](double) {return Physics::Material::water.normalDensity*(1.0 + flow); });
            }
        }

    }

    void VoxelMap::Cave(const Engine::Line& axis, double width, double height)
    {
        Engine::Cylinder cylinder(axis, width, height);
        auto dbgCount = physical->Fill(cylinder, Physics::fillAll, Physics::Material::air, atmosphericTemperature);
        Engine::Debug::Log(std::wstring(L"Cave at ") + Engine::ToWString(axis) + L"=" + std::to_wstring(dbgCount) + L" voxels\n");
    }

    Engine::Coordinate VoxelMap::Center(const Position& p) const
    {
        return grid.Center(p);
    }

    int EdgeStartCoord(int s, int v)
    {
        if (v == 0)
            return 0;
        else if (v < 0)
            return 0;
        else
            return s - v;
    }

    int EdgeEndCoord(int  s, int v)
    {
        if (v == 0)
            return s;
        else if (v < 0)
            return -v;
        else
            return s;
    }

    Engine::Range<int> EdgeRange(int size, int v)
    {
        if (v == 0)
            return Engine::Range<int>(0, size);
        else if (v < 0)
            return Engine::Range<int>(0, -v);
        else
            return Engine::Range<int>(size - v, size);
    }

    Physics::Box VoxelMap::Edge(Physics::Direction side) const
    {
        auto v = side.Vector();
        return Physics::Box(EdgeRange(size.x, v.x),
            EdgeRange(size.y, v.y),
            EdgeRange(size.z, v.z));
    }


    Physics::Directions VoxelMap::Visibility(const Position& p) const
    {
        auto c = Center(p);
        Physics::Directions result;
        for (const Physics::Direction& direction : Physics::Direction::all)
        {
            auto neighbourPosition = p + direction.Vector();

            if (!physical->Color(Engine::Line(c, Center(neighbourPosition))).IsOpaque())
            {
                result |= direction;
            }
        }
        return result;
    }

    void VoxelMap::Tick(double seconds)
    {
        physical->Tick(seconds);
    }

    void VoxelMap::SetDensityToMeshColor()
    {
        if (!mesh)
            return;
        mesh->Invalidate();
        for (auto& vertex : mesh->vertices)
        {
            Engine::Coordinate meters(vertex.c.x, vertex.c.z, vertex.c.y);

            //auto position = Grid(meters);
            auto temperature = physical->Temperature(Engine::Point(meters));
            const auto* material = physical->GetMaterial(meters);
            auto density = physical->Density(Engine::Point(meters));
            double sigmoidDensity = Engine::Sigmoid((density - material->normalDensity) / material->normalDensity);
            double sigmoidTemperature = Engine::Sigmoid(temperature - atmosphericTemperature);
            vertex.color = Engine::RGBA(BYTE(127 + sigmoidTemperature * 127.0),
                BYTE(127 + sigmoidDensity * 127.0),
                BYTE(127 - sigmoidDensity * 127.0), 255);
        }
    }



    void VoxelMap::GenerateMesh()
    {
        mesh = std::make_unique<Engine::Mesh>();
        Position p;
    
        for (p.x = 0; p.x < size.x; ++p.x)
        {
            for (p.y = 0; p.y < size.y; ++p.y)
            {
                for (p.z = 0; p.z < size.z; ++p.z)
                {
                    Engine::Coordinate c = Center(p);
                    auto color = physical->Color(Engine::Line(c + Engine::Vector(0, 0, -0.5), c));
                    if (!color)
                        continue;
                    if (!Visibility(p))
                        continue;
                    // TODO: matrix from position to gl, swapping y and z, translation and grid scaling

                    Engine::Box box(Engine::Vector(grid.x, grid.z, grid.y));    // nb z<=>y
                    box.SetColor(color);
                    box *= Engine::Matrix::Translation(
                        Engine::Vector(c.x, c.z, c.y));
                    (*mesh) += box;

                }
            }
        }
    }

    void VoxelMap::Render() const
    {
        if (!mesh)
        {
            Engine::Debug::Timer t(L"Genrate Mesh");
            const_cast<VoxelMap*>(this)->GenerateMesh();
            Engine::CheckGLError();
        }
        if (mesh)
        {
            glEnable(GL_LIGHTING);
            glDisable(GL_BLEND);
            mesh->Render();
            glDisable(GL_LIGHTING);
            glEnable(GL_BLEND);
            mesh->Render();
        }
        else
        {
            //RenderPretty();
            RenderAnalysis();
        }
        Engine::CheckGLError();
        for (const auto renderable : physical->Render())
        {
            renderable->Render();
        }
        Engine::CheckGLError();

    }

    std::map<Element, double> VoxelMap::ElementVolume() const
    {
        std::map<Element, double> result { {Element::None, 0}, { Element::Nature,0 }, { Element::Stone,0 }, { Element::Air, 0 }, { Element::Water, 0 }, { Element::Fire, 0 } };
        for (Physics::BoxIterator i(Physics::Box(this->size)); i!=i.end(); ++i)
        {
            Engine::Coordinate c = grid.Center(*i);
            auto material = physical->GetMaterial(c);
            if (!material)
                continue;
            auto temperature = physical->Temperature(Engine::Point(c));
            auto element = DetermineElement(*material, temperature);
            result[element] += grid.Volume();
        }
        return result;
    }

void VoxelMap::RenderPretty() const
{
    /*
    assert(glIsEnabled(GL_DEPTH_TEST));

    // Draw opaque
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    for (auto& v : voxels)
    {
        glPushMatrix();
        glTranslated(v.first.x, v.first.z*MeterPerEl, v.first.y); // offset by -1,-1,-1 for boundary
        if (v.second.Opaque())
        {
            if (auto visibility = Visibility(v.first))
            {
                v.second.Render(v.first, visibility);
            }
        }
        glPopMatrix();
    }

    // translucent pass
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    for (auto& v : voxels)
    {
        glPushMatrix();
        glTranslated(v.first.x, v.first.z*MeterPerEl, v.first.y); // offset by -1,-1,-1 for boundary
        if (!v.second.Opaque() && !v.second.Transparent())
        {
            auto visibility = Visibility(v.first);
            if (!visibility.empty())
            {
                v.second.Render(v.first, visibility);
            }
        }
        glPopMatrix();
    }
    Engine::CheckGLError();
    */
}


void VoxelMap::RenderAnalysis() const
{
    /*
    assert(glIsEnabled(GL_DEPTH_TEST));

    // Draw opaque
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    for (const auto v : voxels)
    {
        glPushMatrix();
        glTranslated(v.first.x, v.first.z*MeterPerEl, v.first.y);
        v.second.RenderAnalysis(v.first, Directions::all, voxels.DensityGradient(v.first));
        glPopMatrix();
    }


    glColor3d(1, 1, 1);
    Position p;
    for (auto u : voxels.U())
    {
        glPushMatrix();
        glTranslated(u.first.x, (u.first.z + 0.5)*MeterPerEl, u.first.y + 0.5);
        Engine::glDrawArrow(Engine::Vector(u.second, 0, 0));
        glPopMatrix();
    }
    for (auto v : voxels.V())
    {
        glPushMatrix();
        glTranslated(v.first.x + 0.5, (v.first.z + 0.5)*MeterPerEl, v.first.y);
        Engine::glDrawArrow(Engine::Vector(0, 0, v.second));
        glPopMatrix();
    }
    for (auto w : voxels.W())
    {
        glPushMatrix();
        glTranslated(w.first.x + 0.5, w.first.z*MeterPerEl, w.first.y + 0.5);
        Engine::glDrawArrow(Engine::Vector(0, w.second, 0));
        glPopMatrix();
    }
    glColor3d(0.5, 0.5, 0.5);
    for (auto d : Physics::Direction::all)
    {
        for (auto u : voxels.U().BoundaryCondition(Directions(d)))
        {
            double v = u.first;
            glPushMatrix();
            glTranslated(u.first.x, (u.first.z + 0.5)*MeterPerEl, u.first.y + 0.5);
            Engine::glDrawArrow(Engine::Vector(u.second, 0, 0));
            glPopMatrix();
        }
        for (auto v : voxels.V().BoundaryCondition(Directions(d)))
        {
            glPushMatrix();
            glTranslated(v.first.x + 0.5, (v.first.z + 0.5)*MeterPerEl, v.first.y);
            Engine::glDrawArrow(Engine::Vector(0, 0, v.second));
            glPopMatrix();
        }
        for (auto w : voxels.W().BoundaryCondition(Directions(d)))
        {
            glPushMatrix();
            glTranslated(w.first.x + 0.5, w.first.z*MeterPerEl, w.first.y + 0.5);
            Engine::glDrawArrow(Engine::Vector(0, w.second, 0));
            glPopMatrix();
        }
    }
/*
    for (auto d : Physics::Direction::all)
    {
        for (auto v : voxels.BoundaryCondition(Directions(d)))
        {
            glPushMatrix();
            glTranslated(v.position.x, v.position.z*MeterPerEl, v.position.y);
            v.Render(v.position, Directions(0xFFFF));
            glPopMatrix();
        }
    }
*/
    Engine::CheckGLError();
}


std::wstring VoxelMap::Statistics()
{
    return physical->Statistics();
}

std::wistream& operator>>(std::wistream& s, VoxelMap& map)
{
    Engine::Timer timer;
    std::wstring name;
    Engine::Vector size;
    s >> name >> size.x >> size.y >> size.z;
    map.Space(size);
    double temperature;
    s >> temperature;
    map.Air(temperature, 20000);
    unsigned procedures;
    s >> procedures;
    for (unsigned p = 0; p < procedures; ++p)
    {
        std::wstring procedure;
        s >> procedure;
        procedure = Engine::UpperCase(procedure);
        if (procedure == L"WIND")
        {
            Engine::Vector wind;
            s >> wind;
            map.Wind(wind);
        }
        else if (procedure == L"WALL")
        {
            Engine::Coordinate p0, p1;
            float height, width;
            s >> p0 >> p1 >> height >> width;
            map.Wall(Engine::Line(p0, p1), height, width);
        }
        else if (procedure == L"HILL")
        {
            Engine::Coordinate p0, p1;
            float stddev;
            s >> p0 >> p1 >> stddev;
            map.Hill(Engine::Line(p0, p1), stddev);
        }
        else if (procedure == L"SEA")
        {
            float depth, temperature;
            s >> depth >> temperature;
            map.Sea(depth, temperature);
        }
        else if (procedure == L"RIVER")
        {
            Engine::Coordinate p0, p1;
            float width, depth, flow;
            s >> p0 >> p1 >> width >> depth >> flow;
            map.River(Engine::Line(p0, p1), width, depth, flow);
        }
        else if (procedure == L"CAVE")
        {
            Engine::Coordinate p0, p1;
            float width, height;
            s >> p0 >> p1 >> width >> height;
            map.Cave(Engine::Line(p0, p1), width, height);
        }
        else
        {
            throw std::runtime_error("Unknown procedure: "+Engine::from_string<std::string>(procedure));
        }
    }
    Engine::Debug::Log(L"Map generated in " + std::to_wstring(timer.Seconds()*1000.0) +L"ms "+ map.Statistics());
    return s;
}

}