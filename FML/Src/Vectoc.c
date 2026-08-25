/**
 * @file    vector.c
 * @brief   2D/3D vector helper functions.
 */
#include "Vector.h"
#include <math.h>

float Modulo2d(vector2d v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
}

float Modulo3d(vector3d v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

float InnerProduct2d(vector2d v1, vector2d v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

vector2d Vector_Add(vector2d v1, vector2d v2)
{
    vector2d vr;
    vr.x = v1.x + v2.x;
    vr.y = v1.y + v2.y;
    return vr;
}

vector2d Vector_Minus(vector2d v1, vector2d v2)
{
    vector2d vr;
    vr.x = v1.x - v2.x;
    vr.y = v1.y - v2.y;
    return vr;
}

vector2d Vector_MultiplyNum(vector2d v, float num)
{
    v.x *= num;
    v.y *= num;
    return v;
}
