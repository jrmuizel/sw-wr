#include <vector>
#include <limits>
#include <iostream>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "our_gl.h"

Model *model        = NULL;

const int width  = 800;
const int height = 800;

Vec3f light_dir(1,1,1);
Vec3f       eye(1,1,3);
Vec3f    center(0,0,0);
Vec3f        up(0,1,0);

struct Shader : public IShader {
    mat<2,3,float> varying_uv;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    mat<4,3,float> varying_tri; // triangle coordinates (clip coordinates), written by VS, read by FS
    mat<3,3,float> varying_nrm; // normal per vertex to be interpolated by FS
    mat<3,3,float> ndc_tri;     // triangle in normalized device coordinates

    virtual Output vertex(int iface, int nthvert) {
        Vec2f uv = model->uv(iface, nthvert);
        Vec3f normal = model->normal(iface, nthvert);
        Vec3f vert = model->vert(iface, nthvert);

        Output result;
        result.varying_uv = uv;
        result.varying_nrm = proj<3>((Projection*ModelView).invert_transpose()*embed<4>(normal, 0.f));
        Vec4f gl_Vertex = Projection*ModelView*embed<4>(vert);
        result.varying_tri = gl_Vertex;
        result.ndc_tri = proj<3>(gl_Vertex/gl_Vertex[3]);

        return result;
    }

    virtual bool fragment(Vec3f bar, TGAColor &color) {
        Vec2f uv = varying_uv*bar;
        TGAColor c = model->diffuse(uv);
        color = c;
        return false;

    }
};

int main(int argc, char** argv) {
    if (2>argc) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }

    float *zbuffer = new float[width*height];
    for (int i=width*height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

    TGAImage frame(width, height, TGAImage::RGB);
    lookat(eye, center, up);
    viewport(width/8, height/8, width*3/4, height*3/4);
    projection(-1.f/(eye-center).norm());
    light_dir = proj<3>((Projection*ModelView*embed<4>(light_dir, 0.f))).normalize();

    for (int m=1; m<argc; m++) {
        model = new Model(argv[m]);
        Shader shader;
        for (int i=0; i<model->nfaces(); i++) {
            for (int j=0; j<3; j++) {
                Output out = shader.vertex(i, j);

                shader.varying_uv.set_col(j, out.varying_uv);
                shader.varying_tri.set_col(j, out.varying_tri);
                shader.varying_nrm.set_col(j, out.varying_nrm);
                shader.ndc_tri.set_col(j, out.ndc_tri);
            }
            triangle(shader.varying_tri, shader, frame, zbuffer);
        }
        delete model;
    }
    frame.flip_vertically(); // to place the origin in the bottom left corner of the image
    frame.write_tga_file("framebuffer.tga");

    delete [] zbuffer;
    return 0;
}

