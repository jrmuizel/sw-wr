#include <assert.h>
#ifdef __MACH__
#include <mach/mach.h>
#include <mach/mach_time.h>
#else
#include <time.h>
#endif

#include <map>
#include <string>
#include <algorithm>
#include "glsl.h"

#ifdef NDEBUG
#define debugf(...)
#else
#define debugf(...) printf(__VA_ARGS__)
#endif

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
typedef int64_t GLint64;
typedef uint64_t GLuint64;

typedef float GLfloat;
typedef double GLdouble;

typedef uint32_t GLenum;
typedef int32_t GLboolean;
typedef uint32_t GLbitfield;

typedef int32_t GLsizei;
typedef size_t GLsizeiptr;
typedef intptr_t GLintptr;

struct IntRect {
    int x;
    int y;
    int width;
    int height;
};

struct VertexAttrib {
        size_t size = 0; // in bytes
        GLenum type = 0;
        bool normalized = false;
        GLsizei stride = 0;
        GLuint offset = 0;
        bool enabled = false;
        GLuint divisor = 0;
        int vertex_array = 0;
        int vertex_buffer = 0;
        char *buf = nullptr; // XXX: this can easily dangle
        size_t buf_size = 0; // this will let us bounds check
};
#define GL_RGBA32F                        0x8814
#define GL_RGBA8                          0x8058
#define GL_R8                             0x8229
#define GL_RGBA32I                        0x8D82
#define GL_BGRA8                          0x93A1

#define GL_BYTE                           0x1400
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_SHORT                          0x1402
#define GL_UNSIGNED_SHORT                 0x1403
#define GL_INT                            0x1404
#define GL_UNSIGNED_INT                   0x1405
#define GL_FLOAT                          0x1406

#define GL_RED                            0x1903
#define GL_GREEN                          0x1904
#define GL_BLUE                           0x1905
#define GL_ALPHA                          0x1906
#define GL_RGB                            0x1907
#define GL_RGBA                           0x1908
#define GL_RGBA_INTEGER                   0x8D99
#define GL_BGRA                           0x80E1

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
                case GL_BGRA8:
                case GL_RGBA:
                        return 4;
                case GL_R8:
                case GL_RED:
                        return 1;
                case GL_DEPTH_COMPONENT:
                case GL_DEPTH_COMPONENT16:
                        return 2;
                case GL_DEPTH_COMPONENT24:
                case GL_DEPTH_COMPONENT32:
                        return 4;
                default:
                        debugf("internal format: %x\n", internal_format);
                        assert(0);
        }
}

static inline int aligned_stride(int row_bytes) {
    return (row_bytes + 3) & ~3;
}

glsl::TextureFormat gl_format_to_texture_format(int type) {
        switch (type) {
                case GL_RGBA32F: return glsl::TextureFormat::RGBA32F;
                case GL_RGBA32I: return glsl::TextureFormat::RGBA32I;
                case GL_RGBA8: return glsl::TextureFormat::RGBA8;
                case GL_R8: return glsl::TextureFormat::R8;
                default: assert(0);
        }
}

#define GL_QUERY_RESULT                   0x8866
#define GL_QUERY_RESULT_AVAILABLE         0x8867
#define GL_TIME_ELAPSED                   0x88BF
#define GL_SAMPLES_PASSED                 0x8914

struct Query {
        uint64_t value = 0;
};

struct Buffer {
        char *buf = nullptr;
        size_t size = 0;

        bool allocate(size_t new_size) {
            if (new_size != size) {
                size = new_size;
                buf = (char*)realloc(buf, size);
                return true;
            }
            return false;
        }

        void cleanup() {
            if (buf) {
                free(buf);
                buf = nullptr;
                size = 0;
            }
        }

        ~Buffer() {
            cleanup();
        }
};

#define GL_READ_FRAMEBUFFER               0x8CA8
#define GL_DRAW_FRAMEBUFFER               0x8CA9
#define GL_FRAMEBUFFER                    0x8D40

struct Framebuffer {
        GLint color_attachment = 0;
        GLint layer = 0;
        GLint depth_attachment = 0;
};

struct Renderbuffer {
        GLint texture = 0;

        ~Renderbuffer();
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
                default: assert(0);
        }
}

#define GL_TEXTURE_2D               0x0DE1
#define GL_TEXTURE_3D               0x806F
#define GL_TEXTURE_2D_ARRAY         0x8C1A

struct Texture {
    int levels = 0;
    GLenum internal_format = 0;
    int width = 0;
    int height = 0;
    int depth = 0;
    char *buf = nullptr;
    GLenum min_filter = GL_NEAREST;
    GLenum mag_filter = GL_LINEAR;

    enum FLAGS {
        SHOULD_FREE  = 1 << 1,
    };
    int flags = SHOULD_FREE;
    bool should_free() const { return bool(flags & SHOULD_FREE); }

    void set_flag(int flag, bool val) {
        if (val) { flags |= flag; }
        else { flags &= ~flag; }
    }
    void set_should_free(bool val) { set_flag(SHOULD_FREE, val); }

    int delay_clear = 0;
    uint32_t clear_val = 0;
    uint32_t* cleared_rows = nullptr;

    void enable_delayed_clear(uint32_t val) {
        delay_clear = height;
        clear_val = val;
        if (cleared_rows) {
            memset(cleared_rows, 0, ((height + 31) / 32) * sizeof(uint32_t));
        } else {
            cleared_rows = (uint32_t*)calloc((height + 31) / 32, sizeof(uint32_t));
        }
        if (height & 31) {
            cleared_rows[height / 32] = ~0U << (height & 31);
        }
    }

    void disable_delayed_clear() {
        if (cleared_rows) {
            free(cleared_rows);
            cleared_rows = nullptr;
            delay_clear = 0;
        }
    }

    void allocate() {
        if (!buf && should_free()) {
            size_t size = aligned_stride(bytes_for_internal_format(internal_format) * width) * height * std::max(depth, 1) * levels;
            buf = (char*)malloc(size + sizeof(Float));
        }
    }

    void cleanup() {
        if (buf && should_free()) {
            free(buf);
            buf = nullptr;
        }
        disable_delayed_clear();
    }

    ~Texture() {
        cleanup();
    }
};

#define MAX_ATTRIBS 16
#define NULL_ATTRIB 15
struct VertexArray {
    VertexAttrib attribs[MAX_ATTRIBS];
    int max_attrib = -1;

    void validate();
};

#define GL_VERTEX_SHADER                  0x8B31
#define GL_FRAGMENT_SHADER                0x8B30
struct Shader {
    GLenum type = 0;
    std::string name;
};

struct VertexShaderImpl;
struct FragmentShaderImpl;

struct ProgramImpl {
    virtual ~ProgramImpl() {}
    virtual const char *get_name() const = 0;
    virtual int get_uniform(const char *name) const = 0;
    virtual bool set_sampler(int index, int value) = 0;
    virtual void bind_attrib(const char *name, int index) = 0;
    virtual VertexShaderImpl* get_vertex_shader() = 0;
    virtual FragmentShaderImpl* get_fragment_shader() = 0;
};

struct Program {
    std::string vs_name;
    std::string fs_name;
    std::map<std::string, int> attribs;
    ProgramImpl* impl = nullptr;
    VertexShaderImpl* vert_impl = nullptr;
    FragmentShaderImpl* frag_impl = nullptr;
    bool deleted = false;

    ~Program();
};

struct ShaderImpl {
    typedef void (*SetUniform1iFunc)(ShaderImpl*, int index, int value);
    typedef void (*SetUniform4fvFunc)(ShaderImpl*, int index, const float *value);
    typedef void (*SetUniformMatrix4fvFunc)(ShaderImpl*, int index, const float *value);

    SetUniform1iFunc set_uniform_1i_func = nullptr;
    SetUniform4fvFunc set_uniform_4fv_func = nullptr;
    SetUniformMatrix4fvFunc set_uniform_matrix4fv_func = nullptr;

    void set_uniform_1i(int index, int value) {
       (*set_uniform_1i_func)(this, index, value);
    }

    void set_uniform_4fv(int index, const float *value) {
       (*set_uniform_4fv_func)(this, index, value);
    }

    void set_uniform_matrix4fv(int index, const float *value) {
       (*set_uniform_matrix4fv_func)(this, index, value);
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

    void init_batch(ProgramImpl *prog) {
        (*init_batch_func)(this, prog);
    }

    ALWAYS_INLINE void load_attribs(ProgramImpl *prog, VertexAttrib *attribs, unsigned short *indices, int start, int instance, int count) {
        (*load_attribs_func)(this, prog, attribs, indices, start, instance, count);
    }

    ALWAYS_INLINE void run(char* flats, char* interps, size_t interp_stride) {
        (*run_func)(this, flats, interps, interp_stride);
    }
};

struct FragmentShaderImpl : ShaderImpl {
    typedef void (*InitBatchFunc)(FragmentShaderImpl*, ProgramImpl *prog);
    typedef void (*InitPrimitiveFunc)(FragmentShaderImpl*, const void* flats);
    typedef void (*InitSpanFunc)(FragmentShaderImpl*, const void* interps, const void* step, float step_width);
    typedef void (*RunFunc)(FragmentShaderImpl*);
    typedef void (*SkipFunc)(FragmentShaderImpl*, int chunks);
    typedef bool (*UseDiscardFunc)(FragmentShaderImpl*);
    typedef void (*DrawSpanRGBA8Func)(FragmentShaderImpl*, uint32_t* buf, int len);
    typedef void (*DrawSpanR8Func)(FragmentShaderImpl*, uint8_t* buf, int len);

    InitBatchFunc init_batch_func = nullptr;
    InitPrimitiveFunc init_primitive_func = nullptr;
    InitSpanFunc init_span_func = nullptr;
    RunFunc run_func = nullptr;
    SkipFunc skip_func = nullptr;
    UseDiscardFunc use_discard_func = nullptr;
    DrawSpanRGBA8Func draw_span_RGBA8_func = nullptr;
    DrawSpanR8Func draw_span_R8_func = nullptr;

    vec2 gl_FragCoordXY;
    vec2_scalar gl_FragCoordZW;
    Bool isPixelDiscarded;
    vec4 gl_FragColor;
    vec4 gl_SecondaryFragColor;

    ALWAYS_INLINE void step_fragcoord() {
        gl_FragCoordXY.x += 4;
    }

    ALWAYS_INLINE void step_fragcoord(int chunks) {
        gl_FragCoordXY.x += 4 * chunks;
    }

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

    ALWAYS_INLINE void skip(int chunks = 1) {
        (*skip_func)(this, chunks);
    }

    ALWAYS_INLINE bool use_discard() {
        return (*use_discard_func)(this);
    }

    ALWAYS_INLINE void draw_span(uint32_t* buf, int len) {
        (*draw_span_RGBA8_func)(this, buf, len);
    }

    ALWAYS_INLINE bool has_draw_span(uint32_t*) {
        return draw_span_RGBA8_func != nullptr;
    }

    ALWAYS_INLINE void draw_span(uint8_t* buf, int len) {
        (*draw_span_R8_func)(this, buf, len);
    }

    ALWAYS_INLINE bool has_draw_span(uint8_t*) {
        return draw_span_R8_func != nullptr;
    }
};

Program::~Program() {
    delete impl;
    delete vert_impl;
    delete frag_impl;
}

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

#define GL_FUNC_ADD             0x8006

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

#define GL_NEVER                0x0200
#define GL_LESS                 0x0201
#define GL_EQUAL                0x0202
#define GL_LEQUAL               0x0203
#define GL_GREATER              0x0204
#define GL_NOTEQUAL             0x0205
#define GL_GEQUAL               0x0206
#define GL_ALWAYS               0x0207
#define GL_DEPTH_TEST           0x0B71

#define GL_SCISSOR_TEST             0x0C11

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
#define GL_MAX_TEXTURE_UNITS              0x84E2
#define GL_MAX_TEXTURE_IMAGE_UNITS        0x8872

struct Context {
    bool validate_vertex_array = true;

    GLuint current_program = 0;

    map<GLuint, Query> queries;
    map<GLuint, Buffer> buffers;
    map<GLuint, Texture> textures;
    map<GLuint, VertexArray> vertex_arrays;
    map<GLuint, Framebuffer> framebuffers;
    map<GLuint, Renderbuffer> renderbuffers;
    map<GLuint, Shader> shaders;
    map<GLuint, Program> programs;

    GLuint query_count = 1;
    map<GLenum, GLuint> current_query;

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

    IntRect viewport = { 0, 0, 0, 0 };

    bool blend = false;
    GLenum blendfunc_srgb = GL_ONE;
    GLenum blendfunc_drgb = GL_ZERO;
    GLenum blendfunc_sa = GL_ONE;
    GLenum blendfunc_da = GL_ZERO;
    GLenum blend_equation = GL_FUNC_ADD;
    V8<uint16_t> blendcolor = 0;
    BlendKey blend_key = BLEND_KEY_NONE;

    bool depthtest = false;
    bool depthmask = true;
    GLenum depthfunc = GL_LESS;

    bool scissortest = false;
    IntRect scissor = { 0, 0, 0, 0 };

    uint32_t clearcolor = 0;
    GLdouble cleardepth = 1;

    int active_texture_index = 0;
    #define active_texture(target) ctx->current_texture[(target) + (ctx->active_texture_index << 16)]

    int unpack_row_length = 0;

    int shaded_rows = 0;
    int shaded_pixels = 0;
};
Context default_ctx;
Context *ctx = &default_ctx;
ProgramImpl *program_impl = nullptr;
VertexShaderImpl *vertex_shader = nullptr;
FragmentShaderImpl *fragment_shader = nullptr;
BlendKey blend_key = BLEND_KEY_NONE;

static void prepare_texture(Texture& t, const IntRect* skip = nullptr);

template<typename S>
S *lookup_sampler(S *s, int texture) {
        int tid = ctx->current_texture[GL_TEXTURE_2D + (texture << 16)];
        if (!tid) {
            s->width = 0;
            s->height = 0;
            s->stride = 0;
            s->buf = nullptr;
            s->format = TextureFormat::RGBA8;
            s->filter = TextureFilter::NEAREST;
        } else {
            Texture &t = ctx->textures[tid];
            prepare_texture(t);
            s->width = t.width;
            s->height = t.height;
            int bpp = bytes_for_internal_format(t.internal_format);
            s->stride = aligned_stride(bpp * t.width);
            if(bpp >= 4) s->stride /= 4;
            s->buf = (uint32_t*)t.buf; //XXX: wrong
            s->format = gl_format_to_texture_format(t.internal_format);
            s->filter = gl_filter_to_texture_filter(t.mag_filter);
        }
        return s;
}

template<typename S>
S *lookup_isampler(S *s, int texture) {
        int tid = ctx->current_texture[GL_TEXTURE_2D + (texture << 16)];
        if (!tid) {
            s->width = 0;
            s->height = 0;
            s->stride = 0;
            s->buf = nullptr;
            s->format = TextureFormat::RGBA32I;
        } else {
            Texture &t = ctx->textures[tid];
            prepare_texture(t);
            s->width = t.width;
            s->height = t.height;
            int bpp = bytes_for_internal_format(t.internal_format);
            s->stride = aligned_stride(bpp * t.width);
            if(bpp >= 4) s->stride /= 4;
            s->buf = (uint32_t*)t.buf; //XXX: wrong
            s->format = gl_format_to_texture_format(t.internal_format);
        }
        return s;
}

template<typename S>
S *lookup_sampler_array(S *s, int texture) {
        int tid = ctx->current_texture[GL_TEXTURE_2D_ARRAY + (texture << 16)];
        if (!tid) {
            s->width = 0;
            s->height = 0;
            s->depth = 0;
            s->stride = 0;
            s->height_stride = 0;
            s->buf = nullptr;
            s->format = TextureFormat::RGBA8;
            s->filter = TextureFilter::NEAREST;
        } else {
            Texture &t = ctx->textures[tid];
            prepare_texture(t);
            s->width = t.width;
            s->height = t.height;
            s->depth = t.depth;
            int bpp = bytes_for_internal_format(t.internal_format);
            s->stride = aligned_stride(bpp * t.width);
            if(bpp >= 4) s->stride /= 4;
            s->height_stride = s->stride * t.height;
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

template<typename S>
static inline S load_attrib_scalar(const char *src, size_t size, GLenum type, bool normalized) {
    if (sizeof(S) == size) {
        return *reinterpret_cast<const S*>(src);
    }
    S scalar = {0};
    if (type == GL_UNSIGNED_SHORT) {
        if (normalized) {
            for (int i = 0; i < size/sizeof(uint16_t); i++) {
                typename ElementType<S>::ty x = reinterpret_cast<const uint16_t*>(src)[i];
                put_nth_component(scalar, i, x * (1.0f / 0xFFFF));
            }
        } else {
            for (int i = 0; i < size/sizeof(uint16_t); i++) {
                typename ElementType<S>::ty x = reinterpret_cast<const uint16_t*>(src)[i];
                put_nth_component(scalar, i, x);
            }
        }
    } else {
        assert(sizeof(typename ElementType<S>::ty) == bytes_per_type(type));
        memcpy(&scalar, src, size);
    }
    return scalar;
}

template<typename T>
void load_attrib(T& attrib, VertexAttrib &va, unsigned short *indices, int start, int instance, int count) {
    typedef decltype(force_scalar(attrib)) scalar_type;
    if (!va.enabled) {
        attrib = T(0);
    } else if (va.divisor == 1) {
        char* src = (char*)va.buf + va.stride * instance + va.offset;
        assert(src + va.size <= va.buf + va.buf_size);
        attrib = T(load_attrib_scalar<scalar_type>(src, va.size, va.type, va.normalized));
    } else if (va.divisor == 0) {
        assert(sizeof(typename ElementType<T>::ty) == bytes_per_type(va.type));
        assert(count == 3 || count == 4);
        attrib = (T){
            load_attrib_scalar<scalar_type>((char*)va.buf + va.stride * indices[start + 0] + va.offset, va.size, va.type, va.normalized),
            load_attrib_scalar<scalar_type>((char*)va.buf + va.stride * indices[start + 1] + va.offset, va.size, va.type, va.normalized),
            load_attrib_scalar<scalar_type>((char*)va.buf + va.stride * indices[start + 2] + va.offset, va.size, va.type, va.normalized),
            load_attrib_scalar<scalar_type>((char*)va.buf + va.stride * indices[start + (count > 3 ? 3 : 2)] + va.offset, va.size, va.type, va.normalized),
        };
    } else {
        assert(false);
    }
}

template<typename T>
void load_flat_attrib(T& attrib, VertexAttrib &va, unsigned short *indices, int start, int instance, int count) {
    if (!va.enabled) {
        attrib = T{0};
        return;
    }
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

void setup_program(GLuint program) {
    if (!program) {
        program_impl = nullptr;
        vertex_shader = nullptr;
        fragment_shader = nullptr;
        return;
    }
    Program &p = ctx->programs[program];
    assert(p.impl);
    assert(p.vert_impl);
    assert(p.frag_impl);
    program_impl = p.impl;
    vertex_shader = p.vert_impl;
    fragment_shader = p.frag_impl;
}

extern ProgramImpl* load_shader(const std::string &name);

extern "C" {

void UseProgram(GLuint program) {
    if (ctx->current_program && program != ctx->current_program) {
        auto i = ctx->programs.find(ctx->current_program);
        if (i != ctx->programs.end() && i->second.deleted) {
            ctx->programs.erase(i);
        }
    }
    ctx->current_program = program;
    setup_program(program);
}

void SetViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
	ctx->viewport.x = x;
	ctx->viewport.y = y;
	ctx->viewport.width = width;
	ctx->viewport.height = height;
}

void Enable(GLenum cap) {
    switch (cap) {
    case GL_BLEND: ctx->blend = true; blend_key = ctx->blend_key; break;
    case GL_DEPTH_TEST: ctx->depthtest = true; break;
    case GL_SCISSOR_TEST: ctx->scissortest = true; break;
    }
}

void Disable(GLenum cap) {
    switch (cap) {
    case GL_BLEND: ctx->blend = false; blend_key = BLEND_KEY_NONE; break;
    case GL_DEPTH_TEST: ctx->depthtest = false; break;
    case GL_SCISSOR_TEST: ctx->scissortest = false; break;
    }
}

#define GL_NO_ERROR                 0

GLenum GetError() {
    return GL_NO_ERROR;
}

#define GL_VENDOR                   0x1F00
#define GL_RENDERER                 0x1F01
#define GL_VERSION                  0x1F02
#define GL_EXTENSIONS               0x1F03
#define GL_NUM_EXTENSIONS           0x821D
#define GL_MAX_TEXTURE_SIZE         0x0D33
#define GL_MAX_ARRAY_TEXTURE_LAYERS 0x88FF
#define GL_DRAW_FRAMEBUFFER_BINDING 0x8CA6
#define GL_READ_FRAMEBUFFER_BINDING 0x8CAA
#define GL_DEPTH_WRITEMASK          0x0B72
#define GL_PIXEL_PACK_BUFFER              0x88EB
#define GL_PIXEL_UNPACK_BUFFER            0x88EC
#define GL_PIXEL_PACK_BUFFER_BINDING      0x88ED
#define GL_PIXEL_UNPACK_BUFFER_BINDING    0x88EF

static const char * const extensions[] = {
    "GL_ARB_blend_func_extended",
    "GL_ARB_copy_image",
    "GL_ARB_draw_instanced",
    "GL_ARB_explicit_attrib_location",
    "GL_ARB_instanced_arrays",
    "GL_ARB_texture_storage",
    "GL_EXT_timer_query",
};

void GetIntegerv(GLenum pname, GLint *params) {
    assert(params);
    switch (pname) {
    case GL_MAX_TEXTURE_UNITS:
    case GL_MAX_TEXTURE_IMAGE_UNITS:
        params[0] = 16;
        break;
    case GL_MAX_TEXTURE_SIZE:
        params[0] = 1 << 15;
        break;
    case GL_MAX_ARRAY_TEXTURE_LAYERS:
        params[0] = 1 << 15;
        break;
    case GL_READ_FRAMEBUFFER_BINDING: {
        auto i = ctx->current_framebuffer.find(GL_READ_FRAMEBUFFER);
        params[0] = i != ctx->current_framebuffer.end() ? i->second : 0;
        break;
    }
    case GL_DRAW_FRAMEBUFFER_BINDING: {
        auto i = ctx->current_framebuffer.find(GL_DRAW_FRAMEBUFFER);
        params[0] = i != ctx->current_framebuffer.end() ? i->second : 0;
        break;
    }
    case GL_PIXEL_PACK_BUFFER_BINDING: {
        auto i = ctx->current_buffer.find(GL_PIXEL_PACK_BUFFER);
        params[0] = i != ctx->current_buffer.end() ? i->second : 0;
        break;
    }
    case GL_PIXEL_UNPACK_BUFFER_BINDING: {
        auto i = ctx->current_buffer.find(GL_PIXEL_UNPACK_BUFFER);
        params[0] = i != ctx->current_buffer.end() ? i->second : 0;
        break;
    }
    case GL_NUM_EXTENSIONS:
        params[0] = sizeof(extensions) / sizeof(extensions[0]);
        break;
    default:
        debugf("unhandled glGetIntegerv parameter %x\n", pname);
        assert(false);
    }
}

void GetBooleanv(GLenum pname, GLboolean *params) {
    assert(params);
    switch (pname) {
    case GL_DEPTH_WRITEMASK:
        params[0] = ctx->depthmask;
        break;
    default:
        debugf("unhandled glGetBooleanv parameter %x\n", pname);
        assert(false);
    }
}

const char *GetString(GLenum name) {
    switch (name) {
    case GL_VENDOR:
        return "Mozilla Gfx";
    case GL_RENDERER:
        return "Software WebRender";
    case GL_VERSION:
        return "3.2";
    default:
        debugf("unhandled glGetString parameter %x\n", name);
        assert(false);
    }
}

const char *GetStringi(GLenum name, GLuint index) {
    switch (name) {
    case GL_EXTENSIONS:
        if (index >= sizeof(extensions) / sizeof(extensions[0])) {
            return nullptr;
        }
        return extensions[index];
    default:
        debugf("unhandled glGetStringi parameter %x\n", name);
        assert(false);
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
    ctx->blendfunc_srgb = srgb;
    ctx->blendfunc_drgb = drgb;
    sa = remap_blendfunc(srgb, sa);
    da = remap_blendfunc(drgb, da);
    ctx->blendfunc_sa = sa;
    ctx->blendfunc_da = da;

    #define HASH_BLEND_KEY(x, y, z, w) ((x << 4) | (y) | (z << 24) | (w << 20))
    int hash = HASH_BLEND_KEY(srgb, drgb, 0, 0);
    if (srgb != sa || drgb != da) hash |= HASH_BLEND_KEY(0, 0, sa, da);
    switch (hash) {
    #define MAP_BLEND_KEY(...) \
        case HASH_BLEND_KEY(__VA_ARGS__): \
            ctx->blend_key = BLEND_KEY(__VA_ARGS__); \
            break;
    FOR_EACH_BLEND_KEY(MAP_BLEND_KEY)
    default:
        debugf("blendfunc: %x, %x, separate: %x, %x\n", srgb, drgb, sa, da);
        assert(false);
        break;
    }

    if (ctx->blend) {
        blend_key = ctx->blend_key;
    }
}

void BlendColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
        I32 c = roundto((Float){b, g, r, a}, 255.49f);
        ctx->blendcolor = __builtin_convertvector(c, U16).rgbargba;
}

void BlendEquation(GLenum mode) {
        assert(mode == GL_FUNC_ADD);
        ctx->blend_equation = mode;
}

void DepthMask(GLboolean flag) {
    ctx->depthmask = flag;
}

void DepthFunc(GLenum func) {
    switch (func) {
    case GL_LESS:
    case GL_LEQUAL:
        break;
    default:
        assert(false);
    }
    ctx->depthfunc = func;
}

void SetScissor(GLint x, GLint y, GLsizei width, GLsizei height) {
    ctx->scissor.x = x;
    ctx->scissor.y = y;
    ctx->scissor.width = width;
    ctx->scissor.height = height;
}

void ClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    I32 c = roundto((Float){b, g, r, a}, 255.49f);
    ctx->clearcolor = bit_cast<uint32_t>(__builtin_convertvector(c, U8));
}

void ClearDepth(GLdouble depth) {
    ctx->cleardepth = depth;
}

void ActiveTexture(GLenum texture) {
        assert(texture >= GL_TEXTURE0);
        assert(texture <= GL_TEXTURE15);
        ctx->active_texture_index = texture - GL_TEXTURE0;
}

void GenQueries(GLsizei n, GLuint *result) {
        for (int i = 0; i < n; i++) {
                Query q;
                ctx->queries.insert(pair<GLuint, Query>(ctx->query_count, q));
                result[i] = ctx->query_count++;
        }
}

void DeleteQuery(GLuint n) {
    if (!n) {
        return;
    }
    auto i = ctx->queries.find(n);
    if (i == ctx->queries.end()) {
        return;
    }
    ctx->queries.erase(i);
    for (auto& b : ctx->current_query) {
        if (b.second == n) {
            b.second = 0;
        }
    }
}

void GenBuffers(int n, int *result) {
        for (int i = 0; i < n; i++) {
                Buffer b;
                ctx->buffers.insert(pair<GLuint, Buffer>(ctx->buffer_count, b));
                result[i] = ctx->buffer_count++;
        }
}

void DeleteBuffer(GLuint n) {
    if (!n) {
        return;
    }
    auto i = ctx->buffers.find(n);
    if (i == ctx->buffers.end()) {
        return;
    }
    ctx->buffers.erase(i);
    for (auto& b : ctx->current_buffer) {
        if (b.second == n) {
            b.second = 0;
        }
    }
}

void GenVertexArrays(int n, int *result) {
        for (int i = 0; i < n; i++) {
                VertexArray v;
                ctx->vertex_arrays.insert(pair<GLuint, VertexArray>(ctx->vertex_array_count, v));
                result[i] = ctx->vertex_array_count++;
        }
}

void DeleteVertexArray(GLuint n) {
    if (!n) {
        return;
    }
    auto i = ctx->vertex_arrays.find(n);
    if (i == ctx->vertex_arrays.end()) {
        return;
    }
    ctx->vertex_arrays.erase(i);
    if (ctx->current_vertex_array == n) {
        ctx->current_vertex_array = 0;
    }
}

GLuint CreateShader(GLenum type) {
        Shader s;
        s.type = type;
        ctx->shaders.insert(pair<GLuint, Shader>(ctx->shader_count, s));
        return ctx->shader_count++;
}

void ShaderSourceByName(GLuint shader, char* name) {
    Shader &s = ctx->shaders[shader];
    s.name = name;
}

void AttachShader(GLuint program, GLuint shader) {
    Program &p = ctx->programs[program];
    Shader &s = ctx->shaders[shader];
    if (s.type == GL_VERTEX_SHADER) {
        p.vs_name = s.name;
    } else if (s.type == GL_FRAGMENT_SHADER) {
        p.fs_name = s.name;
    } else {
        assert(0);
    }
}

void DeleteShader(GLuint shader) {
    if (!shader) {
        return;
    }
    auto i = ctx->shaders.find(shader);
    if (i == ctx->shaders.end()) {
        return;
    }
    ctx->shaders.erase(i);
}

GLuint CreateProgram() {
        Program p;
        ctx->programs.insert(pair<GLuint, Program>(ctx->program_count, p));
        return ctx->program_count++;
}

void DeleteProgram(GLuint program) {
    if (!program) {
        return;
    }
    auto i = ctx->programs.find(program);
    if (i == ctx->programs.end()) {
        return;
    }
    if (ctx->current_program == program) {
        i->second.deleted = true;
    } else {
        ctx->programs.erase(i);
    }
}

void LinkProgram(GLuint program) {
    Program &p = ctx->programs[program];
    assert(p.vs_name == p.fs_name);

    p.impl = load_shader(p.vs_name);
    if (!p.impl) {
        debugf("unknown program %s\n", p.vs_name.c_str());
    }

    assert(p.impl);
    for (auto i : p.attribs) {
        p.impl->bind_attrib(i.first.c_str(), i.second);
    }

    p.vert_impl = p.impl->get_vertex_shader();
    p.frag_impl = p.impl->get_fragment_shader();
}

void BindAttribLocation(GLuint program, GLuint index, char *name) {
    Program &p = ctx->programs[program];
    p.attribs[name] = index;
}

GLint GetAttribLocation(GLuint program, char* name) {
    Program &p = ctx->programs[program];
    auto i = p.attribs.find(name);
    return i != p.attribs.end() ? i->second : -1;
}

GLint GetUniformLocation(GLuint program, char* name) {
    Program &p = ctx->programs[program];
    assert(p.impl);
    GLint loc = p.impl->get_uniform(name);
    debugf("location: %d\n", loc);
    return loc;
}

static uint64_t get_time_value() {
#ifdef  __MACH__
        return mach_absolute_time();
#else
        return ({ struct timespec tp; clock_gettime(CLOCK_MONOTONIC, &tp); tp.tv_sec * 1000000000ULL + tp.tv_nsec; });
#endif
}

void BeginQuery(GLenum target, GLuint id) {
    ctx->current_query[target] = id;
    Query &q = ctx->queries[id];
    switch (target) {
    case GL_SAMPLES_PASSED:
        q.value = 0;
        break;
    case GL_TIME_ELAPSED:
        q.value = get_time_value();
        break;
    default:
        debugf("unknown query target %x for query %d\n", target, id);
        assert(false);
    }
}

void EndQuery(GLenum target) {
    Query &q = ctx->queries[ctx->current_query[target]];
    switch (target) {
    case GL_SAMPLES_PASSED:
        break;
    case GL_TIME_ELAPSED:
        q.value = get_time_value() - q.value;
        break;
    default:
        debugf("unknown query target %x for query %d\n", target, ctx->current_query[target]);
        assert(false);
    }
    ctx->current_query[target] = 0;
}

void GetQueryObjectui64v(GLuint id, GLenum pname, GLuint64 *params)
{
    Query &q = ctx->queries[id];
    switch (pname) {
    case GL_QUERY_RESULT:
        assert(params);
        params[0] = q.value;
        break;
    default:
        assert(false);
    }
}

void BindVertexArray(GLuint vertex_array) {
    if (vertex_array != ctx->current_vertex_array) {
        ctx->validate_vertex_array = true;
    }
    ctx->current_vertex_array = vertex_array;
}

void BindTexture(GLenum target, GLuint texture) {
    active_texture(target) = texture;
}

void BindBuffer(GLenum target, GLuint buffer) {
    // XXX: I think we to set the element array buffer on the current vertex array for target == GL_ELEMENT_ARRAY_BUFFER
    ctx->current_buffer[target] = buffer;
}

void BindFramebuffer(GLenum target, GLuint fb) {
    if (target == GL_FRAMEBUFFER) {
        ctx->current_framebuffer[GL_READ_FRAMEBUFFER] = fb;
        ctx->current_framebuffer[GL_DRAW_FRAMEBUFFER] = fb;
    } else {
        assert(target == GL_READ_FRAMEBUFFER || target == GL_DRAW_FRAMEBUFFER);
        ctx->current_framebuffer[target] = fb;
    }
}

void BindRenderbuffer(GLenum target, GLuint rb) {
    ctx->current_renderbuffer[target] = rb;
}

#define GL_UNPACK_ROW_LENGTH              0x0CF2
#define GL_UNPACK_ALIGNMENT               0x0CF5

void PixelStorei(GLenum name, GLint param) {
    if (name == GL_UNPACK_ALIGNMENT) {
        assert(param == 1);
    } else if (name == GL_UNPACK_ROW_LENGTH) {
        ctx->unpack_row_length = param;
    }
}

static GLenum remap_internal_format(GLenum format) {
    switch (format) {
    case GL_DEPTH_COMPONENT:
        return GL_DEPTH_COMPONENT16;
    case GL_RGBA:
        return GL_RGBA8;
    case GL_RED:
        return GL_R8;
    default:
        return format;
    }
}

void TexStorage3D(
        GLenum target,
        GLint levels,
        GLenum internal_format,
        GLsizei width,
        GLsizei height,
        GLsizei depth
    ) {
    Texture &t = ctx->textures[active_texture(target)];
    t.levels = levels;
    t.internal_format = remap_internal_format(internal_format);
    t.width = width;
    t.height = height;
    t.depth = depth;
    t.disable_delayed_clear();
    t.allocate();
}

static void set_tex_storage(
        Texture &t,
        GLint levels,
        GLenum internal_format,
        GLsizei width,
        GLsizei height,
        bool should_free = true,
        void* buf = nullptr
    ) {
    t.levels = levels;
    t.internal_format = remap_internal_format(internal_format);
    t.width = width;
    t.height = height;
    if (t.should_free() != should_free || buf != nullptr) {
        if (t.should_free()) {
            t.cleanup();
        }
        t.set_should_free(should_free);
        t.buf = (char*)buf;
    }
    t.disable_delayed_clear();
    t.allocate();
}

void TexStorage2D(
        GLenum target,
        GLint levels,
        GLenum internal_format,
        GLsizei width,
        GLsizei height
    ) {
    Texture &t = ctx->textures[active_texture(target)];
    set_tex_storage(t, levels, internal_format, width, height);
}

GLenum internal_format_for_data(GLenum format, GLenum ty) {
    if (format == GL_RED && ty == GL_UNSIGNED_BYTE) {
        return GL_R8;
    } else if ((format == GL_RGBA || format == GL_BGRA) && ty == GL_UNSIGNED_BYTE) {
        return GL_RGBA8;
    } else if (format == GL_RGBA && ty == GL_FLOAT) {
        return GL_RGBA32F;
    } else if (format == GL_RGBA_INTEGER && ty == GL_INT) {
        return GL_RGBA32I;
    } else {
        debugf("unknown internal format for format %x, type %x\n", format, ty);
        assert(false);
        return 0;
    }
}

static inline void copy_bgra8_to_rgba8(uint32_t *dest, uint32_t *src, int width) {
        for (; width >= 4; width -= 4, dest += 4, src += 4) {
            U32 p = unaligned_load<U32>(src);
            U32 rb = p & 0x00FF00FF;
            unaligned_store(dest, (p & 0xFF00FF00) | (rb << 16) | (rb >> 16));
        }
        for (; width > 0; width--, dest++, src++) {
            uint32_t p = *src;
            uint32_t rb = p & 0x00FF00FF;
            *dest = (p & 0xFF00FF00) | (rb << 16) | (rb >> 16);
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
        Texture &t = ctx->textures[active_texture(target)];
        IntRect skip = { xoffset, yoffset, width, height };
        prepare_texture(t, &skip);
        assert(xoffset + width <= t.width);
        assert(yoffset + height <= t.height);
        assert(ctx->unpack_row_length == 0 || ctx->unpack_row_length >= width);
        GLsizei row_length = ctx->unpack_row_length != 0 ? ctx->unpack_row_length : width;
        assert(t.internal_format == internal_format_for_data(format, ty));
        int bpp = bytes_for_internal_format(t.internal_format);
        if (!bpp) return;
        int dest_stride = aligned_stride(bpp * t.width);
        char *dest = t.buf + yoffset * dest_stride + xoffset * bpp;
        char *src = (char*)data;
        for (int y = 0; y < height; y++) {
                if (t.internal_format == GL_RGBA8 && format != GL_BGRA) {
                    copy_bgra8_to_rgba8((uint32_t*)dest, (uint32_t*)src, width);
                } else {
                    memcpy(dest, src, width * bpp);
                }
                dest += dest_stride;
                src += row_length * bpp;
        }
}

void TexImage2D(
        GLenum target,
        GLint level,
        GLint internal_format,
        GLsizei width,
        GLsizei height,
        GLint border,
        GLenum format,
        GLenum ty,
        void *data) {
        assert(level == 0);
        assert(border == 0);
        TexStorage2D(target, 1, internal_format, width, height);
        if (data) {
            TexSubImage2D(target, 0, 0, 0, width, height, format, ty, data);
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
        Texture &t = ctx->textures[active_texture(target)];
        prepare_texture(t);
        assert(ctx->unpack_row_length == 0 || ctx->unpack_row_length >= width);
        GLsizei row_length = ctx->unpack_row_length != 0 ? ctx->unpack_row_length : width;

        if (format == GL_BGRA) {
            assert(ty == GL_UNSIGNED_BYTE);
            assert(t.internal_format == GL_RGBA8);
        } else {
            assert(t.internal_format == internal_format_for_data(format, ty));
        }
        int bpp = bytes_for_internal_format(t.internal_format);
        if (!bpp) return;
        char *src = (char*)data;
        assert(xoffset + width <= t.width);
        assert(yoffset + height <= t.height);
        assert(zoffset + depth <= t.depth);
        int dest_stride = aligned_stride(bpp * t.width);
        for (int z = 0; z < depth; z++) {
                char *dest = t.buf + ((zoffset + z) * t.height + yoffset) * dest_stride + xoffset * bpp;
                for (int y = 0; y < height; y++) {
                        if (t.internal_format == GL_RGBA8 && format != GL_BGRA) {
                            copy_bgra8_to_rgba8((uint32_t*)dest, (uint32_t*)src, width);
                        } else {
                            memcpy(dest, src, width * bpp);
                        }
                        dest += dest_stride;
                        src += row_length * bpp;
                }
        }

}

void TexImage3D(
        GLenum target,
        GLint level,
        GLint internal_format,
        GLsizei width,
        GLsizei height,
        GLsizei depth,
        GLint border,
        GLenum format,
        GLenum ty,
        void *data) {
        assert(level == 0);
        assert(border == 0);
        TexStorage3D(target, 1, internal_format, width, height, depth);
        if (data) {
            TexSubImage3D(target, 0, 0, 0, 0, width, height, depth, format, ty, data);
        }
}

void TexParameteri(GLenum target, GLenum pname, GLint param) {
        Texture &t = ctx->textures[active_texture(target)];
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
                ctx->textures.insert(pair<GLuint, Texture>(ctx->texture_count, t));
                result[i] = ctx->texture_count++;
        }
}

void DeleteTexture(GLuint n) {
    if (!n) {
        return;
    }
    auto i = ctx->textures.find(n);
    if (i == ctx->textures.end()) {
        return;
    }
    ctx->textures.erase(i);
    for (auto& b : ctx->current_texture) {
        if (b.second == n) {
            b.second = 0;
        }
    }
}

void GenRenderbuffers(int n, int *result) {
        for (int i = 0; i < n; i++) {
                Renderbuffer r;
                ctx->renderbuffers.insert(pair<GLuint, Renderbuffer>(ctx->renderbuffer_count, r));
                result[i] = ctx->renderbuffer_count++;
        }
}

Renderbuffer::~Renderbuffer() {
    for (auto& i : ctx->framebuffers) {
        auto& fb = i.second;
        if (fb.color_attachment == texture) {
            fb.color_attachment = 0;
            fb.layer = 0;
        }
        if (fb.depth_attachment == texture) {
            fb.depth_attachment = 0;
        }
    }
    DeleteTexture(texture);
}

void DeleteRenderbuffer(GLuint n) {
    if (!n) {
        return;
    }
    auto i = ctx->renderbuffers.find(n);
    if (i == ctx->renderbuffers.end()) {
        return;
    }
    ctx->renderbuffers.erase(i);
    for (auto& b : ctx->current_renderbuffer) {
        if (b.second == n) {
            b.second = 0;
        }
    }
}

void GenFramebuffers(int n, int *result) {
        for (int i = 0; i < n; i++) {
                Framebuffer f;
                ctx->framebuffers.insert(pair<GLuint, Framebuffer>(ctx->framebuffer_count, f));
                result[i] = ctx->framebuffer_count++;
        }
}

void DeleteFramebuffer(GLuint n) {
    if (!n) {
        return;
    }
    auto i = ctx->framebuffers.find(n);
    if (i == ctx->framebuffers.end()) {
        return;
    }
    ctx->framebuffers.erase(i);
    for (auto& b : ctx->current_framebuffer) {
        if (b.second == n) {
            b.second = 0;
        }
    }
}

void RenderbufferStorage(
        GLenum target,
        GLenum internalformat,
        GLsizei width,
        GLsizei height) {
    // Just refer a renderbuffer to a texture to simplify things for now...
    Renderbuffer &r = ctx->renderbuffers[ctx->current_renderbuffer[target]];
    GenTextures(1, &r.texture);
    active_texture(target) = r.texture;
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

void VertexAttribPointer(GLuint index,
        GLint size,
        GLenum type,
        bool normalized,
        GLsizei stride,
        GLuint offset)
{
        debugf("cva: %d\n", ctx->current_vertex_array);
        VertexArray &v = ctx->vertex_arrays[ctx->current_vertex_array];
        VertexAttrib &va = v.attribs[index];
        va.size = size * bytes_per_type(type);
        va.type = type;
        va.normalized = normalized;
        va.stride = stride;
        va.offset = offset;
        Buffer &vertex_buf = ctx->buffers[ctx->current_buffer[GL_ARRAY_BUFFER]];
        va.vertex_buffer = ctx->current_buffer[GL_ARRAY_BUFFER];
        va.vertex_array = ctx->current_vertex_array;
        ctx->validate_vertex_array = true;
}

void VertexAttribIPointer(GLuint index,
        GLint size,
        GLenum type,
        GLsizei stride,
        GLuint offset)
{
        debugf("cva: %d\n", ctx->current_vertex_array);
        VertexArray &v = ctx->vertex_arrays[ctx->current_vertex_array];
        VertexAttrib &va = v.attribs[index];
        va.size = size * bytes_per_type(type);
        va.type = type;
        va.normalized = false;
        va.stride = stride;
        va.offset = offset;
        Buffer &vertex_buf = ctx->buffers[ctx->current_buffer[GL_ARRAY_BUFFER]];
        va.vertex_buffer = ctx->current_buffer[GL_ARRAY_BUFFER];
        va.vertex_array = ctx->current_vertex_array;
        ctx->validate_vertex_array = true;
}

void EnableVertexAttribArray(GLuint index) {
        VertexArray &v = ctx->vertex_arrays[ctx->current_vertex_array];
        VertexAttrib &va = v.attribs[index];
        if (!va.enabled) {
            ctx->validate_vertex_array = true;
        }
        va.enabled = true;
        v.max_attrib = std::max(v.max_attrib, (int)index);
}

void DisableVertexAttribArray(GLuint index) {
        VertexArray &v = ctx->vertex_arrays[ctx->current_vertex_array];
        VertexAttrib &va = v.attribs[index];
        if (va.enabled) {
            ctx->validate_vertex_array = true;
        }
        va.enabled = false;
}

void VertexAttribDivisor(GLuint index, GLuint divisor) {
        VertexArray &v = ctx->vertex_arrays[ctx->current_vertex_array];
        VertexAttrib &va = v.attribs[index];
        va.divisor = divisor;
}

void BufferData(GLenum target,
        GLsizeiptr size,
        void *data,
        GLenum usage)
{
    Buffer &b = ctx->buffers[ctx->current_buffer[target]];
    if (b.allocate(size)) {
        ctx->validate_vertex_array = true;
    }
    memcpy(b.buf, data, size);
}

void BufferSubData(GLenum target,
        GLintptr offset,
        GLsizeiptr size,
        void *data)
{
    Buffer &b = ctx->buffers[ctx->current_buffer[target]];
    assert(offset + size <= b.size);
    memcpy(&b.buf[offset], data, size);
}

void Uniform1i(GLint location, GLint V0) {
    debugf("tex: %d\n", ctx->texture_count);
    if (!program_impl->set_sampler(location, V0)) {
        vertex_shader->set_uniform_1i(location, V0);
        fragment_shader->set_uniform_1i(location, V0);
    }
}
void Uniform4fv(GLint location, GLsizei count, const GLfloat *v) {
        vertex_shader->set_uniform_4fv(location, v);
        fragment_shader->set_uniform_4fv(location, v);
}
void UniformMatrix4fv(GLint location,
 	GLsizei count,
 	GLboolean transpose,
 	const GLfloat *value) {
        vertex_shader->set_uniform_matrix4fv(location, value);
        fragment_shader->set_uniform_matrix4fv(location, value);
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
        assert(target == GL_READ_FRAMEBUFFER || target == GL_DRAW_FRAMEBUFFER);
        Framebuffer &fb = ctx->framebuffers[ctx->current_framebuffer[target]];
        if (attachment == GL_COLOR_ATTACHMENT0) {
                fb.color_attachment = texture;
                fb.layer = 0;
        } else if (attachment == GL_DEPTH_ATTACHMENT) {
                fb.depth_attachment = texture;
        } else {
                assert(0);
        }
}

void FramebufferTextureLayer(
        GLenum target,
        GLenum attachment,
        GLuint texture,
        GLint level,
        GLint layer)
{
        assert(level == 0);
        assert(target == GL_READ_FRAMEBUFFER || target == GL_DRAW_FRAMEBUFFER);
        Framebuffer &fb = ctx->framebuffers[ctx->current_framebuffer[target]];
        if (attachment == GL_COLOR_ATTACHMENT0) {
                fb.color_attachment = texture;
                fb.layer = layer;
        } else if (attachment == GL_DEPTH_ATTACHMENT) {
                assert(layer == 0);
                fb.depth_attachment = texture;
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
    Framebuffer &fb = ctx->framebuffers[ctx->current_framebuffer[target]];
    Renderbuffer &rb = ctx->renderbuffers[renderbuffer];
    if (attachment == GL_COLOR_ATTACHMENT0) {
        fb.color_attachment = rb.texture;
        fb.layer = 0;
    } else if (attachment == GL_DEPTH_ATTACHMENT) {
        fb.depth_attachment = rb.texture;
    } else {
        assert(0);
    }
}

} // extern "C"

Framebuffer *get_framebuffer(GLenum target) {
    GLuint id = 0;
    {
        auto i = ctx->current_framebuffer.find(target);
        if (i != ctx->current_framebuffer.end()) id = i->second;
    }
    {
        auto i = ctx->framebuffers.find(id);
        if (i != ctx->framebuffers.end()) return &i->second;
    }
    return nullptr;
}

static inline void memset32(void* dst, uint32_t val, size_t n) {
#if USE_SSE2
    __asm__ __volatile__ (
        "rep stosl\n"
        : "+D"(dst), "+c"(n)
        : "a"(val)
        : "memory", "cc"
    );
#else
    std::fill_n((uint32_t*)dst, n, val);
#endif
}

template<typename T>
static void clear_buffer(Texture& t, T value, int x0, int x1, int y0, int y1, int layer = 0, int skip_start = 0, int skip_end = 0) {
    skip_start = std::max(skip_start, x0);
    skip_end = std::max(skip_end, skip_start);
    int stride = aligned_stride(sizeof(T) * t.width);
    if (x1 - x0 == t.width && y1 - y0 > 1 && skip_start >= skip_end) {
        x1 += (stride / sizeof(T)) * (y1 - y0 - 1);
        y1 = y0 + 1;
    }
    char* buf = t.buf + stride * (t.height * layer + y0) + x0 * sizeof(T);
    uint32_t chunk = sizeof(T) == 1 ? uint32_t(value) * 0x01010101U : (sizeof(T) == 2 ? uint32_t(value) | (uint32_t(value) << 16) : value);
    for (int y = y0; y < y1; y++) {
        if (x0 < skip_start) {
            memset32(buf, chunk, (skip_start - x0) / (4 / sizeof(T)));
            if (sizeof (T) < 4) {
                std::fill((T*)buf + ((skip_start - x0) & ~(4 / sizeof(T) - 1)), (T*)buf + (skip_start - x0), value);
            }
        }
        if (skip_end < x1) {
            T* skip_buf = (T*)buf + (skip_end - x0);
            memset32(skip_buf, chunk, (x1 - skip_end) / (4 / sizeof(T)));
            if (sizeof (T) < 4) {
                std::fill(skip_buf + ((x1 - skip_end) & ~(4 / sizeof(T) - 1)), skip_buf + (x1 - skip_end), value);
            }
        }
        buf += stride;
    }
}

template<typename T>
static inline void clear_buffer(Texture& t, T value, int layer = 0) {
    int x0 = 0, y0 = 0, x1 = t.width, y1 = t.height;
    if (ctx->scissortest) {
        x0 = std::max(x0, ctx->scissor.x);
        y0 = std::max(y0, ctx->scissor.y);
        x1 = std::min(x1, ctx->scissor.x + ctx->scissor.width);
        y1 = std::min(y1, ctx->scissor.y + ctx->scissor.height);
    }
    if (x1 - x0 > 0) {
        clear_buffer<T>(t, value, x0, x1, y0, y1, layer);
    }
}

template<typename T>
static void force_clear(Texture& t, const IntRect* skip = nullptr) {
    if (!t.delay_clear) {
        return;
    }
    int y0 = 0;
    int y1 = t.height;
    int skip_start = 0;
    int skip_end = 0;
    if (skip) {
        y0 = std::min(std::max(skip->y, 0), t.height);
        y1 = std::min(std::max(skip->y + skip->height, y0), t.height);
        skip_start = std::min(std::max(skip->x, 0), t.width);
        skip_end = std::min(std::max(skip->x + skip->width, y0), t.width);
        if (skip_start <= 0 && skip_end >= t.width && y0 <= 0 && y1 >= t.height) {
            t.disable_delayed_clear();
            return;
        }
    }
    int num_masks = (y1 + 31) / 32;
    uint32_t* rows = t.cleared_rows;
    for (int i = y0 / 32; i < num_masks; i++) {
        uint32_t mask = rows[i];
        if (mask != ~0U) {
            rows[i] = ~0U;
            int start = i*32;
            while (mask) {
                int count = __builtin_ctz(mask);
                if (count > 0) {
                    clear_buffer<T>(t, t.clear_val, 0, t.width, start, start + count, 0, skip_start, skip_end);
                    t.delay_clear -= count;
                    start += count;
                    mask >>= count;
                }
                count = __builtin_ctz(mask + 1);
                start += count;
                mask >>= count;
            }
            int count = (i+1)*32 - start;
            if (count > 0) {
                clear_buffer<T>(t, t.clear_val, 0, t.width, start, start + count, 0, skip_start, skip_end);
                t.delay_clear -= count;
            }
        }
    }
    if (t.delay_clear <= 0) t.disable_delayed_clear();
}

static void prepare_texture(Texture& t, const IntRect* skip) {
    if (t.delay_clear) {
        switch (t.internal_format) {
        case GL_RGBA8:
            force_clear<uint32_t>(t, skip);
            break;
        case GL_R8:
            force_clear<uint8_t>(t, skip);
            break;
        case GL_DEPTH_COMPONENT16:
            force_clear<uint16_t>(t, skip);
            break;
        default:
            assert(false);
            break;
        }
    }
}

extern "C" {

void InitDefaultFramebuffer(int width, int height) {
    Framebuffer& fb = ctx->framebuffers[0];
    if (!fb.color_attachment) {
        GenTextures(1, &fb.color_attachment);
        fb.layer = 0;
    }
    Texture& colortex = ctx->textures[fb.color_attachment];
    if (colortex.width != width || colortex.height != height) {
        colortex.cleanup();
        set_tex_storage(colortex, 1, GL_RGBA8, width, height);
    }
    if (!fb.depth_attachment) {
        GenTextures(1, &fb.depth_attachment);
    }
    Texture& depthtex = ctx->textures[fb.depth_attachment];
    if (depthtex.width != width || depthtex.height != height) {
        depthtex.cleanup();
        set_tex_storage(depthtex, 1, GL_DEPTH_COMPONENT16, width, height);
    }
}

void* GetColorBuffer(GLuint fbo, int32_t* width, int32_t* height) {
    auto i = ctx->framebuffers.find(fbo);
    if (i == ctx->framebuffers.end()) {
        return nullptr;
    }
    Framebuffer& fb = i->second;
    if (!fb.color_attachment) {
        return nullptr;
    }
    Texture& colortex = ctx->textures[fb.color_attachment];
    prepare_texture(colortex);
    *width = colortex.width;
    *height = colortex.height;
    return colortex.buf;
}

void SetTextureBuffer(GLuint texid, GLenum internal_format, GLsizei width, GLsizei height, void* buf) {
    Texture &t = ctx->textures[texid];
    set_tex_storage(t, 1, internal_format, width, height, false, buf);
}

#define GL_FRAMEBUFFER_COMPLETE                      0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT         0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER        0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER        0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED                   0x8CDD

GLenum CheckFramebufferStatus(GLenum target) {
    Framebuffer *fb = get_framebuffer(target);
    if (!fb || !fb->color_attachment) {
        return GL_FRAMEBUFFER_UNSUPPORTED;
    }
    return GL_FRAMEBUFFER_COMPLETE;
}

#define GL_COLOR_BUFFER_BIT         0x00004000
#define GL_DEPTH_BUFFER_BIT         0x00000100
#define GL_STENCIL_BUFFER_BIT       0x00000400

static inline bool clear_requires_scissor(Texture& t) {
    return ctx->scissortest && (ctx->scissor.x > 0 || ctx->scissor.y > 0 || ctx->scissor.width < t.width || ctx->scissor.height < t.height);
}

void Clear(GLbitfield mask) {
    Framebuffer& fb = *get_framebuffer(GL_DRAW_FRAMEBUFFER);
    if ((mask & GL_COLOR_BUFFER_BIT) && fb.color_attachment) {
        Texture& t = ctx->textures[fb.color_attachment];
        if (t.internal_format == GL_RGBA8) {
            uint32_t color = ctx->clearcolor;
            if (clear_requires_scissor(t)) {
                force_clear<uint32_t>(t, &ctx->scissor);
                clear_buffer<uint32_t>(t, color, fb.layer);
            } else if (t.depth > 1) {
                t.disable_delayed_clear();
                clear_buffer<uint32_t>(t, color, fb.layer);
            } else {
                t.enable_delayed_clear(color);
            }
        } else if (t.internal_format == GL_R8) {
            uint8_t color = uint8_t((ctx->clearcolor >> 16) & 0xFF);
            if (clear_requires_scissor(t)) {
                force_clear<uint8_t>(t, &ctx->scissor);
                clear_buffer<uint8_t>(t, color, fb.layer);
            } else if (t.depth > 1) {
                t.disable_delayed_clear();
                clear_buffer<uint8_t>(t, color, fb.layer);
            } else {
                t.enable_delayed_clear(color);
            }
        } else {
            assert(false);
        }
    }
    if ((mask & GL_DEPTH_BUFFER_BIT) && fb.depth_attachment) {
        Texture& t = ctx->textures[fb.depth_attachment];
        assert(t.internal_format == GL_DEPTH_COMPONENT16);
        uint16_t depth = uint16_t(0xFFFF * ctx->cleardepth) - 0x8000;
        if (clear_requires_scissor(t)) {
            force_clear<uint16_t>(t, &ctx->scissor);
            clear_buffer<uint16_t>(t, depth);
        } else {
            t.enable_delayed_clear(depth);
        }
    }
}

void ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *data) {
    Framebuffer *fb = get_framebuffer(GL_READ_FRAMEBUFFER);
    if (!fb) return;
    assert(format == GL_RED || format == GL_RGBA || format == GL_RGBA_INTEGER || format == GL_BGRA);
    Texture &t = ctx->textures[fb->color_attachment];
    if (!t.buf) return;
    prepare_texture(t);
    debugf("read pixels %d, %d, %d, %d from fb %d with format %x\n", x, y, width, height, ctx->current_framebuffer[GL_READ_FRAMEBUFFER], t.internal_format);
    assert(x + width <= t.width);
    assert(y + height <= t.height);
    if (internal_format_for_data(format, type) != t.internal_format) {
        debugf("mismatched format for read pixels: %x vs %x\n", t.internal_format, internal_format_for_data(format, type));
        assert(false);
    }
    int bpp = bytes_for_internal_format(t.internal_format);
    char *dest = (char *)data;
    int src_stride = aligned_stride(bpp * t.width);
    char *src = t.buf + (t.height * fb->layer + y) * src_stride + x * bpp;
    for (; height > 0; height--) {
        if (t.internal_format == GL_RGBA8 && format != GL_BGRA) {
            copy_bgra8_to_rgba8((uint32_t*)dest, (uint32_t*)src, width);
        } else {
            memcpy(dest, src, width * bpp);
        }
        dest += width * bpp;
        src += src_stride;
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
        Renderbuffer &rb = ctx->renderbuffers[srcName];
        srcName = rb.texture;
    }
    if (dstTarget == GL_RENDERBUFFER) {
        Renderbuffer &rb = ctx->renderbuffers[dstName];
        dstName = rb.texture;
    }
    Texture &srctex = ctx->textures[srcName];
    if (!srctex.buf) return;
    prepare_texture(srctex);
    Texture &dsttex = ctx->textures[dstName];
    IntRect skip = { dstX, dstY, srcWidth, std::abs(srcHeight) };
    prepare_texture(dsttex, &skip);
    assert(srctex.internal_format == dsttex.internal_format);
    assert(srcX + srcWidth <= srctex.width);
    assert(srcY + srcHeight <= srctex.height);
    assert(srcZ + srcDepth <= std::max(srctex.depth, 1));
    assert(dstX + srcWidth <= dsttex.width);
    assert(std::max(dstY, dstY + srcHeight) <= dsttex.height);
    assert(dstZ + srcDepth <= std::max(dsttex.depth, 1));
    int bpp = bytes_for_internal_format(srctex.internal_format);
    int src_stride = aligned_stride(bpp * srctex.width);
    int dest_stride = aligned_stride(bpp * dsttex.width);
    for (int z = 0; z < srcDepth; z++) {
        char *dest = dsttex.buf + (dsttex.height * (dstZ + z) + dstY) * dest_stride + dstX * bpp;
        char *src = srctex.buf + (srctex.height * (srcZ + z) + srcY) * src_stride + srcX * bpp;
        if (srcHeight < 0) {
            for (int y = srcHeight; y < 0; y++) {
                dest -= dest_stride;
                memcpy(dest, src, srcWidth * bpp);
                src += src_stride;
            }
        } else {
            for (int y = 0; y < srcHeight; y++) {
                memcpy(dest, src, srcWidth * bpp);
                dest += dest_stride;
                src += src_stride;
            }
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
        active_texture(target), GL_TEXTURE_3D, 0, xoffset, yoffset, zoffset,
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
        active_texture(target), GL_TEXTURE_2D_ARRAY, 0, xoffset, yoffset, 0,
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
    assert(srcWidth == dstWidth && srcHeight == abs(dstHeight));
    CopyImageSubData(
        srcfb->color_attachment, GL_TEXTURE_2D_ARRAY, 0, srcX0, srcY0, srcfb->layer,
        dstfb->color_attachment, GL_TEXTURE_2D_ARRAY, 0, dstX0, dstY0, dstfb->layer,
        dstWidth, dstHeight, 1);
}

#define GL_POINTS                         0x0000
#define GL_LINES                          0x0001
#define GL_LINE_LOOP                      0x0002
#define GL_LINE_STRIP                     0x0003
#define GL_TRIANGLES                      0x0004
#define GL_TRIANGLE_STRIP                 0x0005
#define GL_TRIANGLE_FAN                   0x0006
#define GL_QUADS                          0x0007

} // extern "C"

using PackedRGBA8 = V16<uint8_t>;
using WideRGBA8 = V16<uint16_t>;
using HalfRGBA8 = V8<uint16_t>;

static inline WideRGBA8 unpack(PackedRGBA8 p) {
    return __builtin_convertvector(p, WideRGBA8);
}

static inline HalfRGBA8 lowHalf(WideRGBA8 p) {
    return __builtin_shufflevector(p, p, 0, 1, 2, 3, 4, 5, 6, 7);
}

static inline HalfRGBA8 highHalf(WideRGBA8 p) {
    return __builtin_shufflevector(p, p, 8, 9, 10, 11, 12, 13, 14, 15);
}

static inline WideRGBA8 combine(HalfRGBA8 a, HalfRGBA8 b) {
    return __builtin_shufflevector(a, b, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
}

static inline PackedRGBA8 pack(WideRGBA8 p) {
#if USE_SSE2
    return _mm_packus_epi16(lowHalf(p), highHalf(p));
#elif USE_NEON
    return vcombine_u8(vqmovn_u16(lowHalf(p)), vqmovn_u16(highHalf(p)));
#else
    return __builtin_convertvector(p, PackedRGBA8);
#endif
}

static inline HalfRGBA8 packRGBA8(I32 a, I32 b) {
#if USE_SSE2
    return _mm_packs_epi32(a, b);
#elif USE_NEON
    return vcombine_u16(vqmovun_s32(a), vqmovun_s32(b));
#else
    return __builtin_convertvector(__builtin_shufflevector(a, b, 0, 1, 2, 3, 4, 5, 6, 7), HalfRGBA8);
#endif
}

static inline HalfRGBA8 zipLow(HalfRGBA8 a, HalfRGBA8 b) {
#if USE_SSE2
    return _mm_unpacklo_epi16(a, b);
#else
    return __builtin_shufflevector(a, b, 0, 8, 1, 9, 2, 10, 3, 11);
#endif
}

static inline HalfRGBA8 zipHigh(HalfRGBA8 a, HalfRGBA8 b) {
#if USE_SSE2
    return _mm_unpackhi_epi16(a, b);
#else
    return __builtin_shufflevector(a, b, 4, 12, 5, 13, 6, 14, 7, 15);
#endif
}

static inline HalfRGBA8 zipLow32(HalfRGBA8 a, HalfRGBA8 b) {
#if USE_SSE2
    return _mm_unpacklo_epi32(a, b);
#else
    return __builtin_shufflevector(a, b, 0, 1, 8, 9, 2, 3, 10, 11);
#endif
}

static inline HalfRGBA8 zipHigh32(HalfRGBA8 a, HalfRGBA8 b) {
#if USE_SSE2
    return _mm_unpackhi_epi32(a, b);
#else
    return __builtin_shufflevector(a, b, 4, 5, 12, 13, 6, 7, 14, 15);
#endif
}

using PackedR8 = V4<uint8_t>;
using WideR8 = V4<uint16_t>;

static inline WideR8 unpack(PackedR8 p) {
    return __builtin_convertvector(p, WideR8);
}

static inline WideR8 packR8(I32 a) {
#if USE_SSE2
    return bit_cast<WideR8>(_mm_packs_epi32(a, a));
#elif USE_NEON
    return vqmovun_s32(a);
#else
    return __builtin_convertvector(a, WideR8);
#endif
}

static inline PackedR8 pack(WideR8 p) {
#if USE_SSE2
    __m128i m = __builtin_shufflevector(p, p, 0, 1, 2, 3, -1, -1, -1, -1);
    return bit_cast<PackedR8>(_mm_packus_epi16(m, m));
#elif USE_NEON
    return vqmovn_u16(p);
#else
    return __builtin_convertvector(p, PackedR8);
#endif
}

using ZMask4 = V4<int16_t>;
using ZMask8 = V8<int16_t>;

static inline ZMask4 lowHalf(ZMask8 mask) {
    return __builtin_shufflevector(mask, mask, 0, 1, 2, 3);
}

static inline ZMask4 highHalf(ZMask8 mask) {
    return __builtin_shufflevector(mask, mask, 4, 5, 6, 7);
}

static inline PackedRGBA8 unpack(ZMask4 mask, uint32_t*) {
    return bit_cast<PackedRGBA8>(mask.xxyyzzww);
}

static inline WideR8 unpack(ZMask4 mask, uint8_t*) {
    return bit_cast<WideR8>(mask);
}

#if USE_SSE2
#define ZMASK_NONE_PASSED 0xFFFF
#define ZMASK_ALL_PASSED  0
static inline uint32_t zmask_code(ZMask8 mask) {
    return _mm_movemask_epi8(mask);
}
static inline uint32_t zmask_code(ZMask4 mask) {
    return zmask_code(mask.xyzwxyzw);
}
#else
using ZMask4Code = V4<uint8_t>;
using ZMask8Code = V8<uint8_t>;
#define ZMASK_NONE_PASSED 0xFFFFFFFFU
#define ZMASK_ALL_PASSED  0
static inline uint32_t zmask_code(ZMask4 mask) {
    return bit_cast<uint32_t>(__builtin_convertvector(mask, ZMask4Code));
}
static inline uint32_t zmask_code(ZMask8 mask) {
    return zmask_code(ZMask4((U16(lowHalf(mask)) >> 12) | (U16(highHalf(mask)) << 4)));
}
#endif

template<int FUNC, bool MASK>
static ALWAYS_INLINE int check_depth(uint16_t z, uint16_t* zbuf, ZMask8& outmask) {
    ZMask8 dest = unaligned_load<ZMask8>(zbuf);
    ZMask8 src = int16_t(z);
    // Invert the depth test to check which pixels failed and should be discarded.
    ZMask8 mask = FUNC == GL_LEQUAL ?
        // GL_LEQUAL: Not(LessEqual) = Greater
        ZMask8(src > dest) :
        // GL_LESS: Not(Less) = GreaterEqual
        ZMask8(src >= dest);
    switch (zmask_code(mask)) {
    case ZMASK_NONE_PASSED:
        return 0;
    case ZMASK_ALL_PASSED:
        if (MASK) {
            unaligned_store(zbuf, src);
        }
        return -1;
    default:
        if (MASK) {
            unaligned_store(zbuf, (mask & dest) | (~mask & src));
        }
        outmask = mask;
        return 1;
    }
}

template<bool FULL_SPANS>
static ALWAYS_INLINE bool check_depth(uint16_t z, uint16_t* zbuf, ZMask4& outmask, int span = 0) {
    ZMask4 dest = unaligned_load<ZMask4>(zbuf);
    ZMask4 src = int16_t(z);
    // Invert the depth test to check which pixels failed and should be discarded.
    ZMask4 mask = ctx->depthfunc == GL_LEQUAL ?
        // GL_LEQUAL: Not(LessEqual) = Greater
        ZMask4(src > dest) :
        // GL_LESS: Not(Less) = GreaterEqual
        ZMask4(src >= dest);
    if (!FULL_SPANS) {
        mask |= ZMask4(span) < ZMask4{1, 2, 3, 4};
    }
    if (zmask_code(mask) == ZMASK_NONE_PASSED) {
        return false;
    }
    if (ctx->depthmask) {
        unaligned_store(zbuf, (mask & dest) | (~mask & src));
    }
    outmask = mask;
    return true;
}

static inline WideRGBA8 pack_pixels_RGBA8(const vec4& v) {
    ivec4 i = roundto(v, 255.49f);
    HalfRGBA8 xz = packRGBA8(i.z, i.x);
    HalfRGBA8 yw = packRGBA8(i.y, i.w);
    HalfRGBA8 xy = zipLow(xz, yw);
    HalfRGBA8 zw = zipHigh(xz, yw);
    HalfRGBA8 lo = zipLow32(xy, zw);
    HalfRGBA8 hi = zipHigh32(xy, zw);
    return combine(lo, hi);
}

static inline WideRGBA8 pack_pixels_RGBA8(const vec4_scalar& v) {
    I32 i = roundto(bit_cast<Float>(v).zyxw, 255.49f);
    HalfRGBA8 c = packRGBA8(i, i);
    return combine(c, c);
}

static inline WideRGBA8 pack_pixels_RGBA8() {
    return pack_pixels_RGBA8(fragment_shader->gl_FragColor);
}

template<typename V>
static inline PackedRGBA8 pack_span(uint32_t*, const V& v) {
    return pack(pack_pixels_RGBA8(v));
}

static inline PackedRGBA8 pack_span(uint32_t*) {
    return pack(pack_pixels_RGBA8());
}

static inline WideRGBA8 blend_pixels_RGBA8(PackedRGBA8 pdst, WideRGBA8 src) {
    WideRGBA8 dst = unpack(pdst);
    // (x*y + x) >> 8, cheap approximation of (x*y) / 255
    #define MULDIV255(x, y) ({ WideRGBA8 b = x; (b*y + b) >> 8; })
    #define ALPHAS(c) __builtin_shufflevector(c, c, 3, 3, 3, 3, 7, 7, 7, 7, 11, 11, 11, 11, 15, 15, 15, 15)
    const WideRGBA8 RGB_MASK = {0xFFFF, 0xFFFF, 0xFFFF, 0, 0xFFFF, 0xFFFF, 0xFFFF, 0, 0xFFFF, 0xFFFF, 0xFFFF, 0, 0xFFFF, 0xFFFF, 0xFFFF, 0};
    const WideRGBA8 ALPHA_MASK = {0, 0, 0, 0xFFFF, 0, 0, 0, 0xFFFF, 0, 0, 0, 0xFFFF, 0, 0, 0, 0xFFFF};
    switch (blend_key) {
    case BLEND_KEY_NONE:
        return src;
    case BLEND_KEY(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE):
        return dst + MULDIV255((src - dst) | ALPHA_MASK, ALPHAS(src));
    case BLEND_KEY(GL_ONE, GL_ONE_MINUS_SRC_ALPHA):
        return src + dst - MULDIV255(dst, ALPHAS(src));
    case BLEND_KEY(GL_ZERO, GL_ONE_MINUS_SRC_COLOR):
        return dst - MULDIV255(dst, src);
    case BLEND_KEY(GL_ZERO, GL_ONE_MINUS_SRC_COLOR, GL_ZERO, GL_ONE):
        return dst - (MULDIV255(dst, src) & RGB_MASK);
    case BLEND_KEY(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA):
        return dst - MULDIV255(dst, ALPHAS(src));
    case BLEND_KEY(GL_ZERO, GL_SRC_COLOR):
        return MULDIV255(src, dst);
    case BLEND_KEY(GL_ONE, GL_ONE):
        return src + dst;
    case BLEND_KEY(GL_ONE, GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA):
        return src + dst - (MULDIV255(dst, src) & ALPHA_MASK);
    case BLEND_KEY(GL_ONE, GL_ZERO):
        return src;
    case BLEND_KEY(GL_ONE_MINUS_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE):
        return dst + ((src - MULDIV255(src, ALPHAS(src))) & RGB_MASK);
    case BLEND_KEY(GL_CONSTANT_COLOR, GL_ONE_MINUS_SRC_COLOR):
        return dst + MULDIV255(combine(ctx->blendcolor, ctx->blendcolor) - dst, src);
    case BLEND_KEY(GL_ONE, GL_ONE_MINUS_SRC1_COLOR): {
        WideRGBA8 secondary = pack_pixels_RGBA8(fragment_shader->gl_SecondaryFragColor);
        return src + dst - MULDIV255(dst, secondary);
    }
    default:
        UNREACHABLE;
        return src;
    }
    #undef MULDIV255
    #undef ALPHAS
}

template<bool DISCARD>
static inline void commit_output(uint32_t* buf, PackedRGBA8 mask) {
    fragment_shader->run();
    PackedRGBA8 dst = unaligned_load<PackedRGBA8>(buf);
    WideRGBA8 r = pack_pixels_RGBA8();
    if (blend_key) r = blend_pixels_RGBA8(dst, r);
    if (DISCARD) mask |= bit_cast<PackedRGBA8>(fragment_shader->isPixelDiscarded);
    unaligned_store(buf, (mask & dst) | (~mask & pack(r)));
}

template<bool DISCARD>
static inline void commit_output(uint32_t* buf) {
    commit_output<DISCARD>(buf, 0);
}

template<>
static inline void commit_output<false>(uint32_t* buf) {
    fragment_shader->run();
    WideRGBA8 r = pack_pixels_RGBA8();
    if (blend_key) r = blend_pixels_RGBA8(unaligned_load<PackedRGBA8>(buf), r);
    unaligned_store(buf, pack(r));
}

static inline void commit_span(uint32_t* buf, PackedRGBA8 r) {
    if (blend_key) r = pack(blend_pixels_RGBA8(unaligned_load<PackedRGBA8>(buf), unpack(r)));
    unaligned_store(buf, r);
}

template<bool DISCARD>
static inline void commit_output(uint32_t* buf, uint16_t z, uint16_t* zbuf) {
    ZMask4 zmask;
    if (check_depth<true>(z, zbuf, zmask)) {
        commit_output<DISCARD>(buf, unpack(zmask, buf));
    } else {
        fragment_shader->skip();
    }
}

template<bool DISCARD>
static inline void commit_output(uint32_t* buf, uint16_t z, uint16_t* zbuf, int span) {
    ZMask4 zmask;
    if (check_depth<false>(z, zbuf, zmask, span)) {
        commit_output<DISCARD>(buf, unpack(zmask, buf));
    }
}

static inline PackedRGBA8 span_mask_RGBA8(int span) {
    return bit_cast<PackedRGBA8>(I32(span) < I32{1, 2, 3, 4});
}

template<bool DISCARD>
static inline void commit_output(uint32_t* buf, int span) {
    commit_output<DISCARD>(buf, span_mask_RGBA8(span));
}

static inline WideR8 pack_pixels_R8(Float c) {
    return packR8(roundto(c, 255.49f));
}

static inline WideR8 pack_pixels_R8() {
    return pack_pixels_R8(fragment_shader->gl_FragColor.x);
}

template<typename C>
static inline PackedR8 pack_span(uint8_t*, C c) {
    return pack(pack_pixels_R8(c));
}

static inline PackedR8 pack_span(uint8_t*) {
    return pack(pack_pixels_R8());
}

static inline WideR8 blend_pixels_R8(WideR8 dst, WideR8 src) {
    #define MULDIV255(x, y) ({ WideR8 b = x; (b*y + b) >> 8; })
    switch (blend_key) {
    case BLEND_KEY_NONE:
        return src;
    case BLEND_KEY(GL_ZERO, GL_SRC_COLOR):
        return MULDIV255(src, dst);
    case BLEND_KEY(GL_ONE, GL_ONE):
        return src + dst;
    case BLEND_KEY(GL_ONE, GL_ZERO):
        return src;
    default:
        UNREACHABLE;
        return src;
    }
    #undef MULDIV255
}

template<bool DISCARD>
static inline void commit_output(uint8_t* buf, WideR8 mask) {
    fragment_shader->run();
    WideR8 dst = unpack(unaligned_load<PackedR8>(buf));
    WideR8 r = pack_pixels_R8();
    if (blend_key) r = blend_pixels_R8(dst, r);
    if (DISCARD) mask |= packR8(fragment_shader->isPixelDiscarded);
    unaligned_store(buf, pack((mask & dst) | (~mask & r)));
}

template<bool DISCARD>
static inline void commit_output(uint8_t* buf) {
    commit_output<DISCARD>(buf, 0);
}

template<>
static inline void commit_output<false>(uint8_t* buf) {
    fragment_shader->run();
    WideR8 r = pack_pixels_R8();
    if (blend_key) r = blend_pixels_R8(unpack(unaligned_load<PackedR8>(buf)), r);
    unaligned_store(buf, pack(r));
}

static inline void commit_span(uint8_t* buf, PackedR8 r) {
    if (blend_key) r = pack(blend_pixels_R8(unpack(unaligned_load<PackedR8>(buf)), unpack(r)));
    unaligned_store(buf, r);
}

template<bool DISCARD>
static inline void commit_output(uint8_t* buf, uint16_t z, uint16_t* zbuf) {
    ZMask4 zmask;
    if (check_depth<true>(z, zbuf, zmask)) {
        commit_output<DISCARD>(buf, unpack(zmask, buf));
    } else {
        fragment_shader->skip();
    }
}

template<bool DISCARD>
static inline void commit_output(uint8_t* buf, uint16_t z, uint16_t* zbuf, int span) {
    ZMask4 zmask;
    if (check_depth<false>(z, zbuf, zmask, span)) {
        commit_output<DISCARD>(buf, unpack(zmask, buf));
    }
}

static inline WideR8 span_mask_R8(int span) {
    return bit_cast<WideR8>(WideR8(span) < WideR8{1, 2, 3, 4});
}

template<bool DISCARD>
static inline void commit_output(uint8_t* buf, int span) {
    commit_output<DISCARD>(buf, span_mask_R8(span));
}

static const size_t MAX_FLATS = 64;
typedef float Flats[MAX_FLATS];

static const size_t MAX_INTERPOLANTS = 16;
typedef float Interpolants __attribute__((ext_vector_type(MAX_INTERPOLANTS)));

template<typename S, typename P>
static ALWAYS_INLINE void dispatch_draw_span(S* shader, P* buf, int len) {
    int drawn = shader->draw_span(buf, len);
    if (drawn) shader->step_interp_inputs(drawn >> 2);
    for (buf += drawn; drawn < len; drawn += 4, buf += 4) {
        S::run(shader);
        commit_span(buf, pack_span(buf));
    }
}

#include "load_shader.h"

template<int FUNC, bool MASK, typename P>
static inline void draw_depth_span(uint16_t z, P* buf, uint16_t* depth, int span) {
    int skip = 0;
    if (fragment_shader->has_draw_span(buf)) {
        int len = 0;
        do {
            ZMask8 zmask;
            switch (check_depth<FUNC, MASK>(z, depth, zmask)) {
            case 0:
                if (len) {
                    fragment_shader->draw_span(buf - len, len);
                    len = 0;
                }
                skip += 2;
                break;
            case -1:
                if (skip) {
                    fragment_shader->skip(skip);
                    skip = 0;
                }
                len += 8;
                break;
            default:
                if (len) {
                    fragment_shader->draw_span(buf - len, len);
                    len = 0;
                } else if (skip) {
                    fragment_shader->skip(skip);
                    skip = 0;
                }
                commit_output<false>(buf, unpack(lowHalf(zmask), buf));
                commit_output<false>(buf + 4, unpack(highHalf(zmask), buf));
                break;
            }
            buf += 8;
            depth += 8;
            span -= 8;
        } while (span >= 8);
        if (len) {
            fragment_shader->draw_span(buf - len, len);
        }
    } else {
        do {
            ZMask8 zmask;
            switch (check_depth<FUNC, MASK>(z, depth, zmask)) {
            case 0:
                skip += 2;
                break;
            case -1:
                if (skip) {
                    fragment_shader->skip(skip);
                    skip = 0;
                }
                commit_output<false>(buf);
                commit_output<false>(buf + 4);
                break;
            default:
                if (skip) {
                    fragment_shader->skip(skip);
                    skip = 0;
                }
                commit_output<false>(buf, unpack(lowHalf(zmask), buf));
                commit_output<false>(buf + 4, unpack(highHalf(zmask), buf));
                break;
            }
            buf += 8;
            depth += 8;
            span -= 8;
        } while (span >= 8);
    }
    if (skip) {
        fragment_shader->skip(skip);
    }
}

typedef vec2_scalar Point;

template<typename P>
static inline void draw_quad_spans(int nump, Point p[4], uint16_t z, Interpolants interp_outs[4], Texture& colortex, int layer, Texture& depthtex, float fx0, float fy0, float fx1, float fy1) {
        Point l0, r0, l1, r1;
        int l0i, r0i, l1i, r1i;
        {
            int top = nump > 3 && p[3].y < p[2].y ?
                (p[0].y < p[1].y ? (p[0].y < p[3].y ? 0 : 3) : (p[1].y < p[3].y ? 1 : 3)) :
                (p[0].y < p[1].y ? (p[0].y < p[2].y ? 0 : 2) : (p[1].y < p[2].y ? 1 : 2));
            #define NEXT_POINT(idx) ({ int cur = (idx) + 1; cur < nump ? cur : 0; })
            #define PREV_POINT(idx) ({ int cur = (idx) - 1; cur >= 0 ? cur : nump-1; })
            int next = NEXT_POINT(top);
            int prev = PREV_POINT(top);
            if (p[top].y == p[next].y) {
                l0i = next;
                l1i = NEXT_POINT(next);
                r0i = top;
                r1i = prev;
            } else if (p[top].y == p[prev].y) {
                l0i = top;
                l1i = next;
                r0i = prev;
                r1i = PREV_POINT(prev);
            } else {
                l0i = r0i = top;
                l1i = next;
                r1i = prev;
            }
            l0 = p[l0i];
            r0 = p[r0i];
            l1 = p[l1i];
            r1 = p[r1i];
        //    debugf("l0: %d(%f,%f), r0: %d(%f,%f) -> l1: %d(%f,%f), r1: %d(%f,%f)\n", l0i, l0.x, l0.y, r0i, r0.x, r0.y, l1i, l1.x, l1.y, r1i, r1.x, r1.y);
        }

        float lx = l0.x;
        float lk = 1.0f / (l1.y - l0.y);
        float lm = (l1.x - l0.x) * lk;
        float rx = r0.x;
        float rk = 1.0f / (r1.y - r0.y);
        float rm = (r1.x - r0.x) * rk;
        assert(l0.y == r0.y);
        float y = floor(std::max(l0.y, fy0) + 0.5f) + 0.5f;
        lx += (y - l0.y) * lm;
        rx += (y - r0.y) * rm;
        Interpolants lo = interp_outs[l0i];
        Interpolants lom = (interp_outs[l1i] - lo) * lk;
        lo = lo + lom * (y - l0.y);
        Interpolants ro = interp_outs[r0i];
        Interpolants rom = (interp_outs[r1i] - ro) * rk;
        ro = ro + rom * (y - r0.y);
        P *fbuf = (P*)colortex.buf + (layer * colortex.height + int(y)) * aligned_stride(sizeof(P) * colortex.width) / sizeof(P);
        uint16_t *fdepth = (uint16_t*)depthtex.buf + int(y) * aligned_stride(sizeof(uint16_t) * depthtex.width) / sizeof(uint16_t);
        while (y < fy1) {
            if (y > l1.y) {
                l0i = l1i;
                l0 = l1;
                l1i = NEXT_POINT(l1i);
                l1 = p[l1i];
                if (l1.y <= l0.y) break;
                lk = 1.0f / (l1.y - l0.y);
                lm = (l1.x - l0.x) * lk;
                lx = l0.x + (y - l0.y) * lm;
                lo = interp_outs[l0i];
                lom = (interp_outs[l1i] - lo) * lk;
                lo += lom * (y - l0.y);
            }
            if (y > r1.y) {
                r0i = r1i;
                r0 = r1;
                r1i = PREV_POINT(r1i);
                r1 = p[r1i];
                if (r1.y <= r0.y) break;
                rk = 1.0f / (r1.y - r0.y);
                rm = (r1.x - r0.x) * rk;
                rx = r0.x + (y - r0.y) * rm;
                ro = interp_outs[r0i];
                rom = (interp_outs[r1i] - ro) * rk;
                ro += rom * (y - r0.y);
            }
            int startx = int(std::max(std::min(lx, rx), fx0) + 0.5f);
            int endx = int(std::min(std::max(lx, rx), fx1) + 0.5f);
            int span = endx - startx;
            if (span > 0) {
                ctx->shaded_rows++;
                ctx->shaded_pixels += span;
                P* buf = fbuf + startx;
                uint16_t* depth = fdepth + startx;
                bool use_depth = depthtex.buf != nullptr;
                if (depthtex.delay_clear) {
                    int yi = int(y);
                    uint32_t& mask = depthtex.cleared_rows[yi / 32];
                    if ((mask & (1 << (yi & 31))) == 0) {
                        mask |= 1 << (yi & 31);
                        depthtex.delay_clear--;
                        switch (ctx->depthfunc) {
                        case GL_LESS: if (int16_t(z) < int16_t(depthtex.clear_val)) { use_depth = false; } else { span = 0; } break;
                        case GL_LEQUAL: if (int16_t(z) <= int16_t(depthtex.clear_val)) { use_depth = false; } else { span = 0; } break;
                        }
                        if (ctx->depthmask && !use_depth) {
                            if (startx > 0 || endx < depthtex.width) {
                                clear_buffer<uint16_t>(depthtex, depthtex.clear_val, 0, depthtex.width, yi, yi+1, 0, startx, endx);
                            }
                            clear_buffer<uint16_t>(depthtex, z, startx, endx, yi, yi + 1);
                        } else {
                            clear_buffer<uint16_t>(depthtex, depthtex.clear_val, 0, depthtex.width, yi, yi+1);
                        }
                    }
                }
                if (colortex.delay_clear) {
                    int yi = int(y);
                    uint32_t& mask = colortex.cleared_rows[yi / 32];
                    if ((mask & (1 << (yi & 31))) == 0) {
                        mask |= 1 << (yi & 31);
                        colortex.delay_clear--;
                        if (use_depth || blend_key || fragment_shader->use_discard()) {
                            clear_buffer<P>(colortex, colortex.clear_val, 0, colortex.width, yi, yi + 1);
                        } else if (startx > 0 || endx < colortex.width) {
                            clear_buffer<P>(colortex, colortex.clear_val, 0, colortex.width, yi, yi+1, 0, startx, endx);
                        }
                    }
                }
                fragment_shader->gl_FragCoordXY.x = init_interp(startx + 0.5f, 1);
                fragment_shader->gl_FragCoordXY.y = y;
                {
                    Interpolants step = (ro - lo) * (1.0f / (rx - lx));
                    Interpolants o = lo + step * (startx + 0.5f - lx);
                    fragment_shader->init_span(&o, &step, 4.0f);
                }
                if (!fragment_shader->use_discard()) {
                    if (use_depth) {
                        if (span >= 8) {
                            if (ctx->depthfunc == GL_LEQUAL) {
                                if (ctx->depthmask) draw_depth_span<GL_LEQUAL, true>(z, buf, depth, span);
                                else draw_depth_span<GL_LEQUAL, false>(z, buf, depth, span);
                            } else {
                                if (ctx->depthmask) draw_depth_span<GL_LESS, true>(z, buf, depth, span);
                                else draw_depth_span<GL_LESS, false>(z, buf, depth, span);
                            }
                            buf += span & ~7;
                            depth += span & ~7;
                            span &= 7;
                        }
                        for (; span >= 4; span -= 4, buf += 4, depth += 4) {
                            commit_output<false>(buf, z, depth);
                        }
                        if (span > 0) {
                            commit_output<false>(buf, z, depth, span);
                        }
                    } else {
                        if (span >= 4) {
                            if (fragment_shader->has_draw_span(buf)) {
                                int len = span & ~3;
                                fragment_shader->draw_span(buf, len);
                                buf += len;
                                span &= 3;
                            } else {
                                do {
                                    commit_output<false>(buf);
                                    buf += 4;
                                    span -= 4;
                                } while (span >= 4);
                            }
                        }
                        if (span > 0) {
                            commit_output<false>(buf, span);
                        }
                    }
                } else {
                    if (use_depth) {
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
            fbuf += aligned_stride(sizeof(P) * colortex.width) / sizeof(P);
            fdepth += aligned_stride(sizeof(uint16_t) * depthtex.width) / sizeof(uint16_t);
        }
}

static void draw_quad(int nump, Texture& colortex, int layer, Texture& depthtex) {
        Flats flat_outs;
        Interpolants interp_outs[4] = { 0 };
        vertex_shader->run((char *)flat_outs, (char *)interp_outs, sizeof(Interpolants));
        Float w = 1.0f / vertex_shader->gl_Position.w;
        vec3 clip = vertex_shader->gl_Position.sel(X, Y, Z) * w;
        vec3 screen = (clip + 1)*vec3(ctx->viewport.width/2, ctx->viewport.height/2, 0.5f) + vec3(ctx->viewport.x, ctx->viewport.y, 0);
        Point p[4] = { { screen.x.x, screen.y.x }, { screen.x.y, screen.y.y }, { screen.x.z, screen.y.z }, { screen.x.w, screen.y.w } };

        auto top_left = min(min(p[0], p[1]), p[2]);
        auto bot_right = max(max(p[0], p[1]), p[2]);
        if (nump > 3) {
            top_left = min(top_left, p[3]);
            bot_right = max(bot_right, p[3]);
        }
        //debugf("bbox: %f %f %f %f\n", top_left.x, top_left.y, bot_right.x, bot_right.y);

        float fx0 = 0;
        float fy0 = 0;
        float fx1 = colortex.width;
        float fy1 = colortex.height;
        if (ctx->scissortest) {
            fx0 = std::max(fx0, float(ctx->scissor.x));
            fy0 = std::max(fy0, float(ctx->scissor.y));
            fx1 = std::min(fx1, float(ctx->scissor.x + ctx->scissor.width));
            fy1 = std::min(fy1, float(ctx->scissor.y + ctx->scissor.height));
        }

        if (top_left.x >= fx1 || top_left.y >= fy1 || bot_right.x <= fx0 || bot_right.y <= fy0) {
            return;
        }

        // SSE2 does not support unsigned comparison, so bias Z to be negative.
        uint16_t z = uint16_t(0xFFFF * screen.z.x) - 0x8000;
        fragment_shader->gl_FragCoordZW.x = screen.z.x;
        fragment_shader->gl_FragCoordZW.y = w.x;

        fragment_shader->init_primitive(flat_outs);

        if (colortex.internal_format == GL_RGBA8) {
            draw_quad_spans<uint32_t>(nump, p, z, interp_outs, colortex, layer, depthtex, fx0, fy0, fx1, fy1);
        } else if (colortex.internal_format == GL_R8) {
            draw_quad_spans<uint8_t>(nump, p, z, interp_outs, colortex, layer, depthtex, fx0, fy0, fx1, fy1);
        } else {
            assert(false);
        }
}

void VertexArray::validate() {
    int last_enabled = -1;
    for (int i = 0; i <= max_attrib; i++) {
        if (attribs[i].enabled) {
            VertexAttrib &attr = attribs[i];
            VertexArray &v = ctx->vertex_arrays[attr.vertex_array];
            Buffer &vertex_buf = ctx->buffers[attr.vertex_buffer];
            attr.buf = vertex_buf.buf;
            attr.buf_size = vertex_buf.size;
            //debugf("%d %x %d %d %d %d\n", i, attr.type, attr.size, attr.stride, attr.offset, attr.divisor);
            last_enabled = i;
        }
    }
    max_attrib = last_enabled;
}

extern "C" {

void DrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, void *indicesptr, GLsizei instancecount) {
        assert(mode == GL_TRIANGLES || mode == GL_QUADS);
        assert(type == GL_UNSIGNED_SHORT || type == GL_UNSIGNED_INT);
        assert(count == 6);
        assert(indicesptr == nullptr);
        if (count <= 0 || instancecount <= 0 || indicesptr) {
            return;
        }

        Framebuffer& fb = *get_framebuffer(GL_DRAW_FRAMEBUFFER);
        Texture& colortex = ctx->textures[fb.color_attachment];
        assert(colortex.internal_format == GL_RGBA8 || colortex.internal_format == GL_R8);
        static Texture nodepthtex;
        Texture& depthtex = ctx->depthtest && fb.depth_attachment ? ctx->textures[fb.depth_attachment] : nodepthtex;
        if (&depthtex != &nodepthtex) {
            assert(depthtex.internal_format == GL_DEPTH_COMPONENT16);
            assert(colortex.width == depthtex.width && colortex.height == depthtex.height);
        }

        Buffer &indices_buf = ctx->buffers[ctx->current_buffer[GL_ELEMENT_ARRAY_BUFFER]];

        //debugf("current_vertex_array %d\n", ctx->current_vertex_array);
        //debugf("indices size: %d\n", indices_buf.size);
        VertexArray &v = ctx->vertex_arrays[ctx->current_vertex_array];
        if (ctx->validate_vertex_array) {
            ctx->validate_vertex_array = false;
            v.validate();
        }

        uint64_t start = get_time_value();

        ctx->shaded_rows = 0;
        ctx->shaded_pixels = 0;

        unsigned short *indices = (unsigned short*)indices_buf.buf;
        if (type == GL_UNSIGNED_INT) {
            assert(indices_buf.size == count * 4);
            indices = (unsigned short*)calloc(count, sizeof(unsigned short));
            for (int i = 0; i < count; i++) {
                unsigned int val = ((unsigned int *)indices_buf.buf)[i];
                assert(val <= 0xFFFFU);
                indices[i] = val;
            }
        } else if (type == GL_UNSIGNED_SHORT) {
            assert(indices_buf.size == count * 2);
        } else {
            assert(0);
        }

        vertex_shader->init_batch(program_impl);
        fragment_shader->init_batch(program_impl);
        for (int instance = 0; instance < instancecount; instance++) {
            if (mode == GL_QUADS) for (int i = 0; i + 4 <= count; i += 4) {
                vertex_shader->load_attribs(program_impl, v.attribs, indices, i, instance, 4);
                //debugf("native quad %d %d %d %d\n", indices[i], indices[i+1], indices[i+2], indices[i+3]);
                draw_quad(4, colortex, fb.layer, depthtex);
            } else for (int i = 0; i + 3 <= count; i += 3) {
                if (i + 6 <= count && indices[i+3] == indices[i+2] && indices[i+4] == indices[i+1]) {
                    unsigned short quad_indices[4] = { indices[i], indices[i+1], indices[i+5], indices[i+2] };
                    vertex_shader->load_attribs(program_impl, v.attribs, quad_indices, 0, instance, 4);
                    //debugf("emulate quad %d %d %d %d\n", indices[i], indices[i+1], indices[i+5], indices[i+2]);
                    draw_quad(4, colortex, fb.layer, depthtex);
                    i += 3;
                } else {
                    vertex_shader->load_attribs(program_impl, v.attribs, indices, i, instance, 3);
                    //debugf("triangle %d %d %d %d\n", indices[i], indices[i+1], indices[i+2]);
                    draw_quad(3, colortex, fb.layer, depthtex);
                }
            }
        }

        if (indices != (unsigned short*)indices_buf.buf) {
            free(indices);
        }

        auto qid = ctx->current_query.find(GL_SAMPLES_PASSED);
        if (qid != ctx->current_query.end() && qid->second) {
            Query &q = ctx->queries[qid->second];
            q.value += ctx->shaded_pixels;
        }

        uint64_t end = get_time_value();
        debugf("draw(%d): %fms for %d pixels in %d rows (avg %f pixels/row, %f ns/pixel)\n", instancecount, double(end - start)/(1000.*1000.), ctx->shaded_pixels, ctx->shaded_rows, double(ctx->shaded_pixels)/ctx->shaded_rows, double(end - start)/std::max(ctx->shaded_pixels, 1));
}

void Finish() {
}

void MakeCurrent(void* ctx_ptr) {
    if (!ctx_ptr) {
        return;
    }
    ctx = (Context*)ctx_ptr;
    setup_program(ctx->current_program);
    blend_key = ctx->blend ? ctx->blend_key : BLEND_KEY_NONE;
}

void* CreateContext() {
    return new Context;
}

void DestroyContext(void* ctx_ptr) {
    if (!ctx_ptr) {
        return;
    }
    if (ctx == ctx_ptr) {
        MakeCurrent(&default_ctx);
    }
    delete (Context*)ctx_ptr;
}

void Composite(
    GLuint srcId,
    GLint srcX,
    GLint srcY,
    GLsizei srcWidth,
    GLsizei srcHeight,
    GLint dstX,
    GLint dstY,
    GLboolean opaque,
    GLboolean flip
) {
    Framebuffer& fb = ctx->framebuffers[0];
    if (!fb.color_attachment) {
        return;
    }
    Texture &srctex = ctx->textures[srcId];
    if (!srctex.buf) return;
    prepare_texture(srctex);
    Texture &dsttex = ctx->textures[fb.color_attachment];
    assert(bytes_for_internal_format(srctex.internal_format) == 4);
    const int bpp = 4;
    int src_stride = aligned_stride(bpp * srctex.width);
    int dest_stride = aligned_stride(bpp * dsttex.width);
    if (srcY < 0) {
        dstY -= srcY;
        srcHeight += srcY;
        srcY = 0;
    }
    if (dstY < 0) {
        srcY -= dstY;
        srcHeight += dstY;
        dstY = 0;
    }
    if (srcY + srcHeight > srctex.height) {
        srcHeight = srctex.height - srcY;
    }
    if (dstY + srcHeight > dsttex.height) {
        srcHeight = dsttex.height - dstY;
    }
    IntRect skip = { dstX, dstY, srcWidth, srcHeight };
    prepare_texture(dsttex, &skip);
    char *dest = dsttex.buf + (flip ? dsttex.height - 1 - dstY : dstY) * dest_stride + dstX * bpp;
    char *src = srctex.buf + srcY * src_stride + srcX * bpp;
    if (flip) {
        dest_stride = -dest_stride;
    }
    if (opaque) {
        for (int y = 0; y < srcHeight; y++) {
            memcpy(dest, src, srcWidth * bpp);
            dest += dest_stride;
            src += src_stride;
        }
    } else {
        #define MULDIV255(x, y) ({ WideRGBA8 b = x; (b*y + b) >> 8; })
        #define ALPHAS(c) __builtin_shufflevector(c, c, 3, 3, 3, 3, 7, 7, 7, 7, 11, 11, 11, 11, 15, 15, 15, 15)
        for (int y = 0; y < srcHeight; y++) {
            char *end = src + srcWidth * bpp;
            while (src + 4 * bpp <= end) {
                WideRGBA8 srcpx = unpack(unaligned_load<PackedRGBA8>(src));
                WideRGBA8 dstpx = unpack(unaligned_load<PackedRGBA8>(dest));
                PackedRGBA8 r = pack(srcpx + dstpx - MULDIV255(dstpx, ALPHAS(srcpx)));
                unaligned_store(dest, r);
                src += 4 * bpp;
                dest += 4 * bpp;
            }
            if (src < end) {
                WideRGBA8 srcpx = unpack(unaligned_load<PackedRGBA8>(src));
                WideRGBA8 dstpx = unpack(unaligned_load<PackedRGBA8>(dest));
                U32 r = bit_cast<U32>(pack(srcpx + dstpx - MULDIV255(dstpx, ALPHAS(srcpx))));
                unaligned_store(dest, r.x);
                if (src + bpp < end) {
                    unaligned_store(dest + bpp, r.y);
                    if (src + 2 * bpp < end) {
                        unaligned_store(dest + 2 * bpp, r.z);
                    }
                }
                dest += end - src;
                src = end;
            }
            dest += dest_stride - srcWidth * bpp;
            src += src_stride - srcWidth * bpp;
        }
        #undef MULDIV255
        #undef ALPHAS
    }
}

} // extern "C"
