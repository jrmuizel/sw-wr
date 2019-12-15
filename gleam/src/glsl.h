#include <string.h>
#include <stdint.h>
#include <xmmintrin.h>
#include <assert.h>
#include <math.h>
#include <array>


// Some of this is copied from Skia and is governed by a BSD-style license
// Every function in this file should be marked static and inline using SI.
#ifdef _WIN32
    #define ALWAYS_INLINE __forceinline
#else
    #define ALWAYS_INLINE __attribute__((always_inline)) inline
#endif
#define SI ALWAYS_INLINE static

#define UNREACHABLE __builtin_unreachable()

namespace glsl {

template <typename T> using V = T __attribute__((ext_vector_type(4)));
using Float   = V<float   >;
using I32 = V< int32_t>;
using U64 = V<uint64_t>;
using U32 = V<uint32_t>;
using U16 = V<uint16_t>;
using U8  = V<uint8_t >;
using Bool = V<int>;

float make_float(float n) {
    return n;
}

float make_float(int32_t n) {
    return float(n);
}

int32_t make_int(uint32_t n) {
    return n;
}

Float make_float(I32 v) {
        // Float(v) seems to just convert the bits
        return _mm_cvtepi32_ps(v);
}

int32_t make_int(int32_t n) {
    return n;
}

template<typename T> I32 make_int(T v) {
    return I32(v);
}

uint32_t make_uint(uint32_t n) {
    return n;
}

uint32_t make_uint(int32_t n) {
    return n;
}

U32 make_uint(I32 x) {
    return __builtin_convertvector(x, U32);
}

template<typename T> U32 make_uint(T v) {
    return U32(v);
}

template<typename T> T force_scalar(T n) { return n; }

float force_scalar(Float f) { return f[0]; }

int32_t force_scalar(I32 i) { return i[0]; }

struct vec4;
struct ivec2;

SI int32_t if_then_else(int32_t c, int32_t t, int32_t e) {
    return c ? t : e;
}

SI float if_then_else(int32_t c, float t, float e) {
    return c ? t : e;
}

SI Float if_then_else(I32 c, float t, float e) {
    return _mm_or_ps(_mm_and_ps(c, Float(t)), _mm_andnot_ps(c, Float(e)));
}

SI I32 if_then_else(I32 c, int32_t t, int32_t e) {
    return _mm_or_ps(_mm_and_ps(c, I32(t)), _mm_andnot_ps(c, I32(e)));
}



SI Float if_then_else(I32 c, Float t, Float e) {
    return _mm_or_ps(_mm_and_ps(c, t), _mm_andnot_ps(c, e));
}

SI Float if_then_else(int32_t c, Float t, Float e) {
    return c ? t : e;
}

SI Bool if_then_else(I32 c, Bool t, Bool e) {
    return _mm_or_ps(_mm_and_ps(c, t), _mm_andnot_ps(c, e));
}

SI Bool if_then_else(int32_t c, Bool t, Bool e) {
    return c ? t : e;
}

SI float min(float a, float b) { return a < b ? a : b; }
SI float max(float a, float b) { return a > b ? a : b; }

SI float clamp(float a, float minVal, float maxVal) {
        return min(max(a, minVal), maxVal);
}

SI Float   min(Float a, Float b)       { return _mm_min_ps(a,b);    }
SI Float   max(Float a, Float b)       { return _mm_max_ps(a,b);    }

SI Float clamp(Float a, Float minVal, Float maxVal) {
        return min(max(a, minVal), maxVal);
}

#define sqrt __glsl_sqrt

SI float sqrt(float x) {
    return sqrtf(x);
}

SI Float sqrt(Float x) {
        return _mm_sqrt_ps(x);
}

SI float inversesqrt(float x) {
    return 1.0f/sqrtf(x);
}

SI Float inversesqrt(Float x) {
        return _mm_rsqrt_ps(x);
}


SI float step(float edge, float x) {
    return float(x >= edge);
}

SI Float step(Float edge, Float x) {
        return if_then_else(x < edge, Float(0), Float(1));
}




/*
enum RGBA {
        R,
        G,
        B,
        A
};*/

enum XYZW {
        X = 0,
        Y = 1,
        Z = 2,
        W = 3,
        R = 0,
        G = 1,
        B = 2,
        A = 3,
};

struct bvec2_scalar {
    bool x;
    bool y;
};

struct bvec2 {
        bvec2() : bvec2(0) {}
        bvec2(Bool a): x(a), y(a) {}
        bvec2(Bool x, Bool y): x(x), y(y) {}
        Bool& select(XYZW c) {
                switch (c) {
                    case X: return x;
                    case Y: return y;
                    default: UNREACHABLE;
                }
        }
        Bool sel(XYZW c1) {
                return select(c1);
        }

        bvec2 operator~() {
                return bvec2(~x, ~y);
        }


        Bool x;
        Bool y;
};

bvec2_scalar make_bvec2(bool n) {
    return bvec2_scalar{ n, n };
}

bvec2_scalar make_bvec2(bool x, bool y) {
    return bvec2_scalar{ x, y };
}

template<typename N> bvec2 make_bvec2(const N& n) {
    return bvec2(n);
}

template<typename X, typename Y> bvec2 make_bvec2(const X& x, const Y& y) {
    return bvec2(x, y);
}

struct vec4_scalar;

struct vec2_scalar {
        typedef struct vec2 vector_type;

        float x;
        float y;

        vec2_scalar() : vec2_scalar(0.0f) {}
        vec2_scalar(float a): x(a), y(a) {}
        vec2_scalar(int a): x(a), y(a) {}
        constexpr vec2_scalar(float x, float y): x(x), y(y) {}

        float& select(XYZW c) {
                switch (c) {
                    case X: return x;
                    case Y: return y;
                    default: UNREACHABLE;
                }
        }
        float& sel(XYZW c1) {
                return select(c1);
        }
        vec2_scalar sel(XYZW c1, XYZW c2) {
                return vec2_scalar(select(c1), select(c2));
        }
        vec4_scalar sel(XYZW c1, XYZW c2, XYZW c3, XYZW c4);

        friend bool operator==(const vec2_scalar& l, const vec2_scalar& r)
        {
                return l.x == r.x && l.y == r.y;
        }

        friend bool operator!=(const vec2_scalar& l, const vec2_scalar& r)
        {
                return l.x != r.x || l.y != r.y;
        }

        friend vec2_scalar operator*(float a, vec2_scalar b) {
                return vec2_scalar(a*b.x, a*b.y);
        }
        friend vec2_scalar operator*(vec2_scalar a, float b) {
                return vec2_scalar(a.x*b, a.y*b);
        }
        friend vec2_scalar operator*(vec2_scalar a, vec2_scalar b) {
                return vec2_scalar(a.x*b.x, a.y*b.y);
        }
        friend vec2_scalar operator/(vec2_scalar a, vec2_scalar b) {
                return vec2_scalar(a.x/b.x, a.y/b.y);
        }

        friend vec2_scalar operator-(vec2_scalar a, vec2_scalar b) {
                return vec2_scalar(a.x-b.x, a.y-b.y);
        }
        friend vec2_scalar operator+(vec2_scalar a, vec2_scalar b) {
                return vec2_scalar(a.x+b.x, a.y+b.y);
        }

        vec2_scalar operator-() {
                return vec2_scalar(-x, -y);
        }

        vec2_scalar operator+=(vec2_scalar a) {
                x += a.x;
                y += a.y;
                return *this;
        }

        vec2_scalar operator-=(vec2_scalar a) {
                x += a.x;
                y += a.y;
                return *this;
        }
};

struct vec2_scalar_ref {
        vec2_scalar_ref(float &x, float &y) : x(x), y(y) {
        }
        float &x;
        float &y;
        vec2_scalar_ref& operator=(const vec2_scalar &a) {
                x = a.x;
                y = a.y;
                return *this;
        }
        vec2_scalar_ref& operator*=(vec2_scalar a) {
                x *= a.x;
                y *= a.y;
                return *this;
        }
        operator vec2_scalar() const {
            return vec2_scalar{x, y};
        }

};

struct vec2 {
        typedef struct vec2 vector_type;

        constexpr vec2() : vec2(0) {}
        constexpr vec2(Float a): x(a), y(a) {}
        vec2(Float x, Float y): x(x), y(y) {}
        constexpr vec2(vec2_scalar s) : x(s.x), y(s.y) {}
        constexpr vec2(vec2_scalar s0, vec2_scalar s1, vec2_scalar s2, vec2_scalar s3)
                : x(Float{s0.x, s1.x, s2.x, s3.x}),
                  y(Float{s0.y, s1.y, s2.y, s3.y})
                  {}
        vec2(ivec2 a);
        Float x;
        Float y;

        Float& select(XYZW c) {
                switch (c) {
                    case X: return x;
                    case Y: return y;
                    default: UNREACHABLE;
                }
        }
        Float& sel(XYZW c1) {
                return select(c1);
        }
        vec2 sel(XYZW c1, XYZW c2) {
                return vec2(select(c1), select(c2));
        }

        vec4 sel(XYZW c1, XYZW c2, XYZW c3, XYZW c4);

        vec2 operator*=(Float a) {
                x *= a;
                y *= a;
                return *this;
        }
        vec2 operator*=(vec2 a) {
                x *= a.x;
                y *= a.y;
                return *this;
        }

        vec2 operator/=(Float a) {
                x /= a;
                y /= a;
                return *this;
        }
        vec2 operator/=(vec2 a) {
                x /= a.x;
                y /= a.y;
                return *this;
        }



        vec2 operator+=(vec2 a) {
                x += a.x;
                y += a.y;
                return *this;
        }
        vec2 operator-=(vec2 a) {
                x -= a.x;
                y -= a.y;
                return *this;
        }

        vec2 operator-() {
                return vec2(-x, -y);
        }

        friend I32 operator==(const vec2& l, const vec2& r)
        {
                return l.x == r.x && l.y == r.y;
        }

        friend I32 operator!=(const vec2& l, const vec2& r)
        {
                return l.x != r.x || l.y != r.y;
        }

        friend vec2 operator*(vec2 a, Float b) {
                return vec2(a.x*b, a.y*b);
        }
        friend vec2 operator*(vec2 a, vec2 b) {
                return vec2(a.x*b.x, a.y*b.y);
        }

        friend vec2 operator/(vec2 a, vec2 b) {
                return vec2(a.x/b.x, a.y/b.y);
        }

        friend vec2 operator-(vec2 a, vec2 b) {
                return vec2(a.x-b.x, a.y-b.y);
        }
        friend vec2 operator+(vec2 a, vec2 b) {
                return vec2(a.x+b.x, a.y+b.y);
        }

};

vec2_scalar force_scalar(const vec2& v) {
    return vec2_scalar{ force_scalar(v.x), force_scalar(v.y) };
}

vec2_scalar make_vec2(float n) {
    return vec2_scalar{ n, n };
}

vec2_scalar make_vec2(float x, float y) {
    return vec2_scalar{ x, y };
}

template<typename N> vec2 make_vec2(const N& n) {
    return vec2(n);
}

template<typename X, typename Y> vec2 make_vec2(const X& x, const Y& y) {
    return vec2(x, y);
}

vec2 operator*(vec2_scalar a, Float b) {
    return vec2(a.x*b, a.y*b);
}

vec2 operator*(Float a, vec2_scalar b) {
    return vec2(a*b.x, a*b.y);
}

SI vec2 min(vec2 a, vec2 b)       { return vec2(min(a.x, b.x), min(a.y, b.y));    }

SI vec2 if_then_else(I32 c, vec2 t, vec2 e) {
    return vec2(if_then_else(c, t.x, e.x),
                if_then_else(c, t.y, e.y));
}

SI vec2 if_then_else(int32_t c, vec2 t, vec2 e) {
    return c ? t : e;
}

template <size_t SIZE>
std::array<vec2, SIZE> if_then_else(I32 c, std::array<vec2, SIZE> t, std::array<vec2, SIZE> e) {
    std::array<vec2, SIZE> r;
    for(size_t i = 0; i < SIZE; i++) {
        r[i] = if_then_else(c, t[i], e[i]);
    }
    return r;
}

vec2 step(vec2 edge, vec2 x) {
       return vec2(step(edge.x, x.x), step(edge.y, x.y));
}



vec2 max(vec2 a, vec2 b) {
       return vec2(max(a.x, b.x), max(a.y, b.y));
}

Float length(vec2 a) {
       return sqrt(a.x*a.x+a.y*a.y);
}

SI Float distance(vec2 a, vec2 b) {
        return length(a - b);
}

SI vec2 normalize(vec2 a) {
        return a / length(a);
}


Float abs(Float v) {
        return _mm_and_ps(v, 0-v);
}

template <typename T, typename P>
T unaligned_load(const P* p) {  // const void* would work too, but const P* helps ARMv7 codegen.
    T v;
    memcpy(&v, p, sizeof(v));
    return v;
}

template <typename Dst, typename Src>
Dst bit_cast(const Src& src) {
    static_assert(sizeof(Dst) == sizeof(Src), "");
    return unaligned_load<Dst>(&src);
}

Float   cast  (U32 v) { return      __builtin_convertvector((I32)v,   Float); }
Float   cast  (I32 v) { return      __builtin_convertvector((I32)v,   Float); }
I32   cast  (Float v) { return      __builtin_convertvector(v,   I32); }

    Float floor(Float v) {
    #if defined(JUMPER_IS_SSE41)
        return _mm_floor_ps(v);
    #else
        Float roundtrip = _mm_cvtepi32_ps(_mm_cvttps_epi32(v));
        return roundtrip - if_then_else(roundtrip > v, Float(1), Float(0));
    #endif
    }

vec2 floor(vec2 v) {
        return vec2(floor(v.x), floor(v.y));
}

vec2_scalar floor(vec2_scalar v) {
        return vec2_scalar{floorf(v.x), floorf(v.y)};
}

Float ceil(Float v) {
    #if defined(JUMPER_IS_SSE41)
        return _mm_ceil_ps(v);
    #else
        Float roundtrip = _mm_cvtepi32_ps(_mm_cvttps_epi32(v));
        return roundtrip + if_then_else(roundtrip < v, Float(1), Float(0));
    #endif
    }


    I32 round(Float v, Float scale) { return _mm_cvtps_epi32(v*scale); }

Float fract(Float v) { return v - floor(v); }

Float fwidth(Float p) {
        return abs(p.yyww - p.xxzz) - abs(p.zwzw - p.xyxy);
}

vec2 fwidth(vec2 p) {
        return vec2(fwidth(p.x), fwidth(p.y));
}

// See http://www.machinedlearnings.com/2011/06/fast-approximate-logarithm-exponential.html.
Float approx_log2(Float x) {
    // e - 127 is a fair approximation of log2(x) in its own right...
    Float e = cast(bit_cast<U32>(x)) * (1.0f / (1<<23));

    // ... but using the mantissa to refine its error is _much_ better.
    Float m = bit_cast<Float>((bit_cast<U32>(x) & 0x007fffff) | 0x3f000000);
    return e
         - 124.225514990f
         -   1.498030302f * m
         -   1.725879990f / (0.3520887068f + m);
}
Float approx_pow2(Float x) {
    Float f = fract(x);
    return bit_cast<Float>(round(1.0f * (1<<23),
                             x + 121.274057500f
                               -   1.490129070f * f
                               +  27.728023300f / (4.84252568f - f)));
}

// From skia
Float pow(Float x, Float y) {
    return if_then_else((x == 0)|(x == 1), x
                                         , approx_pow2(approx_log2(x) * y));
}

Float exp(Float y) {
    float x = 2.718281828459045235360287471352;
    return approx_pow2(log2f(x) * y);
}



struct ivec4;

struct ivec2_scalar {
        int32_t x;
        int32_t y;

        int32_t& select(XYZW c) {
                switch (c) {
                    case X: return x;
                    case Y: return y;
                    default: UNREACHABLE;
                }
        }
        int32_t& sel(XYZW c1) {
                return select(c1);
        }
        ivec2_scalar sel(XYZW c1, XYZW c2) {
                return ivec2_scalar{select(c1), select(c2)};
        }

        friend ivec2_scalar operator+(ivec2_scalar a, ivec2_scalar b) {
                return ivec2_scalar{a.x+b.x, a.y+b.y};
        }
};

struct ivec2 {
        ivec2() : ivec2(0) {}
        ivec2(I32 a): x(a), y(a) {}
        ivec2(int32_t x, int32_t y): x(x), y(y) {}
        ivec2(I32 x, I32 y): x(x), y(y) {}
        ivec2(vec2 a): x(cast(a.x)), y(cast(a.y)) {}
        ivec2(U32 x, U32 y): x(__builtin_convertvector(x, I32)), y(__builtin_convertvector(y, I32)) {}
        constexpr ivec2(ivec2_scalar s) : x(s.x), y(s.y) {}
        constexpr ivec2(ivec2_scalar s0, ivec2_scalar s1, ivec2_scalar s2, ivec2_scalar s3)
                : x(I32{s0.x, s1.x, s2.x, s3.x})
                , y(I32{s0.y, s1.y, s2.y, s3.y}) {}
        I32 x;
        I32 y;

        I32& select(XYZW c) {
                switch (c) {
                    case X: return x;
                    case Y: return y;
                    default: UNREACHABLE;
                }
        }
        I32& sel(XYZW c1) {
                return select(c1);
        }

        ivec2 sel(XYZW c1, XYZW c2) {
                return ivec2(select(c1), select(c2));
        }

        ivec4 sel(XYZW c1, XYZW c2, XYZW c3, XYZW c4);


        ivec2& operator*=(I32 a) {
                x *= a;
                y *= a;
                return *this;
        }
        ivec2& operator+=(ivec2 a) {
                x += a.x;
                y += a.y;
                return *this;
        }
        ivec2& operator>>=(int shift) {
                x >>= shift;
                y >>= shift;
                return *this;
        }

        friend ivec2 operator*(ivec2 a, I32 b) {
                return ivec2(a.x*b, a.y*b);
        }
        friend ivec2 operator&(ivec2 a, ivec2 b) {
                return ivec2(a.x&b.x, a.y&b.y);
        }
        friend ivec2 operator&(ivec2 a, I32 b) {
                return ivec2(a.x&b, a.y&b);
        }
        friend ivec2 operator+(ivec2 a, ivec2 b) {
                return ivec2(a.x+b.x, a.y+b.y);
        }
};

vec2::vec2(ivec2 a) : x(cast(a.x)), y(cast(a.y)) {}

ivec2_scalar make_ivec2(int32_t n) {
    return ivec2_scalar{ n, n };
}

ivec2_scalar make_ivec2(uint32_t n) {
    return ivec2_scalar{ int32_t(n), int32_t(n) };
}

ivec2_scalar make_ivec2(int32_t x, int32_t y) {
    return ivec2_scalar{ x, y };
}

ivec2_scalar make_ivec2(uint32_t x, uint32_t y) {
    return ivec2_scalar{ int32_t(x), int32_t(y) };
}

vec2_scalar make_vec2(const ivec2_scalar& v) {
    return vec2_scalar{ float(v.x), float(v.y) };
}


template<typename N> ivec2 make_ivec2(const N& n) {
    return ivec2(n);
}

template<typename X, typename Y> ivec2 make_ivec2(const X& x, const Y& y) {
    return ivec2(x, y);
}

struct ivec3_scalar {
        int32_t x;
        int32_t y;
        int32_t z;

        int32_t& select(XYZW c) {
                switch (c) {
                    case X: return x;
                    case Y: return y;
                    case Z: return z;
                    default: UNREACHABLE;
                }
        }
        int32_t& sel(XYZW c1) {
                return select(c1);
        }
        ivec2_scalar sel(XYZW c1, XYZW c2) {
                return ivec2_scalar{select(c1), select(c2)};
        }

};

struct ivec3 {
        ivec3() : ivec3(0) {}
        ivec3(I32 a): x(a), y(a), z(a) {}
        ivec3(I32 x, I32 y, I32 z): x(x), y(y), z(z) {}
        ivec3(ivec2 a, I32 b): x(a.x), y(a.y), z(b) {}
        ivec3(vec2 a, Float b): x(cast(a.x)), y(cast(a.y)), z(cast(b)) {}
        I32 x;
        I32 y;
        I32 z;

        friend ivec3 operator+(ivec3 a, ivec3 b) {
                return ivec3(a.x+b.x, a.y+b.y, a.z+b.z);
        }
};

vec2_scalar make_vec2(ivec3_scalar s) {
    return vec2_scalar{ float(s.x), float(s.y) };
}

ivec3_scalar make_ivec3(int32_t n) {
    return ivec3_scalar{ n, n, n };
}

ivec3_scalar make_ivec3(const ivec2_scalar& v, int32_t z) {
    return ivec3_scalar{ v.x, v.y, z };
}

ivec3_scalar make_ivec3(int32_t x, int32_t y, int32_t z) {
    return ivec3_scalar{ x, y, z };
}

template<typename N> ivec3 make_ivec3(const N& n) {
    return ivec3(n);
}

template<typename X, typename Y> ivec3 make_ivec3(const X& x, const Y& y) {
    return ivec3(x, y);
}

template<typename X, typename Y, typename Z> ivec3 make_ivec3(const X& x, const Y& y, const Z& z) {
    return ivec3(x, y, z);
}

struct ivec4_scalar {
        int32_t x;
        int32_t y;
        int32_t z;
        int32_t w;

        int32_t& select(XYZW c) {
                switch (c) {
                    case X: return x;
                    case Y: return y;
                    case Z: return z;
                    case W: return w;
                }
        }
        int32_t sel(XYZW c1) {
                return select(c1);
        }
        ivec2_scalar sel(XYZW c1, XYZW c2) {
                return ivec2_scalar{select(c1), select(c2)};
        }

        friend ivec4_scalar operator&(int32_t a, ivec4_scalar b) {
                return ivec4_scalar{a&b.x, a&b.y, a&b.z, a&b.w};
        }
};

struct ivec4 {
        typedef int32_t element_type;

        ivec4() : ivec4(0) {}
        ivec4(I32 a): x(a), y(a), z(a), w(a) {}
        ivec4(I32 x, I32 y, I32 z, I32 w): x(x), y(y), z(z), w(w) {}
        ivec4(ivec2 a, I32 b, I32 c): x(a.x), y(a.y), z(b), w(c) {}
        constexpr ivec4(ivec4_scalar s) : x(s.x), y(s.y), z(s.z), w(s.w) {}
        constexpr ivec4(ivec4_scalar s0, ivec4_scalar s1, ivec4_scalar s2, ivec4_scalar s3) : x(I32{s0.x, s1.x, s2.x, s3.x}),
                                 y(I32{s0.y, s1.y, s2.y, s3.y}),
                                 z(I32{s0.z, s1.z, s2.z, s3.z}),
                                 w(I32{s0.w, s1.w, s2.w, s3.w}) {}

        I32& select(XYZW c) {
                switch (c) {
                    case X: return x;
                    case Y: return y;
                    case Z: return z;
                    case W: return w;
                }
        }
        I32 sel(XYZW c1) {
                return select(c1);
        }

        ivec2 sel(XYZW c1, XYZW c2) {
                return ivec2(select(c1), select(c2));
        }

        ivec3 sel(XYZW c1, XYZW c2, XYZW c3) {
                return ivec3(select(c1), select(c2), select(c3));
        }

        friend ivec4 operator&(I32 a, ivec4 b) {
                return ivec4(a&b.x, a&b.y, a&b.z, a&b.w);
        }

        I32 x;
        I32 y;
        I32 z;
        I32 w;
};

ivec4_scalar force_scalar(const ivec4& v) {
    return ivec4_scalar{ force_scalar(v.x), force_scalar(v.y), force_scalar(v.z), force_scalar(v.w) };
}

ivec4_scalar make_ivec4(int32_t n) {
    return ivec4_scalar{ n, n, n, n };
}

ivec4_scalar make_ivec4(const ivec2_scalar& xy, int32_t z, int32_t w) {
    return ivec4_scalar{ xy.x, xy.y, z, w };
}

ivec4_scalar make_ivec4(int32_t x, int32_t y, int32_t z, int32_t w) {
    return ivec4_scalar{ x, y, z, w };
}

template<typename N> ivec4 make_ivec4(const N& n) {
    return ivec4(n);
}

template<typename X, typename Y, typename Z> ivec4 make_ivec4(const X& x, const Y& y, const Z& z) {
    return ivec4(x, y, z);
}

template<typename X, typename Y, typename Z, typename W> ivec4 make_ivec4(const X& x, const Y& y, const Z& z, const W& w) {
    return ivec4(x, y, z, w);
}

SI ivec2 if_then_else(I32 c, ivec2 t, ivec2 e) {
    return ivec2(if_then_else(c, t.x, e.x),
                if_then_else(c, t.y, e.y));
}

SI ivec2 if_then_else(int32_t c, ivec2 t, ivec2 e) {
    return c ? t : e;
}

SI ivec4 if_then_else(I32 c, ivec4 t, ivec4 e) {
    return ivec4(if_then_else(c, t.x, e.x),
                if_then_else(c, t.y, e.y),
                if_then_else(c, t.z, e.z),
                if_then_else(c, t.w, e.w));
}

SI ivec4 if_then_else(int32_t c, ivec4 t, ivec4 e) {
    return c ? t : e;
}

ivec4 operator&(I32 a, ivec4_scalar b) {
    return ivec4(a&b.x, a&b.y, a&b.z, a&b.w);
}

struct bvec3_scalar {
    bool x;
    bool y;
    bool z;
    bool w;
};

struct bvec3 {
        bvec3() : bvec3(0) {}
        bvec3(Bool a): x(a), y(a), z(a) {}
        bvec3(Bool x, Bool y, Bool z): x(x), y(y), z(z) {}
        Bool& select(XYZW c) {
                switch (c) {
                    case X: return x;
                    case Y: return y;
                    case Z: return z;
                    default: UNREACHABLE;
                }
        }
        Bool sel(XYZW c1) {
                return select(c1);
        }

        Bool x;
        Bool y;
        Bool z;
};


struct bvec4_scalar {
    bool x;
    bool y;
    bool z;
    bool w;
};

struct bvec4 {
        bvec4() : bvec4(0) {}
        bvec4(Bool a): x(a), y(a), z(a), w(a) {}
        bvec4(Bool x, Bool y, Bool z, Bool w): x(x), y(y), z(z), w(w) {}
        bvec4(bvec2 x, bvec2 y): x(x.x), y(x.y), z(y.x), w(y.y) {}
        Bool& select(XYZW c) {
                switch (c) {
                    case X: return x;
                    case Y: return y;
                    case Z: return z;
                    case W: return w;
                }
        }
        Bool sel(XYZW c1) {
                return select(c1);
        }

        Bool x;
        Bool y;
        Bool z;
        Bool w;
};

bvec4_scalar make_bvec4(bool n) {
    return bvec4_scalar{ n, n, n, n };
}

bvec4_scalar make_bvec4(bool x, bool y, bool z, bool w) {
    return bvec4_scalar{ x, y, z, w };
}

template<typename N> bvec4 make_bvec4(const N& n) {
    return bvec4(n);
}

template<typename X, typename Y> bvec4 make_bvec4(const X& x, const Y& y) {
    return bvec4(x, y);
}

template<typename X, typename Y, typename Z, typename W> bvec4 make_bvec4(const X& x, const Y& y, const Z& z, const W& w) {
    return bvec4(x, y, z, w);
}

struct vec2_ref {
        vec2_ref(Float &x, Float &y) : x(x), y(y) {
        }
        Float &x;
        Float &y;
        vec2_ref& operator=(const vec2 &a) {
                x = a.x;
                y = a.y;
                return *this;
        }

        vec2_ref& operator/=(Float a) {
                x /= a;
                y /= a;
                return *this;
        }

        vec2_ref& operator/=(vec2 a) {
                x /= a.x;
                y /= a.y;
                return *this;
        }

        vec2_ref& operator+=(vec2 a) {
                x += a.x;
                y += a.y;
                return *this;
        }
        vec2_ref& operator*=(vec2 a) {
                x *= a.x;
                y *= a.y;
                return *this;
        }

};


struct vec3_scalar {
        typedef struct vec3 vector_type;

        float x;
        float y;
        float z;

        float& select(XYZW c) {
                switch (c) {
                    case X: return x;
                    case Y: return y;
                    case Z: return z;
                    default: UNREACHABLE;
                }
        }
        float& sel(XYZW c1) {
                return select(c1);
        }
        vec2_scalar sel(XYZW c1, XYZW c2) {
                return vec2_scalar(select(c1), select(c2));
        }
        vec2_scalar_ref lsel(XYZW c1, XYZW c2) {
                return vec2_scalar_ref(select(c1), select(c2));
        }

        friend vec3_scalar operator*(vec3_scalar a, float b) {
                return vec3_scalar{a.x*b, a.y*b, a.z*b};
        }

        friend vec3_scalar operator-(vec3_scalar a, vec3_scalar b) {
                return vec3_scalar{a.x-b.x, a.y-b.y, a.z-b.z};
        }
        friend vec3_scalar operator+(vec3_scalar a, vec3_scalar b) {
                return vec3_scalar{a.x+b.x, a.y+b.y, a.z+b.z};
        }
};

struct vec3 {
        typedef struct vec3 vector_type;
        typedef float element_type;

        constexpr vec3() : vec3(0) {  }
        constexpr vec3(Float a): x(a), y(a), z(a) {}
        constexpr vec3(Float x, Float y, Float z): x(x), y(y), z(z)  {}
        vec3(vec2 a, Float z): x(a.x), y(a.y), z(z)  {}
        constexpr vec3(vec3_scalar s) : x(s.x), y(s.y), z(s.z) {}
        constexpr vec3(vec3_scalar s0, vec3_scalar s1, vec3_scalar s2, vec3_scalar s3) : x(Float{s0.x, s1.x, s2.x, s3.x}),
                                 y(Float{s0.y, s1.y, s2.y, s3.y}),
                                 z(Float{s0.z, s1.z, s2.z, s3.z}) {}
        Float x;
        Float y;
        Float z;

        Float& select(XYZW c) {
                switch (c) {
                    case X: return x;
                    case Y: return y;
                    case Z: return z;
                    default: UNREACHABLE;
                }
        }
        Float& sel(XYZW c1) {
                return select(c1);
        }

        vec2 sel(XYZW c1, XYZW c2) {
                return vec2(select(c1), select(c2));
        }

        vec3 sel(XYZW c1, XYZW c2, XYZW c3) {
                return vec3(select(c1), select(c2), select(c3));
        }

        vec2_ref lsel(XYZW c1, XYZW c2) {
                return vec2_ref(select(c1), select(c2));
        }


        friend vec3 operator*(vec3 a, Float b) {
                return vec3(a.x*b, a.y*b, a.z*b);
        }
        friend vec3 operator*(vec3 a, vec3 b) {
                return vec3(a.x*b.x, a.y*b.y, a.z*b.z);
        }

        friend vec3 operator/(vec3 a, Float b) {
                return vec3(a.x/b, a.y/b, a.z/b);
        }

        friend I32 operator==(const vec3& l, const vec3& r)
        {
                return l.x == r.x && l.y == r.y && l.z == r.z;
        }



        friend vec3 operator-(vec3 a, Float b) {
                return vec3(a.x-b, a.y-b, a.z-b);
        }
        friend vec3 operator-(vec3 a, vec3 b) {
                return vec3(a.x-b.x, a.y-b.y, a.z-b.z);
        }
        friend vec3 operator+(vec3 a, Float b) {
                return vec3(a.x+b, a.y+b, a.z+b);
        }
        friend vec3 operator+(vec3 a, vec3 b) {
                return vec3(a.x+b.x, a.y+b.y, a.z+b.z);
        }

        vec3 operator+=(vec3_scalar a) {
                x += a.x;
                y += a.y;
                z += a.z;
                return *this;
        }
};

vec3_scalar force_scalar(const vec3& v) {
    return vec3_scalar{ force_scalar(v.x), force_scalar(v.y), force_scalar(v.z) };
}

vec3_scalar make_vec3(float n) {
    return vec3_scalar{ n, n, n };
}

vec3_scalar make_vec3(const vec2_scalar& v, float z) {
    return vec3_scalar{ v.x, v.y, z };
}

vec3_scalar make_vec3(float x, float y, float z) {
    return vec3_scalar{ x, y, z };
}

vec3_scalar make_vec3(int32_t x, int32_t y, float z) {
    return vec3_scalar{ float(x), float(y), z };
}

template<typename N> vec3 make_vec3(const N& n) {
    return vec3(n);
}

template<typename X, typename Y> vec3 make_vec3(const X& x, const Y& y) {
    return vec3(x, y);
}

template<typename X, typename Y, typename Z> vec3 make_vec3(const X& x, const Y& y, const Z& z) {
    return vec3(x, y, z);
}

SI vec3 if_then_else(I32 c, vec3 t, vec3 e) {
    return vec3(if_then_else(c, t.x, e.x),
                if_then_else(c, t.y, e.y),
                if_then_else(c, t.z, e.z));
}

SI vec3 if_then_else(int32_t c, vec3 t, vec3 e) {
    return c ? t : e;
}

SI vec3 if_then_else(ivec3 c, vec3 t, vec3 e) {
    return vec3(if_then_else(c.x, t.x, e.x),
                if_then_else(c.y, t.y, e.y),
                if_then_else(c.z, t.z, e.z));
}


vec3 step(vec3 edge, vec3 x) {
       return vec3(step(edge.x, x.x), step(edge.y, x.y), step(edge.z, x.z));
}



SI vec3 min(vec3 a, vec3 b)       { return vec3(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));    }
SI vec3 max(vec3 a, vec3 b)       { return vec3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));    }

vec3 pow(vec3 x, vec3 y) {
    return vec3(pow(x.x, y.x), pow(x.y, y.y), pow(x.z, y.z));
}

struct vec3_ref {
        vec3_ref(Float &x, Float &y, Float &z) : x(x), y(y), z(z) {
        }
        Float &x;
        Float &y;
        Float &z;
        vec3_ref& operator=(const vec3 &a) {
                x = a.x;
                y = a.y;
                z = a.z;
                return *this;
        }

        vec3_ref& operator/=(Float a) {
                x /= a;
                y /= a;
                z /= a;
                return *this;
        }

        vec3_ref& operator*=(Float a) {
                x *= a;
                y *= a;
                z *= a;
                return *this;
        }





};

struct vec4_scalar {
        typedef struct vec4 vector_type;

        float x;
        float y;
        float z;
        float w;

        float& select(XYZW c) {
                switch (c) {
                    case X: return x;
                    case Y: return y;
                    case Z: return z;
                    case W: return w;
                }
        }
        float& sel(XYZW c1) {
                return select(c1);
        }
        vec2_scalar sel(XYZW c1, XYZW c2) {
                return vec2_scalar{select(c1), select(c2)};
        }
        vec3_scalar sel(XYZW c1, XYZW c2, XYZW c3) {
                return vec3_scalar{select(c1), select(c2), select(c3)};
        }
        vec2_scalar_ref lsel(XYZW c1, XYZW c2) {
                return vec2_scalar_ref(select(c1), select(c2));
        }

        friend vec4_scalar operator*(vec4_scalar a, vec4_scalar b) {
                return vec4_scalar{a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w};
        }
        friend vec4_scalar operator*(vec4_scalar a, float b) {
                return vec4_scalar{a.x*b, a.y*b, a.z*b, a.w*b};
        }

        friend vec4_scalar operator-(vec4_scalar a, vec4_scalar b) {
                return vec4_scalar{a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w};
        }
        friend vec4_scalar operator+(vec4_scalar a, vec4_scalar b) {
                return vec4_scalar{a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w};
        }

        friend vec4_scalar operator/(vec4_scalar a, vec4_scalar b) {
                return vec4_scalar{a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w};
        }
        vec4_scalar& operator/=(vec4_scalar a) {
                x /= a.x;
                y /= a.y;
                z /= a.z;
                w /= a.w;
                return *this;
        }
};

vec4_scalar vec2_scalar::sel(XYZW c1, XYZW c2, XYZW c3, XYZW c4) {
    return vec4_scalar{select(c1), select(c2), select(c3), select(c4)};
}

struct vec4 {
        typedef struct vec4 vector_type;

        constexpr vec4() : vec4(0) {}
        constexpr vec4(Float a): x(a), y(a), z(a), w(a) {}
        vec4(Float x, Float y, Float z, Float w): x(x), y(y), z(z), w(w) {}
        vec4(vec3 xyz, Float w): x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}
        vec4(vec2 xy, vec2 zw): x(xy.x), y(xy.y), z(zw.x), w(zw.y) {}
        vec4(vec2 xy, Float z, Float w): x(xy.x), y(xy.y), z(z), w(w) {}
        vec4(Float x, Float y, vec2 zw): x(x), y(y), z(zw.x), w(zw.y) {}
        constexpr vec4(vec4_scalar s) : x(s.x), y(s.y), z(s.z), w(s.w) {}
        constexpr vec4(vec4_scalar s0, vec4_scalar s1, vec4_scalar s2, vec4_scalar s3)
                : x(Float{s0.x, s1.x, s2.x, s3.x}),
                  y(Float{s0.y, s1.y, s2.y, s3.y}),
                  z(Float{s0.z, s1.z, s2.z, s3.z}),
                  w(Float{s0.w, s1.w, s2.w, s3.w}) {}
        Float& select(XYZW c) {
                switch (c) {
                    case X: return x;
                    case Y: return y;
                    case Z: return z;
                    case W: return w;
                }
        }
        Float& sel(XYZW c1) {
                return select(c1);
        }

        vec2 sel(XYZW c1, XYZW c2) {
                return vec2(select(c1), select(c2));
        }

        vec3 sel(XYZW c1, XYZW c2, XYZW c3) {
                return vec3(select(c1), select(c2), select(c3));
        }
        vec3_ref lsel(XYZW c1, XYZW c2, XYZW c3) {
                return vec3_ref(select(c1), select(c2), select(c3));
        }

        vec2_ref lsel(XYZW c1, XYZW c2) {
                return vec2_ref(select(c1), select(c2));
        }


        Float& operator[](int index) {
                switch (index) {
                        case 0: return x;
                        case 1: return y;
                        case 2: return z;
                        case 3: return w;
                        default: UNREACHABLE;
                }
        }

        // glsl supports non-const indexing of vecs.
        // hlsl doesn't. The code it generates is probably not wonderful.
        Float operator[](I32 index) {
                float sel_x;
                switch (index.x) {
                        case 0: sel_x = x.x; break;
                        case 1: sel_x = y.x; break;
                        case 2: sel_x = z.x; break;
                        case 3: sel_x = w.x; break;
                }
                float sel_y;
                switch (index.y) {
                        case 0: sel_y = x.y; break;
                        case 1: sel_y = y.y; break;
                        case 2: sel_y = z.y; break;
                        case 3: sel_y = w.y; break;
                }
                float sel_z;
                switch (index.z) {
                        case 0: sel_z = x.z; break;
                        case 1: sel_z = y.z; break;
                        case 2: sel_z = z.z; break;
                        case 3: sel_z = w.z; break;
                }
                float sel_w;
                switch (index.w) {
                        case 0: sel_w = x.w; break;
                        case 1: sel_w = y.w; break;
                        case 2: sel_w = z.w; break;
                        case 3: sel_w = w.w; break;
                }
                Float ret = {sel_x, sel_y, sel_z, sel_w};
                return ret;
        }

        friend vec4 operator/(vec4 a, Float b) {
                return vec4(a.x/b, a.y/b, a.z/b, a.w/b);
        }
        friend vec4 operator/(vec4 a, vec4 b) {
                return vec4(a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w);
        }


        friend vec4 operator*(vec4 a, Float b) {
                return vec4(a.x*b, a.y*b, a.z*b, a.w*b);
        }

        friend vec4 operator*(Float b, vec4 a) {
                return vec4(a.x*b, a.y*b, a.z*b, a.w*b);
        }
        friend vec4 operator*(vec4 a, vec4 b) {
                return vec4(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w);
        }

        friend vec4 operator-(vec4 a, vec4 b) {
                return vec4(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w);
        }
        friend vec4 operator+(vec4 a, vec4 b) {
                return vec4(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w);
        }
        vec4& operator+=(vec4 a) {
                x += a.x;
                y += a.y;
                z += a.z;
                w += a.w;
                return *this;
        }
        vec4& operator/=(vec4 a) {
                x /= a.x;
                y /= a.y;
                z /= a.z;
                w /= a.w;
                return *this;
        }
        vec4& operator*=(Float a) {
                x *= a;
                y *= a;
                z *= a;
                w *= a;
                return *this;
        }


        Float x;
        Float y;
        Float z;
        Float w;
};

vec4_scalar force_scalar(const vec4& v) {
    return vec4_scalar{ force_scalar(v.x), force_scalar(v.y), force_scalar(v.z), force_scalar(v.w) };
}

vec4_scalar make_vec4(float n) {
    return vec4_scalar{ n, n, n, n };
}

vec4_scalar make_vec4(const vec2_scalar& v, float z, float w) {
    return vec4_scalar{ v.x, v.y, z, w };
}

vec4_scalar make_vec4(const vec2_scalar& a, const vec2_scalar& b) {
    return vec4_scalar{ a.x, a.y, b.x, b.y };
}

vec4_scalar make_vec4(const vec3_scalar& v, float w) {
    return vec4_scalar{ v.x, v.y, v.z, w };
}

vec4_scalar make_vec4(float x, float y, float z, float w) {
    return vec4_scalar{ x, y, z, w };
}

template<typename N> vec4 make_vec4(const N& n) {
    return vec4(n);
}

template<typename X, typename Y> vec4 make_vec4(const X& x, const Y& y) {
    return vec4(x, y);
}

template<typename X, typename Y, typename Z> vec4 make_vec4(const X& x, const Y& y, const Z& z) {
    return vec4(x, y, z);
}

template<typename X, typename Y, typename Z, typename W> vec4 make_vec4(const X& x, const Y& y, const Z& z, const W& w) {
    return vec4(x, y, z, w);
}

vec4 operator*(vec4_scalar a, Float b) {
    return vec4(a.x*b, a.y*b, a.z*b, a.w*b);
}

SI vec4 if_then_else(I32 c, vec4 t, vec4 e) {
    return vec4(if_then_else(c, t.x, e.x),
                if_then_else(c, t.y, e.y),
                if_then_else(c, t.z, e.z),
                if_then_else(c, t.w, e.w));
}

SI vec4 if_then_else(int32_t c, vec4 t, vec4 e) {
    return c ? t : e;
}

SI vec2 clamp(vec2 a, vec2 minVal, vec2 maxVal) {
    return vec2(clamp(a.x, minVal.x, maxVal.x),
                clamp(a.y, minVal.y, maxVal.y));
}

SI vec2_scalar clamp(vec2_scalar a, vec2_scalar minVal, vec2_scalar maxVal) {
    return vec2_scalar{clamp(a.x, minVal.x, maxVal.x),
                       clamp(a.y, minVal.y, maxVal.y)};
}

SI I32 clamp(I32 a, I32 minVal, I32 maxVal) {
    assert(0); // does this work?
    a = if_then_else(a < minVal, minVal, a);
    return if_then_else(a > maxVal, maxVal, a);
}

SI vec3 clamp(vec3 a, vec3 minVal, vec3 maxVal) {
    return vec3(clamp(a.x, minVal.x, maxVal.x),
                clamp(a.y, minVal.y, maxVal.y),
                clamp(a.z, minVal.z, maxVal.z));
}

SI vec4 clamp(vec4 a, vec4 minVal, vec4 maxVal) {
    return vec4(clamp(a.x, minVal.x, maxVal.x),
                clamp(a.y, minVal.y, maxVal.y),
                clamp(a.z, minVal.z, maxVal.z),
                clamp(a.w, minVal.w, maxVal.w));
}
template<typename T>
auto lessThanEqual(T x, T y) -> decltype(x <= y) {
        return x <= y;
}

template<typename T>
auto lessThan(T x, T y) -> decltype(x < y) {
        return x < y;
}

SI bvec3 lessThanEqual(vec3 x, vec3 y) {
    return bvec3(lessThanEqual(x.x, y.x),
                 lessThanEqual(x.y, y.y),
                 lessThanEqual(x.z, y.z));
}

SI bvec2 lessThanEqual(vec2 x, vec2 y) {
    return bvec2(lessThanEqual(x.x, y.x),
                 lessThanEqual(x.y, y.y));
}

SI bvec2_scalar lessThanEqual(vec2_scalar x, vec2_scalar y) {
    return bvec2_scalar{lessThanEqual(x.x, y.x),
                        lessThanEqual(x.y, y.y)};
}

SI bvec4 lessThanEqual(vec4 x, vec4 y) {
    return bvec4(lessThanEqual(x.x, y.x),
                 lessThanEqual(x.y, y.y),
                 lessThanEqual(x.z, y.z),
                 lessThanEqual(x.w, y.w));
}


SI bvec2 lessThan(vec2 x, vec2 y) {
    return bvec2(lessThan(x.x, y.x),
                 lessThan(x.y, y.y));
}

template<typename T>
auto greaterThan(T x, T y) -> decltype(x > y) {
        return x > y;
}

bvec2 greaterThan(vec2 x, vec2 y) {
        return bvec2(greaterThan(x.x, y.x),
                     greaterThan(x.y, y.y));
}



template<typename T>
auto greaterThanEqual(T x, T y) -> decltype(x >= y) {
        return x >= y;
}

bvec4 greaterThanEqual(vec4 x, vec4 y) {
        return bvec4(greaterThanEqual(x.x, y.x),
                    greaterThanEqual(x.y, y.y),
                    greaterThanEqual(x.z, y.z),
                    greaterThanEqual(x.w, y.w));
}

enum TextureFormat {
        RGBA32F,
        RGBA32I,
        RGBA8,
        R8
};

enum TextureFilter {
        NEAREST,
        LINEAR
};

struct sampler2DArray_impl {
        uint32_t *buf;
        uint32_t stride; // in dwords
        uint32_t height_stride; // in dwords
        uint32_t height;
        uint32_t width;
        uint32_t depth;
        TextureFormat format;
        TextureFilter filter;
};


typedef sampler2DArray_impl *sampler2DArray;

struct sampler2DArrayRGBA8_impl : sampler2DArray_impl {};
typedef sampler2DArrayRGBA8_impl *sampler2DArrayRGBA8;
struct sampler2DArrayRGBA32F_impl : sampler2DArray_impl {};
typedef sampler2DArrayRGBA32F_impl *sampler2DArrayRGBA32F;

struct sampler2D_impl {
        uint32_t *buf;
        uint32_t stride; // in dwords
        uint32_t height;
        uint32_t width;
        TextureFormat format;
        TextureFilter filter;
};

typedef sampler2D_impl *sampler2D;

struct sampler2DRGBA8_impl : sampler2D_impl {};
typedef sampler2DRGBA8_impl *sampler2DRGBA8;
struct sampler2DRGBA32F_impl : sampler2D_impl {};
typedef sampler2DRGBA32F_impl *sampler2DRGBA32F;

struct isampler2D_impl {
        uint32_t *buf;
        uint32_t stride; // in dwords
        uint32_t height;
        uint32_t width;
        TextureFormat format;
};

typedef isampler2D_impl *isampler2D;

struct isampler2DRGBA32I_impl : isampler2D_impl {};
typedef isampler2DRGBA32I_impl *isampler2DRGBA32I;

struct mat2_scalar {
        vec2_scalar data[2];

        vec2_scalar& operator[](int index) {
                return data[index];
        }
        const vec2_scalar& operator[](int index) const {
                return data[index];
        }

        friend vec2_scalar operator*(mat2_scalar m, vec2_scalar v) {
                vec2_scalar u;
                u.x = m[0].x * v.x + m[1].x * v.y;
                u.y = m[0].y * v.x + m[1].y * v.y;
                return u;
        }

        friend mat2_scalar operator*(mat2_scalar m, float f) {
                mat2_scalar u = m;
                u[0].x *= f;
                u[0].y *= f;
                u[1].x *= f;
                u[1].y *= f;
                return u;
        }
};

struct mat4;

struct mat2 {
        vec2 data[2];

        vec2& operator[](int index) {
                return data[index];
        }
        const vec2& operator[](int index) const {
                return data[index];
        }
        mat2() = default;

        mat2(float a) {
                data[0] = vec2(a);
                data[1] = vec2(a);
        }

        mat2(vec2 a, vec2 b) {
                data[0] = a;
                data[1] = b;
        }
        mat2(const mat4 &mat);
        constexpr mat2(mat2_scalar s) {
                data[0] = vec2(s.data[0]);
                data[1] = vec2(s.data[1]);
        }

        friend vec2 operator*(mat2 m, vec2 v) {
                vec2 u;
                u.x = m[0].x * v.x + m[1].x * v.y;
                u.y = m[0].y * v.x + m[1].y * v.y;
                return u;
        }
        friend mat2 operator*(mat2 m, Float f) {
                mat2 u = m;
                u[0].x *= f;
                u[0].y *= f;
                u[1].x *= f;
                u[1].y *= f;
                return u;
        }

};

mat2_scalar make_mat2(float n) {
    return mat2_scalar{{{n, n}, {n, n}}};
}

mat2_scalar make_mat2(const mat2_scalar& m) {
    return m;
}

mat2_scalar make_mat2(const vec2_scalar& x, const vec2_scalar& y) {
    return mat2_scalar{{x, y}};
}

template<typename N> mat2 make_mat2(const N& n) {
    return mat2(n);
}

template<typename X, typename Y> mat2 make_mat2(const X& x, const Y& y) {
    return mat2(x, y);
}

SI mat2 if_then_else(I32 c, mat2 t, mat2 e) {
    return mat2(if_then_else(c, t[0], e[0]),
                if_then_else(c, t[0], e[1]));
}

SI mat2 if_then_else(int32_t c, mat2 t, mat2 e) {
    return c ? t : e;
}

struct mat3_scalar {
        vec3_scalar data[3];

        vec3_scalar& operator[](int index) {
                return data[index];
        }
        const vec3_scalar& operator[](int index) const {
                return data[index];
        }

        friend vec3 operator*(mat3_scalar m, vec3 v) {
                vec3 u;
                u.x = m[0].x * v.x + m[1].x * v.y + m[2].x * v.z;
                u.y = m[0].y * v.x + m[1].y * v.y + m[2].y * v.z;
                u.z = m[0].z * v.x + m[1].z * v.y + m[2].z * v.z;
                return u;
        }
};

struct mat3 {
        vec3 data[3];

        vec3& operator[](int index) {
                return data[index];
        }
        const vec3& operator[](int index) const {
                return data[index];
        }
        mat3() = default;
        mat3(vec3 a, vec3 b, vec3 c) {
                data[0] = a;
                data[1] = b;
                data[2] = c;
        }

        constexpr mat3(mat3_scalar s) {
                data[0] = vec3(s.data[0]);
                data[1] = vec3(s.data[1]);
                data[2] = vec3(s.data[2]);
        }
        constexpr mat3(mat3_scalar s0, mat3_scalar s1, mat3_scalar s2, mat3_scalar s3) {
                data[0] = vec3(s0.data[0], s1.data[0], s2.data[0], s3.data[0]);
                data[1] = vec3(s0.data[1], s1.data[1], s2.data[1], s3.data[1]);
                data[2] = vec3(s0.data[2], s1.data[2], s2.data[2], s3.data[2]);
        }

        constexpr mat3(Float d1, Float d2, Float d3, Float d4, Float d5, Float d6, Float d7, Float d8, Float d9) {
                data[0] = vec3(d1, d2, d3);
                data[1] = vec3(d4, d5, d6);
                data[2] = vec3(d7, d8, d9);
        }


        mat3(const mat4 &mat);

        friend vec3 operator*(mat3 m, vec3 v) {
                vec3 u;
                u.x = m[0].x * v.x + m[1].x * v.y + m[2].x * v.z;
                u.y = m[0].y * v.x + m[1].y * v.y + m[2].y * v.z;
                u.z = m[0].z * v.x + m[1].z * v.y + m[2].z * v.z;
                return u;
        }
};

mat3_scalar force_scalar(const mat3& v) {
    return mat3_scalar{{ force_scalar(v[0]), force_scalar(v[1]), force_scalar(v[2]) }};
}

mat3_scalar make_mat3(const mat3_scalar& m) {
    return m;
}

mat3_scalar make_mat3(const vec3_scalar& x, const vec3_scalar& y, const vec3_scalar& z) {
    return mat3_scalar{{x, y, z}};
}

constexpr mat3_scalar make_mat3(float m0, float m1, float m2, float m3, float m4, float m5, float m6, float m7, float m8) {
    return mat3_scalar{{{m0, m1, m2}, {m3, m4, m5},  {m6, m7, m8}}};
}

template<typename N> mat3 make_mat3(const N& n) {
    return mat3(n);
}

template<typename X, typename Y, typename Z> mat3 make_mat3(const X& x, const Y& y, const Z& z) {
    return mat3(x, y, z);
}

struct mat4_scalar {
        vec4_scalar data[4];

        vec4_scalar& operator[](int index) {
                return data[index];
        }
        const vec4_scalar& operator[](int index) const {
                return data[index];
        }

        static mat4_scalar load_from_ptr(const float *f) {
                mat4_scalar m;
                // XXX: hopefully this is in the right order
                m.data[0] = vec4_scalar{f[0], f[1], f[2], f[3]};
                m.data[1] = vec4_scalar{f[4], f[5], f[6], f[7]};
                m.data[2] = vec4_scalar{f[8], f[9], f[10], f[11]};
                m.data[3] = vec4_scalar{f[12], f[13], f[14], f[15]};
                return m;
        }

        friend vec4 operator*(mat4_scalar m, vec4 v) {
                vec4 u;
                u.x = m[0].x * v.x + m[1].x * v.y + m[2].x * v.z + m[3].x * v.w;
                u.y = m[0].y * v.x + m[1].y * v.y + m[2].y * v.z + m[3].y * v.w;
                u.z = m[0].z * v.x + m[1].z * v.y + m[2].z * v.z + m[3].z * v.w;
                u.w = m[0].w * v.x + m[1].w * v.y + m[2].w * v.z + m[3].w * v.w;
                return u;
        }
};

struct mat4 {
        vec4 data[4];

        mat4() = default;
        constexpr mat4(mat4_scalar s) {
                data[0] = vec4(s.data[0]);
                data[1] = vec4(s.data[1]);
                data[2] = vec4(s.data[2]);
                data[3] = vec4(s.data[3]);
        }

        mat4(vec4 a, vec4 b, vec4 c, vec4 d) {
                data[0] = a;
                data[1] = b;
                data[2] = c;
                data[3] = d;
        }

        vec4& operator[](int index) {
                return data[index];
        }
        const vec4& operator[](int index) const {
                return data[index];
        }

        friend vec4 operator*(mat4 m, vec4 v) {
                vec4 u;
                u.x = m[0].x * v.x + m[1].x * v.y + m[2].x * v.z + m[3].x * v.w;
                u.y = m[0].y * v.x + m[1].y * v.y + m[2].y * v.z + m[3].y * v.w;
                u.z = m[0].z * v.x + m[1].z * v.y + m[2].z * v.z + m[3].z * v.w;
                u.w = m[0].w * v.x + m[1].w * v.y + m[2].w * v.z + m[3].w * v.w;
                return u;
        }


};

mat3::mat3(const mat4 &mat) : mat3(vec3(mat[0].x, mat[0].y, mat[0].z),
                    vec3(mat[1].x, mat[1].y, mat[1].z),
                    vec3(mat[2].x, mat[2].y, mat[2].z)) {
}

mat2::mat2(const mat4 &mat) : mat2(vec2(mat[0].x, mat[0].y),
                    vec2(mat[1].x, mat[1].y)) {
}

mat2_scalar make_mat2(const mat4_scalar &m) {
    return mat2_scalar{{{m[0].x, m[0].y}, {m[1].x, m[1].y}}};
}

SI mat3 if_then_else(I32 c, mat3 t, mat3 e) {
    return mat3{if_then_else(c, t[0], e[0]),
                if_then_else(c, t[1], e[1]),
                if_then_else(c, t[2], e[2])};
}

SI mat3 if_then_else(int32_t c, mat3 t, mat3 e) {
    return c ? t : e;
}

SI mat4 if_then_else(I32 c, mat4 t, mat4 e) {
    return mat4{if_then_else(c, t[0], e[0]),
                if_then_else(c, t[1], e[1]),
                if_then_else(c, t[2], e[2]),
                if_then_else(c, t[3], e[3])};
}

SI mat4 if_then_else(int32_t c, mat4 t, mat4 e) {
    return c ? t : e;
}

SI I32 clampCoord(I32 coord, int limit) {
    return _mm_min_epi16(_mm_max_epi16(coord, _mm_setzero_si128()), _mm_set1_epi16(limit-1));
}
SI int clampCoord(int coord, int limit) {
    return std::min(std::max(coord, 0), limit);
}
template<typename T, typename S> SI T clamp2D(T P, S sampler) {
    return T{clampCoord(P.x, sampler->width), clampCoord(P.y, sampler->height)};
}
template<typename T> SI T clamp2DArray(T P, sampler2DArray sampler) {
    return T{clampCoord(P.x, sampler->width), clampCoord(P.y, sampler->height), clampCoord(P.z, sampler->depth)};
}

float to_float(uint32_t x) {
        return x * (1.f/255.f);
}

vec4 pixel_to_vec4(int a, int b, int c, int d) {
    I32 pixels = { a, b, c, d };
    return vec4(
        cast((pixels >> 16) & 0xFF),
        cast((pixels >> 8) & 0xFF),
        cast(pixels & 0xFF),
        cast(pixels >> 24)) * (1.0f / 255.0f);
}

vec4 pixel_float_to_vec4(Float a, Float b, Float c, Float d) {
      return vec4(Float{a.x, b.x, c.x, d.x},
                  Float{a.y, b.y, c.y, d.y},
                  Float{a.z, b.z, c.z, d.z},
                  Float{a.w, b.w, c.w, d.w});
}

ivec4 pixel_int_to_ivec4(I32 a, I32 b, I32 c, I32 d) {
      return ivec4(I32{a.x, b.x, c.x, d.x},
                  I32{a.y, b.y, c.y, d.y},
                  I32{a.z, b.z, c.z, d.z},
                  I32{a.w, b.w, c.w, d.w});
}


vec4_scalar pixel_to_vec4(uint32_t p) {
    return vec4_scalar{to_float((p >> 16) & 0xFF),
                       to_float((p >> 8) & 0xFF),
                       to_float((p >> 0) & 0xFF),
                       to_float((p >> 24) & 0xFF)};
}

vec4 texelFetchRGBA8(sampler2D sampler, ivec2 P, int lod) {
        I32 offset = P.x + P.y*sampler->stride;
        return pixel_to_vec4(
                      sampler->buf[offset.x],
                      sampler->buf[offset.y],
                      sampler->buf[offset.z],
                      sampler->buf[offset.w]);
}

vec4 texelFetchR8(sampler2D sampler, ivec2 P, int lod) {
        I32 offset = P.x + P.y*sampler->stride*4;
        return vec4((Float){
                to_float(((uint8_t*)sampler->buf)[offset.x]),
                to_float(((uint8_t*)sampler->buf)[offset.y]),
                to_float(((uint8_t*)sampler->buf)[offset.z]),
                to_float(((uint8_t*)sampler->buf)[offset.w])
        });
}

vec4 texelFetchRGBA8(sampler2DArray sampler, ivec3 P, int lod) {
        I32 offset = P.x + P.y*sampler->stride + P.z*sampler->height_stride;
        return pixel_to_vec4(
                      sampler->buf[offset.x],
                      sampler->buf[offset.y],
                      sampler->buf[offset.z],
                      sampler->buf[offset.w]);
}


vec4 texelFetchFloat(sampler2D sampler, ivec2 P, int lod) {
        I32 offset = P.x*4 + P.y*sampler->stride;
        return pixel_float_to_vec4(
                      *(Float*)&sampler->buf[offset.x],
                      *(Float*)&sampler->buf[offset.y],
                      *(Float*)&sampler->buf[offset.z],
                      *(Float*)&sampler->buf[offset.w]);
}

vec4 texelFetchFloat(sampler2DArray sampler, ivec3 P, int lod) {
        I32 offset = P.x*4 + P.y*sampler->stride + P.z*sampler->height_stride;
        return pixel_float_to_vec4(
                      *(Float*)&sampler->buf[offset.x],
                      *(Float*)&sampler->buf[offset.y],
                      *(Float*)&sampler->buf[offset.z],
                      *(Float*)&sampler->buf[offset.w]);
}



vec4 texelFetch(sampler2D sampler, ivec2 P, int lod) {
        P = clamp2D(P, sampler);
        if (sampler->format == TextureFormat::RGBA32F) {
                return texelFetchFloat(sampler, P, lod);
        } else if (sampler->format == TextureFormat::RGBA8) {
                return texelFetchRGBA8(sampler, P, lod);
        } else {
                assert(sampler->format == TextureFormat::R8);
                return texelFetchR8(sampler, P, lod);
        }
}

vec4 texelFetch(sampler2DRGBA32F sampler, ivec2 P, int lod) {
        P = clamp2D(P, sampler);
        assert(sampler->format == TextureFormat::RGBA32F);
        return texelFetchFloat(sampler, P, lod);
}

vec4 texelFetch(sampler2DRGBA8 sampler, ivec2 P, int lod) {
        P = clamp2D(P, sampler);
        assert(sampler->format == TextureFormat::RGBA8);
        return texelFetchRGBA8(sampler, P, lod);
}

vec4_scalar texelFetch(sampler2D sampler, ivec2_scalar P, int lod) {
        P = clamp2D(P, sampler);
        if (sampler->format == TextureFormat::RGBA32F) {
            return *(vec4_scalar*)&sampler->buf[P.x*4 + P.y*sampler->stride];
        } else {
            assert(sampler->format == TextureFormat::RGBA8);
            return pixel_to_vec4(sampler->buf[P.x + P.y*sampler->stride]);
        }
}

vec4_scalar texelFetch(sampler2DRGBA32F sampler, ivec2_scalar P, int lod) {
        P = clamp2D(P, sampler);
        assert(sampler->format == TextureFormat::RGBA32F);
        return *(vec4_scalar*)&sampler->buf[P.x*4 + P.y*sampler->stride];
}

vec4_scalar texelFetch(sampler2DRGBA8 sampler, ivec2_scalar P, int lod) {
        P = clamp2D(P, sampler);
        assert(sampler->format == TextureFormat::RGBA8);
        return pixel_to_vec4(sampler->buf[P.x + P.y*sampler->stride]);
}

vec4 texelFetch(sampler2DArray sampler, ivec3 P, int lod) {
        P = clamp2DArray(P, sampler);
        if (sampler->format == TextureFormat::RGBA32F) {
                return texelFetchFloat(sampler, P, lod);
        } else {
                assert(sampler->format == TextureFormat::RGBA8);
                return texelFetchRGBA8(sampler, P, lod);
        }
}

vec4 texelFetch(sampler2DArrayRGBA32F sampler, ivec3 P, int lod) {
        P = clamp2DArray(P, sampler);
        assert(sampler->format == TextureFormat::RGBA32F);
        return texelFetchFloat(sampler, P, lod);
}

vec4 texelFetch(sampler2DArrayRGBA8 sampler, ivec3 P, int lod) {
        P = clamp2DArray(P, sampler);
        assert(sampler->format == TextureFormat::RGBA8);
        return texelFetchRGBA8(sampler, P, lod);
}

ivec4 texelFetch(isampler2D sampler, ivec2 P, int lod) {
        P = clamp2D(P, sampler);
        assert(sampler->format == TextureFormat::RGBA32I);
        I32 offset = P.x*4 + P.y*sampler->stride;
        return pixel_int_to_ivec4(
                      *(I32*)&sampler->buf[offset.x],
                      *(I32*)&sampler->buf[offset.y],
                      *(I32*)&sampler->buf[offset.z],
                      *(I32*)&sampler->buf[offset.w]);
}

ivec4_scalar texelFetch(isampler2D sampler, ivec2_scalar P, int lod) {
        P = clamp2D(P, sampler);
        assert(sampler->format == TextureFormat::RGBA32I);
        return *(ivec4_scalar*)&sampler->buf[P.x*4 + P.y*sampler->stride];
}

template <typename T, typename U, typename A, typename R = typename T::vector_type>
SI R mix(T x, U y, A a) {
        return (y - x) * a + x;
}

SI Float mix(Float x, Float y, Float a) {
        return (y - x) * a + x;
}

template<typename T>
SI T mix(T x, T y, float a) {
        return (y - x) * a + x;
}

template <typename T>
SI T mix(T x, T y, vec4_scalar a) {
        return T{mix(x.x, y.x, a.x),
                 mix(x.y, y.y, a.y),
                 mix(x.z, y.z, a.z),
                 mix(x.w, y.w, a.w)};
}

template<typename S>
vec4 textureLinearRGBA8(S sampler, vec2 P, I32 zoffset = 0) {
    assert(sampler->format == TextureFormat::RGBA8);
    P.x *= sampler->width * 256.0f;
    P.y *= sampler->height * 256.0f;
    P -= 0.5f * 256.0f;
    ivec2 i(P);
    ivec2 frac = i & (I32)0xFF;
    i >>= 8;

    __m128i row0 = _mm_min_epi16(_mm_max_epi16(i.y, _mm_setzero_si128()), _mm_set1_epi32(sampler->height - 1));
    row0 = _mm_madd_epi16(row0, _mm_set1_epi32(sampler->stride));
    row0 = _mm_add_epi32(row0, _mm_min_epi16(_mm_max_epi16(i.x, _mm_setzero_si128()), _mm_set1_epi32(sampler->width - 1)));
    row0 = _mm_add_epi32(row0, zoffset);

    __m128i yinside = _mm_andnot_si128(_mm_cmplt_epi32(i.y, _mm_setzero_si128()),
                                       _mm_cmplt_epi32(i.y, _mm_set1_epi32(sampler->height - 1)));
    __m128i row1 = _mm_add_epi32(row0, _mm_and_si128(yinside, _mm_set1_epi32(sampler->stride)));

    __m128i xinside = _mm_andnot_si128(_mm_cmplt_epi32(i.x, _mm_setzero_si128()),
                                       _mm_cmplt_epi32(i.x, _mm_set1_epi32(sampler->width - 1)));
    __m128i fracx = _mm_and_si128(xinside, frac.x);
    fracx = _mm_shufflelo_epi16(fracx, _MM_SHUFFLE(2, 2, 0, 0));
    fracx = _mm_shufflehi_epi16(fracx, _MM_SHUFFLE(2, 2, 0, 0));
    fracx = _mm_slli_epi16(fracx, 4);

    __m128i fracy = _mm_or_si128(_mm_slli_epi32(frac.y, 16), _mm_sub_epi32(_mm_set1_epi32(256), frac.y));

// r0,g0,b0,a0,r1,g1,b1,a1 \/ R0,G0,B0,A0,R1,G1,B1,A1
// r0,R0,g0,G0,b0,B0,a0,A0,r1,R1,g1,G1,b1,B1,a1,A1
// r0_,R0_,g0_,G0_,b0_,B0_,a0_,A0_ /\ r1_,R1_,g1_,G1_,b1_,B1_,a1_,A1_
// (r0*(256-fracy) + R0*fracy), ...
    __m128 r0, r1, r2, r3;
    #define FILTER_LANE(out, idx) \
    { \
        __m128i cc = _mm_unpacklo_epi8( \
            _mm_loadl_epi64((__m128i*)&sampler->buf[_mm_cvtsi128_si32(_mm_shuffle_epi32(row0, _MM_SHUFFLE(idx, idx, idx, idx)))]), \
            _mm_loadl_epi64((__m128i*)&sampler->buf[_mm_cvtsi128_si32(_mm_shuffle_epi32(row1, _MM_SHUFFLE(idx, idx, idx, idx)))])); \
        __m128i cc0 = _mm_unpacklo_epi8(cc, _mm_setzero_si128()); \
        __m128i cc1 = _mm_unpackhi_epi8(cc, _mm_setzero_si128()); \
        cc = _mm_add_epi16(cc0, \
                           _mm_mulhi_epi16(_mm_slli_epi16(_mm_sub_epi16(cc1, cc0), 4), \
                                           _mm_shuffle_epi32(fracx, _MM_SHUFFLE(idx, idx, idx, idx)))); \
        out = _mm_cvtepi32_ps(_mm_madd_epi16(cc, _mm_shuffle_epi32(fracy, _MM_SHUFFLE(idx, idx, idx, idx)))); \
    }
    FILTER_LANE(r0, 0);
    FILTER_LANE(r1, 1);
    FILTER_LANE(r2, 2);
    FILTER_LANE(r3, 3);
    #undef FILTER_LANE

    _MM_TRANSPOSE4_PS(r0, r1, r2, r3);
    return vec4(r2, r1, r0, r3) * (1.0f / 0xFF00);

//    vec4 c00 = texelFetch(sampler, i, 0);
//    vec4 c10 = texelFetch(sampler, i + ivec2(1, 0), 0);
//    vec4 c01 = texelFetch(sampler, i + ivec2(0, 1), 0);
//    vec4 c11 = texelFetch(sampler, i + ivec2(1, 1), 0);
//    return mix(mix(c00, c10, r.x), mix(c10, c11, r.x), r.y);
}

template<typename S>
vec4 textureLinearRGBA32F(S sampler, vec2 P, I32 zoffset = 0) {
    assert(sampler->format == TextureFormat::RGBA32F);
    P.x *= sampler->width;
    P.y *= sampler->height;
    P -= 0.5f;
    vec2 f = floor(P);
    vec2 r = P - f;
    ivec2 i(f);
    ivec2 c = clamp2D(i, sampler);
    r.x = if_then_else(i.x < 0 || i.x > sampler->width-2, 0.0f, r.x);
    I32 offset0 = c.x*4 + c.y*sampler->stride + zoffset;
    I32 offset1 = offset0 + if_then_else(r.y < 0 || r.y > sampler->height-2, 0, sampler->stride);

    Float c0 = mix(mix(*(Float*)&sampler->buf[offset0.x], *(Float*)&sampler->buf[offset0.x+4], r.x),
                   mix(*(Float*)&sampler->buf[offset1.x], *(Float*)&sampler->buf[offset1.x+4], r.x),
                   r.y);
    Float c1 = mix(mix(*(Float*)&sampler->buf[offset0.y], *(Float*)&sampler->buf[offset0.y+4], r.x),
                   mix(*(Float*)&sampler->buf[offset1.y], *(Float*)&sampler->buf[offset1.y+4], r.x),
                   r.y);
    Float c2 = mix(mix(*(Float*)&sampler->buf[offset0.z], *(Float*)&sampler->buf[offset0.z+4], r.x),
                   mix(*(Float*)&sampler->buf[offset1.z], *(Float*)&sampler->buf[offset1.z+4], r.x),
                   r.y);
    Float c3 = mix(mix(*(Float*)&sampler->buf[offset0.w], *(Float*)&sampler->buf[offset0.w+4], r.x),
                   mix(*(Float*)&sampler->buf[offset1.w], *(Float*)&sampler->buf[offset1.w+4], r.x),
                   r.y);
    return pixel_float_to_vec4(c0, c1, c2, c3);
}

vec4 texture(sampler2D sampler, vec2 P) {
    if (sampler->filter == TextureFilter::LINEAR) {
        if (sampler->format == TextureFormat::RGBA32F) {
            return textureLinearRGBA32F(sampler, P);
        } else {
            return textureLinearRGBA8(sampler, P);
        }
    } else {
        ivec2 coord(round(P.x, sampler->width), round(P.y, sampler->height));
        return texelFetch(sampler, coord, 0);
    }
}

vec4 textureLod(sampler2DArray sampler, vec3 P, Float lod) {
        assert(0);
        // just do nearest for now
        return vec4();
}



vec4 texture(sampler2DArray sampler, vec3 P, Float layer) {
        assert(0);
        return vec4();
}
vec4 texture(sampler2DArray sampler, vec3 P) {
    if (sampler->filter == TextureFilter::LINEAR) {
        I32 zoffset = clampCoord(round(P.z, 1.0f), sampler->depth) * sampler->height_stride;
        if (sampler->format == TextureFormat::RGBA32F) {
            return textureLinearRGBA32F(sampler, vec2(P.x, P.y), zoffset);
        } else {
            return textureLinearRGBA8(sampler, vec2(P.x, P.y), zoffset);
        }
    } else {
        // just do nearest for now
        ivec3 coord(round(P.x, sampler->width), round(P.y, sampler->height), round(P.z, 1.0f));
        return texelFetch(sampler, coord, 0);
    }
}

ivec3_scalar textureSize(sampler2DArray sampler, int) {
        return ivec3_scalar{int32_t(sampler->width), int32_t(sampler->height), int32_t(sampler->depth)};
}

ivec2_scalar textureSize(sampler2D sampler, int) {
        return ivec2_scalar{int32_t(sampler->width), int32_t(sampler->height)};
}

ivec4 ivec2::sel(XYZW c1, XYZW c2, XYZW c3, XYZW c4)
        {
                return ivec4(select(c1), select(c2), select(c3), select(c4));
        }

vec4 vec2::sel(XYZW c1, XYZW c2, XYZW c3, XYZW c4)
        {
                return vec4(select(c1), select(c2), select(c3), select(c4));
        }

bool any(bool x) {
        return x;
}

Bool any(bvec4 x) {
        return x.x | x.y | x.z | x.w;
}

Bool any(bvec2 x) {
        return x.x | x.y;
}

bool any(bvec2_scalar x) {
        return x.x | x.y;
}

bool all(bool x) {
        return x;
}

Bool all(bvec2 x) {
        return x.x & x.y;
}
Bool all(bvec4 x) {
        return x.x & x.y & x.z & x.w;
}

SI vec4 if_then_else(bvec4 c, vec4 t, vec4 e) {
    return vec4(if_then_else(c.x, t.x, e.x),
                if_then_else(c.y, t.y, e.y),
                if_then_else(c.z, t.z, e.z),
                if_then_else(c.w, t.w, e.w));
}
SI vec3 if_then_else(bvec3 c, vec3 t, vec3 e) {
    return vec3(if_then_else(c.x, t.x, e.x),
                if_then_else(c.y, t.y, e.y),
                if_then_else(c.z, t.z, e.z));
}


SI vec2 if_then_else(bvec2 c, vec2 t, vec2 e) {
    return vec2(if_then_else(c.x, t.x, e.x),
                if_then_else(c.y, t.y, e.y));
}


template <typename T, typename R = typename T::vector_type>
SI R mix(T x, T y, bvec4 a) {
        return if_then_else(a, y, x);
}

template <typename T, typename R = typename T::vector_type>
SI R mix(T x, T y, bvec3 a) {
        return if_then_else(a, y, x);
}

template <typename T, typename R = typename T::vector_type>
SI R mix(T x, T y, bvec2 a) {
        return if_then_else(a, y, x);
}

template <typename T>
SI T mix(T x, T y, bvec4_scalar a) {
        return T{a.x ? y.x : x.x,
                 a.y ? y.y : x.y,
                 a.z ? y.z : x.z,
                 a.w ? y.w : x.w};
}

template <typename T>
SI T mix(T x, T y, bvec3_scalar a) {
        return T{a.x ? y.x : x.x,
                 a.y ? y.y : x.y,
                 a.z ? y.z : x.z};
}

template <typename T>
SI T mix(T x, T y, bvec2_scalar a) {
        return T{a.x ? y.x : x.x,
                 a.y ? y.y : x.y};
}

template <typename T, typename U, typename R = typename T::vector_type>
SI R mix(T x, T y, U a) {
        return (y - x) * a + x;
}

float dot(vec3_scalar a, vec3_scalar b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
}

Float dot(vec3 a, vec3 b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
}

float dot(vec2_scalar a, vec2_scalar b) {
        return a.x * b.x + a.y * b.y;
}

Float dot(vec2 a, vec2 b) {
        return a.x * b.x + a.y * b.y;
}


Float sin(Float x) {
        assert(false);
        return Float(0);
}
Float cos(Float x) {
        assert(false);
        return Float(0);
}

bvec4 notEqual(ivec4 a, ivec4 b) {
        return bvec4(a.x != b.x, a.y != b.y, a.z != b.z, a.w != b.w);
}

bvec4_scalar notEqual(ivec4_scalar a, ivec4_scalar b) {
        return bvec4_scalar{a.x != b.x, a.y != b.y, a.z != b.z, a.w != b.w};
}

mat3 transpose(mat3 m) {
        return mat3(vec3(m[0].x, m[1].x, m[2].x),
                    vec3(m[0].y, m[1].y, m[2].y),
                    vec3(m[0].z, m[1].z, m[2].z));
}

vec2 abs(vec2 v) {
        return vec2(abs(v.x), abs(v.y));
}

Float mod(Float a, Float b) {
        return a - b * floor(a/b);
}

vec2 mod(vec2 a, vec2 b) {
        return vec2(mod(a.x, b.x), mod(a.y, a.y));
}

vec3 abs(vec3 v) {
        return vec3(abs(v.x), abs(v.y), abs(v.z));
}

mat2 inverse(mat2 v) {
        Float det = v[0].x*v[1].y - v[0].y * v[1].x;
        return mat2(vec2(v[1].y, -v[0].y), vec2(-v[1].x, v[0].x))* (1./det);
}

mat2_scalar inverse(mat2_scalar v) {
        float det = v[0].x*v[1].y - v[0].y * v[1].x;
        return mat2_scalar{{{v[1].y, -v[0].y}, {-v[1].x, v[0].x}}}* (1./det);
}

int32_t get_nth(I32 a, int n) {
        return a[n];
}

float get_nth(Float a, int n) {
        return a[n];
}

float get_nth(float a, int n) {
        return a;
}

ivec2_scalar get_nth(ivec2 a, int n) {
        return ivec2_scalar{a.x[n], a.y[n]};
}

vec2_scalar get_nth(vec2 a, int n) {
        return vec2_scalar{a.x[n], a.y[n]};
}

vec3_scalar get_nth(vec3 a, int n) {
        return vec3_scalar{a.x[n], a.y[n], a.z[n]};
}

vec4_scalar get_nth(vec4 a, int n) {
        return vec4_scalar{a.x[n], a.y[n], a.z[n], a.w[n]};
}

ivec4_scalar get_nth(ivec4 a, int n) {
        return ivec4_scalar{a.x[n], a.y[n], a.z[n], a.w[n]};
}

void put_nth(Float &dst, int n, float src) {
        dst[n] = src;
}

void put_nth(I32 &dst, int n, int32_t src) {
        dst[n] = src;
}

void put_nth(ivec2 &dst, int n, ivec2_scalar src) {
        dst.x[n] = src.x;
        dst.y[n] = src.y;
}

void put_nth(vec2 &dst, int n, vec2_scalar src) {
        dst.x[n] = src.x;
        dst.y[n] = src.y;
}

void put_nth(vec3 &dst, int n, vec3_scalar src) {
        dst.x[n] = src.x;
        dst.y[n] = src.y;
        dst.z[n] = src.z;
}

void put_nth(ivec4 &dst, int n, ivec4_scalar src) {
        dst.x[n] = src.x;
        dst.y[n] = src.y;
        dst.z[n] = src.z;
        dst.w[n] = src.w;
}

void put_nth(vec4 &dst, int n, vec4_scalar src) {
        dst.x[n] = src.x;
        dst.y[n] = src.y;
        dst.z[n] = src.z;
        dst.w[n] = src.w;
}

Float init_interp(float init0, float step) {
    float init1 = init0 + step;
    float init2 = init1 + step;
    float init3 = init2 + step;
    return {init0, init1, init2, init3};
}

vec2 init_interp(vec2_scalar init, vec2_scalar step) {
    return vec2(init_interp(init.x, step.x), init_interp(init.y, step.y));
}

vec3 init_interp(vec3_scalar init, vec3_scalar step) {
    return vec3(init_interp(init.x, step.x), init_interp(init.y, step.y), init_interp(init.z, step.z));
}

vec4 init_interp(vec4_scalar init, vec4_scalar step) {
    return vec4(init_interp(init.x, step.x), init_interp(init.y, step.y), init_interp(init.z, step.z), init_interp(init.w, step.w));
}

template <size_t SIZE>
std::array<int32_t, SIZE> get_nth(std::array<int32_t, SIZE> a, int n) {
    return a;
}

mat3_scalar get_nth(mat3 a, int n) {
        return make_mat3(get_nth(a[0], n),
                         get_nth(a[1], n),
                         get_nth(a[2], n));
}

template<typename D, typename S, size_t N>
SI void convert_array(std::array<D, N>& d, std::array<S, N>& s) {
    for (size_t i = 0; i < N; ++i) d[i] = D(s[i]);
}

// See lp_build_sample_soa_code(
// lp_build_sample_aos used for common cases
// lp_build_sample_image_linear for an actual mip
// lp_build_sample_fetch_image_linear
// lp_build_lerp_simple

// sampleQuad2D - does the bilinear lerp on 8bit values expanded to 16bit
// it does the lerp on 4 pixels at a time
// i.e. 4 Vector4s is 4*4*4 shorts
}
