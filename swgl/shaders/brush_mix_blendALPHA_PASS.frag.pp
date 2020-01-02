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




































































































































struct Fragment {
    vec4 color;



};

Fragment brush_fs();

void main(void){




    Fragment frag = brush_fs();



    float clip_alpha = do_clip();

    frag . color *= clip_alpha;






    write_output(frag . color);

}


      in vec3 vSrcUv;
      in vec3 vBackdropUv;
flat in int vOp;







































vec3 Multiply(vec3 Cb, vec3 Cs){
    return Cb * Cs;
}

vec3 Screen(vec3 Cb, vec3 Cs){
    return Cb + Cs -(Cb * Cs);
}

vec3 HardLight(vec3 Cb, vec3 Cs){
    vec3 m = Multiply(Cb, 2.0 * Cs);
    vec3 s = Screen(Cb, 2.0 * Cs - 1.0);
    vec3 edge = vec3(0.5, 0.5, 0.5);
    return mix(m, s, step(edge, Cs));
}


float ColorDodge(float Cb, float Cs){
    if(Cb == 0.0)
        return 0.0;
    else if(Cs == 1.0)
        return 1.0;
    else
        return min(1.0, Cb /(1.0 - Cs));
}


float ColorBurn(float Cb, float Cs){
    if(Cb == 1.0)
        return 1.0;
    else if(Cs == 0.0)
        return 0.0;
    else
        return 1.0 - min(1.0,(1.0 - Cb)/ Cs);
}

float SoftLight(float Cb, float Cs){
    if(Cs <= 0.5){
        return Cb -(1.0 - 2.0 * Cs)* Cb *(1.0 - Cb);
    } else {
        float D;

        if(Cb <= 0.25)
            D =((16.0 * Cb - 12.0)* Cb + 4.0)* Cb;
        else
            D = sqrt(Cb);

        return Cb +(2.0 * Cs - 1.0)*(D - Cb);
    }
}

vec3 Difference(vec3 Cb, vec3 Cs){
    return abs(Cb - Cs);
}

vec3 Exclusion(vec3 Cb, vec3 Cs){
    return Cb + Cs - 2.0 * Cb * Cs;
}




float Sat(vec3 c){
    return max(c . r, max(c . g, c . b))- min(c . r, min(c . g, c . b));
}

float Lum(vec3 c){
    vec3 f = vec3(0.3, 0.59, 0.11);
    return dot(c, f);
}

vec3 ClipColor(vec3 C){
    float L = Lum(C);
    float n = min(C . r, min(C . g, C . b));
    float x = max(C . r, max(C . g, C . b));

    if(n < 0.0)
        C = L +(((C - L)* L)/(L - n));

    if(x > 1.0)
        C = L +(((C - L)*(1.0 - L))/(x - L));

    return C;
}

vec3 SetLum(vec3 C, float l){
    float d = l - Lum(C);
    return ClipColor(C + d);
}

void SetSatInner(inout float Cmin, inout float Cmid, inout float Cmax, float s){
    if(Cmax > Cmin){
        Cmid =(((Cmid - Cmin)* s)/(Cmax - Cmin));
        Cmax = s;
    } else {
        Cmid = 0.0;
        Cmax = 0.0;
    }
    Cmin = 0.0;
}

vec3 SetSat(vec3 C, float s){
    if(C . r <= C . g){
        if(C . g <= C . b){
            SetSatInner(C . r, C . g, C . b, s);
        } else {
            if(C . r <= C . b){
                SetSatInner(C . r, C . b, C . g, s);
            } else {
                SetSatInner(C . b, C . r, C . g, s);
            }
        }
    } else {
        if(C . r <= C . b){
            SetSatInner(C . g, C . r, C . b, s);
        } else {
            if(C . g <= C . b){
                SetSatInner(C . g, C . b, C . r, s);
            } else {
                SetSatInner(C . b, C . g, C . r, s);
            }
        }
    }
    return C;
}

vec3 Hue(vec3 Cb, vec3 Cs){
    return SetLum(SetSat(Cs, Sat(Cb)), Lum(Cb));
}

vec3 Saturation(vec3 Cb, vec3 Cs){
    return SetLum(SetSat(Cb, Sat(Cs)), Lum(Cb));
}

vec3 Color(vec3 Cb, vec3 Cs){
    return SetLum(Cs, Lum(Cb));
}

vec3 Luminosity(vec3 Cb, vec3 Cs){
    return SetLum(Cb, Lum(Cs));
}

const int MixBlendMode_Multiply = 1;
const int MixBlendMode_Screen = 2;
const int MixBlendMode_Overlay = 3;
const int MixBlendMode_Darken = 4;
const int MixBlendMode_Lighten = 5;
const int MixBlendMode_ColorDodge = 6;
const int MixBlendMode_ColorBurn = 7;
const int MixBlendMode_HardLight = 8;
const int MixBlendMode_SoftLight = 9;
const int MixBlendMode_Difference = 10;
const int MixBlendMode_Exclusion = 11;
const int MixBlendMode_Hue = 12;
const int MixBlendMode_Saturation = 13;
const int MixBlendMode_Color = 14;
const int MixBlendMode_Luminosity = 15;

Fragment brush_fs(){
    vec4 Cb = textureLod(sPrevPassColor, vBackdropUv, 0.0);
    vec4 Cs = textureLod(sPrevPassColor, vSrcUv, 0.0);


    if(Cb . a != 0.0){
        Cb . rgb /= Cb . a;
    }

    if(Cs . a != 0.0){
        Cs . rgb /= Cs . a;
    }


    vec4 result = vec4(1.0, 1.0, 0.0, 1.0);

    switch(vOp){
        case MixBlendMode_Multiply :
            result . rgb = Multiply(Cb . rgb, Cs . rgb);
            break;
        case MixBlendMode_Screen :
            result . rgb = Screen(Cb . rgb, Cs . rgb);
            break;
        case MixBlendMode_Overlay :

            result . rgb = HardLight(Cs . rgb, Cb . rgb);
            break;
        case MixBlendMode_Darken :
            result . rgb = min(Cs . rgb, Cb . rgb);
            break;
        case MixBlendMode_Lighten :
            result . rgb = max(Cs . rgb, Cb . rgb);
            break;
        case MixBlendMode_ColorDodge :
            result . r = ColorDodge(Cb . r, Cs . r);
            result . g = ColorDodge(Cb . g, Cs . g);
            result . b = ColorDodge(Cb . b, Cs . b);
            break;
        case MixBlendMode_ColorBurn :
            result . r = ColorBurn(Cb . r, Cs . r);
            result . g = ColorBurn(Cb . g, Cs . g);
            result . b = ColorBurn(Cb . b, Cs . b);
            break;
        case MixBlendMode_HardLight :
            result . rgb = HardLight(Cb . rgb, Cs . rgb);
            break;
        case MixBlendMode_SoftLight :
            result . r = SoftLight(Cb . r, Cs . r);
            result . g = SoftLight(Cb . g, Cs . g);
            result . b = SoftLight(Cb . b, Cs . b);
            break;
        case MixBlendMode_Difference :
            result . rgb = Difference(Cb . rgb, Cs . rgb);
            break;
        case MixBlendMode_Exclusion :
            result . rgb = Exclusion(Cb . rgb, Cs . rgb);
            break;
        case MixBlendMode_Hue :
            result . rgb = Hue(Cb . rgb, Cs . rgb);
            break;
        case MixBlendMode_Saturation :
            result . rgb = Saturation(Cb . rgb, Cs . rgb);
            break;
        case MixBlendMode_Color :
            result . rgb = Color(Cb . rgb, Cs . rgb);
            break;
        case MixBlendMode_Luminosity :
            result . rgb = Luminosity(Cb . rgb, Cs . rgb);
            break;
        default : break;
    }

    result . rgb =(1.0 - Cb . a)* Cs . rgb + Cb . a * result . rgb;
    result . a = Cs . a;

    result . rgb *= result . a;

    return Fragment(result);
}

