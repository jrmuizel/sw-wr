#version 300 es















































        precision highp sampler2DArray;


































    precision highp float;


























































































            out vec4 oFragColor;



        void write_output(vec4 color){
            oFragColor = color;
        }







    float distance_to_line(vec2 p0, vec2 perp_dir, vec2 p){
        vec2 dir_to_p0 = p0 - p;
        return dot(normalize(perp_dir), dir_to_p0);
    }



    float compute_aa_range(vec2 position){
















        return 0.35355 * length(fwidth(position));
    }


















    float distance_aa(float aa_range, float signed_distance){
        float dist = 0.5 * signed_distance / aa_range;
        if(dist <= - 0.5 + 0.0001)
            return 1.0;
        if(dist >= 0.5 - 0.0001)
            return 0.0;
        return 0.5 + dist *(0.8431027 * dist * dist - 1.14453603);
    }


































uniform sampler2DArray sColor0;
uniform sampler2DArray sColor1;
uniform sampler2DArray sColor2;

























struct RectWithSize {
    vec2 p0;
    vec2 size;
};

struct RectWithEndpoint {
    vec2 p0;
    vec2 p1;
};

RectWithEndpoint to_rect_with_endpoint(RectWithSize rect){
    RectWithEndpoint result;
    result . p0 = rect . p0;
    result . p1 = rect . p0 + rect . size;

    return result;
}

RectWithSize to_rect_with_size(RectWithEndpoint rect){
    RectWithSize result;
    result . p0 = rect . p0;
    result . size = rect . p1 - rect . p0;

    return result;
}

RectWithSize intersect_rects(RectWithSize a, RectWithSize b){
    RectWithSize result;
    result . p0 = max(a . p0, b . p0);
    result . size = min(a . p0 + a . size, b . p0 + b . size)- result . p0;

    return result;
}

float point_inside_rect(vec2 p, vec2 p0, vec2 p1){
    vec2 s = step(p0, p)- step(p1, p);
    return s . x * s . y;
}


























































































































uniform highp sampler2D sGpuCache;










ivec2 get_gpu_cache_uv(highp int address){
    return ivec2(uint(address)% 1024U,
                 uint(address)/ 1024U);
}

vec4[2]fetch_from_gpu_cache_2_direct(ivec2 address){
    return vec4[2](
                                                      texelFetch(sGpuCache, address + ivec2(0, 0), 0),
                                                      texelFetch(sGpuCache, address + ivec2(1, 0), 0)
    );
}

vec4[2]fetch_from_gpu_cache_2(highp int address){
    ivec2 uv = get_gpu_cache_uv(address);
    return vec4[2](
                                                 texelFetch(sGpuCache, uv + ivec2(0, 0), 0),
                                                 texelFetch(sGpuCache, uv + ivec2(1, 0), 0)
    );
}

vec4 fetch_from_gpu_cache_1_direct(ivec2 address){
    return texelFetch(sGpuCache, address, 0);
}

vec4 fetch_from_gpu_cache_1(highp int address){
    ivec2 uv = get_gpu_cache_uv(address);
    return texelFetch(sGpuCache, uv, 0);
}



































































































flat in vec4 vTransformBounds;


























































































float signed_distance_rect(vec2 pos, vec2 p0, vec2 p1){
    vec2 d = max(p0 - pos, pos - p1);
    return length(max(vec2(0.0), d))+ min(0.0, max(d . x, d . y));
}

float init_transform_fs(vec2 local_pos){

    float d = signed_distance_rect(
        local_pos,
        vTransformBounds . xy,
        vTransformBounds . zw
    );


    float aa_range = compute_aa_range(local_pos);


    return distance_aa(aa_range, d);
}

float init_transform_rough_fs(vec2 local_pos){
    return point_inside_rect(
        local_pos,
        vTransformBounds . xy,
        vTransformBounds . zw
    );
}































































































      in vec4 vLocalPos;
      in vec2 vClipMaskImageUv;

flat in vec4 vClipMaskUvRect;
flat in vec4 vClipMaskUvInnerRect;
flat in float vLayer;










































void main(void){
    vec2 local_pos = vLocalPos . xy / vLocalPos . w;
    float alpha = vLocalPos . w > 0.0 ? init_transform_fs(local_pos): 0.0;


    vec2 clamped_mask_uv = clamp(vClipMaskImageUv, vec2(0.0, 0.0), vLocalPos . ww);



    if(clamped_mask_uv != vClipMaskImageUv)
        discard;

    vec2 source_uv = clamp(
        clamped_mask_uv / vLocalPos . w * vClipMaskUvRect . zw + vClipMaskUvRect . xy,
        vClipMaskUvInnerRect . xy, vClipMaskUvInnerRect . zw);
    float clip_alpha = texture(sColor0, vec3(source_uv, vLayer)). r;
    oFragColor = vec4(alpha * clip_alpha, 1.0, 1.0, 1.0);
}

