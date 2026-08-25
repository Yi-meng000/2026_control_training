/**
 * @file    vector.h
 * @brief   2D/3D vector helper functions.
 */
#ifndef VECTOR_H
#define VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Vec2d {
    float x;
    float y;
} vector2d;

typedef struct Vec3d {
    float x;
    float y;
    float z;
} vector3d;

float    Modulo2d(vector2d v);
float    Modulo3d(vector3d v);
float    InnerProduct2d(vector2d v1, vector2d v2);
vector2d Vector_Add(vector2d v1, vector2d v2);
vector2d Vector_Minus(vector2d v1, vector2d v2);
vector2d Vector_MultiplyNum(vector2d v, float num);

#ifdef __cplusplus
}
#endif

#endif /* VECTOR_H */