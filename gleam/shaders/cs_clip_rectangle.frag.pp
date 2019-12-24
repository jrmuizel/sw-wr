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


















































































































float distance_to_ellipse(vec2 p, vec2 radii, float aa_range){
    float dist;
    if(any(lessThanEqual(radii, vec2(0.0)))){
        dist = length(p);
    } else {
        vec2 invRadiiSq = 1.0 /(radii * radii);
        float g = dot(p * p * invRadiiSq, vec2(1.0))- 1.0;
        vec2 dG = 2.0 * p * invRadiiSq;
        dist = g * inversesqrt(dot(dG, dG));
    }
    return clamp(dist, - aa_range, aa_range);
}

float clip_against_ellipse_if_needed(
    vec2 pos,
    float current_distance,
    vec4 ellipse_center_radius,
    vec2 sign_modifier,
    float aa_range
){
    if(! all(lessThan(sign_modifier * pos, sign_modifier * ellipse_center_radius . xy))){
      return current_distance;
    }

    float distance = distance_to_ellipse(pos - ellipse_center_radius . xy,
                                         ellipse_center_radius . zw,
                                         aa_range);

    return max(distance, current_distance);
}

float rounded_rect(vec2 pos,
                   vec4 clip_center_radius_tl,
                   vec4 clip_center_radius_tr,
                   vec4 clip_center_radius_br,
                   vec4 clip_center_radius_bl,
                   float aa_range){



    float current_distance = - aa_range;


    current_distance = clip_against_ellipse_if_needed(pos,
                                                      current_distance,
                                                      clip_center_radius_tl,
                                                      vec2(1.0),
                                                      aa_range);

    current_distance = clip_against_ellipse_if_needed(pos,
                                                      current_distance,
                                                      clip_center_radius_tr,
                                                      vec2(- 1.0, 1.0),
                                                      aa_range);

    current_distance = clip_against_ellipse_if_needed(pos,
                                                      current_distance,
                                                      clip_center_radius_br,
                                                      vec2(- 1.0),
                                                      aa_range);

    current_distance = clip_against_ellipse_if_needed(pos,
                                                      current_distance,
                                                      clip_center_radius_bl,
                                                      vec2(1.0, - 1.0),
                                                      aa_range);




    return distance_aa(aa_range, current_distance);
}






      in vec4 vLocalPos;
flat in vec4 vClipCenter_Radius_TL;
flat in vec4 vClipCenter_Radius_TR;
flat in vec4 vClipCenter_Radius_BL;
flat in vec4 vClipCenter_Radius_BR;


flat in float vClipMode;














































































































void main(void){



    vec2 local_pos = vLocalPos . xy / vLocalPos . w;

    float aa_range = compute_aa_range(local_pos);







    float alpha = init_transform_fs(local_pos . xy);

    float clip_alpha = rounded_rect(local_pos . xy,
                                    vClipCenter_Radius_TL,
                                    vClipCenter_Radius_TR,
                                    vClipCenter_Radius_BR,
                                    vClipCenter_Radius_BL,
                                    aa_range);

    float combined_alpha = alpha * clip_alpha;


    float final_alpha = mix(combined_alpha, 1.0 - combined_alpha, vClipMode);
    float final_final_alpha = vLocalPos . w > 0.0 ? final_alpha : 0.0;

    oFragColor = vec4(final_final_alpha, 0.0, 0.0, 1.0);

}

