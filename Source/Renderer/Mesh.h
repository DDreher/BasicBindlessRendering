#pragma once

struct CubeMeshData
{
    static inline std::vector<uint32> INDICES = {
        // front
        0,  1,  2,
        0,  2,  3,
        // back
        4,  5,  6,
        4,  6,  7,
        // top
        8,  9, 10,
        8, 10, 11,
        // bottom
        12, 13, 14,
        12, 14, 15,
        // left
        16, 17, 18,
        16, 18, 19,
        // right
        20, 21, 22,
        20, 22, 23
    };

    static inline std::vector<Vec4> POS =
    {
        // front
        Vec4(-1.0f, -1.0f,  -1.0f, 1.0f),
        Vec4(-1.0f,  1.0f,  -1.0f, 1.0f),
        Vec4(1.0f,  1.0f,   -1.0f, 1.0f),
        Vec4(1.0f, -1.0f,   -1.0f, 1.0f),
        // back
        Vec4(-1.0f, -1.0f, 1.0f, 1.0f),
        Vec4(1.0f, -1.0f, 1.0f , 1.0f), 
        Vec4(1.0f,  1.0f, 1.0f , 1.0f), 
        Vec4(-1.0f,  1.0f, 1.0f, 1.0f),
        // top
        Vec4(-1.0f, 1.0f, -1.0f, 1.0f),
        Vec4(-1.0f, 1.0f,  1.0f, 1.0f),
        Vec4(1.0f, 1.0f,  1.0f , 1.0f), 
        Vec4(1.0f, 1.0f, -1.0f , 1.0f), 
        // bottom
        Vec4(-1.0f, -1.0f, -1.0f, 1.0f),
        Vec4(1.0f, -1.0f, -1.0f , 1.0f), 
        Vec4(1.0f, -1.0f,  1.0f , 1.0f), 
        Vec4(-1.0f, -1.0f,  1.0f, 1.0f),
        // left
        Vec4(-1.0f, -1.0f,  1.0f, 1.0f),
        Vec4(-1.0f,  1.0f,  1.0f, 1.0f),
        Vec4(-1.0f,  1.0f, -1.0f, 1.0f),
        Vec4(-1.0f, -1.0f, -1.0f, 1.0f),
        // right
        Vec4(1.0f, -1.0f, -1.0f, 1.0f),
        Vec4(1.0f,  1.0f, -1.0f, 1.0f),
        Vec4(1.0f,  1.0f,  1.0f, 1.0f),
        Vec4(1.0f, -1.0f,  1.0f, 1.0f)
    };

    static inline std::vector<Vec2> UVS =
    {
        // front
        Vec2(0.0f, 1.0f),
        Vec2(0.0f, 0.0f),
        Vec2(1.0f, 0.0f),
        Vec2(1.0f, 1.0f),
        // back
        Vec2(1.0f, 1.0f),
        Vec2(0.0f, 1.0f),
        Vec2(0.0f, 0.0f),
        Vec2(1.0f, 0.0f),
        // top
        Vec2(0.0f, 1.0f),
        Vec2(0.0f, 0.0f),
        Vec2(1.0f, 0.0f),
        Vec2(1.0f, 1.0f),
        // botttom
        Vec2(1.0f, 1.0f),
        Vec2(0.0f, 1.0f),
        Vec2(0.0f, 0.0f),
        Vec2(1.0f, 0.0f),
        // left
        Vec2(0.0f, 1.0f),
        Vec2(0.0f, 0.0f),
        Vec2(1.0f, 0.0f),
        Vec2(1.0f, 1.0f),
        // right
        Vec2(0.0f, 1.0f),
        Vec2(0.0f, 0.0f),
        Vec2(1.0f, 0.0f),
        Vec2(1.0f, 1.0f)
    };
};
