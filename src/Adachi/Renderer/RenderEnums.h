#pragma once

namespace Adachi {

// The render technique of current pass, to let material provide the write info
enum class PassRenderTechnique { Geometry, Shadow, DepthOnly, Picking };

enum class PrimitiveTopology { TriangleList, TriangleStrip, LineList, LineStrip, PointList };

enum class CompareOperation { Never, Less, LessEqual, Equal, GreaterEqual, Greater, NotEqual, Always };

struct DepthSetting
{
    bool enabled = true;
    bool writeEnabled = true;
    CompareOperation comparison = CompareOperation::Less;
};

struct ColorSetting
{
    bool red;
    bool green;
    bool blue;
    bool alpha;
};

struct AlphaSetting
{
    bool alphaClipping = false;
    float alphaCutoff = 0.5f;
};
}