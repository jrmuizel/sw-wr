#include <assert.h>
#ifdef __MACH__
#include <mach/mach.h>
#include <mach/mach_time.h>
#else
#include <time.h>
#endif

#include <map>
#include "glsl.h"
#include <cmath>
#include <limits>
using namespace std;
#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893


typedef uint32_t GLuint;
typedef int32_t GLboolean;
typedef float GLfloat;

typedef int32_t GLint;
typedef int32_t GLsizei;
typedef uint32_t GLenum;
typedef size_t GLsizeiptr;

struct VertexAttrib {
        size_t size; // in bytes
        GLenum type;
        bool normalized;
        GLsizei stride;
        GLuint offset;
        bool enabled = false;
        GLuint divisor;
        int vertex_array;
        int vertex_buffer;
        char *buf; // XXX: this can easily dangle
        size_t buf_size; // this will let us bounds check
};
#define GL_RGBA32F                        0x8814
#define GL_RGBA8                          0x8058
#define GL_R8                             0x8229
#define GL_RGBA32I                        0x8D82

#define GL_UNSIGNED_BYTE                  0x1401
#define GL_UNSIGNED_SHORT                 0x1403
#define GL_INT                            0x1404
#define GL_FLOAT                          0x1406

int bytes_for_internal_format(GLenum internal_format) {
        switch (internal_format) {
                case GL_RGBA32F:
                        return 4*4;
                case GL_RGBA32I:
                        return 4*4;
                case GL_RGBA8:
                        return 4;
                case GL_R8:
                        return 1;
                default:
                        printf("internal format: %x\n", internal_format);
                        assert(0);
        }
}

glsl::TextureFormat gl_format_to_texture_format(int type) {
        switch (type) {
                case GL_RGBA32F: return glsl::TextureFormat::RGBA32F;
                case GL_RGBA32I: return glsl::TextureFormat::RGBA32I;
                case GL_RGBA8: return glsl::TextureFormat::RGBA8;
                case GL_R8: return glsl::TextureFormat::R8;
                defaut: assert(0);
        }
}

struct Buffer {
        char *buf;
        size_t size;
};

struct Framebuffer {
        GLint color_attachment;
        GLint layer;
};

struct Renderbuffer {
};

struct Program {
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

#define MAX_ATTRS 16
struct VertexArray {
    VertexAttrib attribs[MAX_ATTRS];
};

struct Shader {
};

map<GLuint, Buffer> buffers;
map<GLuint, Texture> textures;
map<GLuint, VertexArray> vertex_arrays;
map<GLuint, Framebuffer> framebuffers;
map<GLuint, Renderbuffer> renderbuffers;
map<GLuint, Shader> shaders;
map<GLuint, Program> programs;

GLuint buffer_count;
map<GLenum, GLuint> current_buffer;

GLuint texture_count;
map<GLenum, GLuint> current_texture;
map<GLenum, GLuint> current_framebuffer;
map<GLenum, GLuint> current_renderbuffer;

GLuint vertex_array_count;
GLuint current_vertex_array;
GLuint shader_count;

GLuint renderbuffer_count;
GLuint framebuffer_count;
GLuint program_count;

struct Viewport {
	int x;
	int y;
	int width;
	int height;
};

Viewport viewport;

using namespace glsl;

#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1
#define GL_TEXTURE2                       0x84C2
#define GL_TEXTURE3                       0x84C3
#define GL_TEXTURE4                       0x84C4
#define GL_TEXTURE5                       0x84C5
#define GL_TEXTURE6                       0x84C6
#define GL_TEXTURE7                       0x84C7
#define GL_TEXTURE8                       0x84C8
#define GL_TEXTURE9                       0x84C9
#define GL_TEXTURE10                      0x84CA
#define GL_TEXTURE11                      0x84CB
#define GL_TEXTURE12                      0x84CC
#define GL_TEXTURE13                      0x84CD
#define GL_TEXTURE14                      0x84CE
#define GL_TEXTURE15                      0x84CF
#define GL_TEXTURE16                      0x84D0

GLenum active_texture;
GLuint texture_slots[16];

sampler2D lookup_sampler(int texture) {
        sampler2D s = new sampler2D_impl; //XXX: going to leak it
        Texture &t = textures[texture_slots[texture]];
        s->width = t.width;
        s->height = t.height;
        s->stride = bytes_for_internal_format(t.internal_format) * t.width;
        s->buf = (uint32_t*)t.buf; //XXX: wrong
        s->format = gl_format_to_texture_format(t.internal_format);
        return s;
}

isampler2D lookup_isampler(int texture) {
        isampler2D s = new isampler2D_impl; //XXX: going to leak it
        Texture &t = textures[texture_slots[texture]];
        s->width = t.width;
        s->height = t.height;
        s->stride = bytes_for_internal_format(t.internal_format) * t.width;
        s->buf = (uint32_t*)t.buf; //XXX: wrong
        s->format = gl_format_to_texture_format(t.internal_format);
        return s;
}

sampler2DArray lookup_sampler_array(int texture) {
        sampler2DArray s = new sampler2DArray_impl; //XXX: going to leak it
        Texture &t = textures[texture_slots[texture]];
        s->width = t.width;
        s->height = t.height;
        s->depth = t.depth;
        s->stride = bytes_for_internal_format(t.internal_format) * t.width;
        s->height_stride = bytes_for_internal_format(t.internal_format) * t.width * t.height;
        s->buf = (uint32_t*)t.buf; //XXX: wrong
        s->format = gl_format_to_texture_format(t.internal_format);
        return s;
}


vec4 gl_Position;
vec4 gl_FragCoord;
#include "brush_solid.h"
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
extern "C" {

void SetViewport(int x, int y, int width, int height) {
	viewport.x = x;
	viewport.y = y;
	viewport.width = width;
	viewport.height = height;

}


void ActiveTexture(GLenum texture) {
        assert(texture >= GL_TEXTURE0);
        assert(texture <= GL_TEXTURE16); // this is just arbitrary
        active_texture = texture - GL_TEXTURE0;
}

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

GLuint CreateShader(GLenum type) {
        Shader s;
        shaders.insert(pair<GLuint, Shader>(shader_count, s));
        return shader_count++;
}

GLuint CreateProgram() {
        Program p;
        programs.insert(pair<GLuint, Program>(program_count, p));
        return program_count++;
}

void BindAttribLocation(GLuint program, GLuint index, char *name) {
        brush_solid_vert::bind_attrib_location(name, index);
}

GLint GetUniformLocation(GLuint program, char* name) {
        Program &p = programs[program];
        GLint loc = brush_solid_get_uniform_location(name);
        printf("location: %d\n", loc);
        return loc;
}

void BindVertexArray(GLuint vertex_array) {
    current_vertex_array = vertex_array;
}

void BindTexture(GLenum target, GLuint texture) {
    texture_slots[active_texture] = texture;
    current_texture[target] = texture;
}

void BindBuffer(GLenum target, GLuint buffer) {
    // XXX: I think we to set the element array buffer on the current vertex array for target == GL_ELEMENT_ARRAY_BUFFER
    current_buffer[target] = buffer;
}

void BindFramebuffer(GLenum target, GLuint fb) {
    current_framebuffer[target] = fb;
}

void BindRenderbuffer(GLenum target, GLuint rb) {
    current_renderbuffer[target] = rb;
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

#define GL_RED                            0x1903
#define GL_RGBA                           0x1908
#define GL_RGBA_INTEGER                   0x8D99
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
        Texture &t = textures[current_texture[target]];
        assert(xoffset + width <= t.width);
        assert(yoffset + height <= t.height);
        if (format == GL_RED) {
                assert(ty == GL_UNSIGNED_BYTE);
                char *dest = t.buf;
                char *src = (char*)data;
                for (int y = yoffset; y < height; y++) {
                        for (int x = xoffset; x < width; x++) {
                                dest[y * t.width + x] = *src++;
                        }
                }
        } else if (format == GL_RGBA) {
                assert(ty == GL_FLOAT);
                float *dest = (float*)t.buf;
                float *src = (float*)data;
                for (int y = yoffset; y < height; y++) {
                        for (int x = 0; x < width; x++) {
                                dest[y * t.width * 4 + x * 4] = *src++;
                                dest[y * t.width * 4 + x * 4 + 1] = *src++;
                                dest[y * t.width * 4 + x * 4 + 2] = *src++;
                                dest[y * t.width * 4 + x * 4 + 3] = *src++;
                        }
                }
        } else {
                assert(format == GL_RGBA_INTEGER);
                assert(ty == GL_INT);
                int *dest = (int*)t.buf;
                int *src = (int*)data;
                for (int y = yoffset; y < height; y++) {
                        for (int x = 0; x < width; x++) {
                                dest[y * t.width * 4 + x * 4] = *src++;
                                dest[y * t.width * 4 + x * 4 + 1] = *src++;
                                dest[y * t.width * 4 + x * 4 + 2] = *src++;
                                dest[y * t.width * 4 + x * 4 + 3] = *src++;
                        }
                }
        }
}



void TexSubImage3D(
        GLenum target,
        GLint level,
        GLint xoffset,
        GLint yoffset,
        GLint zoffset,
        GLsizei width,
        GLsizei height,
        GLsizei depth,
        GLenum format,
        GLenum ty,
        void *data) {
        Texture &t = textures[current_texture[target]];
        assert(format == GL_RGBA);
        assert(ty == GL_UNSIGNED_BYTE);
        char *dest = t.buf;
        char *src = (char*)data;
        for (int y = yoffset; y < height; y++) {
                for (int x = 0; x < width; x++) {
                        dest[y * t.width * 4 + x * 4] = *src++;
                        dest[y * t.width * 4 + x * 4 + 1] = *src++;
                        dest[y * t.width * 4 + x * 4 + 2] = *src++;
                        dest[y * t.width * 4 + x * 4 + 3] = *src++;
                }
        }

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

int bytes_per_type(GLenum type) {
        switch (type) {
                case GL_INT: return 4;
                case GL_FLOAT: return 4;
                case GL_UNSIGNED_SHORT: return 4;
                default: assert(0);
        }
}

void VertexAttribPointer(GLuint index,
        GLint size,
        GLenum type,
        bool normalized,
        GLsizei stride,
        GLuint offset)
{
        printf("cva: %d\n", current_vertex_array);
        VertexArray &v = vertex_arrays[current_vertex_array];
        VertexAttrib &va = v.attribs[index];
        va.size = size * bytes_per_type(type);
        va.type = type;
        va.normalized = normalized;
        va.stride = stride;
        va.offset = offset;
        Buffer &vertex_buf = buffers[current_buffer[GL_ARRAY_BUFFER]];
        va.vertex_buffer = current_buffer[GL_ARRAY_BUFFER];
        va.vertex_array = current_vertex_array;
}

void VertexAttribIPointer(GLuint index,
        GLint size,
        GLenum type,
        GLsizei stride,
        GLuint offset)
{
        printf("cva: %d\n", current_vertex_array);
        VertexArray &v = vertex_arrays[current_vertex_array];
        VertexAttrib &va = v.attribs[index];
        va.size = size * bytes_per_type(type);
        va.type = type;
        va.stride = stride;
        va.offset = offset;
        Buffer &vertex_buf = buffers[current_buffer[GL_ARRAY_BUFFER]];
        va.vertex_buffer = current_buffer[GL_ARRAY_BUFFER];
        va.vertex_array = current_vertex_array;
}

void EnableVertexAttribArray(GLuint index) {
        VertexArray &v = vertex_arrays[current_vertex_array];
        VertexAttrib &va = v.attribs[index];
        va.enabled = true;
}

void DisableVertexAttribArray(GLuint index) {
        VertexArray &v = vertex_arrays[current_vertex_array];
        VertexAttrib &va = v.attribs[index];
        va.enabled = false;
}



void VertexAttribDivisor(GLuint index, GLuint divisor) {
        VertexArray &v = vertex_arrays[current_vertex_array];
        VertexAttrib &va = v.attribs[index];
        va.divisor = divisor;
}



void BufferData(GLenum target,
        GLsizeiptr size,
        void *data,
        GLenum usage)
{
    Buffer &b = buffers[current_buffer[target]];
    //XXX: leak the stuff
    b.buf = (char*)malloc(size);
    memcpy(b.buf, data, size);
    b.size = size;
}

brush_solid_vert shader;
brush_solid_frag frag_shader;
void Uniform1i(GLint location, GLint V0) {
        printf("tex: %d\n", texture_count);
        shader.set_uniform_int(location, V0);
        frag_shader.set_uniform_int(location, V0);
}
void Uniform4fv(GLint location, GLsizei count, float *v) {
        shader.set_uniform_4f(location, v);
        frag_shader.set_uniform_4f(location, v);
}
void UniformMatrix4fv(GLint location,
 	GLsizei count,
 	GLboolean transpose,
 	const GLfloat *value) {
        shader.set_uniform_matrix4fv(location, value);
        frag_shader.set_uniform_matrix4fv(location, value);
}



#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_STENCIL_ATTACHMENT             0x8D20
void FramebufferTexture2D(
        GLenum target,
        GLenum attachment,
        GLenum textarget,
        GLuint texture,
        GLint level)
{
        if (attachment == GL_COLOR_ATTACHMENT0) {
               Framebuffer &fb = framebuffers[current_framebuffer[target]];
               fb.color_attachment = texture;
               fb.layer = 0;
        } else {
                assert(0);
        }
}

#define GL_DRAW_FRAMEBUFFER               0x8CA9

void FramebufferTextureLayer(
        GLenum target,
        GLenum attachment,
        GLuint texture,
        GLint level,
        GLint layer)
{
        assert(level == 0);
        assert(target == GL_DRAW_FRAMEBUFFER);
        if (attachment == GL_COLOR_ATTACHMENT0) {
               Framebuffer &fb = framebuffers[current_framebuffer[target]];
               fb.color_attachment = texture;
               fb.layer = layer;
        } else {
                assert(0);
        }
}

#define GL_POINTS                         0x0000
#define GL_LINES                          0x0001
#define GL_LINE_LOOP                      0x0002
#define GL_LINE_STRIP                     0x0003
#define GL_TRIANGLES                      0x0004
#define GL_TRIANGLE_STRIP                 0x0005
#define GL_TRIANGLE_FAN                   0x0006

void LinkProgram(GLuint program) {
}

struct Point {
        float x;
        float y;
};

struct Vec3f {
        float x;
        float y;
        float z;
};


Vec3f cross(Vec3f v1, Vec3f v2) {
    return Vec3f{v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x};
}

Vec3f barycentric(Point a, Point b, Point c, Point p) {
        Vec3f s[2];
        s[0].x = c.x - a.x;
        s[0].y = b.x - a.x;
        s[0].z = a.x - p.x;

        s[1].x = c.y - a.y;
        s[1].y = b.y - a.y;
        s[1].z = a.y - p.y;

        Vec3f u = cross(s[0], s[1]);

        if (std::abs(u.z)>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
                return Vec3f{1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z};
        return Vec3f{-1,1,1}; // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

void triangle(brush_solid_frag &shader, brush_solid_vert::FlatOutputs &flat_outs, brush_solid_vert::InterpOutputs interp_outs[4], Point a, Point b, Point c) {
#ifdef  __MACH__
        double start = mach_absolute_time();
#else
        double start = ({ struct timespec tp; clock_gettime(CLOCK_MONOTONIC, &tp); tp.tv_sec * 1e9 + tp.tv_nsec; });
#endif

        auto top_left = Point{std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
        auto bot_right = Point{std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()};

        top_left.x = std::min(top_left.x, a.x);
        top_left.x = std::min(top_left.x, b.x);
        top_left.x = std::min(top_left.x, c.x);

        top_left.y = std::min(top_left.y, a.y);
        top_left.y = std::min(top_left.y, b.y);
        top_left.y = std::min(top_left.y, c.y);

        bot_right.x = std::max(bot_right.x, a.x);
        bot_right.x = std::max(bot_right.x, b.x);
        bot_right.x = std::max(bot_right.x, c.x);

        bot_right.y = std::max(bot_right.y, a.y);
        bot_right.y = std::max(bot_right.y, b.y);
        bot_right.y = std::max(bot_right.y, c.y);

        printf("bbox: %f %f %f %f\n", top_left.x, top_left.y, bot_right.x, bot_right.y);

        Point p[3] = { a, b, c };
        Point l0, r0, l1, r1;
        int l0i, r0i, l1i, r1i;
        {
            int top = b.y < a.y ? (c.y < b.y ? 2 : 1) : (c.y < a.y ? 2 : 0);
            int next = (top+1)%3;
            int prev = (top+2)%3;
            if (p[top].y == p[next].y) {
                l0i = next;
                r0i = top;
                l1i = r1i = prev;
            } else if (p[top].y == p[prev].y) {
                l0i = top;
                r0i = prev;
                l1i = r1i = next;
            } else {
                l0i = r0i = top;
                l1i = next;
                r1i = prev;
            }
            l0 = p[l0i];
            r0 = p[r0i];
            l1 = p[l1i];
            r1 = p[r1i];
        }

        Point l = l0;
        float lm = (l1.x - l0.x) / (l1.y - l0.y);
        Point r = r0;
        float rm = (r1.x - r0.x) / (r1.y - r0.y);
        assert(l.y == r.y);
        l.y = r.y = floor(l.y + 0.5f) + 0.5f;
        l.x += (l.y - l0.y) * lm;
        r.x += (r.y - r0.y) * rm;
        brush_solid_vert::InterpOutputs lo = interp_outs[l0i];
        brush_solid_vert::InterpOutputs lom = (interp_outs[l1i] - lo) * (1.0f / (l1.y - l0.y));
        lo = lo + lom * (l.y - l0.y);
        brush_solid_vert::InterpOutputs ro = interp_outs[r0i];
        brush_solid_vert::InterpOutputs rom = (interp_outs[r1i] - ro) * (1.0f / (r1.y - r0.y));
        ro = ro + rom * (r.y - r0.y);
        int shaded_pixels = 0;
        frag_shader.read_flat_inputs(flat_outs);
        while (true) {
            if (l.y > l1.y) {
                l0i = l1i;
                l0 = l1;
                l1i = (l1i+1)%3;
                l1 = p[l1i];
                if (l1.y <= l0.y) break;
                lm = (l1.x - l0.x) / (l1.y - l0.y);
                l.x = l0.x + (l.y - l0.y) * lm;
                lo = interp_outs[l0i];
                lom = (interp_outs[l1i] - lo) * (1.0f / (l1.y - l0.y));
                lo = lo + lom * (l.y - l0.y);
            }
            if (r.y > r1.y) {
                r0i = r1i;
                r0 = r1;
                r1i = (r1i+2)%3;
                r1 = p[r1i];
                if (r1.y <= r0.y) break;
                rm = (r1.x - r0.x) / (r1.y - r0.y);
                r.x = r0.x + (r.y - r0.y) * rm;
                ro = interp_outs[r0i];
                rom = (interp_outs[r1i] - ro) * (1.0f / (r1.y - r0.y));
                ro = ro + rom * (r.y - r0.y);
            }
            Point p = l;
            p.x = floor(l.x + 0.5f) + 0.5f;
            if (p.x < r.x) {
                brush_solid_vert::InterpOutputs stepo = (ro - lo) * (1.0f / (r.x - l.x));
                {
                    brush_solid_vert::InterpOutputs o0 = lo + stepo * (p.x - l.x);
                    brush_solid_vert::InterpOutputs o1 = o0 + stepo;
                    brush_solid_vert::InterpOutputs o2 = o1 + stepo;
                    brush_solid_vert::InterpOutputs o3 = o2 + stepo;
                    frag_shader.read_interp_inputs(o0, o1, o2, o3);
                }
                stepo = stepo * 4;
                for (; p.x < r.x; p.x += 4) {
                    shaded_pixels += 4;
                    frag_shader.main();
                    frag_shader.step_interp_inputs(stepo);
                }
            }
            l.x += lm;
            l.y++;
            r.x += rm;
            r.y++;
            lo = lo + lom;
            ro = ro + rom;
        }

        printf("color %f %f %f %f\n",
               frag_shader.oFragColor.x.x,
               frag_shader.oFragColor.y.x,
               frag_shader.oFragColor.z.x,
               frag_shader.oFragColor.w.x);
#ifdef  __MACH__
        double end = mach_absolute_time();
#else
        double end = ({ struct timespec tp; clock_gettime(CLOCK_MONOTONIC, &tp); tp.tv_sec * 1e9 + tp.tv_nsec; });
#endif
        printf("%fms for %d pixels\n", (end - start)/(1000.*1000.), shaded_pixels);
}

void DrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, void *indices, GLsizei instancecount) {
        assert(mode == GL_TRIANGLES);
        assert(type == GL_UNSIGNED_SHORT);
        assert(count == 6);
        assert(instancecount == 1);
        assert(indices == 0);
        if (indices == 0) {
                Buffer &indices_buf = buffers[current_buffer[GL_ELEMENT_ARRAY_BUFFER]];
                printf("current_vertex_array %d\n", current_vertex_array);
                printf("indices size: %d\n", indices_buf.size);
                VertexArray &v = vertex_arrays[current_vertex_array];
                for (int i = 0; i < MAX_ATTRS; i++) {
                        if (v.attribs[i].enabled) {
                                VertexAttrib &attr = v.attribs[i];
                                VertexArray &v = vertex_arrays[attr.vertex_array];
                                Buffer &vertex_buf = buffers[attr.vertex_buffer];
                                attr.buf = vertex_buf.buf;
                                attr.buf_size = vertex_buf.size;
                                printf("%d %x %d %d %d\n", i, attr.type, attr.size, attr.stride, attr.offset);
                        }
                }
                if (type == GL_UNSIGNED_SHORT) {
                        assert(indices_buf.size == count * 2);
                        unsigned short *indices = (unsigned short*)indices_buf.buf;
                        for (int i = 0; i < count; i+=3) {
                                printf(" %d\n", indices[i]);
                                shader.load_attribs_for_tri(v.attribs, indices, i);
                                shader.main();
                                brush_solid_vert::FlatOutputs flat_outs;
                                shader.store_flat_outputs(flat_outs);
                                brush_solid_vert::InterpOutputs interp_outs[4];
                                shader.store_interp_outputs(interp_outs);
                                Point a;
                                Point b;
                                Point c;
                                {
                                        printf("%f %f %f %f\n", gl_Position.x.x, gl_Position.y.x, gl_Position.z.x, gl_Position.y.x);
                                        float xw = (gl_Position.x.x + 1)*(viewport.width/2) + viewport.x;
                                        float yw = (gl_Position.y.x + 1)*(viewport.height/2) + viewport.y;
                                        a = Point { xw, yw };
                                        printf("%f %f\n", xw, yw);
                                }
                                {
                                        printf("%f %f %f %f\n", gl_Position.x.y, gl_Position.y.y, gl_Position.z.y, gl_Position.y.y);
                                        float xw = (gl_Position.x.y + 1)*(viewport.width/2) + viewport.x;
                                        float yw = (gl_Position.y.y + 1)*(viewport.height/2) + viewport.y;
                                        b = Point { xw, yw };
                                        printf("%f %f\n", xw, yw);
                                }
                                {
                                        printf("%f %f %f %f\n", gl_Position.x.z, gl_Position.y.z, gl_Position.z.z, gl_Position.y.z);
                                        float xw = (gl_Position.x.z + 1)*(viewport.width/2) + viewport.x;
                                        float yw = (gl_Position.y.z + 1)*(viewport.height/2) + viewport.y;
                                        c = Point { xw, yw };
                                        printf("%f %f\n", xw, yw);
                                }
                                triangle(frag_shader, flat_outs, interp_outs, a, b, c);
                        }

                } else {
                        assert(0);
                }
        }
        printf("dodraw");
}

}
