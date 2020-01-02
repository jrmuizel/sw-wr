#version 300 es
// brush_radial_gradient
#define WR_VERTEX_SHADER
#define WR_MAX_VERTEX_TEXTURE_WIDTH 1024U
#define WR_FEATURE_DITHERING
#define WR_FEATURE_ALPHA_PASS
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#define VECS_PER_SPECIFIC_BRUSH 2

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef WR_FEATURE_PIXEL_LOCAL_STORAGE
// For now, we need both extensions here, in order to initialize
// the PLS to the current framebuffer color. In future, we can
// possibly remove that requirement, or at least support the
// other framebuffer fetch extensions that provide the same
// functionality.
#extension GL_EXT_shader_pixel_local_storage : require
#extension GL_ARM_shader_framebuffer_fetch : require
#endif

#ifdef WR_FEATURE_TEXTURE_EXTERNAL
// Please check https://www.khronos.org/registry/OpenGL/extensions/OES/OES_EGL_image_external_essl3.txt
// for this extension.
#extension GL_OES_EGL_image_external_essl3 : require
#endif

#ifdef WR_FEATURE_ADVANCED_BLEND
#extension GL_KHR_blend_equation_advanced : require
#endif

#ifdef WR_FEATURE_DUAL_SOURCE_BLENDING
#ifdef GL_ES
#extension GL_EXT_blend_func_extended : require
#else
#extension GL_ARB_explicit_attrib_location : require
#endif
#endif

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#if defined(GL_ES)
    #if GL_ES == 1
        #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp sampler2DArray;
        #else
        precision mediump sampler2DArray;
        #endif

        // Sampler default precision is lowp on mobile GPUs.
        // This causes RGBA32F texture data to be clamped to 16 bit floats on some GPUs (e.g. Mali-T880).
        // Define highp precision macro to allow lossless FLOAT texture sampling.
        #define HIGHP_SAMPLER_FLOAT highp

        // Default int precision in GLES 3 is highp (32 bits) in vertex shaders
        // and mediump (16 bits) in fragment shaders. If an int is being used as
        // a texel address in a fragment shader it, and therefore requires > 16
        // bits, it must be qualified with this.
        #define HIGHP_FS_ADDRESS highp

        // texelFetchOffset is buggy on some Android GPUs (see issue #1694).
        // Fallback to texelFetch on mobile GPUs.
        #define TEXEL_FETCH(sampler, position, lod, offset) texelFetch(sampler, position + offset, lod)
    #else
        #define HIGHP_SAMPLER_FLOAT
        #define HIGHP_FS_ADDRESS
        #define TEXEL_FETCH(sampler, position, lod, offset) texelFetchOffset(sampler, position, lod, offset)
    #endif
#else
    #define HIGHP_SAMPLER_FLOAT
    #define HIGHP_FS_ADDRESS
    #define TEXEL_FETCH(sampler, position, lod, offset) texelFetchOffset(sampler, position, lod, offset)
#endif

#ifdef WR_VERTEX_SHADER
    #define varying out
#endif

#ifdef WR_FRAGMENT_SHADER
    precision highp float;
    #define varying in
#endif

#if defined(WR_FEATURE_TEXTURE_EXTERNAL) || defined(WR_FEATURE_TEXTURE_RECT) || defined(WR_FEATURE_TEXTURE_2D)
#define TEX_SAMPLE(sampler, tex_coord) texture(sampler, tex_coord.xy)
#else
#define TEX_SAMPLE(sampler, tex_coord) texture(sampler, tex_coord)
#endif

//======================================================================================
// Vertex shader attributes and uniforms
//======================================================================================
#ifdef WR_VERTEX_SHADER
    // A generic uniform that shaders can optionally use to configure
    // an operation mode for this batch.
    uniform int uMode;

    // Uniform inputs
    uniform mat4 uTransform;       // Orthographic projection

    // Attribute inputs
    in vec3 aPosition;

    // get_fetch_uv is a macro to work around a macOS Intel driver parsing bug.
    // TODO: convert back to a function once the driver issues are resolved, if ever.
    // https://github.com/servo/webrender/pull/623
    // https://github.com/servo/servo/issues/13953
    // Do the division with unsigned ints because that's more efficient with D3D
    #define get_fetch_uv(i, vpi)  ivec2(int(vpi * (uint(i) % (WR_MAX_VERTEX_TEXTURE_WIDTH/vpi))), int(uint(i) / (WR_MAX_VERTEX_TEXTURE_WIDTH/vpi)))
#endif

//======================================================================================
// Fragment shader attributes and uniforms
//======================================================================================
#ifdef WR_FRAGMENT_SHADER
    // Uniform inputs

    #ifdef WR_FEATURE_PIXEL_LOCAL_STORAGE
        // Define the storage class of the pixel local storage.
        // If defined as writable, it's a compile time error to
        // have a normal fragment output variable declared.
        #if defined(PLS_READONLY)
            #define PLS_BLOCK __pixel_local_inEXT
        #elif defined(PLS_WRITEONLY)
            #define PLS_BLOCK __pixel_local_outEXT
        #else
            #define PLS_BLOCK __pixel_localEXT
        #endif

        // The structure of pixel local storage. Right now, it's
        // just the current framebuffer color. In future, we have
        // (at least) 12 bytes of space we can store extra info
        // here (such as clip mask values).
        PLS_BLOCK FrameBuffer {
            layout(rgba8) highp vec4 color;
        } PLS;

        #ifndef PLS_READONLY
        // Write the output of a fragment shader to PLS. Applies
        // premultipled alpha blending by default, since the blender
        // is disabled when PLS is active.
        // TODO(gw): Properly support alpha blend mode for webgl / canvas.
        void write_output(vec4 color) {
            PLS.color = color + PLS.color * (1.0 - color.a);
        }

        // Write a raw value straight to PLS, if the fragment shader has
        // already applied blending.
        void write_output_raw(vec4 color) {
            PLS.color = color;
        }
        #endif

        #ifndef PLS_WRITEONLY
        // Retrieve the current framebuffer color. Useful in conjunction with
        // the write_output_raw function.
        vec4 get_current_framebuffer_color() {
            return PLS.color;
        }
        #endif
    #else
        // Fragment shader outputs
        #ifdef WR_FEATURE_ADVANCED_BLEND
            layout(blend_support_all_equations) out;
        #endif

        #ifdef WR_FEATURE_DUAL_SOURCE_BLENDING
            layout(location = 0, index = 0) out vec4 oFragColor;
            layout(location = 0, index = 1) out vec4 oFragBlend;
        #else
            out vec4 oFragColor;
        #endif

        // Write an output color in normal (non-PLS) shaders.
        void write_output(vec4 color) {
            oFragColor = color;
        }
    #endif

    #define EPSILON                     0.0001

    // "Show Overdraw" color. Premultiplied.
    #define WR_DEBUG_OVERDRAW_COLOR     vec4(0.110, 0.077, 0.027, 0.125)

    float distance_to_line(vec2 p0, vec2 perp_dir, vec2 p) {
        vec2 dir_to_p0 = p0 - p;
        return dot(normalize(perp_dir), dir_to_p0);
    }

    /// Find the appropriate half range to apply the AA approximation over.
    /// This range represents a coefficient to go from one CSS pixel to half a device pixel.
    float compute_aa_range(vec2 position) {
        // The constant factor is chosen to compensate for the fact that length(fw) is equal
        // to sqrt(2) times the device pixel ratio in the typical case. 0.5/sqrt(2) = 0.35355.
        //
        // This coefficient is chosen to ensure that any sample 0.5 pixels or more inside of
        // the shape has no anti-aliasing applied to it (since pixels are sampled at their center,
        // such a pixel (axis aligned) is fully inside the border). We need this so that antialiased
        // curves properly connect with non-antialiased vertical or horizontal lines, among other things.
        //
        // Lines over a half-pixel away from the pixel center *can* intersect with the pixel square;
        // indeed, unless they are horizontal or vertical, they are guaranteed to. However, choosing
        // a nonzero area for such pixels causes noticeable artifacts at the junction between an anti-
        // aliased corner and a straight edge.
        //
        // We may want to adjust this constant in specific scenarios (for example keep the principled
        // value for straight edges where we want pixel-perfect equivalence with non antialiased lines
        // when axis aligned, while selecting a larger and smoother aa range on curves).
        return 0.35355 * length(fwidth(position));
    }

    /// Return the blending coefficient for distance antialiasing.
    ///
    /// 0.0 means inside the shape, 1.0 means outside.
    ///
    /// This cubic polynomial approximates the area of a 1x1 pixel square under a
    /// line, given the signed Euclidean distance from the center of the square to
    /// that line. Calculating the *exact* area would require taking into account
    /// not only this distance but also the angle of the line. However, in
    /// practice, this complexity is not required, as the area is roughly the same
    /// regardless of the angle.
    ///
    /// The coefficients of this polynomial were determined through least-squares
    /// regression and are accurate to within 2.16% of the total area of the pixel
    /// square 95% of the time, with a maximum error of 3.53%.
    ///
    /// See the comments in `compute_aa_range()` for more information on the
    /// cutoff values of -0.5 and 0.5.
    float distance_aa(float aa_range, float signed_distance) {
        float dist = 0.5 * signed_distance / aa_range;
        if (dist <= -0.5 + EPSILON)
            return 1.0;
        if (dist >= 0.5 - EPSILON)
            return 0.0;
        return 0.5 + dist * (0.8431027 * dist * dist - 1.14453603);
    }

    /// Component-wise selection.
    ///
    /// The idea of using this is to ensure both potential branches are executed before
    /// selecting the result, to avoid observable timing differences based on the condition.
    ///
    /// Example usage: color = if_then_else(LessThanEqual(color, vec3(0.5)), vec3(0.0), vec3(1.0));
    ///
    /// The above example sets each component to 0.0 or 1.0 independently depending on whether
    /// their values are below or above 0.5.
    ///
    /// This is written as a macro in order to work with vectors of any dimension.
    ///
    /// Note: Some older android devices don't support mix with bvec. If we ever run into them
    /// the only option we have is to polyfill it with a branch per component.
    #define if_then_else(cond, then_branch, else_branch) mix(else_branch, then_branch, cond)
#endif

//======================================================================================
// Shared shader uniforms
//======================================================================================
#ifdef WR_FEATURE_TEXTURE_2D
uniform sampler2D sColor0;
uniform sampler2D sColor1;
uniform sampler2D sColor2;
#elif defined WR_FEATURE_TEXTURE_RECT
uniform sampler2DRect sColor0;
uniform sampler2DRect sColor1;
uniform sampler2DRect sColor2;
#elif defined WR_FEATURE_TEXTURE_EXTERNAL
uniform samplerExternalOES sColor0;
uniform samplerExternalOES sColor1;
uniform samplerExternalOES sColor2;
#else
uniform sampler2DArray sColor0;
uniform sampler2DArray sColor1;
uniform sampler2DArray sColor2;
#endif

#ifdef WR_FEATURE_DITHERING
uniform sampler2D sDither;
#endif

//======================================================================================
// Interpolator definitions
//======================================================================================

//======================================================================================
// VS only types and UBOs
//======================================================================================

//======================================================================================
// VS only functions
//======================================================================================
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

struct RectWithSize {
    vec2 p0;
    vec2 size;
};

struct RectWithEndpoint {
    vec2 p0;
    vec2 p1;
};

RectWithEndpoint to_rect_with_endpoint(RectWithSize rect) {
    RectWithEndpoint result;
    result.p0 = rect.p0;
    result.p1 = rect.p0 + rect.size;

    return result;
}

RectWithSize to_rect_with_size(RectWithEndpoint rect) {
    RectWithSize result;
    result.p0 = rect.p0;
    result.size = rect.p1 - rect.p0;

    return result;
}

RectWithSize intersect_rects(RectWithSize a, RectWithSize b) {
    RectWithSize result;
    result.p0 = max(a.p0, b.p0);
    result.size = min(a.p0 + a.size, b.p0 + b.size) - result.p0;

    return result;
}

float point_inside_rect(vec2 p, vec2 p0, vec2 p1) {
    vec2 s = step(p0, p) - step(p1, p);
    return s.x * s.y;
}
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifdef WR_VERTEX_SHADER
#define VECS_PER_RENDER_TASK        2U

uniform HIGHP_SAMPLER_FLOAT sampler2D sRenderTasks;

struct RenderTaskCommonData {
    RectWithSize task_rect;
    float texture_layer_index;
};

struct RenderTaskData {
    RenderTaskCommonData common_data;
    vec3 user_data;
};

RenderTaskData fetch_render_task_data(int index) {
    ivec2 uv = get_fetch_uv(index, VECS_PER_RENDER_TASK);

    vec4 texel0 = TEXEL_FETCH(sRenderTasks, uv, 0, ivec2(0, 0));
    vec4 texel1 = TEXEL_FETCH(sRenderTasks, uv, 0, ivec2(1, 0));

    RectWithSize task_rect = RectWithSize(
        texel0.xy,
        texel0.zw
    );

    RenderTaskCommonData common_data = RenderTaskCommonData(
        task_rect,
        texel1.x
    );

    RenderTaskData data = RenderTaskData(
        common_data,
        texel1.yzw
    );

    return data;
}

RenderTaskCommonData fetch_render_task_common_data(int index) {
    ivec2 uv = get_fetch_uv(index, VECS_PER_RENDER_TASK);

    vec4 texel0 = TEXEL_FETCH(sRenderTasks, uv, 0, ivec2(0, 0));
    vec4 texel1 = TEXEL_FETCH(sRenderTasks, uv, 0, ivec2(1, 0));

    RectWithSize task_rect = RectWithSize(
        texel0.xy,
        texel0.zw
    );

    RenderTaskCommonData data = RenderTaskCommonData(
        task_rect,
        texel1.x
    );

    return data;
}

#define PIC_TYPE_IMAGE          1
#define PIC_TYPE_TEXT_SHADOW    2

/*
 The dynamic picture that this brush exists on. Right now, it
 contains minimal information. In the future, it will describe
 the transform mode of primitives on this picture, among other things.
 */
struct PictureTask {
    RenderTaskCommonData common_data;
    float device_pixel_scale;
    vec2 content_origin;
};

PictureTask fetch_picture_task(int address) {
    RenderTaskData task_data = fetch_render_task_data(address);

    PictureTask task = PictureTask(
        task_data.common_data,
        task_data.user_data.x,
        task_data.user_data.yz
    );

    return task;
}

#define CLIP_TASK_EMPTY 0x7FFF

struct ClipArea {
    RenderTaskCommonData common_data;
    float device_pixel_scale;
    vec2 screen_origin;
};

ClipArea fetch_clip_area(int index) {
    ClipArea area;

    if (index >= CLIP_TASK_EMPTY) {
        RectWithSize rect = RectWithSize(vec2(0.0), vec2(0.0));

        area.common_data = RenderTaskCommonData(rect, 0.0);
        area.device_pixel_scale = 0.0;
        area.screen_origin = vec2(0.0);
    } else {
        RenderTaskData task_data = fetch_render_task_data(index);

        area.common_data = task_data.common_data;
        area.device_pixel_scale = task_data.user_data.x;
        area.screen_origin = task_data.user_data.yz;
    }

    return area;
}

#endif //WR_VERTEX_SHADER
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

uniform HIGHP_SAMPLER_FLOAT sampler2D sGpuCache;

#define VECS_PER_IMAGE_RESOURCE     2

// TODO(gw): This is here temporarily while we have
//           both GPU store and cache. When the GPU
//           store code is removed, we can change the
//           PrimitiveInstance instance structure to
//           use 2x unsigned shorts as vertex attributes
//           instead of an int, and encode the UV directly
//           in the vertices.
ivec2 get_gpu_cache_uv(HIGHP_FS_ADDRESS int address) {
    return ivec2(uint(address) % WR_MAX_VERTEX_TEXTURE_WIDTH,
                 uint(address) / WR_MAX_VERTEX_TEXTURE_WIDTH);
}

vec4[2] fetch_from_gpu_cache_2_direct(ivec2 address) {
    return vec4[2](
        TEXEL_FETCH(sGpuCache, address, 0, ivec2(0, 0)),
        TEXEL_FETCH(sGpuCache, address, 0, ivec2(1, 0))
    );
}

vec4[2] fetch_from_gpu_cache_2(HIGHP_FS_ADDRESS int address) {
    ivec2 uv = get_gpu_cache_uv(address);
    return vec4[2](
        TEXEL_FETCH(sGpuCache, uv, 0, ivec2(0, 0)),
        TEXEL_FETCH(sGpuCache, uv, 0, ivec2(1, 0))
    );
}

vec4 fetch_from_gpu_cache_1_direct(ivec2 address) {
    return texelFetch(sGpuCache, address, 0);
}

vec4 fetch_from_gpu_cache_1(HIGHP_FS_ADDRESS int address) {
    ivec2 uv = get_gpu_cache_uv(address);
    return texelFetch(sGpuCache, uv, 0);
}

#ifdef WR_VERTEX_SHADER

vec4[8] fetch_from_gpu_cache_8(int address) {
    ivec2 uv = get_gpu_cache_uv(address);
    return vec4[8](
        TEXEL_FETCH(sGpuCache, uv, 0, ivec2(0, 0)),
        TEXEL_FETCH(sGpuCache, uv, 0, ivec2(1, 0)),
        TEXEL_FETCH(sGpuCache, uv, 0, ivec2(2, 0)),
        TEXEL_FETCH(sGpuCache, uv, 0, ivec2(3, 0)),
        TEXEL_FETCH(sGpuCache, uv, 0, ivec2(4, 0)),
        TEXEL_FETCH(sGpuCache, uv, 0, ivec2(5, 0)),
        TEXEL_FETCH(sGpuCache, uv, 0, ivec2(6, 0)),
        TEXEL_FETCH(sGpuCache, uv, 0, ivec2(7, 0))
    );
}

vec4[3] fetch_from_gpu_cache_3(int address) {
    ivec2 uv = get_gpu_cache_uv(address);
    return vec4[3](
        TEXEL_FETCH(sGpuCache, uv, 0, ivec2(0, 0)),
        TEXEL_FETCH(sGpuCache, uv, 0, ivec2(1, 0)),
        TEXEL_FETCH(sGpuCache, uv, 0, ivec2(2, 0))
    );
}

vec4[3] fetch_from_gpu_cache_3_direct(ivec2 address) {
    return vec4[3](
        TEXEL_FETCH(sGpuCache, address, 0, ivec2(0, 0)),
        TEXEL_FETCH(sGpuCache, address, 0, ivec2(1, 0)),
        TEXEL_FETCH(sGpuCache, address, 0, ivec2(2, 0))
    );
}

vec4[4] fetch_from_gpu_cache_4_direct(ivec2 address) {
    return vec4[4](
        TEXEL_FETCH(sGpuCache, address, 0, ivec2(0, 0)),
        TEXEL_FETCH(sGpuCache, address, 0, ivec2(1, 0)),
        TEXEL_FETCH(sGpuCache, address, 0, ivec2(2, 0)),
        TEXEL_FETCH(sGpuCache, address, 0, ivec2(3, 0))
    );
}

vec4[4] fetch_from_gpu_cache_4(int address) {
    ivec2 uv = get_gpu_cache_uv(address);
    return vec4[4](
        TEXEL_FETCH(sGpuCache, uv, 0, ivec2(0, 0)),
        TEXEL_FETCH(sGpuCache, uv, 0, ivec2(1, 0)),
        TEXEL_FETCH(sGpuCache, uv, 0, ivec2(2, 0)),
        TEXEL_FETCH(sGpuCache, uv, 0, ivec2(3, 0))
    );
}

//TODO: image resource is too specific for this module

struct ImageResource {
    RectWithEndpoint uv_rect;
    float layer;
    vec3 user_data;
};

ImageResource fetch_image_resource(int address) {
    //Note: number of blocks has to match `renderer::BLOCKS_PER_UV_RECT`
    vec4 data[2] = fetch_from_gpu_cache_2(address);
    RectWithEndpoint uv_rect = RectWithEndpoint(data[0].xy, data[0].zw);
    return ImageResource(uv_rect, data[1].x, data[1].yzw);
}

ImageResource fetch_image_resource_direct(ivec2 address) {
    vec4 data[2] = fetch_from_gpu_cache_2_direct(address);
    RectWithEndpoint uv_rect = RectWithEndpoint(data[0].xy, data[0].zw);
    return ImageResource(uv_rect, data[1].x, data[1].yzw);
}

// Fetch optional extra data for a texture cache resource. This can contain
// a polygon defining a UV rect within the texture cache resource.
// Note: the polygon coordinates are in homogeneous space.
struct ImageResourceExtra {
    vec4 st_tl;
    vec4 st_tr;
    vec4 st_bl;
    vec4 st_br;
};

ImageResourceExtra fetch_image_resource_extra(int address) {
    vec4 data[4] = fetch_from_gpu_cache_4(address + VECS_PER_IMAGE_RESOURCE);
    return ImageResourceExtra(
        data[0],
        data[1],
        data[2],
        data[3]
    );
}

#endif //WR_VERTEX_SHADER
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

flat varying vec4 vTransformBounds;

#ifdef WR_VERTEX_SHADER

#define VECS_PER_TRANSFORM   8U
uniform HIGHP_SAMPLER_FLOAT sampler2D sTransformPalette;

void init_transform_vs(vec4 local_bounds) {
    vTransformBounds = local_bounds;
}

struct Transform {
    mat4 m;
    mat4 inv_m;
    bool is_axis_aligned;
};

Transform fetch_transform(int id) {
    Transform transform;

    transform.is_axis_aligned = (id >> 24) == 0;
    int index = id & 0x00ffffff;

    // Create a UV base coord for each 8 texels.
    // This is required because trying to use an offset
    // of more than 8 texels doesn't work on some versions
    // of macOS.
    ivec2 uv = get_fetch_uv(index, VECS_PER_TRANSFORM);
    ivec2 uv0 = ivec2(uv.x + 0, uv.y);

    transform.m[0] = TEXEL_FETCH(sTransformPalette, uv0, 0, ivec2(0, 0));
    transform.m[1] = TEXEL_FETCH(sTransformPalette, uv0, 0, ivec2(1, 0));
    transform.m[2] = TEXEL_FETCH(sTransformPalette, uv0, 0, ivec2(2, 0));
    transform.m[3] = TEXEL_FETCH(sTransformPalette, uv0, 0, ivec2(3, 0));

    transform.inv_m[0] = TEXEL_FETCH(sTransformPalette, uv0, 0, ivec2(4, 0));
    transform.inv_m[1] = TEXEL_FETCH(sTransformPalette, uv0, 0, ivec2(5, 0));
    transform.inv_m[2] = TEXEL_FETCH(sTransformPalette, uv0, 0, ivec2(6, 0));
    transform.inv_m[3] = TEXEL_FETCH(sTransformPalette, uv0, 0, ivec2(7, 0));

    return transform;
}

// Return the intersection of the plane (set up by "normal" and "point")
// with the ray (set up by "ray_origin" and "ray_dir"),
// writing the resulting scaler into "t".
bool ray_plane(vec3 normal, vec3 pt, vec3 ray_origin, vec3 ray_dir, out float t)
{
    float denom = dot(normal, ray_dir);
    if (abs(denom) > 1e-6) {
        vec3 d = pt - ray_origin;
        t = dot(d, normal) / denom;
        return t >= 0.0;
    }

    return false;
}

// Apply the inverse transform "inv_transform"
// to the reference point "ref" in CSS space,
// producing a local point on a Transform plane,
// set by a base point "a" and a normal "n".
vec4 untransform(vec2 ref, vec3 n, vec3 a, mat4 inv_transform) {
    vec3 p = vec3(ref, -10000.0);
    vec3 d = vec3(0, 0, 1.0);

    float t = 0.0;
    // get an intersection of the Transform plane with Z axis vector,
    // originated from the "ref" point
    ray_plane(n, a, p, d, t);
    float z = p.z + d.z * t; // Z of the visible point on the Transform

    vec4 r = inv_transform * vec4(ref, z, 1.0);
    return r;
}

// Given a CSS space position, transform it back into the Transform space.
vec4 get_node_pos(vec2 pos, Transform transform) {
    // get a point on the scroll node plane
    vec4 ah = transform.m * vec4(0.0, 0.0, 0.0, 1.0);
    vec3 a = ah.xyz / ah.w;

    // get the normal to the scroll node plane
    vec3 n = transpose(mat3(transform.inv_m)) * vec3(0.0, 0.0, 1.0);
    return untransform(pos, n, a, transform.inv_m);
}

#endif //WR_VERTEX_SHADER

#ifdef WR_FRAGMENT_SHADER

float signed_distance_rect(vec2 pos, vec2 p0, vec2 p1) {
    vec2 d = max(p0 - pos, pos - p1);
    return length(max(vec2(0.0), d)) + min(0.0, max(d.x, d.y));
}

float init_transform_fs(vec2 local_pos) {
    // Get signed distance from local rect bounds.
    float d = signed_distance_rect(
        local_pos,
        vTransformBounds.xy,
        vTransformBounds.zw
    );

    // Find the appropriate distance to apply the AA smoothstep over.
    float aa_range = compute_aa_range(local_pos);

    // Only apply AA to fragments outside the signed distance field.
    return distance_aa(aa_range, d);
}

float init_transform_rough_fs(vec2 local_pos) {
    return point_inside_rect(
        local_pos,
        vTransformBounds.xy,
        vTransformBounds.zw
    );
}

#endif //WR_FRAGMENT_SHADER

#define EXTEND_MODE_CLAMP  0
#define EXTEND_MODE_REPEAT 1

#define SUBPX_DIR_NONE        0
#define SUBPX_DIR_HORIZONTAL  1
#define SUBPX_DIR_VERTICAL    2
#define SUBPX_DIR_MIXED       3

#define RASTER_LOCAL            0
#define RASTER_SCREEN           1

uniform sampler2DArray sPrevPassAlpha;
uniform sampler2DArray sPrevPassColor;

vec2 clamp_rect(vec2 pt, RectWithSize rect) {
    return clamp(pt, rect.p0, rect.p0 + rect.size);
}

// TODO: convert back to RectWithEndPoint if driver issues are resolved, if ever.
flat varying vec4 vClipMaskUvBounds;
// XY and W are homogeneous coordinates, Z is the layer index
varying vec4 vClipMaskUv;


#ifdef WR_VERTEX_SHADER

#define COLOR_MODE_FROM_PASS          0
#define COLOR_MODE_ALPHA              1
#define COLOR_MODE_SUBPX_CONST_COLOR  2
#define COLOR_MODE_SUBPX_BG_PASS0     3
#define COLOR_MODE_SUBPX_BG_PASS1     4
#define COLOR_MODE_SUBPX_BG_PASS2     5
#define COLOR_MODE_SUBPX_DUAL_SOURCE  6
#define COLOR_MODE_BITMAP             7
#define COLOR_MODE_COLOR_BITMAP       8
#define COLOR_MODE_IMAGE              9

uniform HIGHP_SAMPLER_FLOAT sampler2D sPrimitiveHeadersF;
uniform HIGHP_SAMPLER_FLOAT isampler2D sPrimitiveHeadersI;

// Instanced attributes
in ivec4 aData;

#define VECS_PER_PRIM_HEADER_F 2U
#define VECS_PER_PRIM_HEADER_I 2U

struct PrimitiveHeader {
    RectWithSize local_rect;
    RectWithSize local_clip_rect;
    float z;
    int specific_prim_address;
    int transform_id;
    ivec4 user_data;
};

PrimitiveHeader fetch_prim_header(int index) {
    PrimitiveHeader ph;

    ivec2 uv_f = get_fetch_uv(index, VECS_PER_PRIM_HEADER_F);
    vec4 local_rect = TEXEL_FETCH(sPrimitiveHeadersF, uv_f, 0, ivec2(0, 0));
    vec4 local_clip_rect = TEXEL_FETCH(sPrimitiveHeadersF, uv_f, 0, ivec2(1, 0));
    ph.local_rect = RectWithSize(local_rect.xy, local_rect.zw);
    ph.local_clip_rect = RectWithSize(local_clip_rect.xy, local_clip_rect.zw);

    ivec2 uv_i = get_fetch_uv(index, VECS_PER_PRIM_HEADER_I);
    ivec4 data0 = TEXEL_FETCH(sPrimitiveHeadersI, uv_i, 0, ivec2(0, 0));
    ivec4 data1 = TEXEL_FETCH(sPrimitiveHeadersI, uv_i, 0, ivec2(1, 0));
    ph.z = float(data0.x);
    ph.specific_prim_address = data0.y;
    ph.transform_id = data0.z;
    ph.user_data = data1;

    return ph;
}

struct VertexInfo {
    vec2 local_pos;
    vec2 snap_offset;
    vec4 world_pos;
};

VertexInfo write_vertex(RectWithSize instance_rect,
                        RectWithSize local_clip_rect,
                        float z,
                        Transform transform,
                        PictureTask task) {

    // Select the corner of the local rect that we are processing.
    vec2 local_pos = instance_rect.p0 + instance_rect.size * aPosition.xy;

    // Clamp to the two local clip rects.
    vec2 clamped_local_pos = clamp_rect(local_pos, local_clip_rect);

    // Transform the current vertex to world space.
    vec4 world_pos = transform.m * vec4(clamped_local_pos, 0.0, 1.0);

    // Convert the world positions to device pixel space.
    vec2 device_pos = world_pos.xy * task.device_pixel_scale;

    // Apply offsets for the render task to get correct screen location.
    vec2 final_offset = -task.content_origin + task.common_data.task_rect.p0;

    gl_Position = uTransform * vec4(device_pos + final_offset * world_pos.w, z * world_pos.w, world_pos.w);

    VertexInfo vi = VertexInfo(
        clamped_local_pos,
        vec2(0.0, 0.0),
        world_pos
    );

    return vi;
}

float cross2(vec2 v0, vec2 v1) {
    return v0.x * v1.y - v0.y * v1.x;
}

// Return intersection of line (p0,p1) and line (p2,p3)
vec2 intersect_lines(vec2 p0, vec2 p1, vec2 p2, vec2 p3) {
    vec2 d0 = p0 - p1;
    vec2 d1 = p2 - p3;

    float s0 = cross2(p0, p1);
    float s1 = cross2(p2, p3);

    float d = cross2(d0, d1);
    float nx = s0 * d1.x - d0.x * s1;
    float ny = s0 * d1.y - d0.y * s1;

    return vec2(nx / d, ny / d);
}

VertexInfo write_transform_vertex(RectWithSize local_segment_rect,
                                  RectWithSize local_prim_rect,
                                  RectWithSize local_clip_rect,
                                  vec4 clip_edge_mask,
                                  float z,
                                  Transform transform,
                                  PictureTask task) {
    // Calculate a clip rect from local_rect + local clip
    RectWithEndpoint clip_rect = to_rect_with_endpoint(local_clip_rect);
    RectWithEndpoint segment_rect = to_rect_with_endpoint(local_segment_rect);
    segment_rect.p0 = clamp(segment_rect.p0, clip_rect.p0, clip_rect.p1);
    segment_rect.p1 = clamp(segment_rect.p1, clip_rect.p0, clip_rect.p1);

    // Calculate a clip rect from local_rect + local clip
    RectWithEndpoint prim_rect = to_rect_with_endpoint(local_prim_rect);
    prim_rect.p0 = clamp(prim_rect.p0, clip_rect.p0, clip_rect.p1);
    prim_rect.p1 = clamp(prim_rect.p1, clip_rect.p0, clip_rect.p1);

    // As this is a transform shader, extrude by 2 (local space) pixels
    // in each direction. This gives enough space around the edge to
    // apply distance anti-aliasing. Technically, it:
    // (a) slightly over-estimates the number of required pixels in the simple case.
    // (b) might not provide enough edge in edge case perspective projections.
    // However, it's fast and simple. If / when we ever run into issues, we
    // can do some math on the projection matrix to work out a variable
    // amount to extrude.

    // Only extrude along edges where we are going to apply AA.
    float extrude_amount = 2.0;
    vec4 extrude_distance = vec4(extrude_amount) * clip_edge_mask;
    local_segment_rect.p0 -= extrude_distance.xy;
    local_segment_rect.size += extrude_distance.xy + extrude_distance.zw;

    // Select the corner of the local rect that we are processing.
    vec2 local_pos = local_segment_rect.p0 + local_segment_rect.size * aPosition.xy;

    // Convert the world positions to device pixel space.
    vec2 task_offset = task.common_data.task_rect.p0 - task.content_origin;

    // Transform the current vertex to the world cpace.
    vec4 world_pos = transform.m * vec4(local_pos, 0.0, 1.0);
    vec4 final_pos = vec4(
        world_pos.xy * task.device_pixel_scale + task_offset * world_pos.w,
        z * world_pos.w,
        world_pos.w
    );

    gl_Position = uTransform * final_pos;

    init_transform_vs(mix(
        vec4(prim_rect.p0, prim_rect.p1),
        vec4(segment_rect.p0, segment_rect.p1),
        clip_edge_mask
    ));

    VertexInfo vi = VertexInfo(
        local_pos,
        vec2(0.0),
        world_pos
    );

    return vi;
}

void write_clip(vec4 world_pos, vec2 snap_offset, ClipArea area) {
    vec2 uv = world_pos.xy * area.device_pixel_scale +
        world_pos.w * (snap_offset + area.common_data.task_rect.p0 - area.screen_origin);
    vClipMaskUvBounds = vec4(
        area.common_data.task_rect.p0,
        area.common_data.task_rect.p0 + area.common_data.task_rect.size
    );
    vClipMaskUv = vec4(uv, area.common_data.texture_layer_index, world_pos.w);
}

// Read the exta image data containing the homogeneous screen space coordinates
// of the corners, interpolate between them, and return real screen space UV.
vec2 get_image_quad_uv(int address, vec2 f) {
    ImageResourceExtra extra_data = fetch_image_resource_extra(address);
    vec4 x = mix(extra_data.st_tl, extra_data.st_tr, f.x);
    vec4 y = mix(extra_data.st_bl, extra_data.st_br, f.x);
    vec4 z = mix(x, y, f.y);
    return z.xy / z.w;
}
#endif //WR_VERTEX_SHADER

#ifdef WR_FRAGMENT_SHADER

float do_clip() {
    // check for the dummy bounds, which are given to the opaque objects
    if (vClipMaskUvBounds.xy == vClipMaskUvBounds.zw) {
        return 1.0;
    }
    // anything outside of the mask is considered transparent
    //Note: we assume gl_FragCoord.w == interpolated(1 / vClipMaskUv.w)
    vec2 mask_uv = vClipMaskUv.xy * gl_FragCoord.w;
    bvec2 left = lessThanEqual(vClipMaskUvBounds.xy, mask_uv); // inclusive
    bvec2 right = greaterThan(vClipMaskUvBounds.zw, mask_uv); // non-inclusive
    // bail out if the pixel is outside the valid bounds
    if (!all(bvec4(left, right))) {
        return 0.0;
    }
    // finally, the slow path - fetch the mask value from an image
    // Note the Z getting rounded to the nearest integer because the variable
    // is still interpolated and becomes a subject of precision-caused
    // fluctuations, see https://bugzilla.mozilla.org/show_bug.cgi?id=1491911
    ivec3 tc = ivec3(mask_uv, vClipMaskUv.z + 0.5);
    return texelFetch(sPrevPassAlpha, tc, 0).r;
}

#ifdef WR_FEATURE_DITHERING
vec4 dither(vec4 color) {
    const int matrix_mask = 7;

    ivec2 pos = ivec2(gl_FragCoord.xy) & ivec2(matrix_mask);
    float noise_normalized = (texelFetch(sDither, pos, 0).r * 255.0 + 0.5) / 64.0;
    float noise = (noise_normalized - 0.5) / 256.0; // scale down to the unit length

    return color + vec4(noise, noise, noise, 0);
}
#else
vec4 dither(vec4 color) {
    return color;
}
#endif //WR_FEATURE_DITHERING

vec4 sample_gradient(HIGHP_FS_ADDRESS int address, float offset, float gradient_repeat) {
    // Modulo the offset if the gradient repeats.
    float x = mix(offset, fract(offset), gradient_repeat);

    // Calculate the color entry index to use for this offset:
    //     offsets < 0 use the first color entry, 0
    //     offsets from [0, 1) use the color entries in the range of [1, N-1)
    //     offsets >= 1 use the last color entry, N-1
    //     so transform the range [0, 1) -> [1, N-1)

    // TODO(gw): In the future we might consider making the size of the
    // LUT vary based on number / distribution of stops in the gradient.
    const int GRADIENT_ENTRIES = 128;
    x = 1.0 + x * float(GRADIENT_ENTRIES);

    // Calculate the texel to index into the gradient color entries:
    //     floor(x) is the gradient color entry index
    //     fract(x) is the linear filtering factor between start and end
    int lut_offset = 2 * int(floor(x));     // There is a [start, end] color per entry.

    // Ensure we don't fetch outside the valid range of the LUT.
    lut_offset = clamp(lut_offset, 0, 2 * (GRADIENT_ENTRIES + 1));

    // Fetch the start and end color.
    vec4 texels[2] = fetch_from_gpu_cache_2(address + lut_offset);

    // Finally interpolate and apply dithering
    return dither(mix(texels[0], texels[1], fract(x)));
}

#endif //WR_FRAGMENT_SHADER
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef WR_VERTEX_SHADER

void brush_vs(
    VertexInfo vi,
    int prim_address,
    RectWithSize local_rect,
    RectWithSize segment_rect,
    ivec4 prim_user_data,
    int segment_user_data,
    mat4 transform,
    PictureTask pic_task,
    int brush_flags,
    vec4 segment_data
);

#define VECS_PER_SEGMENT                    2

#define BRUSH_FLAG_PERSPECTIVE_INTERPOLATION    1
#define BRUSH_FLAG_SEGMENT_RELATIVE             2
#define BRUSH_FLAG_SEGMENT_REPEAT_X             4
#define BRUSH_FLAG_SEGMENT_REPEAT_Y             8
#define BRUSH_FLAG_TEXEL_RECT                  16

#define INVALID_SEGMENT_INDEX                   0xffff

void main(void) {
    // Load the brush instance from vertex attributes.
    int prim_header_address = aData.x;
    int render_task_index = aData.y >> 16;
    int clip_address = aData.y & 0xffff;
    int segment_index = aData.z & 0xffff;
    int edge_flags = (aData.z >> 16) & 0xff;
    int brush_flags = (aData.z >> 24) & 0xff;
    int segment_user_data = aData.w;
    PrimitiveHeader ph = fetch_prim_header(prim_header_address);

    // Fetch the segment of this brush primitive we are drawing.
    vec4 segment_data;
    RectWithSize segment_rect;
    if (segment_index == INVALID_SEGMENT_INDEX) {
        segment_rect = ph.local_rect;
        segment_data = vec4(0.0);
    } else {
        int segment_address = ph.specific_prim_address +
                              VECS_PER_SPECIFIC_BRUSH +
                              segment_index * VECS_PER_SEGMENT;

        vec4[2] segment_info = fetch_from_gpu_cache_2(segment_address);
        segment_rect = RectWithSize(segment_info[0].xy, segment_info[0].zw);
        segment_rect.p0 += ph.local_rect.p0;
        segment_data = segment_info[1];
    }

    VertexInfo vi;

    // Fetch the dynamic picture that we are drawing on.
    PictureTask pic_task = fetch_picture_task(render_task_index);
    ClipArea clip_area = fetch_clip_area(clip_address);

    Transform transform = fetch_transform(ph.transform_id);

    // Write the normal vertex information out.
    if (transform.is_axis_aligned) {
        vi = write_vertex(
            segment_rect,
            ph.local_clip_rect,
            ph.z,
            transform,
            pic_task
        );

        // TODO(gw): transform bounds may be referenced by
        //           the fragment shader when running in
        //           the alpha pass, even on non-transformed
        //           items. For now, just ensure it has no
        //           effect. We can tidy this up as we move
        //           more items to be brush shaders.
#ifdef WR_FEATURE_ALPHA_PASS
        init_transform_vs(vec4(vec2(-1.0e16), vec2(1.0e16)));
#endif
    } else {
        bvec4 edge_mask = notEqual(edge_flags & ivec4(1, 2, 4, 8), ivec4(0));

        vi = write_transform_vertex(
            segment_rect,
            ph.local_rect,
            ph.local_clip_rect,
            mix(vec4(0.0), vec4(1.0), edge_mask),
            ph.z,
            transform,
            pic_task
        );
    }

    // For brush instances in the alpha pass, always write
    // out clip information.
    // TODO(gw): It's possible that we might want alpha
    //           shaders that don't clip in the future,
    //           but it's reasonable to assume that one
    //           implies the other, for now.
#ifdef WR_FEATURE_ALPHA_PASS
    write_clip(
        vi.world_pos,
        vi.snap_offset,
        clip_area
    );
#endif

    // Run the specific brush VS code to write interpolators.
    brush_vs(
        vi,
        ph.specific_prim_address,
        ph.local_rect,
        segment_rect,
        ph.user_data,
        segment_user_data,
        transform.m,
        pic_task,
        brush_flags,
        segment_data
    );
}
#endif

#ifdef WR_FRAGMENT_SHADER

struct Fragment {
    vec4 color;
#ifdef WR_FEATURE_DUAL_SOURCE_BLENDING
    vec4 blend;
#endif
};

Fragment brush_fs();

void main(void) {
#ifdef WR_FEATURE_DEBUG_OVERDRAW
    oFragColor = WR_DEBUG_OVERDRAW_COLOR;
#else
    // Run the specific brush FS code to output the color.
    Fragment frag = brush_fs();

#ifdef WR_FEATURE_ALPHA_PASS
    // Apply the clip mask
    float clip_alpha = do_clip();

    frag.color *= clip_alpha;

    #ifdef WR_FEATURE_DUAL_SOURCE_BLENDING
        oFragBlend = frag.blend * clip_alpha;
    #endif
#endif

    write_output(frag.color);
#endif
}
#endif

flat varying HIGHP_FS_ADDRESS int vGradientAddress;
flat varying float vGradientRepeat;

flat varying vec2 vCenter;
flat varying float vStartRadius;
flat varying float vEndRadius;

varying vec2 vPos;
flat varying vec2 vRepeatedSize;

#ifdef WR_FEATURE_ALPHA_PASS
varying vec2 vLocalPos;
flat varying vec2 vTileRepeat;
#endif

#ifdef WR_VERTEX_SHADER

struct RadialGradient {
    vec4 center_start_end_radius;
    float ratio_xy;
    int extend_mode;
    vec2 stretch_size;
};

RadialGradient fetch_radial_gradient(int address) {
    vec4 data[2] = fetch_from_gpu_cache_2(address);
    return RadialGradient(
        data[0],
        data[1].x,
        int(data[1].y),
        data[1].zw
    );
}

void brush_vs(
    VertexInfo vi,
    int prim_address,
    RectWithSize local_rect,
    RectWithSize segment_rect,
    ivec4 prim_user_data,
    int segment_user_data,
    mat4 transform,
    PictureTask pic_task,
    int brush_flags,
    vec4 texel_rect
) {
    RadialGradient gradient = fetch_radial_gradient(prim_address);

    if ((brush_flags & BRUSH_FLAG_SEGMENT_RELATIVE) != 0) {
        vPos = (vi.local_pos - segment_rect.p0) / segment_rect.size;
        vPos = vPos * (texel_rect.zw - texel_rect.xy) + texel_rect.xy;
        vPos = vPos * local_rect.size;
    } else {
        vPos = vi.local_pos - local_rect.p0;
    }

    vCenter = gradient.center_start_end_radius.xy;
    vStartRadius = gradient.center_start_end_radius.z;
    vEndRadius = gradient.center_start_end_radius.w;

    // Transform all coordinates by the y scale so the
    // fragment shader can work with circles
    vec2 tile_repeat = local_rect.size / gradient.stretch_size;
    vPos.y *= gradient.ratio_xy;
    vCenter.y *= gradient.ratio_xy;
    vRepeatedSize = gradient.stretch_size;
    vRepeatedSize.y *=  gradient.ratio_xy;

    vGradientAddress = prim_user_data.x;

    // Whether to repeat the gradient instead of clamping.
    vGradientRepeat = float(gradient.extend_mode != EXTEND_MODE_CLAMP);

#ifdef WR_FEATURE_ALPHA_PASS
    vTileRepeat = tile_repeat.xy;
    vLocalPos = vi.local_pos;
#endif
}
#endif

#ifdef WR_FRAGMENT_SHADER
Fragment brush_fs() {

#ifdef WR_FEATURE_ALPHA_PASS
    // Handle top and left inflated edges (see brush_image).
    vec2 local_pos = max(vPos, vec2(0.0));

    // Apply potential horizontal and vertical repetitions.
    vec2 pos = mod(local_pos, vRepeatedSize);

    vec2 prim_size = vRepeatedSize * vTileRepeat;
    // Handle bottom and right inflated edges (see brush_image).
    if (local_pos.x >= prim_size.x) {
        pos.x = vRepeatedSize.x;
    }
    if (local_pos.y >= prim_size.y) {
        pos.y = vRepeatedSize.y;
    }
#else
    // Apply potential horizontal and vertical repetitions.
    vec2 pos = mod(vPos, vRepeatedSize);
#endif

    vec2 pd = pos - vCenter;
    float rd = vEndRadius - vStartRadius;

    // Solve for t in length(t - pd) = vStartRadius + t * rd
    // using a quadratic equation in form of At^2 - 2Bt + C = 0
    float A = -(rd * rd);
    float B = vStartRadius * rd;
    float C = dot(pd, pd) - vStartRadius * vStartRadius;

    float offset;
    if (A == 0.0) {
        // Since A is 0, just solve for -2Bt + C = 0
        if (B == 0.0) {
            discard;
        }
        float t = 0.5 * C / B;
        if (vStartRadius + rd * t >= 0.0) {
            offset = t;
        } else {
            discard;
        }
    } else {
        float discr = B * B - A * C;
        if (discr < 0.0) {
            discard;
        }
        discr = sqrt(discr);
        float t0 = (B + discr) / A;
        float t1 = (B - discr) / A;
        if (vStartRadius + rd * t0 >= 0.0) {
            offset = t0;
        } else if (vStartRadius + rd * t1 >= 0.0) {
            offset = t1;
        } else {
            discard;
        }
    }

    vec4 color = sample_gradient(vGradientAddress,
                                 offset,
                                 vGradientRepeat);

#ifdef WR_FEATURE_ALPHA_PASS
    color *= init_transform_fs(vLocalPos);
#endif

    return Fragment(color);
}
#endif
