#ifdef __EMSCRIPTEN__
#include "Geometries/RoadGeometry.h"
#include "Lanes.h"
#include "OpenDriveMap.h"
#include "RefLine.h"
#include "Road.h"

#include <emscripten/bind.h>
#include <map>
#include <memory>
#include <string>

namespace odr
{

struct RoadGeometryWrapper : public emscripten::wrapper<RoadGeometry>
{
    EMSCRIPTEN_WRAPPER(RoadGeometryWrapper);

    virtual void update() override
    {
        return call<void>("update");
    }

    virtual Vec2D get_xy(double s) const override
    {
        return call<Vec2D>("get_xy", s);
    }

    virtual Vec2D get_grad(double s) const override
    {
        return call<Vec2D>("get_grad", s);
    }
};

EMSCRIPTEN_BINDINGS(OpenDriveMap)
{
    emscripten::register_vector<int>("vector<int>");
    emscripten::register_vector<double>("vector<double>");

    emscripten::value_array<Vec2D>("Vec2d")
        .element(emscripten::index<0>())
        .element(emscripten::index<1>());

    emscripten::value_array<Vec3D>("Vec3d")
        .element(emscripten::index<0>())
        .element(emscripten::index<1>())
        .element(emscripten::index<2>());

    emscripten::register_vector<Vec3D>("vector<Vec3D>");

    emscripten::class_<Box2D>("Box2D")
        .function("get_distance", &Box2D::get_distance)
        .property("min", &Box2D::min)
        .property("max", &Box2D::max)
        .property("width", &Box2D::width)
        .property("height", &Box2D::height)
        .property("center", &Box2D::center);

    emscripten::class_<Poly3>("Poly3")
        .smart_ptr<std::shared_ptr<Poly3>>("shared_ptr<Poly3>")
        .constructor<double, double, double, double, double>()
        .function("get", &Poly3::get)
        .function("get_grad", &Poly3::get_grad)
        .property("s0", &Poly3::s0)
        .property("a", &Poly3::a)
        .property("b", &Poly3::b)
        .property("c", &Poly3::c)
        .property("d", &Poly3::d);

    emscripten::register_map<double, std::shared_ptr<Poly3>>("map<double, shared_ptr<Poly3>>");

    emscripten::class_<CubicSpline>("CubicSpline")
        .constructor<>()
        .function("size", &CubicSpline::size)
        .function("get", &CubicSpline::get)
        .function("get_grad", &CubicSpline::get_grad)
        .function("get_poly", &CubicSpline::get_poly)
        .property("polys", &CubicSpline::polys);

    emscripten::enum_<GeometryType>("GeometryType")
        .value("Line", GeometryType::Line)
        .value("Spiral", GeometryType::Spiral)
        .value("Arc", GeometryType::Arc)
        .value("ParamPoly3", GeometryType::ParamPoly3);

    emscripten::class_<RoadGeometry>("RoadGeometry")
        .smart_ptr<std::shared_ptr<RoadGeometry>>("shared_ptr<RoadGeometry>")
        .allow_subclass<RoadGeometryWrapper>("RoadGeometryWrapper", emscripten::constructor<double, double, double, double, double, GeometryType, std::shared_ptr<Road>>())
        .function("update", &RoadGeometry::update, emscripten::pure_virtual())
        .function("get_xy", &RoadGeometry::get_xy, emscripten::pure_virtual())
        .function("get_grad", &RoadGeometry::get_grad, emscripten::pure_virtual())
        .property("type", &RoadGeometry::type)
        .property("s0", &RoadGeometry::s0)
        .property("x0", &RoadGeometry::x0)
        .property("y0", &RoadGeometry::y0)
        .property("hdg0", &RoadGeometry::hdg0)
        .property("length", &RoadGeometry::length)
        .property("bounding_box", &RoadGeometry::bounding_box)
        .property("road", &RoadGeometry::road);

    emscripten::class_<Arc, emscripten::base<RoadGeometry>>("Arc")
        .constructor<double, double, double, double, double, double, std::shared_ptr<Road>>()
        .property("curvature", &Arc::curvature);

    emscripten::class_<Line, emscripten::base<RoadGeometry>>("Line")
        .constructor<double, double, double, double, double, std::shared_ptr<Road>>();

    emscripten::class_<ParamPoly3, emscripten::base<RoadGeometry>>("ParamPoly3")
        .constructor<double, double, double, double, double, double, double, double, double, double, double, double, double, std::shared_ptr<Road>>()
        .property("aU", &ParamPoly3::aU)
        .property("bU", &ParamPoly3::bU)
        .property("cU", &ParamPoly3::cU)
        .property("dU", &ParamPoly3::dU)
        .property("aV", &ParamPoly3::aV)
        .property("bV", &ParamPoly3::bV)
        .property("cV", &ParamPoly3::cV)
        .property("dV", &ParamPoly3::dV);

    emscripten::class_<Spiral, emscripten::base<RoadGeometry>>("Spiral")
        .constructor<double, double, double, double, double, double, double, std::shared_ptr<Road>>()
        .property("curv_start", &Spiral::curv_start)
        .property("curv_end", &Spiral::curv_end)
        .property("c_dot", &Spiral::c_dot);

    emscripten::class_<Lane>("Lane")
        .smart_ptr<std::shared_ptr<Lane>>("shared_ptr<Lane>")
        .constructor<int, std::string, bool>()
        .function("get_border", &Lane::get_border)
        .property("id", &Lane::id)
        .property("type", &Lane::type)
        .property("level", &Lane::level)
        .property("predecessor", &Lane::predecessor)
        .property("successor", &Lane::successor)
        .property("lane_section", &Lane::lane_section)
        .property("lane_width", &Lane::lane_width);

    emscripten::register_map<int, std::shared_ptr<Lane>>("map<int, shared_ptr<Lane>>");
    emscripten::register_map<int, double>("map<int, double>");

    emscripten::class_<LaneSection>("LaneSection")
        .smart_ptr<std::shared_ptr<LaneSection>>("shared_ptr<LaneSection>")
        .constructor<double>()
        .function("get_lane_borders", &LaneSection::get_lane_borders)
        .property("s0", &LaneSection::s0)
        .property("road", &LaneSection::road)
        .property("lanes", &LaneSection::lanes);

    emscripten::register_map<double, std::shared_ptr<RoadGeometry>>("map<double, shared_ptr<RoadGeometry>>");
    emscripten::register_map<double, std::shared_ptr<LaneSection>>("map<double, shared_ptr<LaneSection>>");

    emscripten::class_<RefLine>("RefLine")
        .smart_ptr<std::shared_ptr<RefLine>>("shared_ptr<RefLine>")
        .constructor<double>()
        .function("get_xyz", &RefLine::get_xyz)
        .function("get_grad", &RefLine::get_grad)
        .function("match", &RefLine::match)
        .function("get_geometry", &RefLine::get_geometry)
        .property("length", &RefLine::length)
        .property("elevation_profile", &RefLine::elevation_profile)
        .property("geometries", &RefLine::geometries);

    emscripten::class_<Road>("Road")
        .smart_ptr<std::shared_ptr<Road>>("shared_ptr<Road>")
        .constructor<double, int, int>()
        .function("get_xyz", &Road::get_xyz)
        .function("get_surface_pt", &Road::get_surface_pt)
        .property("id", &Road::id)
        .property("junction", &Road::junction)
        .property("length", &Road::length)
        .property("ref_line", &Road::ref_line)
        .property("lane_sections", &Road::lane_sections)
        .property("lane_offset", &Road::lane_offset)
        .property("lane_offset", &Road::superelevation);

    emscripten::register_map<int, std::shared_ptr<Road>>("map<int, shared_ptr<Road>>");

    emscripten::class_<OpenDriveMap>("OpenDriveMap")
        .constructor<std::string>()
        .property("xodr_file", &OpenDriveMap::xodr_file)
        .property("roads", &OpenDriveMap::roads);
}

} // namespace odr
#endif