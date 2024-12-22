#pragma once
#include <sc2api/sc2_api.h>

//! Outputs text at the top, left of the screen.
//!< \param out The string of text to display.
//!< \param color (Optional) Color of the text.
void DebugText(Agent* agent, const std::string& out, Color color = Colors::White) {
#ifndef BUILD_FOR_LADDER
    agent->Debug()->DebugTextOut(out, color);
#endif
}
//! Outputs text at any 2D point on the screen. Coordinate ranges are 0..1 in X and Y.
//!< \param out The string of text to display.
//!< \param pt_virtual_2D The screen position to draw text at.
//!< \param color (Optional) Color of the text.
//!< \param size (Optional) Pixel height of the text.
void DebugText(Agent* agent, const std::string& out, const Point2D& pt_virtual_2D, Color color = Colors::White,
    uint32_t size = 8) {
#ifndef BUILD_FOR_LADDER
    agent->Debug()->DebugTextOut(out, pt_virtual_2D, color, size);
#endif
}
//! Outputs text at any 3D point in the game world. Map coordinates are used.
//!< \param out The string of text to display.
//!< \param pt3D The world position to draw text at.
//!< \param color (Optional) Color of the text.
//!< \param size (Optional) Pixel height of the text.
void DebugText(Agent* agent, const std::string& out, const Point3D& pt3D, Color color = Colors::White,
    uint32_t size = 8) {
#ifndef BUILD_FOR_LADDER
    agent->Debug()->DebugTextOut(out, pt3D, color, size);
#endif
}
//! Outputs a line between two 3D points in the game world. Map coordinates are used.
//!< \param p0 The starting position of the line.
//!< \param p1 The ending position of the line.
//!< \param color (Optional) Color of the line.
void DebugLine(Agent* agent, const Point3D& p0, const Point3D& p1, Color color = Colors::White) {
#ifndef BUILD_FOR_LADDER
    agent->Debug()->DebugLineOut(p0, p1, color);
#endif
}
//! Outputs a box specified as two 3D points in the game world. Map coordinates are used.
//!< \param p0 One corner of the box.
//!< \param p1 The far corner of the box.
//!< \param color (Optional) Color of the lines.
void DebugBox(Agent* agent, const Point3D& p_min, const Point3D& p_max, Color color = Colors::White) {
#ifndef BUILD_FOR_LADDER
    agent->Debug()->DebugBoxOut(p_min, p_max, color);
#endif
}

//! Outputs a sphere specified as a 3D point in the game world and a radius. Map coordinates are used.
//!< \param p Center of the sphere.
//!< \param r Radius of the sphere.
//!< \param color (Optional) Color of the lines.
void DebugSphere(Agent* agent, const Point3D& p, float r, Color color = Colors::White) {
#ifndef BUILD_FOR_LADDER
    agent->Debug()->DebugSphereOut(p, r, color);
#endif
}

//! Outputs a sphere specified as a 3D point in the game world and a radius. Map coordinates are used.
//!< \param p Center of the sphere.
//!< \param r Radius of the sphere.
//!< \param color (Optional) Color of the lines.
void SendDebug(Agent* agent) {
#ifndef BUILD_FOR_LADDER
    agent->Debug()->SendDebug();
#endif
}