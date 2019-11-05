#include <assert.h>
//brush_solid
/*
clear_color()
get_string()
get_integer_v()
get_string_i()
get_error()
active_texture()
bind_texture()
bind_vertex_array()
pixel_store_i()
bind_buffer()
gen_textures()
tex_parmater_i()
tex_storage_3d()
tex_storage_2d()
tex_sub_image_2d()
gen_buffers()
gen_vertex_arrays()
enable_vertex_attrib_array()
vertex_attrib_divisor()
vertex_attrib_pointer()
vertex_attrib_i_pointer()
buffer_data_untyped()
tex_sub_image_3d()
gen_framebuffers()*/
#include <map>
using namespace std;
extern "C" {
typedef uint32_t GLuint;
typedef int32_t GLint;
typedef int32_t GLsizei;
typedef uint32_t GLenum;
struct Buffer {
};

struct Framebuffer {
};

struct Renderbuffer {
};


struct Texture {
    GLenum target;
    GLenum levels;
    GLenum internal_format;
    GLenum width;
    GLenum height;
    GLenum depth;
    char *buf;
};

struct VertexArray {
};

map<GLuint, Buffer> buffers;
map<GLuint, Texture> textures;
map<GLuint, VertexArray> vertex_arrays;
map<GLuint, Framebuffer> framebuffers;
map<GLuint, Renderbuffer> renderbuffers;
GLuint buffer_count;
map<GLenum, GLuint> current_buffer;

GLuint texture_count;
map<GLenum, GLuint> current_texture;
map<GLenum, GLuint> current_framebuffer;
map<GLenum, GLuint> current_renderbuffer;

GLuint vertex_array_count;
GLuint current_vertex_array;

GLuint renderbuffer_count;
GLuint framebuffer_count;

void GenBuffers(int n, int *result) {
        for (int i = 0; i < n; i++) {
                Buffer b;
                buffers.insert(pair<GLuint, Buffer>(buffer_count, b));
                result[i] = buffer_count++;
        }
}

void GenVertexArrays(int n, int *result) {
        for (int i = 0; i < n; i++) {
                VertexArray v;
                vertex_arrays.insert(pair<GLuint, VertexArray>(vertex_array_count, v));
                result[i] = vertex_array_count++;
        }
}

void BindVertexArray(GLuint vertex_array) {
    current_vertex_array = vertex_array;
}


void BindTexture(GLenum target, GLuint texture) {
    current_texture[target] = texture;
}

void BindBuffer(GLenum target, GLuint buffer) {
    current_buffer[target] = buffer;
}

void BindFramebuffer(GLenum target, GLuint fb) {
    current_framebuffer[target] = fb;
}

void BindRenderbuffer(GLenum target, GLuint rb) {
    current_renderbuffer[target] = rb;
}

int bytes_for_internal_format(GLenum internal_format) {

}

void TexStorage3D(
        GLenum target,
        GLenum levels,
        GLenum internal_format,
        GLenum width,
        GLenum height,
        GLenum depth
    ) {
    Texture &t = textures[current_texture[target]];

    t.levels = levels;
    t.internal_format = internal_format;
    t.width = width;
    t.height = height;
    t.depth = depth;
    size_t size = bytes_for_internal_format(internal_format) * width * height * depth * levels;
    t.buf = (char*)malloc(size);
}

void TexStorage2D(
        GLenum target,
        GLenum levels,
        GLenum internal_format,
        GLenum width,
        GLenum height
    ) {
    Texture &t = textures[current_texture[target]];
    t.levels = levels;
    t.internal_format = internal_format;
    t.width = width;
    t.height = height;
    size_t size = bytes_for_internal_format(internal_format) * width * height * levels;
    t.buf = (char*)malloc(size);
}

void TexSubImage2D(
        GLenum target,
        GLint level,
        GLint xoffset,
        GLint yoffset,
        GLsizei width,
        GLsizei height,
        GLenum format,
        GLenum ty,
        void *data) {
        assert(xoffset == 0);
        assert(yoffset == 0);
}

void GenTextures(int n, int *result) {
        for (int i = 0; i < n; i++) {
                Texture t;
                textures.insert(pair<GLuint, Texture>(texture_count, t));
                result[i] = texture_count++;
        }
}

void GenRenderbuffers(int n, int *result) {
        for (int i = 0; i < n; i++) {
                Renderbuffer r;
                renderbuffers.insert(pair<GLuint, Renderbuffer>(renderbuffer_count, r));
                result[i] = renderbuffer_count++;
        }
}


void GenFramebuffers(int n, int *result) {
        for (int i = 0; i < n; i++) {
                Framebuffer f;
                framebuffers.insert(pair<GLuint, Framebuffer>(framebuffer_count, f));
                result[i] = framebuffer_count++;
        }
}



}
