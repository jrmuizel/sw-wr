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














uniform sampler2DArray sPrevPassAlpha;
uniform sampler2DArray sPrevPassColor;

vec2 clamp_rect(vec2 pt, RectWithSize rect){
    return clamp(pt, rect . p0, rect . p0 + rect . size);
}


flat in vec4 vClipMaskUvBounds;

      in vec4 vClipMaskUv;





































































































































































































float do_clip(){

    if(vClipMaskUvBounds . xy == vClipMaskUvBounds . zw){
        return 1.0;
    }


    vec2 mask_uv = vClipMaskUv . xy * gl_FragCoord . w;
    bvec2 left = lessThanEqual(vClipMaskUvBounds . xy, mask_uv);
    bvec2 right = greaterThan(vClipMaskUvBounds . zw, mask_uv);

    if(! all(bvec4(left, right))){
        return 0.0;
    }




    ivec3 tc = ivec3(mask_uv, vClipMaskUv . z + 0.5);
    return texelFetch(sPrevPassAlpha, tc, 0). r;
}












vec4 dither(vec4 color){
    return color;
}


vec4 sample_gradient(highp int address, float offset, float gradient_repeat){

    float x = mix(offset, fract(offset), gradient_repeat);









    const int GRADIENT_ENTRIES = 128;
    x = 1.0 + x * float(GRADIENT_ENTRIES);




    int lut_offset = 2 * int(floor(x));


    lut_offset = clamp(lut_offset, 0, 2 *(GRADIENT_ENTRIES + 1));


    vec4 texels[2]= fetch_from_gpu_cache_2(address + lut_offset);


    return dither(mix(texels[0], texels[1], fract(x)));
}



flat in vec4 vColor;
      in vec3 vUv;
flat in vec4 vUvBorder;
flat in vec2 vMaskSwizzle;


      in vec4 vUvClip;































































































































































































































































































void main(void){
    vec3 tc = vec3(clamp(vUv . xy, vUvBorder . xy, vUvBorder . zw), vUv . z);
    vec4 mask = texture(sColor0, tc);
    mask . rgb = mask . rgb * vMaskSwizzle . x + mask . aaa * vMaskSwizzle . y;

    float alpha = do_clip();

    alpha *= float(all(greaterThanEqual(vUvClip, vec4(0.0))));









    write_output(vColor * mask * alpha);

}

