
#include "glsl.h"
using namespace glsl;
typedef uint32_t GLuint;
typedef int32_t GLboolean;
typedef float GLfloat;

typedef int32_t GLint;
typedef int32_t GLsizei;
typedef uint32_t GLenum;
typedef size_t GLsizeiptr;

template<typename S>
S *lookup_sampler(S *s, int slot) {
        return nullptr;
}

template<typename S>
S *lookup_isampler(S *s, int slot) {
        return nullptr;
}

template<typename S>
S *lookup_sampler_array(S *s, int slot) {
        return nullptr;
}

struct VertexAttrib {
        GLint size;
        GLenum type;
        bool normalized;
        GLsizei stride;
        GLuint offset;
        bool enabled = false;
        GLuint divisor;
        int vertex_array;
        char *buf; // XXX: this can easily dangle
};

struct ProgramImpl {
    virtual ~ProgramImpl() {}
    virtual const char *get_name() const = 0;
    virtual int get_uniform(const char *name) const = 0;
    virtual bool set_sampler(int index, int value) = 0;
    virtual void bind_attrib(const char *name, int index) = 0;
    virtual void init_shaders(void *vertex_shader, void *fragment_shader) = 0;
};

struct ShaderImpl {
    typedef void (*SetUniform1iFunc)(ShaderImpl*, int index, int value);
    typedef void (*SetUniform4fvFunc)(ShaderImpl*, int index, const float *value);
    typedef void (*SetUniformMatrix4fvFunc)(ShaderImpl*, int index, const float *value);

    SetUniform1iFunc set_uniform_1i_func = nullptr;
    SetUniform4fvFunc set_uniform_4fv_func = nullptr;
    SetUniformMatrix4fvFunc set_uniform_matrix4fv_func = nullptr;
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

    void step_fragcoord() {}
};

template<typename T>
void load_attrib(T& attrib, VertexAttrib &va, unsigned short *indices, int start, int instance, int count) {
}

template<typename T>
void load_flat_attrib(T& attrib, VertexAttrib &va, unsigned short *indices, int start, int instance, int count) {
}

#include "all.h"

int main() {
}
