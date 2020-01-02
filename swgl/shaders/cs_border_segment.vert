#version 300 es
// cs_border_segment
#define WR_VERTEX_SHADER
#define WR_MAX_VERTEX_TEXTURE_WIDTH 1024U
#define WR_FEATURE_
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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

#ifdef WR_FRAGMENT_SHADER

// One iteration of Newton's method on the 2D equation of an ellipse:
//
//     E(x, y) = x^2/a^2 + y^2/b^2 - 1
//
// The Jacobian of this equation is:
//
//     J(E(x, y)) = [ 2*x/a^2 2*y/b^2 ]
//
// We approximate the distance with:
//
//     E(x, y) / ||J(E(x, y))||
//
// See G. Taubin, "Distance Approximations for Rasterizing Implicit
// Curves", section 3.
float distance_to_ellipse(vec2 p, vec2 radii, float aa_range) {
    float dist;
    if (any(lessThanEqual(radii, vec2(0.0)))) {
        dist = length(p);
    } else {
        vec2 invRadiiSq = 1.0 / (radii * radii);
        float g = dot(p * p * invRadiiSq, vec2(1.0)) - 1.0;
        vec2 dG = 2.0 * p * invRadiiSq;
        dist = g * inversesqrt(dot(dG, dG));
    }
    return clamp(dist, -aa_range, aa_range);
}

float clip_against_ellipse_if_needed(
    vec2 pos,
    float current_distance,
    vec4 ellipse_center_radius,
    vec2 sign_modifier,
    float aa_range
) {
    if (!all(lessThan(sign_modifier * pos, sign_modifier * ellipse_center_radius.xy))) {
      return current_distance;
    }

    float distance = distance_to_ellipse(pos - ellipse_center_radius.xy,
                                         ellipse_center_radius.zw,
                                         aa_range);

    return max(distance, current_distance);
}

float rounded_rect(vec2 pos,
                   vec4 clip_center_radius_tl,
                   vec4 clip_center_radius_tr,
                   vec4 clip_center_radius_br,
                   vec4 clip_center_radius_bl,
                   float aa_range) {
    // Start with a negative value (means "inside") for all fragments that are not
    // in a corner. If the fragment is in a corner, one of the clip_against_ellipse_if_needed
    // calls below will update it.
    float current_distance = -aa_range;

    // Clip against each ellipse.
    current_distance = clip_against_ellipse_if_needed(pos,
                                                      current_distance,
                                                      clip_center_radius_tl,
                                                      vec2(1.0),
                                                      aa_range);

    current_distance = clip_against_ellipse_if_needed(pos,
                                                      current_distance,
                                                      clip_center_radius_tr,
                                                      vec2(-1.0, 1.0),
                                                      aa_range);

    current_distance = clip_against_ellipse_if_needed(pos,
                                                      current_distance,
                                                      clip_center_radius_br,
                                                      vec2(-1.0),
                                                      aa_range);

    current_distance = clip_against_ellipse_if_needed(pos,
                                                      current_distance,
                                                      clip_center_radius_bl,
                                                      vec2(1.0, -1.0),
                                                      aa_range);

    // Apply AA
    // See comment in ps_border_corner about the choice of constants.

    return distance_aa(aa_range, current_distance);
}
#endif

// For edges, the colors are the same. For corners, these
// are the colors of each edge making up the corner.
flat varying vec4 vColor00;
flat varying vec4 vColor01;
flat varying vec4 vColor10;
flat varying vec4 vColor11;

// A point + tangent defining the line where the edge
// transition occurs. Used for corners only.
flat varying vec4 vColorLine;

// x = segment, y = styles, z = edge axes, w = clip mode
flat varying ivec4 vConfig;

// xy = Local space position of the clip center.
// zw = Scale the rect origin by this to get the outer
// corner from the segment rectangle.
flat varying vec4 vClipCenter_Sign;

// An outer and inner elliptical radii for border
// corner clipping.
flat varying vec4 vClipRadii;

// Reference point for determine edge clip lines.
flat varying vec4 vEdgeReference;

// Stores widths/2 and widths/3 to save doing this in FS.
flat varying vec4 vPartialWidths;

// Clipping parameters for dot or dash.
flat varying vec4 vClipParams1;
flat varying vec4 vClipParams2;

// Local space position
varying vec2 vPos;

#define SEGMENT_TOP_LEFT        0
#define SEGMENT_TOP_RIGHT       1
#define SEGMENT_BOTTOM_RIGHT    2
#define SEGMENT_BOTTOM_LEFT     3
#define SEGMENT_LEFT            4
#define SEGMENT_TOP             5
#define SEGMENT_RIGHT           6
#define SEGMENT_BOTTOM          7

// Border styles as defined in webrender_api/types.rs
#define BORDER_STYLE_NONE         0
#define BORDER_STYLE_SOLID        1
#define BORDER_STYLE_DOUBLE       2
#define BORDER_STYLE_DOTTED       3
#define BORDER_STYLE_DASHED       4
#define BORDER_STYLE_HIDDEN       5
#define BORDER_STYLE_GROOVE       6
#define BORDER_STYLE_RIDGE        7
#define BORDER_STYLE_INSET        8
#define BORDER_STYLE_OUTSET       9

#define CLIP_NONE        0
#define CLIP_DASH_CORNER 1
#define CLIP_DASH_EDGE   2
#define CLIP_DOT         3

#ifdef WR_VERTEX_SHADER

in vec2 aTaskOrigin;
in vec4 aRect;
in vec4 aColor0;
in vec4 aColor1;
in int aFlags;
in vec2 aWidths;
in vec2 aRadii;
in vec4 aClipParams1;
in vec4 aClipParams2;

vec2 get_outer_corner_scale(int segment) {
    vec2 p;

    switch (segment) {
        case SEGMENT_TOP_LEFT:
            p = vec2(0.0, 0.0);
            break;
        case SEGMENT_TOP_RIGHT:
            p = vec2(1.0, 0.0);
            break;
        case SEGMENT_BOTTOM_RIGHT:
            p = vec2(1.0, 1.0);
            break;
        case SEGMENT_BOTTOM_LEFT:
            p = vec2(0.0, 1.0);
            break;
        default:
            // The result is only used for non-default segment cases
            p = vec2(0.0);
            break;
    }

    return p;
}

// NOTE(emilio): If you change this algorithm, do the same change
// in border.rs
vec4 mod_color(vec4 color, bool is_black, bool lighter) {
    const float light_black = 0.7;
    const float dark_black = 0.3;

    const float dark_scale = 0.66666666;
    const float light_scale = 1.0;

    if (is_black) {
        if (lighter) {
            return vec4(vec3(light_black), color.a);
        }
        return vec4(vec3(dark_black), color.a);
    }

    if (lighter) {
        return vec4(color.rgb * light_scale, color.a);
    }
    return vec4(color.rgb * dark_scale, color.a);
}

vec4[2] get_colors_for_side(vec4 color, int style) {
    vec4 result[2];

    bool is_black = color.rgb == vec3(0.0, 0.0, 0.0);

    switch (style) {
        case BORDER_STYLE_GROOVE:
            result[0] = mod_color(color, is_black, true);
            result[1] = mod_color(color, is_black, false);
            break;
        case BORDER_STYLE_RIDGE:
            result[0] = mod_color(color, is_black, false);
            result[1] = mod_color(color, is_black, true);
            break;
        default:
            result[0] = color;
            result[1] = color;
            break;
    }

    return result;
}

void main(void) {
    int segment = aFlags & 0xff;
    int style0 = (aFlags >> 8) & 0xff;
    int style1 = (aFlags >> 16) & 0xff;
    int clip_mode = (aFlags >> 24) & 0x0f;

    vec2 outer_scale = get_outer_corner_scale(segment);
    vec2 outer = outer_scale * aRect.zw;
    vec2 clip_sign = 1.0 - 2.0 * outer_scale;

    // Set some flags used by the FS to determine the
    // orientation of the two edges in this corner.
    ivec2 edge_axis = ivec2(0, 0);
    // Derive the positions for the edge clips, which must be handled
    // differently between corners and edges.
    vec2 edge_reference = vec2(0.0);
    switch (segment) {
        case SEGMENT_TOP_LEFT:
            edge_axis = ivec2(0, 1);
            edge_reference = outer;
            break;
        case SEGMENT_TOP_RIGHT:
            edge_axis = ivec2(1, 0);
            edge_reference = vec2(outer.x - aWidths.x, outer.y);
            break;
        case SEGMENT_BOTTOM_RIGHT:
            edge_axis = ivec2(0, 1);
            edge_reference = outer - aWidths;
            break;
        case SEGMENT_BOTTOM_LEFT:
            edge_axis = ivec2(1, 0);
            edge_reference = vec2(outer.x, outer.y - aWidths.y);
            break;
        case SEGMENT_TOP:
        case SEGMENT_BOTTOM:
            edge_axis = ivec2(1, 1);
            break;
        case SEGMENT_LEFT:
        case SEGMENT_RIGHT:
        default:
            break;
    }

    vConfig = ivec4(
        segment,
        style0 | (style1 << 16),
        edge_axis.x | (edge_axis.y << 16),
        clip_mode
    );
    vPartialWidths = vec4(aWidths / 3.0, aWidths / 2.0);
    vPos = aRect.zw * aPosition.xy;

    vec4[2] color0 = get_colors_for_side(aColor0, style0);
    vColor00 = color0[0];
    vColor01 = color0[1];
    vec4[2] color1 = get_colors_for_side(aColor1, style1);
    vColor10 = color1[0];
    vColor11 = color1[1];
    vClipCenter_Sign = vec4(outer + clip_sign * aRadii, clip_sign);
    vClipRadii = vec4(aRadii, max(aRadii - aWidths, 0.0));
    vColorLine = vec4(outer, aWidths.y * -clip_sign.y, aWidths.x * clip_sign.x);
    vEdgeReference = vec4(edge_reference, edge_reference + aWidths);
    vClipParams1 = aClipParams1;
    vClipParams2 = aClipParams2;

    // For the case of dot and dash clips, optimize the number of pixels that
    // are hit to just include the dot itself.
    if (clip_mode == CLIP_DOT) {
        float radius = aClipParams1.z;

        // Expand by a small amount to allow room for AA around
        // the dot if it's big enough.
        if (radius > 0.5)
            radius += 2.0;

        vPos = vClipParams1.xy + radius * (2.0 * aPosition.xy - 1.0);
        vPos = clamp(vPos, vec2(0.0), aRect.zw);
    } else if (clip_mode == CLIP_DASH_CORNER) {
        vec2 center = (aClipParams1.xy + aClipParams2.xy) * 0.5;
        // This is a gross approximation which works out because dashes don't have
        // a strong curvature and we will overshoot by inflating the geometry by
        // this amount on each side (sqrt(2) * length(dash) would be enough and we
        // compute 2 * approx_length(dash)).
        float dash_length = length(aClipParams1.xy - aClipParams2.xy);
        float width = max(aWidths.x, aWidths.y);
        // expand by a small amout for AA just like we do for dots.
        vec2 r = vec2(max(dash_length, width)) + 2.0;
        vPos = clamp(vPos, center - r, center + r);
    }

    gl_Position = uTransform * vec4(aTaskOrigin + aRect.xy + vPos, 0.0, 1.0);
}
#endif

#ifdef WR_FRAGMENT_SHADER
vec4 evaluate_color_for_style_in_corner(
    vec2 clip_relative_pos,
    int style,
    vec4 color0,
    vec4 color1,
    vec4 clip_radii,
    float mix_factor,
    int segment,
    float aa_range
) {
    switch (style) {
        case BORDER_STYLE_DOUBLE: {
            // Get the distances from 0.33 of the radii, and
            // also 0.67 of the radii. Use these to form a
            // SDF subtraction which will clip out the inside
            // third of the rounded edge.
            float d_radii_a = distance_to_ellipse(
                clip_relative_pos,
                clip_radii.xy - vPartialWidths.xy,
                aa_range
            );
            float d_radii_b = distance_to_ellipse(
                clip_relative_pos,
                clip_radii.xy - 2.0 * vPartialWidths.xy,
                aa_range
            );
            float d = min(-d_radii_a, d_radii_b);
            color0 *= distance_aa(aa_range, d);
            break;
        }
        case BORDER_STYLE_GROOVE:
        case BORDER_STYLE_RIDGE: {
            float d = distance_to_ellipse(
                clip_relative_pos,
                clip_radii.xy - vPartialWidths.zw,
                aa_range
            );
            float alpha = distance_aa(aa_range, d);
            float swizzled_factor;
            switch (segment) {
                case SEGMENT_TOP_LEFT: swizzled_factor = 0.0; break;
                case SEGMENT_TOP_RIGHT: swizzled_factor = mix_factor; break;
                case SEGMENT_BOTTOM_RIGHT: swizzled_factor = 1.0; break;
                case SEGMENT_BOTTOM_LEFT: swizzled_factor = 1.0 - mix_factor; break;
                default: swizzled_factor = 0.0; break;
            };
            vec4 c0 = mix(color1, color0, swizzled_factor);
            vec4 c1 = mix(color0, color1, swizzled_factor);
            color0 = mix(c0, c1, alpha);
            break;
        }
        default:
            break;
    }

    return color0;
}

vec4 evaluate_color_for_style_in_edge(
    vec2 pos_vec,
    int style,
    vec4 color0,
    vec4 color1,
    float aa_range,
    int edge_axis_id
) {
    vec2 edge_axis = edge_axis_id != 0 ? vec2(0.0, 1.0) : vec2(1.0, 0.0);
    float pos = dot(pos_vec, edge_axis);
    switch (style) {
        case BORDER_STYLE_DOUBLE: {
            float d = -1.0;
            float partial_width = dot(vPartialWidths.xy, edge_axis);
            if (partial_width >= 1.0) {
                vec2 ref = vec2(
                    dot(vEdgeReference.xy, edge_axis) + partial_width,
                    dot(vEdgeReference.zw, edge_axis) - partial_width
                );
                d = min(pos - ref.x, ref.y - pos);
            }
            color0 *= distance_aa(aa_range, d);
            break;
        }
        case BORDER_STYLE_GROOVE:
        case BORDER_STYLE_RIDGE: {
            float ref = dot(vEdgeReference.xy + vPartialWidths.zw, edge_axis);
            float d = pos - ref;
            float alpha = distance_aa(aa_range, d);
            color0 = mix(color0, color1, alpha);
            break;
        }
        default:
            break;
    }

    return color0;
}

void main(void) {
    float aa_range = compute_aa_range(vPos);
    vec4 color0, color1;

    int segment = vConfig.x;
    ivec2 style = ivec2(vConfig.y & 0xffff, vConfig.y >> 16);
    ivec2 edge_axis = ivec2(vConfig.z & 0xffff, vConfig.z >> 16);
    int clip_mode = vConfig.w;

    float mix_factor = 0.0;
    if (edge_axis.x != edge_axis.y) {
        float d_line = distance_to_line(vColorLine.xy, vColorLine.zw, vPos);
        mix_factor = distance_aa(aa_range, -d_line);
    }

    // Check if inside corner clip-region
    vec2 clip_relative_pos = vPos - vClipCenter_Sign.xy;
    bool in_clip_region = all(lessThan(vClipCenter_Sign.zw * clip_relative_pos, vec2(0.0)));
    float d = -1.0;

    switch (clip_mode) {
        case CLIP_DOT: {
            // Set clip distance based or dot position and radius.
            d = distance(vClipParams1.xy, vPos) - vClipParams1.z;
            break;
        }
        case CLIP_DASH_EDGE: {
            bool is_vertical = vClipParams1.x == 0.;
            float half_dash = is_vertical ? vClipParams1.y : vClipParams1.x;
            // We want to draw something like:
            // +---+---+---+---+
            // |xxx|   |   |xxx|
            // +---+---+---+---+
            float pos = is_vertical ? vPos.y : vPos.x;
            bool in_dash = pos < half_dash || pos > 3.0 * half_dash;
            if (!in_dash) {
                d = 1.;
            }
            break;
        }
        case CLIP_DASH_CORNER: {
            // Get SDF for the two line/tangent clip lines,
            // do SDF subtract to get clip distance.
            float d0 = distance_to_line(vClipParams1.xy,
                                        vClipParams1.zw,
                                        vPos);
            float d1 = distance_to_line(vClipParams2.xy,
                                        vClipParams2.zw,
                                        vPos);
            d = max(d0, -d1);
            break;
        }
        case CLIP_NONE:
        default:
            break;
    }

    if (in_clip_region) {
        float d_radii_a = distance_to_ellipse(clip_relative_pos, vClipRadii.xy, aa_range);
        float d_radii_b = distance_to_ellipse(clip_relative_pos, vClipRadii.zw, aa_range);
        float d_radii = max(d_radii_a, -d_radii_b);
        d = max(d, d_radii);

        color0 = evaluate_color_for_style_in_corner(
            clip_relative_pos,
            style.x,
            vColor00,
            vColor01,
            vClipRadii,
            mix_factor,
            segment,
            aa_range
        );
        color1 = evaluate_color_for_style_in_corner(
            clip_relative_pos,
            style.y,
            vColor10,
            vColor11,
            vClipRadii,
            mix_factor,
            segment,
            aa_range
        );
    } else {
        color0 = evaluate_color_for_style_in_edge(
            vPos,
            style.x,
            vColor00,
            vColor01,
            aa_range,
            edge_axis.x
        );
        color1 = evaluate_color_for_style_in_edge(
            vPos,
            style.y,
            vColor10,
            vColor11,
            aa_range,
            edge_axis.y
        );
    }

    float alpha = distance_aa(aa_range, d);
    vec4 color = mix(color0, color1, mix_factor);
    oFragColor = color * alpha;
}
#endif
