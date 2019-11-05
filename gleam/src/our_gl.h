#ifndef __OUR_GL_H__
#define __OUR_GL_H__
#include "tgaimage.h"
#include "geometry.h"

extern Matrix ModelView;
extern Matrix Projection;

void viewport(int x, int y, int w, int h);
void projection(float coeff=0.f); // coeff = -1/c
void lookat(Vec3f eye, Vec3f center, Vec3f up);

struct Output {
    vec<2,float> varying_uv;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    vec<4,float> varying_tri; // triangle coordinates (clip coordinates), written by VS, read by FS
    vec<3,float> varying_nrm; // normal per vertex to be interpolated by FS
    vec<3,float> ndc_tri;     // triangle in normalized device coordinates
};

struct IShader {
    virtual ~IShader();
    virtual Output vertex(int iface, int nthvert) = 0;
    virtual bool fragment(Vec3f bar, TGAColor &color) = 0;
};

//void triangle(Vec4f *pts, IShader &shader, TGAImage &image, float *zbuffer);
void triangle(mat<4,3,float> &pts, IShader &shader, TGAImage &image, float *zbuffer);
#endif //__OUR_GL_H__

