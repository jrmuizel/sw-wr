#include <assert.h>
#ifdef __MACH__
#include <mach/mach.h>
#include <mach/mach_time.h>
#else
#include <time.h>
#endif

#include <map>
#include <string>
#include "minpng.h"
#include "glsl.h"

using namespace std;
using namespace glsl;

#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893


typedef int8_t GLbyte;
typedef uint8_t GLubyte;
typedef int16_t GLshort;
typedef uint16_t GLushort;
typedef int32_t GLint;
typedef uint32_t GLuint;

typedef float GLfloat;
typedef double GLdouble;

typedef uint32_t GLenum;
typedef int32_t GLboolean;
typedef uint32_t GLbitfield;

typedef int32_t GLsizei;
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

#define GL_NEAREST                  0x2600
#define GL_LINEAR                   0x2601
#define GL_NEAREST_MIPMAP_NEAREST   0x2700
#define GL_NEAREST_MIPMAP_LINEAR    0x2702
#define GL_LINEAR_MIPMAP_NEAREST    0x2701
#define GL_LINEAR_MIPMAP_LINEAR     0x2703
#define GL_TEXTURE_WRAP_S           0x2802
#define GL_TEXTURE_WRAP_T           0x2803
#define GL_TEXTURE_MAG_FILTER       0x2800
#define GL_TEXTURE_MIN_FILTER       0x2801
#define GL_CLAMP_TO_EDGE            0x812F

glsl::TextureFilter gl_filter_to_texture_filter(int type) {
        switch (type) {
                case GL_NEAREST: return glsl::TextureFilter::NEAREST;
                case GL_NEAREST_MIPMAP_LINEAR: return glsl::TextureFilter::NEAREST;
                case GL_NEAREST_MIPMAP_NEAREST: return glsl::TextureFilter::NEAREST;
                case GL_LINEAR: return glsl::TextureFilter::LINEAR;
                case GL_LINEAR_MIPMAP_LINEAR: return glsl::TextureFilter::LINEAR;
                case GL_LINEAR_MIPMAP_NEAREST: return glsl::TextureFilter::LINEAR;
                defaut: assert(0);
        }
}

#define GL_TEXTURE_2D               0x0DE1
#define GL_TEXTURE_3D               0x806F
#define GL_TEXTURE_2D_ARRAY         0x8C1A

struct Texture {
    GLenum target = 0;
    int levels = 0;
    GLenum internal_format = 0;
    int width = 0;
    int height = 0;
    int depth = 0;
    char *buf = nullptr;
    GLenum min_filter = GL_NEAREST;
    GLenum mag_filter = GL_NEAREST;

    void allocate() {
        if (!buf) {
            size_t size = bytes_for_internal_format(internal_format) * width * height * std::max(depth, 1) * levels;
            buf = (char*)malloc(size + sizeof(__m128i));
        }
    }

    void make_renderable() {
        switch (internal_format) {
        case GL_DEPTH_COMPONENT:
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32:
            assert(!buf);
            internal_format = GL_DEPTH_COMPONENT16;
            break;
        case GL_DEPTH_COMPONENT16:
            break;
        case GL_R8:
            assert(!buf);
            internal_format = GL_RGBA8;
            break;
        case GL_RGBA8:
            break;
        default:
            printf("non-renderable texture format %x\n", internal_format);
            assert(false);
        }
        allocate();
    }
};

#define MAX_ATTRS 16
struct VertexArray {
    VertexAttrib attribs[MAX_ATTRS];
};


#define GL_VERTEX_SHADER                  0x8B31
#define GL_FRAGMENT_SHADER                0x8B30
struct Shader {
    GLenum type;
    std::string name;
};

struct ProgramImpl {
    virtual ~ProgramImpl() {}
    virtual const char *get_name() const = 0;
    virtual int get_uniform(const char *name) const = 0;
    virtual bool set_sampler(int index, int value) = 0;
    virtual void bind_attrib(const char *name, int index) = 0;
    virtual void init_shaders(void *vertex_shader, void *fragment_shader) = 0;
};

struct Program {
    std::string vs_name;
    std::string fs_name;
    std::map<std::string, int> attribs;
    ProgramImpl* impl = nullptr;
};

struct ShaderImpl {
    typedef void (*SetUniform1iFunc)(ShaderImpl*, int index, int value);
    typedef void (*SetUniform4fvFunc)(ShaderImpl*, int index, const float *value);
    typedef void (*SetUniformMatrix4fvFunc)(ShaderImpl*, int index, const float *value);

    SetUniform1iFunc set_uniform_1i_func = nullptr;
    SetUniform4fvFunc set_uniform_4fv_func = nullptr;
    SetUniformMatrix4fvFunc set_uniform_matrix4fv_func = nullptr;
};

template<typename T>
struct ShaderCommon : T {
    char impl_data[1024];

    void set_uniform_1i(int index, int value) {
       (*ShaderImpl::set_uniform_1i_func)(this, index, value);
    }

    void set_uniform_4fv(int index, const float *value) {
       (*ShaderImpl::set_uniform_4fv_func)(this, index, value);
    }

    void set_uniform_matrix4fv(int index, const float *value) {
       (*ShaderImpl::set_uniform_matrix4fv_func)(this, index, value);
    }
};

struct VertexShaderImpl : ShaderImpl {
    typedef void (*InitBatchFunc)(VertexShaderImpl*, ProgramImpl *prog);
    typedef void (*LoadAttribsFunc)(VertexShaderImpl*, ProgramImpl *prog, VertexAttrib *attribs, unsigned short *indices, int start, int instance, int count);
    typedef void (*RunFunc)(VertexShaderImpl*, char* flats, char* interps, size_t interp_stride);

    InitBatchFunc init_batch_func = nullptr;
    LoadAttribsFunc load_attribs_func = nullptr;
    RunFunc run_func = nullptr;

    vec4 gl_Position;
};

struct VertexShader : ShaderCommon<VertexShaderImpl> {
    void init_batch(ProgramImpl *prog) {
        (*init_batch_func)(this, prog);
    }

    ALWAYS_INLINE void load_attribs(ProgramImpl *prog, VertexAttrib *attribs, unsigned short *indices, int start, int instance, int count) {
        (*load_attribs_func)(this, prog, attribs, indices, start, instance, count);
    }

    ALWAYS_INLINE void run(char* flats, char* interps, size_t interp_stride) {
        (*run_func)(this, flats, interps, interp_stride);
    }
} vertex_shader;

GLuint current_program = 0;

struct FragmentShaderImpl : ShaderImpl {
    typedef void (*InitBatchFunc)(FragmentShaderImpl*, ProgramImpl *prog);
    typedef void (*InitPrimitiveFunc)(FragmentShaderImpl*, const void* flats);
    typedef void (*InitSpanFunc)(FragmentShaderImpl*, const void* interps, const void* step, float step_width);
    typedef void (*RunFunc)(FragmentShaderImpl*);
    typedef void (*SkipFunc)(FragmentShaderImpl*);
    typedef bool (*UseDiscardFunc)(FragmentShaderImpl*);

    InitBatchFunc init_batch_func = nullptr;
    InitPrimitiveFunc init_primitive_func = nullptr;
    InitSpanFunc init_span_func = nullptr;
    RunFunc run_func = nullptr;
    SkipFunc skip_func = nullptr;
    UseDiscardFunc use_discard_func = nullptr;

    vec4 gl_FragCoord;
    Bool isPixelDiscarded;
    vec4 gl_FragColor;
    vec4 gl_SecondaryFragColor;

    ALWAYS_INLINE void step_fragcoord() {
        gl_FragCoord.x += 4;
    }
};

struct FragmentShader : ShaderCommon<FragmentShaderImpl> {
    void init_batch(ProgramImpl *prog) {
        (*init_batch_func)(this, prog);
    }

    void init_primitive(const void* flats) {
        (*init_primitive_func)(this, flats);
    }

    ALWAYS_INLINE void init_span(const void* interps, const void* step, float step_width) {
        (*init_span_func)(this, interps, step, step_width);
    }

    ALWAYS_INLINE void run() {
        (*run_func)(this);
    }

    ALWAYS_INLINE void skip() {
        (*skip_func)(this);
    }

    ALWAYS_INLINE bool use_discard() {
        return (*use_discard_func)(this);
    }
} fragment_shader;

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

#define GL_BLEND                    0x0BE2
#define GL_ZERO                     0
#define GL_ONE                      1
#define GL_SRC_COLOR                0x0300
#define GL_ONE_MINUS_SRC_COLOR      0x0301
#define GL_SRC_ALPHA                0x0302
#define GL_ONE_MINUS_SRC_ALPHA      0x0303
#define GL_DST_ALPHA                0x0304
#define GL_ONE_MINUS_DST_ALPHA      0x0305
#define GL_DST_COLOR                0x0306
#define GL_ONE_MINUS_DST_COLOR      0x0307
#define GL_CONSTANT_COLOR           0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR 0x8002
#define GL_CONSTANT_ALPHA           0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA 0x8004
#define GL_SRC1_ALPHA               0x8589
#define GL_SRC1_COLOR               0x88F9
#define GL_ONE_MINUS_SRC1_COLOR     0x88FA
#define GL_ONE_MINUS_SRC1_ALPHA     0x88FB

bool blend = false;
GLenum blendfunc_srgb = GL_ONE;
GLenum blendfunc_drgb = GL_ZERO;
GLenum blendfunc_sa = GL_ONE;
GLenum blendfunc_da = GL_ZERO;

#define GL_FUNC_ADD             0x8006

GLenum blend_equation_mode = GL_FUNC_ADD;

__m128i blendcolor = _mm_set1_epi16(0);

// for GL defines to fully expand
#define CONCAT_KEY(prefix, x, y, z, w, ...) prefix ## x ## y ## z ## w
#define BLEND_KEY(...) CONCAT_KEY(BLEND_, __VA_ARGS__, 0, 0)
#define FOR_EACH_BLEND_KEY(macro) \
    macro(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE) \
    macro(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, 0, 0) \
    macro(GL_ZERO, GL_ONE_MINUS_SRC_COLOR, 0, 0) \
    macro(GL_ZERO, GL_ONE_MINUS_SRC_COLOR, GL_ZERO, GL_ONE) \
    macro(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA, 0, 0) \
    macro(GL_ZERO, GL_SRC_COLOR, 0, 0) \
    macro(GL_ONE, GL_ONE, 0, 0) \
    macro(GL_ONE, GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA) \
    macro(GL_ONE, GL_ZERO, 0, 0) \
    macro(GL_ONE_MINUS_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE) \
    macro(GL_CONSTANT_COLOR, GL_ONE_MINUS_SRC_COLOR, 0, 0) \
    macro(GL_ONE, GL_ONE_MINUS_SRC1_COLOR, 0, 0)

#define DEFINE_BLEND_KEY(...) BLEND_KEY(__VA_ARGS__),
enum BlendKey : uint8_t
{
    BLEND_KEY_NONE = 0,
    FOR_EACH_BLEND_KEY(DEFINE_BLEND_KEY)
};

BlendKey blend_key = BLEND_KEY_NONE;

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

template<typename S>
S *lookup_sampler(S *s, int texture) {
        if (!texture_slots[texture]) {
            s->width = 0;
            s->height = 0;
            s->stride = 0;
            s->buf = nullptr;
            s->format = TextureFormat::RGBA8;
            s->filter = TextureFilter::NEAREST;
        } else {
            Texture &t = textures[texture_slots[texture]];
            s->width = t.width;
            s->height = t.height;
            s->stride = bytes_for_internal_format(t.internal_format) * t.width / 4;
            s->buf = (uint32_t*)t.buf; //XXX: wrong
            s->format = gl_format_to_texture_format(t.internal_format);
            s->filter = gl_filter_to_texture_filter(t.mag_filter);
        }
        return s;
}

template<typename S>
S *lookup_isampler(S *s, int texture) {
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
            s->stride = bytes_for_internal_format(t.internal_format) * t.width / 4;
            s->buf = (uint32_t*)t.buf; //XXX: wrong
            s->format = gl_format_to_texture_format(t.internal_format);
        }
        return s;
}

template<typename S>
S *lookup_sampler_array(S *s, int texture) {
        if (!texture_slots[texture]) {
            s->width = 0;
            s->height = 0;
            s->depth = 0;
            s->stride = 0;
            s->height_stride = 0;
            s->buf = nullptr;
            s->format = TextureFormat::RGBA8;
            s->filter = TextureFilter::NEAREST;
        } else {
            Texture &t = textures[texture_slots[texture]];
            s->width = t.width;
            s->height = t.height;
            s->depth = t.depth;
            s->stride = bytes_for_internal_format(t.internal_format) * t.width / 4;
            s->height_stride = bytes_for_internal_format(t.internal_format) * t.width * t.height / 4;
            s->buf = (uint32_t*)t.buf; //XXX: wrong
            s->format = gl_format_to_texture_format(t.internal_format);
            s->filter = gl_filter_to_texture_filter(t.mag_filter);
        }
        return s;
}

int bytes_per_type(GLenum type) {
        switch (type) {
                case GL_INT: return 4;
                case GL_FLOAT: return 4;
                case GL_UNSIGNED_SHORT: return 2;
                case GL_UNSIGNED_BYTE: return 1;
                default: assert(0);
        }
}

template<typename T>
void load_attrib(T& attrib, VertexAttrib &va, unsigned short *indices, int start, int instance, int count) {
    typedef decltype(force_scalar(attrib)) scalar_type;
    if (va.divisor == 1) {
        char* src = (char*)va.buf + va.stride * instance + va.offset;
        assert(src + va.size <= va.buf + va.buf_size);
        if (sizeof(scalar_type) > va.size) {
            scalar_type scalar = {0};
            if (va.type == GL_UNSIGNED_SHORT) {
                for (int i = 0; i < va.size/sizeof(short); i++) {
                    short s;
                    memcpy(&s, src + i, sizeof(short));
                    typename ElementType<T>::ty x = s;
                    put_nth_component(scalar, i, x);
                }
            } else {
                assert(sizeof(typename ElementType<T>::ty) == bytes_per_type(va.type));
                memcpy(&scalar, src, va.size);
            }
            attrib = T(scalar);
        } else {
            attrib = T(*reinterpret_cast<scalar_type*>(src));
        }
    } else if (va.divisor == 0) {
        assert(sizeof(typename ElementType<T>::ty) == bytes_per_type(va.type));
        for (int n = 0; n < count; n++) {
            char* src = (char*)va.buf + va.stride * indices[start + n] + va.offset;
            assert(src + va.size <= va.buf + va.buf_size);
            if (sizeof(scalar_type) > va.size) {
                scalar_type scalar = {0};
                memcpy(&scalar, src, va.size);
                put_nth(attrib, n, scalar);
            } else {
                put_nth(attrib, n, *reinterpret_cast<scalar_type*>(src));
            }
        }
    } else {
        assert(false);
    }
}

template<typename T>
void load_flat_attrib(T& attrib, VertexAttrib &va, unsigned short *indices, int start, int instance, int count) {
    char* src;
    if (va.divisor == 1) {
        src = (char*)va.buf + va.stride * instance + va.offset;
    } else if (va.divisor == 0) {
        src = (char*)va.buf + va.stride * indices[start] + va.offset;
    } else {
        assert(false);
    }
    assert(src + va.size <= va.buf + va.buf_size);
    if (sizeof(T) > va.size) {
        attrib = T{0};
        memcpy(&attrib, src, va.size);
    } else {
        attrib = *reinterpret_cast<T*>(src);
    }
}

#include "brush_solid.h"
#include "brush_solidALPHA_PASS.h"
#include "brush_linear_gradientDITHERING.ALPHA_PASS.h"
#include "brush_image.h"
#include "brush_imageALPHA_PASS.h"
#include "cs_blurALPHA_TARGET.h"
#include "cs_border_solid.h"
#include "cs_clip_box_shadow.h"
#include "cs_clip_rectangle.h"
#include "cs_clip_rectangleFAST_PATH.h"
#include "ps_text_run.h"
#include "ps_text_runDUAL_SOURCE_BLENDING.h"
extern "C" {

void UseProgram(GLuint program) {
    current_program = program;
    if (!program)
        return;
    Program &p = programs[program];
    assert(p.impl);
    p.impl->init_shaders(&vertex_shader, &fragment_shader);
}

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

GLenum remap_blendfunc(GLenum rgb, GLenum a) {
    switch (a) {
    case GL_SRC_ALPHA: if (rgb == GL_SRC_COLOR) a = GL_SRC_COLOR; break;
    case GL_ONE_MINUS_SRC_ALPHA: if (rgb == GL_ONE_MINUS_SRC_COLOR) a = GL_ONE_MINUS_SRC_COLOR; break;
    case GL_DST_ALPHA: if (rgb == GL_DST_COLOR) a = GL_DST_COLOR; break;
    case GL_ONE_MINUS_DST_ALPHA: if (rgb == GL_ONE_MINUS_DST_COLOR) a = GL_ONE_MINUS_DST_COLOR; break;
    case GL_CONSTANT_ALPHA: if (rgb == GL_CONSTANT_COLOR) a = GL_CONSTANT_COLOR; break;
    case GL_ONE_MINUS_CONSTANT_ALPHA: if (rgb == GL_ONE_MINUS_CONSTANT_COLOR) a = GL_ONE_MINUS_CONSTANT_COLOR; break;
    case GL_SRC_COLOR: if (rgb == GL_SRC_ALPHA) a = GL_SRC_ALPHA; break;
    case GL_ONE_MINUS_SRC_COLOR: if (rgb == GL_ONE_MINUS_SRC_ALPHA) a = GL_ONE_MINUS_SRC_ALPHA; break;
    case GL_DST_COLOR: if (rgb == GL_DST_ALPHA) a = GL_DST_ALPHA; break;
    case GL_ONE_MINUS_DST_COLOR: if (rgb == GL_ONE_MINUS_DST_ALPHA) a = GL_ONE_MINUS_DST_ALPHA; break;
    case GL_CONSTANT_COLOR: if (rgb == GL_CONSTANT_ALPHA) a = GL_CONSTANT_ALPHA; break;
    case GL_ONE_MINUS_CONSTANT_COLOR: if (rgb == GL_ONE_MINUS_CONSTANT_ALPHA) a = GL_ONE_MINUS_CONSTANT_ALPHA; break;
    case GL_SRC1_ALPHA: if (rgb == GL_SRC1_COLOR) a = GL_SRC1_COLOR; break;
    case GL_ONE_MINUS_SRC1_ALPHA: if (rgb == GL_ONE_MINUS_SRC1_COLOR) a = GL_ONE_MINUS_SRC1_COLOR; break;
    case GL_SRC1_COLOR: if (rgb == GL_SRC1_ALPHA) a = GL_SRC1_ALPHA; break;
    case GL_ONE_MINUS_SRC1_COLOR: if (rgb == GL_ONE_MINUS_SRC1_ALPHA) a = GL_ONE_MINUS_SRC1_ALPHA; break;
    }
    return a;
}

void BlendFunc(GLenum srgb, GLenum drgb, GLenum sa, GLenum da) {
    blendfunc_srgb = srgb;
    blendfunc_drgb = drgb;
    sa = remap_blendfunc(srgb, sa);
    da = remap_blendfunc(drgb, da);
    blendfunc_sa = sa;
    blendfunc_da = da;

    #define HASH_BLEND_KEY(x, y, z, w) ((x << 4) | (y) | (z << 24) | (w << 20))
    int hash = HASH_BLEND_KEY(srgb, drgb, 0, 0);
    if (srgb != sa || drgb != da) hash |= HASH_BLEND_KEY(0, 0, sa, da);
    switch (hash) {
    #define MAP_BLEND_KEY(...) \
        case HASH_BLEND_KEY(__VA_ARGS__): \
            blend_key = BLEND_KEY(__VA_ARGS__); \
            break;
    FOR_EACH_BLEND_KEY(MAP_BLEND_KEY)
    default:
        printf("blendfunc: %x, %x, separate: %x, %x\n", srgb, drgb, sa, da);
        assert(false);
        break;
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
        s.type = type;
        shaders.insert(pair<GLuint, Shader>(shader_count, s));
        return shader_count++;
}

void ShaderSourceByName(GLuint shader, char* name) {
    Shader &s = shaders[shader];
    s.name = name;
}

void AttachShader(GLuint program, GLuint shader) {
    Program &p = programs[program];
    Shader &s = shaders[shader];
    if (s.type == GL_VERTEX_SHADER) {
        p.vs_name = s.name;
    } else if (s.type == GL_FRAGMENT_SHADER) {
        p.fs_name = s.name;
    } else {
        assert(0);
    }
}

GLuint CreateProgram() {
        Program p;
        programs.insert(pair<GLuint, Program>(program_count, p));
        return program_count++;
}

void LinkProgram(GLuint program) {
    Program &p = programs[program];
    assert(p.vs_name == p.fs_name);
    if (p.vs_name == "brush_solid") {
        static brush_solid_program impl;
        p.impl = &impl;
    } else if (p.vs_name == "brush_solidALPHA_PASS") {
        static brush_solidALPHA_PASS_program impl;
        p.impl = &impl;
    } else if (p.vs_name == "brush_linear_gradientDITHERING\n.ALPHA_PASS") {
        static brush_linear_gradientDITHERING_program impl;
        p.impl = &impl;
    } else if (p.vs_name == "brush_image") {
        static brush_image_program impl;
        p.impl = &impl;
    } else if (p.vs_name == "brush_imageALPHA_PASS") {
        static brush_imageALPHA_PASS_program impl;
        p.impl = &impl;
    } else if (p.vs_name == "cs_border_solid") {
        static cs_border_solid_program impl;
        p.impl = &impl;
    } else if (p.vs_name == "cs_clip_rectangle") {
        static cs_clip_rectangle_program impl;
        p.impl = &impl;
    } else if (p.vs_name == "cs_clip_rectangleFAST_PATH") {
        static cs_clip_rectangleFAST_PATH_program impl;
        p.impl = &impl;
    } else if (p.vs_name == "cs_clip_box_shadow") {
        static cs_clip_box_shadow_program impl;
        p.impl = &impl;
    } else if (p.vs_name == "cs_blurALPHA_TARGET") {
        static cs_blurALPHA_TARGET_program impl;
        p.impl = &impl;
    } else if (p.vs_name == "ps_text_run") {
        static ps_text_run_program impl;
        p.impl = &impl;
    } else if (p.vs_name == "ps_text_runDUAL_SOURCE_BLENDING") {
        static ps_text_runDUAL_SOURCE_BLENDING_program impl;
        p.impl = &impl;
    } else {
        printf("unknown program %s\n", p.vs_name.c_str());
    }

    assert(p.impl);
    for (auto i : p.attribs) {
        p.impl->bind_attrib(i.first.c_str(), i.second);
    }
}

void BindAttribLocation(GLuint program, GLuint index, char *name) {
    Program &p = programs[program];
    p.attribs[name] = index;
}

GLint GetUniformLocation(GLuint program, char* name) {
    Program &p = programs[program];
    assert(p.impl);
    GLint loc = p.impl->get_uniform(name);
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
}

#define GL_RED                            0x1903
#define GL_RGBA                           0x1908
#define GL_RGBA_INTEGER                   0x8D99
GLenum internal_format_for_data(GLenum format, GLenum ty) {
    if (format == GL_RED && ty == GL_UNSIGNED_BYTE) {
        return GL_R8;
    } else if (format == GL_RGBA && ty == GL_UNSIGNED_BYTE) {
        return GL_RGBA8;
    } else if (format == GL_RGBA && ty == GL_FLOAT) {
        return GL_RGBA32F;
    } else if (format == GL_RGBA_INTEGER && ty == GL_INT) {
        return GL_RGBA32I;
    } else {
        assert(false);
        return 0;
    }
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
        Texture &t = textures[current_texture[target]];
        assert(xoffset + width <= t.width);
        assert(yoffset + height <= t.height);
        assert(t.internal_format == internal_format_for_data(format, ty));
        int bpp = bytes_for_internal_format(t.internal_format);
        if (!bpp) return;
        t.allocate();
        char *dest = t.buf + (yoffset * t.width + xoffset) * bpp;
        char *src = (char*)data;
        for (int y = 0; y < height; y++) {
                memcpy(dest, src, width * bpp);
                dest += t.width * bpp;
                src += width * bpp;
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
        assert(t.internal_format == internal_format_for_data(format, ty));
        int bpp = bytes_for_internal_format(t.internal_format);
        if (!bpp) return;
        t.allocate();
        char *src = (char*)data;
        assert(xoffset + width <= t.width);
        assert(yoffset + height <= t.height);
        assert(zoffset + depth <= t.depth);
        for (int z = 0; z < depth; z++) {
                char *dest = t.buf + ((zoffset + z) * t.width * t.height + yoffset * t.width + xoffset) * bpp;
                for (int y = 0; y < height; y++) {
                        memcpy(dest, src, width * bpp);
                        dest += t.width * bpp;
                        src += width * bpp;
                }
        }

}

void TexParameteri(GLenum target, GLenum pname, GLint param) {
        Texture &t = textures[current_texture[target]];
        switch (pname) {
        case GL_TEXTURE_WRAP_S: assert(param == GL_CLAMP_TO_EDGE); break;
        case GL_TEXTURE_WRAP_T: assert(param == GL_CLAMP_TO_EDGE); break;
        case GL_TEXTURE_MIN_FILTER:
            assert(param == GL_NEAREST || param == GL_LINEAR);
            t.min_filter = param;
            break;
        case GL_TEXTURE_MAG_FILTER:
            assert(param == GL_NEAREST || param == GL_LINEAR);
            t.mag_filter = param;
            break;
        default:
            break;
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
        case GL_R8:
            // Force color format to 32 bits...
            internalformat = GL_RGBA8;
            break;
    }
    TexStorage2D(target, 1, internalformat, width, height);
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

void Uniform1i(GLint location, GLint V0) {
    printf("tex: %d\n", texture_count);
    Program &p = programs[current_program];
    assert(p.impl);
    if (!p.impl->set_sampler(location, V0)) {
        vertex_shader.set_uniform_1i(location, V0);
        fragment_shader.set_uniform_1i(location, V0);
    }
}
void Uniform4fv(GLint location, GLsizei count, const GLfloat *v) {
        vertex_shader.set_uniform_4fv(location, v);
        fragment_shader.set_uniform_4fv(location, v);
}
void UniformMatrix4fv(GLint location,
 	GLsizei count,
 	GLboolean transpose,
 	const GLfloat *value) {
        vertex_shader.set_uniform_matrix4fv(location, value);
        fragment_shader.set_uniform_matrix4fv(location, value);
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

#define GL_READ_FRAMEBUFFER               0x8CA8
#define GL_DRAW_FRAMEBUFFER               0x8CA9

void FramebufferTextureLayer(
        GLenum target,
        GLenum attachment,
        GLuint texture,
        GLint level,
        GLint layer)
{
        assert(level == 0);
        assert(target == GL_READ_FRAMEBUFFER || target == GL_DRAW_FRAMEBUFFER);
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
    assert(target == GL_READ_FRAMEBUFFER || target == GL_DRAW_FRAMEBUFFER);
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

Framebuffer *get_framebuffer(GLenum target) {
    GLuint id = 0;
    {
        auto i = current_framebuffer.find(target);
        if (i != current_framebuffer.end()) id = i->second;
    }
    {
        auto i = framebuffers.find(id);
        if (i != framebuffers.end()) return &i->second;
    }
    return nullptr;
}

Framebuffer &get_draw_framebuffer() {
    if (Framebuffer *fb = get_framebuffer(GL_DRAW_FRAMEBUFFER)) {
      return *fb;
    }
    // allocate a default framebuffer
    Framebuffer& fb = framebuffers[0];
    const size_t width = 3072;
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

template<typename T>
static void clear_buffer(Texture& t, __m128i chunk, T value, int layer = 0) {
    int x0 = 0, y0 = 0, x1 = t.width, y1 = t.height;
    if (scissortest) {
        x0 = std::max(x0, scissor.x);
        y0 = std::max(y0, scissor.y);
        x1 = std::min(x1, scissor.x + scissor.width);
        y1 = std::min(y1, scissor.y + scissor.height);
    }
    const int chunk_size = sizeof(chunk) / sizeof(T);
    T* buf = (T*)t.buf + t.width * t.height * layer + t.width * y0 + x0;
    for (int y = y0; y < y1; y++) {
        T* end = buf + (x1 - x0);
        for (; buf + chunk_size <= end; buf += chunk_size) {
            _mm_storeu_si128((__m128i*)buf, chunk);
        }
        for (; buf < end; buf++) {
            *buf = value;
        }
        buf += t.width - (x1 - x0);
    }
}

extern "C" {

#define GL_COLOR_BUFFER_BIT         0x00004000
#define GL_DEPTH_BUFFER_BIT         0x00000100
#define GL_STENCIL_BUFFER_BIT       0x00000400

void Clear(GLbitfield mask) {
    Framebuffer& fb = get_draw_framebuffer();
    if ((mask & GL_COLOR_BUFFER_BIT) && fb.color_attachment) {
        Texture& t = textures[fb.color_attachment];
        t.make_renderable();
        if (t.internal_format == GL_RGBA8) {
            __m128 colorf = _mm_loadu_ps(clearcolor);
            __m128i colori = _mm_cvtps_epi32(_mm_mul_ps(colorf, _mm_set1_ps(255.5f)));
            colori = _mm_packs_epi32(colori, colori);
            colori = _mm_packus_epi16(colori, colori);
            uint32_t color = _mm_cvtsi128_si32(colori);
            clear_buffer<uint32_t>(t, colori, color, fb.layer);
        } else {
            assert(false);
        }
    }
    if ((mask & GL_DEPTH_BUFFER_BIT) && fb.depth_attachment) {
        Texture& t = textures[fb.depth_attachment];
        t.make_renderable();
        assert(t.internal_format == GL_DEPTH_COMPONENT16);
        uint16_t depth = uint16_t(0xFFFF * cleardepth) - 0x8000;
        __m128i depthi = _mm_set1_epi16(depth);
        clear_buffer<uint16_t>(t, depthi, depth);
    }
}

void ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *data) {
    Framebuffer *fb = get_framebuffer(GL_READ_FRAMEBUFFER);
    if (!fb) return;
    assert(format == GL_RED || GL_RGBA || GL_RGBA_INTEGER);
    Texture &t = textures[fb->color_attachment];
    if (!t.buf) return;
    printf("read pixels %d, %d, %d, %d from fb %d with format %x\n", x, y, width, height, current_framebuffer[GL_READ_FRAMEBUFFER], t.internal_format);
    assert(x + width <= t.width);
    assert(y + height <= t.height);
    if (internal_format_for_data(format, type) != t.internal_format) {
        printf("mismatched format for read pixels: %x vs %x\n", t.internal_format, internal_format_for_data(format, type));
        assert(false);
    }
    int bpp = bytes_for_internal_format(t.internal_format);
    char *dest = (char *)data;
    char *src = t.buf + (t.width * t.height * fb->layer + t.width * y + x) * bpp;
    for (; height > 0; height--) {
        memcpy(dest, src, width * bpp);
        dest += width * bpp;
        src += t.width * bpp;
    }
}

void CopyImageSubData(
    GLuint srcName,
    GLenum srcTarget,
    GLint srcLevel,
    GLint srcX,
    GLint srcY,
    GLint srcZ,
    GLuint dstName,
    GLenum dstTarget,
    GLint dstLevel,
    GLint dstX,
    GLint dstY,
    GLint dstZ,
    GLsizei srcWidth,
    GLsizei srcHeight,
    GLsizei srcDepth
) {
    if (srcTarget == GL_RENDERBUFFER) {
        Renderbuffer &rb = renderbuffers[srcName];
        srcName = rb.texture;
    }
    if (dstTarget == GL_RENDERBUFFER) {
        Renderbuffer &rb = renderbuffers[dstName];
        dstName = rb.texture;
    }
    Texture &srctex = textures[srcName];
    if (!srctex.buf) return;
    Texture &dsttex = textures[dstName];
    assert(srctex.internal_format == dsttex.internal_format);
    assert(srcX + srcWidth <= srctex.width);
    assert(srcY + srcHeight <= srctex.height);
    assert(srcZ + srcDepth <= srctex.depth);
    assert(dstX + srcWidth <= dsttex.width);
    assert(dstY + srcHeight <= dsttex.height);
    assert(dstZ + srcDepth <= std::max(dsttex.depth, 1));
    dsttex.allocate();
    int bpp = bytes_for_internal_format(srctex.internal_format);
    for (int z = 0; z < srcDepth; z++) {
        char *dest = dsttex.buf + (dsttex.width * dsttex.height * (dstZ + z) + dsttex.width * dstY + dstX) * bpp;
        char *src = srctex.buf + (srctex.width * srctex.height * (srcZ + z) + srctex.width * srcY + srcX) * bpp;
        for (int y = 0; y < srcHeight; y++) {
            memcpy(dest, src, srcWidth * bpp);
            dest += dsttex.width * bpp;
            src += srctex.width * bpp;
        }
    }
}

void CopyTexSubImage3D(
        GLenum target,
        GLint level,
        GLint xoffset,
        GLint yoffset,
        GLint zoffset,
        GLint x,
        GLint y,
        GLsizei width,
        GLsizei height
) {
    Framebuffer *fb = get_framebuffer(GL_READ_FRAMEBUFFER);
    if (!fb) return;
    CopyImageSubData(
        fb->color_attachment, GL_TEXTURE_3D, 0, x, y, fb->layer,
        current_texture[target], GL_TEXTURE_3D, 0, xoffset, yoffset, zoffset,
        width, height, 1);
}

void CopyTexSubImage2D(
        GLenum target,
        GLint level,
        GLint xoffset,
        GLint yoffset,
        GLint x,
        GLint y,
        GLsizei width,
        GLsizei height
) {
    Framebuffer *fb = get_framebuffer(GL_READ_FRAMEBUFFER);
    if (!fb) return;
    CopyImageSubData(
        fb->color_attachment, GL_TEXTURE_2D_ARRAY, 0, x, y, fb->layer,
        current_texture[target], GL_TEXTURE_2D_ARRAY, 0, xoffset, yoffset, 0,
        width, height, 1);
}

void BlitFramebuffer(
        GLint srcX0,
        GLint srcY0,
        GLint srcX1,
        GLint srcY1,
        GLint dstX0,
        GLint dstY0,
        GLint dstX1,
        GLint dstY1,
        GLbitfield mask,
        GLenum filter
) {
    assert(mask == GL_COLOR_BUFFER_BIT);
    Framebuffer *srcfb = get_framebuffer(GL_READ_FRAMEBUFFER);
    if (!srcfb) return;
    Framebuffer *dstfb = get_framebuffer(GL_DRAW_FRAMEBUFFER);
    if (!dstfb) return;
    int srcWidth = srcX1 - srcX0;
    int srcHeight = srcY1 - srcY0;
    int dstWidth = dstX1 - dstX0;
    int dstHeight = dstY1 - dstY0;
    assert(srcWidth == dstWidth && srcHeight == dstHeight);
    CopyImageSubData(
        srcfb->color_attachment, GL_TEXTURE_2D_ARRAY, 0, srcX0, srcY0, srcfb->layer,
        dstfb->color_attachment, GL_TEXTURE_2D_ARRAY, 0, dstX0, dstY0, dstfb->layer,
        srcWidth, srcHeight, 1);
}

#define GL_POINTS                         0x0000
#define GL_LINES                          0x0001
#define GL_LINE_LOOP                      0x0002
#define GL_LINE_STRIP                     0x0003
#define GL_TRIANGLES                      0x0004
#define GL_TRIANGLE_STRIP                 0x0005
#define GL_TRIANGLE_FAN                   0x0006
#define GL_QUADS                          0x0007

#define GL_UNPACK_ALIGNMENT               0x0CF5
void PixelStorei(GLenum name, GLint param) {
    if (name == GL_UNPACK_ALIGNMENT) {
        assert(param == 1);
    } else {
        assert(0);
    }
}

} // extern "C"

struct Point {
        float x;
        float y;
};

template<int FULL_SPANS>
static inline int check_depth(uint16_t z, uint16_t* zbuf, __m128i& outmask, int span = 0) {
    __m128i dest;
    if (FULL_SPANS > 1) {
        dest = _mm_loadu_si128((__m128i*)zbuf);
    } else {
        dest = _mm_loadl_epi64((__m128i*)zbuf);
    }
    __m128i src = _mm_set1_epi16(z);
    // Invert the depth test to check which pixels failed and should be discarded.
    __m128i mask = depthfunc == GL_LEQUAL ?
        // GL_LEQUAL: Not(LessEqual) = Greater
        _mm_cmpgt_epi16(src, dest) :
        // GL_LESS: Not(Less) = GreaterEqual
        // No GreaterEqual in SSE2, so use Not(Less) directly.
        _mm_xor_si128(_mm_cmplt_epi16(src, dest), _mm_set1_epi16(0xFFFF));
    if (FULL_SPANS > 1) {
        switch (_mm_movemask_epi8(mask)) {
        case 0xFFFF:
            return 0;
        case 0:
            if (depthmask) _mm_storeu_si128((__m128i*)zbuf, src);
            return -1;
        }
        if (depthmask) {
            _mm_storeu_si128((__m128i*)zbuf,
                _mm_or_si128(_mm_and_si128(mask, dest),
                             _mm_andnot_si128(mask, src)));
        }
        outmask = mask;
    } else {
        if (FULL_SPANS) {
            mask = _mm_move_epi64(mask);
            if (_mm_movemask_epi8(mask) == 0xFF) {
                return 0;
            }
        } else {
            mask = _mm_or_si128(mask,
                                _mm_cmplt_epi16(_mm_set1_epi16(span), _mm_set_epi16(8, 7, 6, 5, 4, 3, 2, 1)));
            if (_mm_movemask_epi8(mask) == 0xFFFF) {
                return 0;
            }
        }
        if (depthmask) {
            _mm_storel_epi64((__m128i*)zbuf,
                _mm_or_si128(_mm_and_si128(mask, dest),
                             _mm_andnot_si128(mask, src)));
        }
        outmask = _mm_unpacklo_epi16(mask, mask);
    }
    return 1;
}

static inline __m128i pack_pixels() {
    auto output = fragment_shader.gl_FragColor * 255.5f;
    __m128i rxz = _mm_packs_epi32(_mm_cvtps_epi32(output.x), _mm_cvtps_epi32(output.z));
    __m128i ryw = _mm_packs_epi32(_mm_cvtps_epi32(output.y), _mm_cvtps_epi32(output.w));
    __m128i rxy = _mm_unpacklo_epi16(rxz, ryw);
    __m128i rzw = _mm_unpackhi_epi16(rxz, ryw);
    __m128i rlo = _mm_unpacklo_epi32(rxy, rzw);
    __m128i rhi = _mm_unpackhi_epi32(rxy, rzw);
    return _mm_packus_epi16(rlo, rhi);
}

static inline __m128i blend_pixels(__m128i dst) {
    auto output = fragment_shader.gl_FragColor * 255.5f;
    __m128i sxz = _mm_packs_epi32(_mm_cvtps_epi32(output.x), _mm_cvtps_epi32(output.z));
    __m128i syw = _mm_packs_epi32(_mm_cvtps_epi32(output.y), _mm_cvtps_epi32(output.w));
    __m128i sxy = _mm_unpacklo_epi16(sxz, syw);
    __m128i szw = _mm_unpackhi_epi16(sxz, syw);
    __m128i slo = _mm_unpacklo_epi32(sxy, szw);
    __m128i shi = _mm_unpackhi_epi32(sxy, szw);
    __m128i dlo = _mm_unpacklo_epi8(dst, _mm_setzero_si128());
    __m128i dhi = _mm_unpackhi_epi8(dst, _mm_setzero_si128());
    __m128i r;
    // (x*y + x) >> 8, cheap approximation of (x*y) / 255
    #define muldiv255(x, y) ({ __m128i b = x; _mm_srli_epi16(_mm_add_epi16(_mm_mullo_epi16(b, y), b), 8); })
    #define alphas(c) _mm_shufflehi_epi16(_mm_shufflelo_epi16(c, 0xFF), 0xFF)
    #define rgb_mask _mm_set1_epi64x(0x0000FFFFFFFFFFFFULL)
    #define alpha_mask _mm_set1_epi64x(0xFFFF000000000000ULL)
    switch (blend_key) {
    case BLEND_KEY_NONE:
        r = _mm_packus_epi16(slo, shi);
        break;
    case BLEND_KEY(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE):
        r = _mm_packus_epi16(
                _mm_add_epi16(dlo, muldiv255(_mm_or_si128(_mm_sub_epi16(slo, dlo), alpha_mask), alphas(slo))),
                _mm_add_epi16(dhi, muldiv255(_mm_or_si128(_mm_sub_epi16(shi, dhi), alpha_mask), alphas(shi))));
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
    case BLEND_KEY(GL_ZERO, GL_ONE_MINUS_SRC_COLOR, GL_ZERO, GL_ONE):
        r = _mm_packus_epi16(
                _mm_sub_epi16(dlo, _mm_and_si128(muldiv255(dlo, slo), rgb_mask)),
                _mm_sub_epi16(dhi, _mm_and_si128(muldiv255(dhi, shi), rgb_mask)));
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
    case BLEND_KEY(GL_ONE, GL_ONE):
        r = _mm_packus_epi16(_mm_add_epi16(slo, dlo), _mm_add_epi16(shi, dhi));
        break;
    case BLEND_KEY(GL_ONE, GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA):
        r = _mm_packus_epi16(
                _mm_add_epi16(slo, _mm_sub_epi16(dlo, _mm_and_si128(muldiv255(dlo, slo), alpha_mask))),
                _mm_add_epi16(shi, _mm_sub_epi16(dhi, _mm_and_si128(muldiv255(dhi, shi), alpha_mask))));
        break;
    case BLEND_KEY(GL_ONE, GL_ZERO):
        r = _mm_packus_epi16(slo, shi);
        break;
    case BLEND_KEY(GL_ONE_MINUS_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE):
        r = _mm_packus_epi16(
                _mm_add_epi16(dlo, _mm_and_si128(_mm_sub_epi16(slo, muldiv255(slo, alphas(slo))), rgb_mask)),
                _mm_add_epi16(dhi, _mm_and_si128(_mm_sub_epi16(shi, muldiv255(shi, alphas(shi))), rgb_mask)));
        break;
    case BLEND_KEY(GL_CONSTANT_COLOR, GL_ONE_MINUS_SRC_COLOR):
        r = _mm_packus_epi16(
                _mm_add_epi16(dlo, muldiv255(_mm_sub_epi16(blendcolor, dlo), slo)),
                _mm_add_epi16(dhi, muldiv255(_mm_sub_epi16(blendcolor, dhi), shi)));
        break;
    case BLEND_KEY(GL_ONE, GL_ONE_MINUS_SRC1_COLOR): {
        auto secondary = fragment_shader.gl_SecondaryFragColor * 255.5f;
        __m128i s1xz = _mm_packs_epi32(_mm_cvtps_epi32(secondary.x), _mm_cvtps_epi32(secondary.z));
        __m128i s1yw = _mm_packs_epi32(_mm_cvtps_epi32(secondary.y), _mm_cvtps_epi32(secondary.w));
        __m128i s1xy = _mm_unpacklo_epi16(s1xz, s1yw);
        __m128i s1zw = _mm_unpackhi_epi16(s1xz, s1yw);
        __m128i s1lo = _mm_unpacklo_epi32(s1xy, s1zw);
        __m128i s1hi = _mm_unpackhi_epi32(s1xy, s1zw);
        r = _mm_packus_epi16(
                _mm_add_epi16(slo, _mm_sub_epi16(dlo, muldiv255(dlo, s1lo))),
                _mm_add_epi16(shi, _mm_sub_epi16(dhi, muldiv255(dhi, s1hi))));
        break;
    }
    default:
        UNREACHABLE;
        break;
    }
    return r;
}

static const size_t MAX_FLATS = 64;
typedef float Flats[MAX_FLATS];

static const size_t MAX_INTERPOLANTS = 16;
typedef float Interpolants __attribute__((ext_vector_type(MAX_INTERPOLANTS)));

template<bool DISCARD>
static inline void commit_output(uint32_t* buf, __m128i mask = _mm_setzero_si128()) {
    fragment_shader.run();
    __m128i dst = _mm_loadu_si128((__m128i*)buf);
    __m128i r = blend ? blend_pixels(dst) : pack_pixels();
    if (DISCARD) mask = _mm_or_si128(mask, fragment_shader.isPixelDiscarded);
    _mm_storeu_si128((__m128i*)buf,
        _mm_or_si128(_mm_and_si128(mask, dst),
                     _mm_andnot_si128(mask, r)));
}

template<>
static inline void commit_output<false>(uint32_t* buf, __m128i mask) {
    fragment_shader.run();
    __m128i r = blend ? blend_pixels(_mm_loadu_si128((__m128i*)buf)) : pack_pixels();
    _mm_storeu_si128((__m128i*)buf, r);
}

template<bool DISCARD>
static inline void commit_output(uint32_t* buf, uint16_t z, uint16_t* zbuf) {
    __m128i mask;
    if (check_depth<1>(z, zbuf, mask)) {
        commit_output<DISCARD>(buf, mask);
    } else {
        fragment_shader.skip();
    }
}

template<bool DISCARD>
static inline void commit_output(uint32_t* buf, uint16_t z, uint16_t* zbuf, int span) {
    __m128i mask;
    if (check_depth<0>(z, zbuf, mask, span)) {
        commit_output<DISCARD>(buf, mask);
    }
}

template<bool DISCARD>
static inline void commit_output(uint32_t* buf, int span) {
    commit_output<DISCARD>(buf, _mm_cmplt_epi32(_mm_set1_epi32(span), _mm_set_epi32(4, 3, 2, 1)));
}

static int shaded_rows = 0;
static int shaded_pixels = 0;

void draw_quad(int nump, Texture& colortex, Texture& depthtex) {
        Flats flat_outs;
        Interpolants interp_outs[4] = { 0 };
        vertex_shader.run((char *)flat_outs, (char *)interp_outs, sizeof(Interpolants));
        Float w = 1.0f / vertex_shader.gl_Position.w;
        vec3 clip = vertex_shader.gl_Position.sel(X, Y, Z) * w;
        Point p[4];
        vec3 screen = (clip + 1)*vec3(viewport.width/2, viewport.height/2, 0.5f) + vec3(viewport.x, viewport.y, 0);
        for (int k = 0; k < 4; k++)
        {
        //        vec4 pos = vertex_shader.gl_Position;
        //        printf("%f %f %f %f\n", pos.x[k], pos.y[k], pos.z[k], pos.y[k]);
                p[k] = Point { screen.x[k], screen.y[k] };
        //        printf("%f %f\n", p[k].x, p[k].y);
        }

        auto top_left = p[0];
        auto bot_right = p[0];
        for (int i = 1; i < nump; i++) {
            top_left.x = std::min(top_left.x, p[i].x);
            top_left.y = std::min(top_left.y, p[i].y);
            bot_right.x = std::max(bot_right.x, p[i].x);
            bot_right.y = std::max(bot_right.y, p[i].y);
        }
        //printf("bbox: %f %f %f %f\n", top_left.x, top_left.y, bot_right.x, bot_right.y);

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

        if (top_left.x >= fx1 || top_left.y >= fy1 || bot_right.x <= fx0 || bot_right.y <= fy0) {
            return;
        }

        // SSE2 does not support unsigned comparison, so bias Z to be negative.
        uint16_t z = uint16_t(0xFFFF * screen.z.x) - 0x8000;
        fragment_shader.gl_FragCoord.z = screen.z.x;
        fragment_shader.gl_FragCoord.w = w.x;

        fragment_shader.init_primitive(flat_outs);

        Point l0, r0, l1, r1;
        int l0i, r0i, l1i, r1i;
        {
            int top = 0;
            for (int k = 1; k < nump; k++) {
                if (p[k].y < p[top].y) {
                    top = k;
                }
            }
            int next = (top+1)%nump;
            int prev = (top+nump-1)%nump;
            if (p[top].y == p[next].y) {
                l0i = next;
                l1i = (next+1)%nump;
                r0i = top;
                r1i = prev;
            } else if (p[top].y == p[prev].y) {
                l0i = top;
                l1i = next;
                r0i = prev;
                r1i = (prev+nump-1)%nump;
            } else {
                l0i = r0i = top;
                l1i = next;
                r1i = prev;
            }
            l0 = p[l0i];
            r0 = p[r0i];
            l1 = p[l1i];
            r1 = p[r1i];
        //    printf("l0: %d(%f,%f), r0: %d(%f,%f) -> l1: %d(%f,%f), r1: %d(%f,%f)\n", l0i, l0.x, l0.y, r0i, r0.x, r0.y, l1i, l1.x, l1.y, r1i, r1.x, r1.y);
        }

        float lx = l0.x;
        float lm = (l1.x - l0.x) / (l1.y - l0.y);
        float rx = r0.x;
        float rm = (r1.x - r0.x) / (r1.y - r0.y);
        assert(l0.y == r0.y);
        float y = floor(std::max(l0.y, fy0) + 0.5f) + 0.5f;
        lx += (y - l0.y) * lm;
        rx += (y - r0.y) * rm;
        Interpolants lo = interp_outs[l0i];
        Interpolants lom = (interp_outs[l1i] - lo) * (1.0f / (l1.y - l0.y));
        lo = lo + lom * (y - l0.y);
        Interpolants ro = interp_outs[r0i];
        Interpolants rom = (interp_outs[r1i] - ro) * (1.0f / (r1.y - r0.y));
        ro = ro + rom * (y - r0.y);
        fbuf += int(y) * colortex.width;
        fdepth += int(y) * depthtex.width;
        while (y < fy1) {
            if (y > l1.y) {
                l0i = l1i;
                l0 = l1;
                l1i = (l1i+1)%nump;
                l1 = p[l1i];
                if (l1.y <= l0.y) break;
                lm = (l1.x - l0.x) / (l1.y - l0.y);
                lx = l0.x + (y - l0.y) * lm;
                lo = interp_outs[l0i];
                lom = (interp_outs[l1i] - lo) * (1.0f / (l1.y - l0.y));
                lo += lom * (y - l0.y);
            }
            if (y > r1.y) {
                r0i = r1i;
                r0 = r1;
                r1i = (r1i+nump-1)%nump;
                r1 = p[r1i];
                if (r1.y <= r0.y) break;
                rm = (r1.x - r0.x) / (r1.y - r0.y);
                rx = r0.x + (y - r0.y) * rm;
                ro = interp_outs[r0i];
                rom = (interp_outs[r1i] - ro) * (1.0f / (r1.y - r0.y));
                ro += rom * (y - r0.y);
            }
            int startx = int(std::max(std::min(lx, rx), fx0) + 0.5f);
            int endx = int(std::min(std::max(lx, rx), fx1) + 0.5f);
            int span = endx - startx;
            if (span > 0) {
                shaded_rows++;
                shaded_pixels += span;
                fragment_shader.gl_FragCoord.x = init_interp(startx + 0.5f, 1);
                fragment_shader.gl_FragCoord.y = y;
                {
                    Interpolants step = (ro - lo) * (1.0f / (rx - lx));
                    Interpolants o = lo + step * (startx + 0.5f - lx);
                    fragment_shader.init_span(&o, &step, 4.0f);
                }
                uint32_t* buf = fbuf + startx;
                uint16_t* depth = fdepth + startx;
                if (!fragment_shader.use_discard()) {
                    if (fdepth) {
                        for (; span >= 8; span -= 8, buf += 8, depth += 8) {
                            __m128i zmask;
                            switch (check_depth<2>(z, depth, zmask)) {
                            case 0:
                                fragment_shader.skip();
                                fragment_shader.skip();
                                break;
                            case -1:
                                commit_output<false>(buf);
                                commit_output<false>(buf + 4);
                                break;
                            default:
                                commit_output<false>(buf, _mm_unpacklo_epi16(zmask, zmask));
                                commit_output<false>(buf + 4, _mm_unpackhi_epi16(zmask, zmask));
                                break;
                            }
                        }
                        for (; span >= 4; span -= 4, buf += 4, depth += 4) {
                            commit_output<false>(buf, z, depth);
                        }
                        if (span > 0) {
                            commit_output<false>(buf, z, depth, span);
                        }
                    } else {
                        for (; span >= 4; span -= 4, buf += 4) {
                            commit_output<false>(buf);
                        }
                        if (span > 0) {
                            commit_output<false>(buf, span);
                        }
                    }
                } else {
                    if (fdepth) {
                        for (; span >= 4; span -= 4, buf += 4, depth += 4) {
                            commit_output<true>(buf, z, depth);
                        }
                        if (span > 0) {
                            commit_output<true>(buf, z, depth, span);
                        }
                    } else {
                        for (; span >= 4; span -= 4, buf += 4, depth += 4) {
                            commit_output<true>(buf);
                        }
                        if (span > 0) {
                            commit_output<true>(buf, span);
                        }
                    }
                }
            }
            lx += lm;
            rx += rm;
            y++;
            lo += lom;
            ro += rom;
            fbuf += colortex.width;
            fdepth += depthtex.width;
        }
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
        assert(mode == GL_TRIANGLES || mode == GL_QUADS);
        assert(type == GL_UNSIGNED_SHORT);
        assert(count == 6);
        assert(indices == 0);

        Framebuffer& fb = get_draw_framebuffer();
        Texture& colortex = textures[fb.color_attachment];
        colortex.make_renderable();
        assert(colortex.internal_format == GL_RGBA8);
        static Texture nodepthtex;
        Texture& depthtex = depthtest && fb.depth_attachment ? textures[fb.depth_attachment] : nodepthtex;
        if (&depthtex != &nodepthtex) {
            assert(depthtex.internal_format == GL_DEPTH_COMPONENT16);
            assert(colortex.width == depthtex.width && colortex.height == depthtex.height);
            depthtex.make_renderable();
        }

#ifdef  __MACH__
        double start = mach_absolute_time();
#else
        double start = ({ struct timespec tp; clock_gettime(CLOCK_MONOTONIC, &tp); tp.tv_sec * 1e9 + tp.tv_nsec; });
#endif

        shaded_rows = 0;
        shaded_pixels = 0;

        Program &p = programs[current_program];
        assert(p.impl);
        vertex_shader.init_batch(p.impl);
        fragment_shader.init_batch(p.impl);
        for (int instance = 0; instance < instancecount; instance++) {
            if (indices == 0) {
                Buffer &indices_buf = buffers[current_buffer[GL_ELEMENT_ARRAY_BUFFER]];
                //printf("current_vertex_array %d\n", current_vertex_array);
                //printf("indices size: %d\n", indices_buf.size);
                VertexArray &v = vertex_arrays[current_vertex_array];
                for (int i = 0; i < MAX_ATTRS; i++) {
                        if (v.attribs[i].enabled) {
                                VertexAttrib &attr = v.attribs[i];
                                VertexArray &v = vertex_arrays[attr.vertex_array];
                                Buffer &vertex_buf = buffers[attr.vertex_buffer];
                                attr.buf = vertex_buf.buf;
                                attr.buf_size = vertex_buf.size;
                                //printf("%d %x %d %d %d %d\n", i, attr.type, attr.size, attr.stride, attr.offset, attr.divisor);
                        }
                }
                if (type == GL_UNSIGNED_SHORT) {
                        assert(indices_buf.size == count * 2);
                        unsigned short *indices = (unsigned short*)indices_buf.buf;
                        if (mode == GL_QUADS) for (int i = 0; i + 4 <= count; i += 4) {
                                vertex_shader.load_attribs(p.impl, v.attribs, indices, i, instance, 4);
                                //printf("native quad %d %d %d %d\n", indices[i], indices[i+1], indices[i+2], indices[i+3]);
                                draw_quad(4, colortex, depthtex);
                        } else for (int i = 0; i + 3 <= count; i += 3) {
                                if (i + 6 <= count && indices[i+3] == indices[i+2] && indices[i+4] == indices[i+1]) {
                                    unsigned short quad_indices[4] = { indices[i], indices[i+1], indices[i+5], indices[i+2] };
                                    vertex_shader.load_attribs(p.impl, v.attribs, quad_indices, 0, instance, 4);
                                    //printf("emulate quad %d %d %d %d\n", indices[i], indices[i+1], indices[i+5], indices[i+2]);
                                    draw_quad(4, colortex, depthtex);
                                    i += 3;
                                } else {
                                    vertex_shader.load_attribs(p.impl, v.attribs, indices, i, instance, 3);
                                    //printf("triangle %d %d %d %d\n", indices[i], indices[i+1], indices[i+2]);
                                    draw_quad(3, colortex, depthtex);
                                }
                         }

                } else {
                        assert(0);
                }
            }
        }

#ifdef  __MACH__
        double end = mach_absolute_time();
#else
        double end = ({ struct timespec tp; clock_gettime(CLOCK_MONOTONIC, &tp); tp.tv_sec * 1e9 + tp.tv_nsec; });
#endif
        printf("draw(%d): %fms for %d pixels in %d rows (avg %f pixels/row, %f ns/pixel)\n", instancecount, (end - start)/(1000.*1000.), shaded_pixels, shaded_rows, double(shaded_pixels)/shaded_rows, (end - start)/std::max(shaded_pixels, 1));
}

void Finish() {
        Framebuffer& fb = get_draw_framebuffer();
        Texture& t = textures[fb.color_attachment];
        t.make_renderable();
        //writetga("out.tga", t.width, t.height, t.buf);
        write_png("out.png", t.buf, t.width, t.height);
}

} // extern "C"
