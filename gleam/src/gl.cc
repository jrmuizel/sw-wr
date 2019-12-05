#include <assert.h>
#ifdef __MACH__
#include <mach/mach.h>
#include <mach/mach_time.h>
#else
#include <time.h>
#endif

#include <map>
#include "minpng.h"
#include "glsl.h"
#include <cmath>
#include <limits>
using namespace std;
#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893


typedef uint32_t GLuint;
typedef int32_t GLboolean;
typedef float GLfloat;
typedef double GLdouble;
typedef uint32_t GLbitfield;

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

#define GL_DEPTH_COMPONENT                0x1902
#define GL_DEPTH_COMPONENT16              0x81A5
#define GL_DEPTH_COMPONENT24              0x81A6
#define GL_DEPTH_COMPONENT32              0x81A7

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
                case GL_DEPTH_COMPONENT:
                case GL_DEPTH_COMPONENT16:
                        return 2;
                case GL_DEPTH_COMPONENT24:
                case GL_DEPTH_COMPONENT32:
                        return 4;
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
        GLint depth_attachment;

        Framebuffer() : color_attachment(0), layer(0), depth_attachment(0) {}
};

struct Renderbuffer {
        GLint texture;

        Renderbuffer() : texture(0) {}
};

struct Program {
};

struct Texture {
    GLenum target;
    int levels;
    GLenum internal_format;
    int width;
    int height;
    int depth;
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

GLuint buffer_count = 1;
map<GLenum, GLuint> current_buffer;

GLuint texture_count = 1;
map<GLenum, GLuint> current_texture;
map<GLenum, GLuint> current_framebuffer;
map<GLenum, GLuint> current_renderbuffer;

GLuint vertex_array_count = 1;
GLuint current_vertex_array;
GLuint shader_count = 1;

GLuint renderbuffer_count = 1;
GLuint framebuffer_count = 1;
GLuint program_count = 1;

struct Viewport {
	int x;
	int y;
	int width;
	int height;
};

Viewport viewport;

#define GL_BLEND                0x0BE2
#define GL_ZERO                 0
#define GL_ONE                  1
#define GL_SRC_COLOR            0x0300
#define GL_ONE_MINUS_SRC_COLOR  0x0301
#define GL_SRC_ALPHA            0x0302
#define GL_ONE_MINUS_SRC_ALPHA  0x0303
#define GL_DST_ALPHA            0x0304
#define GL_ONE_MINUS_DST_ALPHA  0x0305
#define GL_DST_COLOR            0x0306
#define GL_ONE_MINUS_DST_COLOR  0x0307
#define GL_CONSTANT_COLOR           0x8001U
#define GL_ONE_MINUS_CONSTANT_COLOR 0x8002U
#define GL_CONSTANT_ALPHA           0x8003U
#define GL_ONE_MINUS_CONSTANT_ALPHA 0x8004U

bool blend = false;
GLenum blendfunc_srgb = GL_ONE;
GLenum blendfunc_drgb = GL_ZERO;
GLenum blendfunc_sa = GL_ONE;
GLenum blendfunc_da = GL_ZERO;
bool blendfunc_separate = false;

#define GL_FUNC_ADD             0x8006

GLenum blend_equation_mode = GL_FUNC_ADD;

__m128i blendcolor = _mm_set1_epi16(0);

// for GL defines to fully expand
#define EXPAND_KEY(x, y, z, w) x ## y ## z ## w
#define BLEND_KEY(x, y) EXPAND_KEY(BLEND_KEY_, x, _, y)
#define FOR_EACH_BLEND_KEY(macro) \
    macro(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) \
    macro(GL_CONSTANT_COLOR, GL_ONE_MINUS_SRC_COLOR) \
    macro(GL_ONE, GL_ONE_MINUS_SRC_COLOR) \
    macro(GL_ONE, GL_ONE_MINUS_SRC_ALPHA) \
    macro(GL_ZERO, GL_ONE_MINUS_SRC_COLOR) \
    macro(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA) \
    macro(GL_ZERO, GL_SRC_COLOR) \
    macro(GL_ZERO, GL_SRC_ALPHA) \
    macro(GL_ONE, GL_ONE) \
    macro(GL_ZERO, GL_ONE) \
    macro(GL_ONE, GL_ZERO) \
    macro(GL_ONE_MINUS_DST_ALPHA, GL_ONE)

#define DEFINE_BLEND_KEY(x, y) BLEND_KEY(x, y),
enum BlendKey : uint8_t
{
    BLEND_KEY_NONE = 0,
    FOR_EACH_BLEND_KEY(DEFINE_BLEND_KEY)
};

BlendKey blend_key = BLEND_KEY_NONE;

#define BLEND_ALPHA_KEY(x, y) EXPAND_KEY(BLEND_ALPHA_KEY_, x, _, y)
#define FOR_EACH_BLEND_ALPHA_KEY(macro) \
    macro(GL_ONE, GL_ONE) \
    macro(GL_ZERO, GL_ONE) \
    macro(GL_ONE, GL_ZERO) \
    macro(GL_ONE, GL_ONE_MINUS_SRC_COLOR) \
    macro(GL_ZERO, GL_SRC_COLOR)

#define DEFINE_BLEND_ALPHA_KEY(x, y) BLEND_ALPHA_KEY(x, y),
enum BlendAlphaKey : uint8_t
{
    BLEND_ALPHA_KEY_NONE = 0,
    FOR_EACH_BLEND_ALPHA_KEY(DEFINE_BLEND_ALPHA_KEY)
};

BlendAlphaKey blend_alpha_key = BLEND_ALPHA_KEY_NONE;


#define GL_NEVER                0x0200
#define GL_LESS                 0x0201
#define GL_EQUAL                0x0202
#define GL_LEQUAL               0x0203
#define GL_GREATER              0x0204
#define GL_NOTEQUAL             0x0205
#define GL_GEQUAL               0x0206
#define GL_ALWAYS               0x0207
#define GL_DEPTH_TEST           0x0B71

bool depthtest = false;
bool depthmask = true;
GLenum depthfunc = GL_LESS;

#define GL_SCISSOR_TEST             0x0C11

bool scissortest = false;

struct Scissor {
    int x;
    int y;
    int width;
    int height;
};

Scissor scissor;

GLfloat clearcolor[4] = { 0, 0, 0, 0 };
GLdouble cleardepth = 1;

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
        if (!texture_slots[texture]) {
            s->width = 0;
            s->height = 0;
            s->stride = 0;
            s->buf = nullptr;
            s->format = TextureFormat::RGBA8;
        } else {
            Texture &t = textures[texture_slots[texture]];
            s->width = t.width;
            s->height = t.height;
            s->stride = bytes_for_internal_format(t.internal_format) * t.width;
            s->buf = (uint32_t*)t.buf; //XXX: wrong
            s->format = gl_format_to_texture_format(t.internal_format);
        }
        return s;
}

isampler2D lookup_isampler(int texture) {
        isampler2D s = new isampler2D_impl; //XXX: going to leak it
        if (!texture_slots[texture]) {
            s->width = 0;
            s->height = 0;
            s->stride = 0;
            s->buf = nullptr;
            s->format = TextureFormat::RGBA32I;
        } else {
            Texture &t = textures[texture_slots[texture]];
            s->width = t.width;
            s->height = t.height;
            s->stride = bytes_for_internal_format(t.internal_format) * t.width;
            s->buf = (uint32_t*)t.buf; //XXX: wrong
            s->format = gl_format_to_texture_format(t.internal_format);
        }
        return s;
}

sampler2DArray lookup_sampler_array(int texture) {
        sampler2DArray s = new sampler2DArray_impl; //XXX: going to leak it
        if (!texture_slots[texture]) {
            s->width = 0;
            s->height = 0;
            s->depth = 0;
            s->stride = 0;
            s->height_stride = 0;
            s->buf = nullptr;
            s->format = TextureFormat::RGBA8;
        } else {
            Texture &t = textures[texture_slots[texture]];
            s->width = t.width;
            s->height = t.height;
            s->depth = t.depth;
            s->stride = bytes_for_internal_format(t.internal_format) * t.width;
            s->height_stride = bytes_for_internal_format(t.internal_format) * t.width * t.height;
            s->buf = (uint32_t*)t.buf; //XXX: wrong
            s->format = gl_format_to_texture_format(t.internal_format);
        }
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

void SetViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
	viewport.x = x;
	viewport.y = y;
	viewport.width = width;
	viewport.height = height;
}

void Enable(GLenum cap) {
    switch (cap) {
    case GL_BLEND: blend = true; break;
    case GL_DEPTH_TEST: depthtest = true; break;
    case GL_SCISSOR_TEST: scissortest = true; break;
    }
}

void Disable(GLenum cap) {
    switch (cap) {
    case GL_BLEND: blend = false; break;
    case GL_DEPTH_TEST: depthtest = false; break;
    case GL_SCISSOR_TEST: scissortest = false; break;
    }
}

void BlendFunc(GLenum srgb, GLenum drgb, GLenum sa, GLenum da) {
    blendfunc_srgb = srgb;
    blendfunc_drgb = drgb;
    switch ((srgb << 16) | drgb) {
    #define MAP_BLEND_KEY(x, y) \
        case (x << 16) | y: \
            blend_key = BLEND_KEY(x, y); \
            break;
    FOR_EACH_BLEND_KEY(MAP_BLEND_KEY)
    default:
        assert(false);
        break;
    }

    switch (sa) {
    case GL_SRC_ALPHA: sa = GL_SRC_COLOR; break;
    case GL_ONE_MINUS_SRC_ALPHA: sa = GL_ONE_MINUS_SRC_COLOR; break;
    case GL_DST_ALPHA: sa = GL_DST_COLOR; break;
    case GL_ONE_MINUS_DST_ALPHA: sa = GL_ONE_MINUS_DST_COLOR; break;
    case GL_CONSTANT_ALPHA: sa = GL_CONSTANT_COLOR; break;
    case GL_ONE_MINUS_CONSTANT_ALPHA: sa = GL_ONE_MINUS_CONSTANT_COLOR; break;
    }
    blendfunc_sa = sa;
    switch (da) {
    case GL_SRC_ALPHA: da = GL_SRC_COLOR; break;
    case GL_ONE_MINUS_SRC_ALPHA: da = GL_ONE_MINUS_SRC_COLOR; break;
    case GL_DST_ALPHA: da = GL_DST_COLOR; break;
    case GL_ONE_MINUS_DST_ALPHA: da = GL_ONE_MINUS_DST_COLOR; break;
    case GL_CONSTANT_ALPHA: da = GL_CONSTANT_COLOR; break;
    case GL_ONE_MINUS_CONSTANT_ALPHA: da = GL_ONE_MINUS_CONSTANT_COLOR; break;
    }
    blendfunc_da = da;
    blendfunc_separate = srgb != sa || drgb != da;
    blend_alpha_key = BLEND_ALPHA_KEY_NONE;
    if (blendfunc_separate) {
        switch ((sa << 16) | da) {
        #define MAP_BLEND_ALPHA_KEY(x, y) \
            case (x << 16) | y: \
                blend_alpha_key = BLEND_ALPHA_KEY(x, y); \
                break;
        FOR_EACH_BLEND_ALPHA_KEY(MAP_BLEND_ALPHA_KEY)
        default:
            assert(false);
            break;
        }
    }
}

void BlendColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
        __m128i c32 = _mm_cvtps_epi32(_mm_mul_ps(_mm_set_ps(a, b, g, r), _mm_set1_ps(255.5f)));
        blendcolor = _mm_packs_epi32(c32, c32);
}

void BlendEquation(GLenum mode) {
        assert(mode == GL_FUNC_ADD);
        blend_equation_mode = mode;
}

void DepthMask(GLboolean flag) {
    depthmask = flag;
}

void DepthFunc(GLenum func) {
    switch (func) {
    case GL_LESS:
    case GL_LEQUAL:
        break;
    default:
        assert(false);
    }
    depthfunc = func;
}

void SetScissor(GLint x, GLint y, GLsizei width, GLsizei height) {
    scissor.x = x;
    scissor.y = y;
    scissor.width = width;
    scissor.height = height;
}

void ClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    clearcolor[0] = r;
    clearcolor[1] = g;
    clearcolor[2] = b;
    clearcolor[3] = a;
}

void ClearDepth(GLdouble depth) {
    cleardepth = depth;
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
        GLint levels,
        GLenum internal_format,
        GLsizei width,
        GLsizei height,
        GLsizei depth
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
        GLint levels,
        GLenum internal_format,
        GLsizei width,
        GLsizei height
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

void RenderbufferStorage(
        GLenum target,
        GLenum internalformat,
        GLsizei width,
        GLsizei height) {
    // Just refer a renderbuffer to a texture to simplify things for now...
    Renderbuffer &r = renderbuffers[current_renderbuffer[target]];
    GenTextures(1, &r.texture);
    current_texture[target] = r.texture;
    switch (internalformat) {
        case GL_DEPTH_COMPONENT:
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32:
            // Force depth format to 16 bits...
            internalformat = GL_DEPTH_COMPONENT16;
            break;
    }
    TexStorage2D(target, 1, internalformat, width, height);
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

#define GL_RENDERBUFFER                   0x8D41

void FramebufferRenderbuffer(
    GLenum target,
    GLenum attachment,
    GLenum renderbuffertarget,
    GLuint renderbuffer)
{
    assert(target == GL_DRAW_FRAMEBUFFER);
    assert(renderbuffertarget == GL_RENDERBUFFER);
    Framebuffer &fb = framebuffers[current_framebuffer[target]];
    Renderbuffer &rb = renderbuffers[renderbuffer];
    if (attachment == GL_COLOR_ATTACHMENT0) {
        fb.color_attachment = rb.texture;
        fb.layer = 0;
    } else if (attachment == GL_DEPTH_ATTACHMENT) {
        fb.depth_attachment = rb.texture;
    }
}

} // extern "C"

Framebuffer& get_draw_framebuffer() {
    GLuint id = 0;
    {
        auto i = current_framebuffer.find(GL_DRAW_FRAMEBUFFER);
        if (i != current_framebuffer.end()) id = i->second;
    }
    {
        auto i = framebuffers.find(id);
        if (i != framebuffers.end()) return i->second;
    }
    // allocate a default framebuffer
    Framebuffer& fb = framebuffers[0];
    const size_t width = 2048;
    const size_t height = 2048;
    GenTextures(1, &fb.color_attachment);
    fb.layer = 0;
    current_texture[GL_DRAW_FRAMEBUFFER] = fb.color_attachment;
    TexStorage2D(GL_DRAW_FRAMEBUFFER, 1, GL_RGBA8, width, height);
    GenTextures(1, &fb.depth_attachment);
    current_texture[GL_DRAW_FRAMEBUFFER] = fb.depth_attachment;
    TexStorage2D(GL_DRAW_FRAMEBUFFER, 1, GL_DEPTH_COMPONENT16, width, height);
    return fb;
}

extern "C" {

#define GL_COLOR_BUFFER_BIT         0x00004000
#define GL_DEPTH_BUFFER_BIT         0x00000100
#define GL_STENCIL_BUFFER_BIT       0x00000400

void Clear(GLbitfield mask) {
    Framebuffer& fb = get_draw_framebuffer();
    if ((mask & GL_COLOR_BUFFER_BIT) && fb.color_attachment) {
        Texture& t = textures[fb.color_attachment];
        int x0 = 0, y0 = 0, x1 = t.width, y1 = t.height;
        if (scissortest) {
            x0 = std::max(x0, scissor.x);
            y0 = std::max(y0, scissor.y);
            x1 = std::min(x1, scissor.x + scissor.width);
            y1 = std::min(y1, scissor.y + scissor.height);
        }
        if (t.internal_format == GL_RGBA8) {
            __m128 colorf = _mm_loadu_ps(clearcolor);
            __m128i colori = _mm_cvtps_epi32(_mm_mul_ps(colorf, _mm_set1_ps(255.5f)));
            colori = _mm_packs_epi32(colori, colori);
            colori = _mm_packus_epi16(colori, colori);
            uint32_t color = _mm_cvtsi128_si32(colori);
            uint32_t* buf = (uint32_t*)t.buf + t.width * t.height * fb.layer + t.width * y0 + x0;
            for (int y = y0; y < y1; y++) {
                int x = x0;
                for (; x + 4 <= x1; x += 4, buf += 4) {
                    _mm_storeu_si128((__m128i*)buf, colori);
                }
                for (; x < x1; x++, buf++) {
                    *buf = color;
                }
                buf += t.width - (x1 - x0);
            }
        } else {
            assert(false);
        }
    }
    if ((mask & GL_DEPTH_BUFFER_BIT) && fb.depth_attachment) {
        Texture& t = textures[fb.depth_attachment];
        assert(t.internal_format == GL_DEPTH_COMPONENT16);
        int x0 = 0, y0 = 0, x1 = t.width, y1 = t.height;
        if (scissortest) {
            x0 = std::max(x0, scissor.x);
            y0 = std::max(y0, scissor.y);
            x1 = std::min(x1, scissor.x + scissor.width);
            y1 = std::min(y1, scissor.y + scissor.height);
        }
        uint16_t depth = uint16_t(0xFFFF * cleardepth) - 0x8000;
        __m128i depthi = _mm_set1_epi16(depth);
        uint16_t* buf = (uint16_t*)t.buf + t.width * y0 + x0;
        for (int y = y0; y < y1; y++) {
            int x = x0;
            for (; x + 8 <= x1; x += 8, buf += 8) {
                _mm_storeu_si128((__m128i*)buf, depthi);
            }
            for (; x < x1; x++, buf++) {
                *buf = depth;
            }
            buf += (t.width - x1) + x0;
        }
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

} // extern "C"

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

template<int FULL_SPANS>
static inline int check_depth(uint16_t z, int span, uint16_t* zbuf, int discarded = 0) {
    __m128i dest;
    if (FULL_SPANS > 1) {
        dest = _mm_loadu_si128((__m128i*)zbuf);
    } else if (FULL_SPANS) {
        dest = _mm_loadl_epi64((__m128i*)zbuf);
    } else if (span & 2) {
        dest = _mm_cvtsi32_si128(*(int32_t*)zbuf);
        if (span & 1) dest = _mm_unpacklo_epi32(dest, _mm_cvtsi32_si128(zbuf[2]));
    } else {
        dest = _mm_cvtsi32_si128(*(int32_t*)zbuf);
    }
    __m128i src = _mm_set1_epi16(z);
    // Invert the depth test to check which pixels failed and should be discarded.
    __m128i mask = depthfunc == GL_LEQUAL ?
        // GL_LEQUAL: Not(LessEqual) = Greater
        _mm_cmpgt_epi16(src, dest) :
        // GL_LESS: Not(Less) = GreaterEqual
        // No GreaterEqual in SSE2, so use Not(Less) directly.
        _mm_xor_si128(_mm_cmplt_epi16(src, dest), _mm_set1_epi16(0xFFFF));
    uint32_t failed = _mm_movemask_epi8(_mm_packus_epi16(mask, mask));
    if (FULL_SPANS > 1) {
        failed &= 0xFF;
    } else if (FULL_SPANS) {
        failed &= 0xF;
    } else {
        failed |= (0xF << span) & 0xF;
    }
    discarded |= failed;
    if (depthmask && discarded != (FULL_SPANS > 1 ? 0xFF : 0xF)) {
        if (FULL_SPANS > 1) {
            if (!discarded) {
                _mm_storeu_si128((__m128i*)zbuf, src);
            } else {
                _mm_storeu_si128((__m128i*)zbuf,
                    _mm_or_si128(_mm_and_si128(mask, dest),
                                 _mm_andnot_si128(mask, src)));
            }
        } else if (FULL_SPANS) {
            if (!discarded) {
                _mm_storel_epi64((__m128i*)zbuf, src);
            } else {
                _mm_storel_epi64((__m128i*)zbuf,
                    _mm_or_si128(_mm_and_si128(mask, dest),
                                 _mm_andnot_si128(mask, src)));
            }
        } else {
            if (!(discarded & 1)) zbuf[0] = z;
            if (!(discarded & 2)) zbuf[1] = z;
            if (!(discarded & 4)) zbuf[2] = z;
            if (!(discarded & 8)) zbuf[3] = z;
        }
    }
    return discarded;
}

static inline __m128i pack_pixels(vec4 output) {
    output *= 255.5f;
    __m128i rxz = _mm_packs_epi32(_mm_cvtps_epi32(output.x), _mm_cvtps_epi32(output.z));
    __m128i ryw = _mm_packs_epi32(_mm_cvtps_epi32(output.y), _mm_cvtps_epi32(output.w));
    __m128i rxy = _mm_unpacklo_epi16(rxz, ryw);
    __m128i rzw = _mm_unpackhi_epi16(rxz, ryw);
    __m128i rlo = _mm_unpacklo_epi32(rxy, rzw);
    __m128i rhi = _mm_unpackhi_epi32(rxy, rzw);
    return _mm_packus_epi16(rlo, rhi);
}

template<bool FULL_SPAN>
static inline __m128i blend_pixels(__m128i r, int span, const uint32_t* buf) {
    const __m128i zero = _mm_setzero_si128();
    __m128i slo = _mm_unpacklo_epi8(r, zero);
    __m128i shi = _mm_unpackhi_epi8(r, zero);
    __m128i dlo, dhi;
    if (FULL_SPAN) {
        __m128i dst = _mm_loadu_si128((__m128i*)buf);
        dlo = _mm_unpacklo_epi8(dst, zero);
        dhi = _mm_unpackhi_epi8(dst, zero);
    } else {
        dhi = zero;
        if (span & 2) {
            dlo = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)buf), zero);
            if (span & 1) dhi = _mm_unpacklo_epi8(_mm_cvtsi32_si128(buf[2]), zero);
        } else {
            dlo = _mm_unpacklo_epi8(_mm_cvtsi32_si128(buf[0]), zero);
        }
    }
    // (x*y + x) >> 8, cheap approximation of (x*y) / 255
    #define muldiv255(x, y) ({ __m128i b = x; _mm_srli_epi16(_mm_add_epi16(_mm_mullo_epi16(b, y), b), 8); })
    #define alphas(c) _mm_shufflehi_epi16(_mm_shufflelo_epi16(c, 0xFF), 0xFF)
    switch (blend_key) {
    case BLEND_KEY_NONE:
        break;
    case BLEND_KEY(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA):
        r = _mm_packus_epi16(
                _mm_add_epi16(muldiv255(_mm_sub_epi16(slo, dlo), alphas(slo)), dlo),
                _mm_add_epi16(muldiv255(_mm_sub_epi16(shi, dhi), alphas(shi)), dhi));
        break;
    case BLEND_KEY(GL_CONSTANT_COLOR, GL_ONE_MINUS_SRC_COLOR):
        r = _mm_packus_epi16(
                _mm_add_epi16(dlo, muldiv255(_mm_sub_epi16(blendcolor, dlo), slo)),
                _mm_add_epi16(dhi, muldiv255(_mm_sub_epi16(blendcolor, dhi), shi)));
        break;
    case BLEND_KEY(GL_ONE, GL_ONE_MINUS_SRC_COLOR):
        r = _mm_packus_epi16(
                _mm_add_epi16(slo, _mm_sub_epi16(dlo, muldiv255(dlo, slo))),
                _mm_add_epi16(shi, _mm_sub_epi16(dhi, muldiv255(dhi, shi))));
        break;
    case BLEND_KEY(GL_ONE, GL_ONE_MINUS_SRC_ALPHA):
        r = _mm_packus_epi16(
                _mm_add_epi16(slo, _mm_sub_epi16(dlo, muldiv255(dlo, alphas(slo)))),
                _mm_add_epi16(shi, _mm_sub_epi16(dhi, muldiv255(dhi, alphas(shi)))));
        break;
    case BLEND_KEY(GL_ZERO, GL_ONE_MINUS_SRC_COLOR):
        r = _mm_packus_epi16(
                _mm_sub_epi16(dlo, muldiv255(dlo, slo)),
                _mm_sub_epi16(dhi, muldiv255(dhi, shi)));
        break;
    case BLEND_KEY(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA):
        r = _mm_packus_epi16(
                _mm_sub_epi16(dlo, muldiv255(dlo, alphas(slo))),
                _mm_sub_epi16(dhi, muldiv255(dhi, alphas(shi))));
        break;
    case BLEND_KEY(GL_ZERO, GL_SRC_COLOR):
        r = _mm_packus_epi16(
                muldiv255(slo, dlo),
                muldiv255(shi, dhi));
        break;
    case BLEND_KEY(GL_ZERO, GL_SRC_ALPHA):
        r = _mm_packus_epi16(
                muldiv255(alphas(slo), dlo),
                muldiv255(alphas(shi), dhi));
        break;
    case BLEND_KEY(GL_ONE, GL_ONE):
        r = _mm_packus_epi16(_mm_add_epi16(slo, dlo), _mm_add_epi16(shi, dhi));
        break;
    case BLEND_KEY(GL_ZERO, GL_ONE):
        r = _mm_packus_epi16(dlo, dhi);
        break;
    case BLEND_KEY(GL_ONE, GL_ZERO):
        r = _mm_packus_epi16(slo, shi);
        break;
    case BLEND_KEY(GL_ONE_MINUS_DST_ALPHA, GL_ONE):
        r = _mm_packus_epi16(
                _mm_add_epi16(dlo, _mm_sub_epi16(slo, muldiv255(slo, alphas(slo)))),
                _mm_add_epi16(dhi, _mm_sub_epi16(shi, muldiv255(shi, alphas(shi)))));
        break;
    default:
        UNREACHABLE;
        break;
    }
    if (blend_alpha_key) {
        __m128i a;
        switch (blend_alpha_key) {
        case BLEND_ALPHA_KEY_NONE:
            a = r;
            break;
        case BLEND_ALPHA_KEY(GL_ONE, GL_ONE):
            a = _mm_packus_epi16(_mm_add_epi16(dlo, slo), _mm_add_epi16(dhi, shi));
            break;
        case BLEND_ALPHA_KEY(GL_ZERO, GL_ONE):
            a = _mm_packus_epi16(dlo, dhi);
            break;
        case BLEND_ALPHA_KEY(GL_ONE, GL_ZERO):
            a = _mm_packus_epi16(slo, shi);
            break;
        case BLEND_ALPHA_KEY(GL_ONE, GL_ONE_MINUS_SRC_COLOR):
            a = _mm_packus_epi16(
                    _mm_add_epi16(slo, _mm_sub_epi16(dlo, muldiv255(dlo, slo))),
                    _mm_add_epi16(shi, _mm_sub_epi16(dhi, muldiv255(dhi, shi))));
            break;
        case BLEND_ALPHA_KEY(GL_ZERO, GL_SRC_COLOR):
            a = _mm_packus_epi16(
                    muldiv255(dlo, slo),
                    muldiv255(dhi, shi));
            break;
        default:
            UNREACHABLE;
            break;
        }
        r = _mm_or_si128(_mm_and_si128(_mm_set1_epi32(0x00FFFFFF), r),
                         _mm_andnot_si128(_mm_set1_epi32(0x00FFFFFF), a));
    }
    return r;
}

template<bool FULL_SPAN>
static inline void write_pixels(__m128i r, int span, uint32_t* buf) {
    if (FULL_SPAN) {
        _mm_storeu_si128((__m128i*)buf, r);
    } else if (span & 2) {
        _mm_storel_epi64((__m128i*)buf, r);
        if (span & 1) buf[2] = _mm_cvtsi128_si32(_mm_shuffle_epi32(r, 0xAA));
    } else {
        buf[0] = _mm_cvtsi128_si32(r);
    }
}

template<bool FULL_SPAN>
static inline void discard_pixels(__m128i r, int span, uint32_t* buf, int discarded) {
    if (!FULL_SPAN) discarded |= 0xF << span;
    if (!(discarded & 1)) {
        if (!(discarded & 2)) {
            _mm_storel_epi64((__m128i*)buf, r);
        } else {
            buf[0] = _mm_cvtsi128_si32(r);
        }
    } else if (!(discarded & 2)) {
        buf[1] = _mm_cvtsi128_si32(_mm_shuffle_epi32(r, 0x55));
    }
    if (!(discarded & 4)) {
        if (!(discarded & 8)) {
            _mm_storeh_pd((double*)&buf[2], _mm_castsi128_pd(r));
        } else {
            buf[2] = _mm_cvtsi128_si32(_mm_shuffle_epi32(r, 0xAA));
        }
    } else if (!(discarded & 8)) {
        buf[3] = _mm_cvtsi128_si32(_mm_shuffle_epi32(r, 0xFF));
    }
}

static inline void commit_output_no_depth_test(int span, uint32_t* buf, int discarded = 0) {
    __m128i r = pack_pixels(frag_shader.get_output());
    if (blend) r = blend_pixels<true>(r, span, buf);
    if (!discarded) {
        write_pixels<true>(r, span, buf);
    } else if (discarded != 0xF) {
        discard_pixels<true>(r, span, buf, discarded);
    }
}

template<bool FULL_SPAN>
static inline void commit_output(int span, uint32_t* buf, uint16_t *depth, uint16_t z) {
    int discarded = _mm_movemask_ps(frag_shader.discard_mask());
    if (depthtest) discarded = check_depth<FULL_SPAN ? 1 : 0>(z, span, depth, discarded);
    if (discarded != 0xF) {
        __m128i r = pack_pixels(frag_shader.get_output());
        if (blend) r = blend_pixels<FULL_SPAN>(r, span, buf);
        if (!discarded) {
            write_pixels<FULL_SPAN>(r, span, buf);
        } else {
            discard_pixels<FULL_SPAN>(r, span, buf, discarded);
        }
    }
}

void triangle(brush_solid_frag &shader, brush_solid_vert::FlatOutputs &flat_outs, brush_solid_vert::InterpOutputs interp_outs[4], Point a, Point b, Point c, uint16_t z) {
        Framebuffer& fb = get_draw_framebuffer();
        Texture& colortex = textures[fb.color_attachment];
        assert(colortex.internal_format == GL_RGBA8);
        Texture& depthtex = textures[fb.depth_attachment];
        assert(depthtex.internal_format == GL_DEPTH_COMPONENT16);
        assert(colortex.width == depthtex.width && colortex.height == depthtex.height);
        float fx0 = 0;
        float fy0 = 0;
        float fx1 = colortex.width;
        float fy1 = colortex.height;
        uint32_t *fbuf = (uint32_t*)colortex.buf;
        uint16_t *fdepth = (uint16_t*)depthtex.buf;
        if (scissortest) {
            fx0 = std::max(fx0, float(scissor.x));
            fy0 = std::max(fy0, float(scissor.y));
            fx1 = std::min(fx1, float(scissor.x + scissor.width));
            fy1 = std::min(fy1, float(scissor.y + scissor.height));
        }

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

        float lx = l0.x;
        float lm = (l1.x - l0.x) / (l1.y - l0.y);
        float rx = r0.x;
        float rm = (r1.x - r0.x) / (r1.y - r0.y);
        assert(l0.y == r0.y);
        float y = floor(std::max(l0.y, fy0) + 0.5f) + 0.5f;
        lx += (y - l0.y) * lm;
        rx += (y - r0.y) * rm;
        brush_solid_vert::InterpOutputs lo = interp_outs[l0i];
        brush_solid_vert::InterpOutputs lom = (interp_outs[l1i] - lo) * (1.0f / (l1.y - l0.y));
        lo = lo + lom * (y - l0.y);
        brush_solid_vert::InterpOutputs ro = interp_outs[r0i];
        brush_solid_vert::InterpOutputs rom = (interp_outs[r1i] - ro) * (1.0f / (r1.y - r0.y));
        ro = ro + rom * (y - r0.y);
        int shaded_pixels = 0;
        int shaded_lines = 0;
        frag_shader.read_flat_inputs(flat_outs);
        fbuf += int(y) * colortex.width;
        fdepth += int(y) * depthtex.width;
        while (y < fy1) {
            if (y > l1.y) {
                l0i = l1i;
                l0 = l1;
                l1i = (l1i+1)%3;
                l1 = p[l1i];
                if (l1.y <= l0.y) break;
                lm = (l1.x - l0.x) / (l1.y - l0.y);
                lx = l0.x + (y - l0.y) * lm;
                lo = interp_outs[l0i];
                lom = (interp_outs[l1i] - lo) * (1.0f / (l1.y - l0.y));
                lo = lo + lom * (y - l0.y);
            }
            if (y > r1.y) {
                r0i = r1i;
                r0 = r1;
                r1i = (r1i+2)%3;
                r1 = p[r1i];
                if (r1.y <= r0.y) break;
                rm = (r1.x - r0.x) / (r1.y - r0.y);
                rx = r0.x + (y - r0.y) * rm;
                ro = interp_outs[r0i];
                rom = (interp_outs[r1i] - ro) * (1.0f / (r1.y - r0.y));
                ro = ro + rom * (y - r0.y);
            }
            int startx = int(std::max(lx, fx0) + 0.5f);
            int endx = int(std::min(rx, fx1) + 0.5f);
            int span = endx - startx;
            if (span > 0) {
                shaded_lines++;
                shaded_pixels += span;
                brush_solid_vert::InterpOutputs stepo = (ro - lo) * (1.0f / (rx - lx));
                {
                    brush_solid_vert::InterpOutputs o0 = lo + stepo * (startx + 0.5f - lx);
                    brush_solid_vert::InterpOutputs o1 = o0 + stepo;
                    brush_solid_vert::InterpOutputs o2 = o1 + stepo;
                    brush_solid_vert::InterpOutputs o3 = o2 + stepo;
                    frag_shader.read_interp_inputs(o0, o1, o2, o3);
                }
                stepo = stepo * 4;
                uint32_t* buf = fbuf + startx;
                uint16_t* depth = fdepth + startx;
                if (!frag_shader.uses_discard()) {
                    if (depthtest) {
                        for (; span >= 8; span -= 8, buf += 8, depth += 8) {
                            int discarded = check_depth<2>(z, span, depth);
                            if (discarded == 0xFF) continue;
                            frag_shader.main();
                            frag_shader.step_interp_inputs(stepo);
                            commit_output_no_depth_test(span, buf, discarded&0xF);
                            frag_shader.main();
                            frag_shader.step_interp_inputs(stepo);
                            commit_output_no_depth_test(span, buf + 4, discarded>>4);
                        }
                    } else {
                        for (; span >= 4; span -= 4, buf += 4) {
                            frag_shader.main();
                            frag_shader.step_interp_inputs(stepo);
                            commit_output_no_depth_test(span, buf);
                        }
                    }
                }
                for (; span >= 4; span -= 4, buf += 4, depth += 4) {
                    frag_shader.main();
                    frag_shader.step_interp_inputs(stepo);
                    commit_output<true>(span, buf, depth, z);
                }
                if (span > 0) {
                    frag_shader.main();
                    commit_output<false>(span, buf, depth, z);
                }
            }
            lx += lm;
            rx += rm;
            y++;
            lo = lo + lom;
            ro = ro + rom;
            fbuf += colortex.width;
        }

        auto output = get_nth(frag_shader.get_output(), 0);
        auto pixels = pack_pixels(frag_shader.get_output());
        int pixel0 = _mm_cvtsi128_si32(_mm_shuffle_epi32(pixels, 0));
        int pixel1 = _mm_cvtsi128_si32(_mm_shuffle_epi32(pixels, 0x55));
        int pixel2 = _mm_cvtsi128_si32(_mm_shuffle_epi32(pixels, 0xAA));
        int pixel3 = _mm_cvtsi128_si32(_mm_shuffle_epi32(pixels, 0xFF));
        printf("color %f %f %f %f -> %x, %x, %x, %x\n", output.x, output.y, output.z, output.w, pixel0, pixel1, pixel2, pixel3);
#ifdef  __MACH__
        double end = mach_absolute_time();
#else
        double end = ({ struct timespec tp; clock_gettime(CLOCK_MONOTONIC, &tp); tp.tv_sec * 1e9 + tp.tv_nsec; });
#endif
        printf("%fms for %d pixels in %d rows\n", (end - start)/(1000.*1000.), shaded_pixels, shaded_lines);
}

#if 0
struct tgaheader
{
    uint8_t  identsize;
    uint8_t  cmaptype;
    uint8_t  imagetype;
    uint8_t  cmaporigin[2];
    uint8_t  cmapsize[2];
    uint8_t  cmapentrysize;
    uint8_t  xorigin[2];
    uint8_t  yorigin[2];
    uint8_t  width[2];
    uint8_t  height[2];
    uint8_t  pixelsize;
    uint8_t  descbyte;
};

void writetga(const char *name, int width, int height, uint8_t *data)
{
    FILE *f = fopen(name, "wb");
    if(!f) return;

    bool compress = true;
    tgaheader hdr;
    memset(&hdr, 0, sizeof(hdr));
    hdr.pixelsize = 32;
    hdr.width[0] = width&0xFF;
    hdr.width[1] = (width>>8)&0xFF;
    hdr.height[0] = height&0xFF;
    hdr.height[1] = (height>>8)&0xFF;
    hdr.imagetype = compress ? 10 : 2;
    fwrite(&hdr, 1, sizeof(hdr), f);

    uint8_t buf[128*4];
    for(int i = 0; i < height; i++)
    {
        uint8_t *src = data + (height-i-1)*width*4;
        for(int remaining = width; remaining > 0;)
        {
            int raw = 1;
            if(compress)
            {
                int run = 1;
                for(uint8_t *scan = src; run < std::min(remaining, 128); run++)
                {
                    scan += 4;
                    if(src[0]!=scan[0] || src[1]!=scan[1] || src[2]!=scan[2]) break;
                }
                if(run > 1)
                {
                    fputc(0x80 | (run-1), f);
                    fputc(src[2], f); fputc(src[1], f); fputc(src[0], f);
                    fputc(src[3], f);
                    src += run*4;
                    remaining -= run;
                    if(remaining <= 0) break;
                }
                for(uint8_t *scan = src; raw < std::min(remaining, 128); raw++)
                {
                    scan += 4;
                    if(src[0]==scan[0] && src[1]==scan[1] && src[2]==scan[2] && src[3]==scan[3]) break;
                }
                fputc(raw - 1, f);
            }
            else raw = std::min(remaining, 128);
            uint8_t *dst = buf;
            for(int j = 0; j < raw; j++)
            {
                dst[0] = src[2];
                dst[1] = src[1];
                dst[2] = src[0];
                dst[3] = src[3];
                dst += 4;
                src += 4;
            }
            fwrite(buf, 1, raw*4, f);
            remaining -= raw;
        }
    }

    fclose(f);
}
#endif

extern "C" {

void DrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, void *indices, GLsizei instancecount) {
        assert(mode == GL_TRIANGLES);
        assert(type == GL_UNSIGNED_SHORT);
        assert(count == 6);
        assert(indices == 0);
        for (int instance = 0; instance < instancecount; instance++) {
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
                                shader.load_attribs_for_tri(v.attribs, indices, i, instance);
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
                                // SSE2 does not support unsigned comparison, so bias Z to be negative.
                                uint16_t z = uint16_t(0xFFFF * (1.0f + 0.5f * gl_Position.z.x / gl_Position.w.x)) - 0x8000;
                                triangle(frag_shader, flat_outs, interp_outs, a, b, c, z);
                        }

                } else {
                        assert(0);
                }
        }
        }
        printf("dodraw");
}

void Finish() {
        Framebuffer& fb = get_draw_framebuffer();
        Texture& t = textures[fb.color_attachment];
        //writetga("out.tga", t.width, t.height, t.buf);
        write_png("out.png", t.buf, t.width, t.height);
}

} // extern "C"
