#include <string.h>
#include <stdint.h>
#include <xmmintrin.h>
#include <assert.h>
#include <math.h>
#include <array>


// Some of this is copied from Skia and is governed by a BSD-style license
// Every function in this file should be marked static and inline using SI.
#if defined(__clang__)
    #define SI __attribute__((always_inline)) static inline
#else
    #define SI static inline
#endif

namespace glsl {

template <typename T> using V = T __attribute__((ext_vector_type(4)));
using Float   = V<float   >;
using I32 = V< int32_t>;
using U64 = V<uint64_t>;
using U32 = V<uint32_t>;
using U16 = V<uint16_t>;
using U8  = V<uint8_t >;
using Bool = V<int>;

struct vec4;
struct ivec2;

SI Float if_then_else(I32 c, float t, float e) {
    return _mm_or_ps(_mm_and_ps(c, Float(t)), _mm_andnot_ps(c, Float(e)));
}

SI Float if_then_else(I32 c, double t, double e) {
    return _mm_or_ps(_mm_and_ps(c, Float(t)), _mm_andnot_ps(c, Float(e)));
}


SI I32 if_then_else(I32 c, int t, int e) {
    return _mm_or_ps(_mm_and_ps(c, I32(t)), _mm_andnot_ps(c, I32(e)));
}



SI Float if_then_else(I32 c, Float t, Float e) {
    return _mm_or_ps(_mm_and_ps(c, t), _mm_andnot_ps(c, e));
}

SI Bool if_then_else(I32 c, Bool t, Bool e) {
    return _mm_or_ps(_mm_and_ps(c, t), _mm_andnot_ps(c, e));
}



SI Float   min(Float a, Float b)       { return _mm_min_ps(a,b);    }
SI Float   max(Float a, Float b)       { return _mm_max_ps(a,b);    }

SI Float clamp(Float a, Float minVal, Float maxVal) {
        return min(max(a, minVal), maxVal);
}

SI Float sqrt(Float x) {
        return _mm_sqrt_ps(x);
}

SI Float inversesqrt(Float x) {
        return 1./_mm_sqrt_ps(x);
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

struct bvec2 {
        bvec2() { }
        bvec2(Bool a): x(a), y(a) {}
        bvec2(Bool x, Bool y): x(x), y(y) {}
        Bool& select(XYZW c) {
                switch (c) {
                    case X: return x;
                    case Y: return y;
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



struct vec2 {
        vec2() { vec2(0); }
        vec2(Float a): x(a), y(a) {}
        vec2(Float x, Float y): x(x), y(y) {}
        vec2(ivec2 a);
        Float x;
        Float y;

        Float& select(XYZW c) {
                switch (c) {
                    case X: return x;
                    case Y: return y;
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

SI vec2 min(vec2 a, vec2 b)       { return vec2(min(a.x, b.x), min(a.y, b.y));    }

SI vec2 if_then_else(I32 c, vec2 t, vec2 e) {
    return vec2(if_then_else(c, t.x, e.x),
                if_then_else(c, t.y, e.y));
}

template <size_t SIZE=4>
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

Float ceil(Float v) {
    #if defined(JUMPER_IS_SSE41)
        return _mm_ceil_ps(v);
    #else
        Float roundtrip = _mm_cvtepi32_ps(_mm_cvttps_epi32(v));
        return roundtrip + if_then_else(roundtrip < v, Float(1), Float(0));
    #endif
    }


    U32 round(Float v, Float scale) { return _mm_cvtps_epi32(v*scale); }

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

struct ivec2 {
        ivec2() { ivec2(0); }
        ivec2(I32 a): x(a), y(a) {}
        ivec2(int x, int y): x(x), y(y) {}
        ivec2(I32 x, I32 y): x(x), y(y) {}
        ivec2(vec2 a): x(cast(a.x)), y(cast(a.y)) {}
        ivec2(U32 x, U32 y): x(__builtin_convertvector(x, I32)), y(__builtin_convertvector(y, I32)) {}
        I32 x;
        I32 y;

        I32& select(XYZW c) {
                switch (c) {
                    case X: return x;
                    case Y: return y;
                }
        }
        I32& sel(XYZW c1) {
                return select(c1);
        }

        ivec2 sel(XYZW c1, XYZW c2) {
                return ivec2(select(c1), select(c2));
        }



        ivec4 sel(XYZW c1, XYZW c2, XYZW c3, XYZW c4);


        ivec2 operator*=(I32 a) {
                x *= a;
                y *= a;
                return *this;
        }
        ivec2 operator+=(ivec2 a) {
                x += a.x;
                y += a.y;
                return *this;
        }

        friend ivec2 operator*(ivec2 a, I32 b) {
                return ivec2(a.x*b, a.y*b);
        }
        friend ivec2 operator&(ivec2 a, ivec2 b) {
                return ivec2(a.x&b.x, a.y&b.y);
        }

        friend ivec2 operator+(ivec2 a, ivec2 b) {
                return ivec2(a.x*b.x, a.y*b.y);
        }

};

vec2::vec2(ivec2 a) : x(cast(a.x)), y(cast(a.y)) {}

struct ivec3 {
        ivec3() { ivec3(0); }
        ivec3(I32 a): x(a), y(a), z(a) {}
        ivec3(I32 x, I32 y, I32 z): x(x), y(y), z(z) {}
        ivec3(ivec2 a, I32 b): x(a.x), y(a.y), z(b) {}
        ivec3(vec2 a, Float b): x(cast(a.x)), y(cast(a.y)), z(cast(b)) {}
        I32 x;
        I32 y;
        I32 z;

};

struct ivec4 {
        ivec4() { ivec4(0); }
        ivec4(I32 a): x(a), y(a), z(a), w(a) {}
        ivec4(I32 x, I32 y, I32 z, I32 w): x(x), y(y), z(z), w(w) {}
        ivec4(ivec2 a, I32 b, I32 c): x(a.x), y(a.y), z(b), w(c) {}
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

SI ivec2 if_then_else(I32 c, ivec2 t, ivec2 e) {
    return ivec2(if_then_else(c, t.x, e.x),
                if_then_else(c, t.y, e.y));
}


SI ivec4 if_then_else(I32 c, ivec4 t, ivec4 e) {
    return ivec4(if_then_else(c, t.x, e.x),
                if_then_else(c, t.y, e.y),
                if_then_else(c, t.z, e.z),
                if_then_else(c, t.w, e.w));
}

struct bvec3 {
        bvec3() { }
        bvec3(Bool a): x(a), y(a), z(a) {}
        bvec3(Bool x, Bool y, Bool z): x(x), y(y), z(z) {}
        Bool& select(XYZW c) {
                switch (c) {
                    case X: return x;
                    case Y: return y;
                    case Z: return z;
                }
        }
        Bool sel(XYZW c1) {
                return select(c1);
        }

        Bool x;
        Bool y;
        Bool z;
};


struct bvec4 {
        bvec4() { ivec4(0); }
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



struct vec3 {
        constexpr vec3() : vec3(0) {  }
        constexpr vec3(Float a): x(a), y(a), z(a) {}
        constexpr vec3(Float x, Float y, Float z): x(x), y(y), z(z)  {}
        vec3(vec2 a, Float z): x(a.x), y(a.y), z(z)  {}
        Float x;
        Float y;
        Float z;

        Float& select(XYZW c) {
                switch (c) {
                    case X: return x;
                    case Y: return y;
                    case Z: return z;
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




};
SI vec3 if_then_else(I32 c, vec3 t, vec3 e) {
    return vec3(if_then_else(c, t.x, e.x),
                if_then_else(c, t.y, e.y),
                if_then_else(c, t.z, e.z));
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


U32 uint(I32 x) {
        return __builtin_convertvector(x,   U32);
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

struct vec4 {
        vec4() { vec4(0); }
        vec4(Float a): x(a), y(a), z(a), w(a) {}
        vec4(Float x, Float y, Float z, Float w): x(x), y(y), z(z), w(w) {}
        vec4(vec3 xyz, Float w): x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}
        vec4(vec2 xy, vec2 zw): x(xy.x), y(xy.y), z(zw.x), w(zw.y) {}
        vec4(vec2 xy, Float z, Float w): x(xy.x), y(xy.y), z(z), w(w) {}
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

SI vec4 if_then_else(I32 c, vec4 t, vec4 e) {
    return vec4(if_then_else(c, t.x, e.x),
                if_then_else(c, t.y, e.y),
                if_then_else(c, t.z, e.z),
                if_then_else(c, t.w, e.w));
}

SI vec2 clamp(vec2 a, vec2 minVal, vec2 maxVal) {
    return vec2(clamp(a.x, minVal.x, maxVal.x),
                clamp(a.y, minVal.y, maxVal.y));
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
Bool lessThanEqual(Float x, Float y) {
        return x <= y;
}

Bool lessThan(Float x, Float y) {
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

Bool greaterThan(Float x, Float y) {
        return x > y;
}
bvec2 greaterThan(vec2 x, vec2 y) {
        return bvec2(greaterThan(x.x, y.x),
                     greaterThan(x.y, y.y));
}



Bool greaterThanEqual(Float x, Float y) {
        return x >= y;
}

bvec4 greaterThanEqual(vec4 x, vec4 y) {
        return bvec4(greaterThanEqual(x.x, y.x),
                    greaterThanEqual(x.y, y.y),
                    greaterThanEqual(x.z, y.z),
                    greaterThanEqual(x.w, y.w));
}


struct sampler2DArray_impl {
        uint32_t *buf;
        uint32_t stride;
        uint32_t height;
        uint32_t width;
};


typedef sampler2DArray_impl *sampler2DArray;

struct sampler2D_impl {
        uint32_t *buf;
        uint32_t stride;
        uint32_t height;
        uint32_t width;
};

typedef sampler2D_impl *sampler2D;

struct isampler2D_impl {
        uint32_t *buf;
        uint32_t stride;
        uint32_t height;
        uint32_t width;
};

typedef isampler2D_impl *isampler2D;

struct mat4;

struct mat2 {
        vec2 data[];

        vec2& operator[](int index) {
                return data[index];
        }
        mat2() {
                data[0] = vec2();
                data[1] = vec2();
        }

        mat2(float a) {
                data[0] = vec2(a);
                data[1] = vec2(a);
        }

        mat2(vec2 a, vec2 b) {
                data[0] = a;
                data[1] = b;
        }
        mat2(mat4 &mat);
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

SI mat2 if_then_else(I32 c, mat2 t, mat2 e) {
    return mat2(if_then_else(c, t[0], e[0]),
                if_then_else(c, t[0], e[1]));
}



struct mat3 {
        vec3 data[3];

        vec3& operator[](int index) {
                return data[index];
        }
        mat3() {
                data[0] = vec3();
                data[1] = vec3();
                data[2] = vec3();
        }
        mat3(vec3 a, vec3 b, vec3 c) {
                data[0] = a;
                data[1] = b;
                data[2] = c;
        }

        constexpr mat3(Float d1, Float d2, Float d3, Float d4, Float d5, Float d6, Float d7, Float d8, Float d9) {
                data[0] = vec3(d1, d2, d3);
                data[1] = vec3(d4, d5, d6);
                data[2] = vec3(d7, d8, d9);
        }


        mat3(mat4 &mat);

        friend vec3 operator*(mat3 m, vec3 v) {
                vec3 u;
                u.x = m[0].x * v.x + m[1].x * v.y + m[2].x * v.z;
                u.y = m[0].y * v.x + m[1].y * v.y + m[2].y * v.z;
                u.z = m[0].z * v.x + m[1].z * v.y + m[2].z * v.z;
                return u;
        }
};

struct mat4 {
        vec4 data[4];
        vec4& operator[](int index) {
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

mat3::mat3(mat4 &mat) : mat3(vec3(mat[0].x, mat[0].y, mat[0].z),
                    vec3(mat[1].x, mat[1].y, mat[1].z),
                    vec3(mat[2].x, mat[2].y, mat[2].z)) {
}

mat2::mat2(mat4 &mat) : mat2(vec2(mat[0].x, mat[0].y),
                    vec2(mat[1].x, mat[1].y)) {
}
SI mat3 if_then_else(I32 c, mat3 t, mat3 e) {
    return mat3{if_then_else(c, t[0], e[0]),
                if_then_else(c, t[1], e[1]),
                if_then_else(c, t[2], e[2])};
}


SI mat4 if_then_else(I32 c, mat4 t, mat4 e) {
    return mat4{if_then_else(c, t[0], e[0]),
                if_then_else(c, t[1], e[1]),
                if_then_else(c, t[2], e[2]),
                if_then_else(c, t[3], e[3])};
}

uint32_t fetchPixel(isampler2D sampler, int x, int y) {
        return sampler->buf[x  + y * sampler->stride];
}

uint32_t fetchPixel(sampler2D sampler, int x, int y) {
        return sampler->buf[x  + y * sampler->stride];
}

float to_float(uint32_t x) {
        return x * (1.f/255.f);
}

Float extract_component(uint32_t a, uint32_t b, uint32_t c, uint32_t d, int shift) {
        int mask = 0xff << shift;
        Float ret = {
                to_float((a & mask) >> shift),
                to_float((b & mask) >> shift),
                to_float((c & mask) >> shift),
                to_float((d & mask) >> shift)};
        return ret;
}

// This needs to be faster
vec4 pixel_to_vec4(uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
      return vec4(extract_component(a, b, c, d, 16),
           extract_component(a, b, c, d, 8),
           extract_component(a, b, c, d, 0),
           extract_component(a, b, c, d, 24));
}

vec4 texelFetch(sampler2D sampler, ivec2 P, int lod) {
        return pixel_to_vec4(
                      fetchPixel(sampler, P.x.x, P.y.x),
                      fetchPixel(sampler, P.x.y, P.y.y),
                      fetchPixel(sampler, P.x.z, P.y.z),
                      fetchPixel(sampler, P.x.w, P.y.w)
                      );
}
vec4 texelFetch(sampler2DArray sampler, ivec3 P, int lod) {
        assert(0); // handle P.
        return vec4();
}

ivec4 texelFetch(isampler2D sampler, ivec2 P, int lod) {
        return ivec4(
                      fetchPixel(sampler, P.x.x, P.y.x),
                      fetchPixel(sampler, P.x.y, P.y.y),
                      fetchPixel(sampler, P.x.z, P.y.z),
                      fetchPixel(sampler, P.x.w, P.y.w)
                      );
}

vec4 texture(sampler2D sampler, vec3 P) {
        // just do nearest for now
        ivec2 coord(round(P.x, sampler->width), round(P.y, sampler->height));
        return texelFetch(sampler, coord, 0);
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
        assert(0);
        return vec4();
}




ivec2 textureSize(sampler2DArray sampler, int) {
        return ivec2(sampler->width, sampler->height);
}

 ivec4 ivec2::sel(XYZW c1, XYZW c2, XYZW c3, XYZW c4)
        {
                return ivec4(select(c1), select(c2), select(c3), select(c4));
        }

vec4 vec2::sel(XYZW c1, XYZW c2, XYZW c3, XYZW c4)
        {
                return vec4(select(c1), select(c2), select(c3), select(c4));
        }

template <typename T>
SI T mix(T x, T y, Float a) {
        return (x - y) * a + x;
}

//XXX: having this version helps
//for arguments like mix(Float, double, Float)
template <typename T>
SI T mix(T x, Float y, Float a) {
        return (x - y) * a + x;
}

SI Float mix(Float x, Float y, Float a) {
        return (x - y) * a + x;
}


Bool any(bvec4 x) {
        return x.x | x.y | x.z | x.w;
}

Bool any(bvec2 x) {
        return x.x | x.y;
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



SI vec4 mix(vec4 x, vec4 y, bvec4 a) {
        return if_then_else(a, x, y);
}
SI vec3 mix(vec3 x, vec3 y, bvec3 a) {
        return if_then_else(a, x, y);
}


SI vec2 mix(vec2 x, vec2 y, bvec2 a) {
        return if_then_else(a, x, y);
}

template <typename T>
SI T mix(T x, T y, T a) {
        return (x - y) * a + x;
}

Float dot(vec3 a, vec3 b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
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

mat3 transpose(mat3 m) {
        return mat3(vec3(m[0].x, m[1].x, m[2].x),
                    vec3(m[0].y, m[1].y, m[2].y),
                    vec3(m[0].z, m[1].z, m[2].z));
}

vec2 floor(vec2 v) {
        return vec2(floor(v.x), floor(v.y));
}

vec2 abs(vec2 v) {
        return vec2(abs(v.x), abs(v.y));
}

Float mod(Float a, Float b) {
        assert(0);
        return a;
}

vec2 mod(vec2 a, vec2 b) {
        return vec2(mod(a.x, b.x), mod(a.y, a.y));
}

vec3 abs(vec3 v) {
        return vec3(abs(v.x), abs(v.y), abs(v.z));
}

mat2 inverse(mat2 v) {
        assert(0);
        return mat2();
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
