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



      in vec3 vInput1Uv;
      in vec3 vInput2Uv;
flat in vec4 vInput1UvRect;
flat in vec4 vInput2UvRect;
flat in int vFilterInputCount;
flat in int vFilterKind;
flat in ivec4 vData;
flat in vec4 vFilterData0;
flat in vec4 vFilterData1;
flat in float vFloat0;
flat in mat3 vColorMat;
flat in int vFuncs[4];




























































































































































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

const int BlendMode_Normal = 0;
const int BlendMode_Multiply = 1;
const int BlendMode_Screen = 2;
const int BlendMode_Overlay = 3;
const int BlendMode_Darken = 4;
const int BlendMode_Lighten = 5;
const int BlendMode_ColorDodge = 6;
const int BlendMode_ColorBurn = 7;
const int BlendMode_HardLight = 8;
const int BlendMode_SoftLight = 9;
const int BlendMode_Difference = 10;
const int BlendMode_Exclusion = 11;
const int BlendMode_Hue = 12;
const int BlendMode_Saturation = 13;
const int BlendMode_Color = 14;
const int BlendMode_Luminosity = 15;

vec4 blend(vec4 Cs, vec4 Cb, int mode){
    vec4 result = vec4(1.0, 0.0, 0.0, 1.0);

    switch(mode){
        case BlendMode_Normal :
            result . rgb = Cs . rgb;
            break;
        case BlendMode_Multiply :
            result . rgb = Multiply(Cb . rgb, Cs . rgb);
            break;
        case BlendMode_Screen :
            result . rgb = Screen(Cb . rgb, Cs . rgb);
            break;
        case BlendMode_Overlay :

            result . rgb = HardLight(Cs . rgb, Cb . rgb);
            break;
        case BlendMode_Darken :
            result . rgb = min(Cs . rgb, Cb . rgb);
            break;
        case BlendMode_Lighten :
            result . rgb = max(Cs . rgb, Cb . rgb);
            break;
        case BlendMode_ColorDodge :
            result . r = ColorDodge(Cb . r, Cs . r);
            result . g = ColorDodge(Cb . g, Cs . g);
            result . b = ColorDodge(Cb . b, Cs . b);
            break;
        case BlendMode_ColorBurn :
            result . r = ColorBurn(Cb . r, Cs . r);
            result . g = ColorBurn(Cb . g, Cs . g);
            result . b = ColorBurn(Cb . b, Cs . b);
            break;
        case BlendMode_HardLight :
            result . rgb = HardLight(Cb . rgb, Cs . rgb);
            break;
        case BlendMode_SoftLight :
            result . r = SoftLight(Cb . r, Cs . r);
            result . g = SoftLight(Cb . g, Cs . g);
            result . b = SoftLight(Cb . b, Cs . b);
            break;
        case BlendMode_Difference :
            result . rgb = Difference(Cb . rgb, Cs . rgb);
            break;
        case BlendMode_Exclusion :
            result . rgb = Exclusion(Cb . rgb, Cs . rgb);
            break;
        case BlendMode_Hue :
            result . rgb = Hue(Cb . rgb, Cs . rgb);
            break;
        case BlendMode_Saturation :
            result . rgb = Saturation(Cb . rgb, Cs . rgb);
            break;
        case BlendMode_Color :
            result . rgb = Color(Cb . rgb, Cs . rgb);
            break;
        case BlendMode_Luminosity :
            result . rgb = Luminosity(Cb . rgb, Cs . rgb);
            break;
        default : break;
    }
    vec3 rgb =(1.0 - Cb . a)* Cs . rgb + Cb . a * result . rgb;
    result = mix(vec4(Cb . rgb * Cb . a, Cb . a), vec4(rgb, 1.0), Cs . a);
    return result;
}






vec3 SrgbToLinear(vec3 color){
    vec3 c1 = color / 12.92;
    vec3 c2 = pow(color / 1.055 + vec3(0.055 / 1.055), vec3(2.4));
    return mix(c2, c1, lessThanEqual(color, vec3(0.04045)));
}

vec3 LinearToSrgb(vec3 color){
    vec3 c1 = color * 12.92;
    vec3 c2 = vec3(1.055)* pow(color, vec3(1.0 / 2.4))- vec3(0.055);
    return mix(c2, c1, lessThanEqual(color, vec3(0.0031308)));
}




vec4 ComponentTransfer(vec4 colora){











    int offset = 0;
    vec4 texel;
    int k;

    for(int i = 0;i < 4;i ++){
        switch(vFuncs[i]){
            case 0 :
                break;
            case 1 :
            case 2 :

                k = int(floor(colora[i]* 255.0));
                texel = fetch_from_gpu_cache_1_direct(vData . xy + ivec2(offset + k / 4, 0));
                colora[i]= clamp(texel[k % 4], 0.0, 1.0);

                offset = offset + 64;
                break;
            case 3 :

                texel = fetch_from_gpu_cache_1_direct(vData . xy + ivec2(offset, 0));
                colora[i]= clamp(texel[0]* colora[i]+ texel[1], 0.0, 1.0);

                offset = offset + 1;
                break;
            case 4 :

                texel = fetch_from_gpu_cache_1_direct(vData . xy + ivec2(offset, 0));
                colora[i]= clamp(texel[0]* pow(colora[i], texel[1])+ texel[2], 0.0, 1.0);

                offset = offset + 1;
                break;
            default :

                break;
        }
    }
    return colora;
}



vec4 composite(vec4 Cs, vec4 Cb, int mode){
    vec4 Cr = vec4(0.0, 1.0, 0.0, 1.0);
    switch(mode){
        case 0 :
            Cr . rgb = Cs . a * Cs . rgb + Cb . a * Cb . rgb *(1.0 - Cs . a);
            Cr . a = Cs . a + Cb . a *(1.0 - Cs . a);
            break;
        case 1 :
            Cr . rgb = Cs . a * Cs . rgb * Cb . a;
            Cr . a = Cs . a * Cb . a;
            break;
        case 2 :
            Cr . rgb = Cs . a * Cs . rgb *(1.0 - Cb . a);
            Cr . a = Cs . a *(1.0 - Cb . a);
            break;
        case 3 :
            Cr . rgb = Cs . a * Cs . rgb * Cb . a + Cb . a * Cb . rgb *(1.0 - Cs . a);
            Cr . a = Cs . a * Cb . a + Cb . a *(1.0 - Cs . a);
            break;
        case 4 :
            Cr . rgb = Cs . a * Cs . rgb *(1.0 - Cb . a)+ Cb . a * Cb . rgb *(1.0 - Cs . a);
            Cr . a = Cs . a *(1.0 - Cb . a)+ Cb . a *(1.0 - Cs . a);
            break;
        case 5 :
            Cr . rgb = Cs . a * Cs . rgb + Cb . a * Cb . rgb;
            Cr . a = Cs . a + Cb . a;
            Cr = clamp(Cr, vec4(0.0), vec4(1.0));
            break;
        case 6 :
            Cr = vec4(vFilterData0 . x)* Cs * Cb + vec4(vFilterData0 . y)* Cs + vec4(vFilterData0 . z)* Cb + vec4(vFilterData0 . w);
            Cr = clamp(Cr, vec4(0.0), vec4(1.0));
            break;
        default :
            break;
    }
    return Cr;
}

vec4 sampleInUvRect(sampler2DArray sampler, vec3 uv, vec4 uvRect){
    vec2 clamped = clamp(uv . xy, uvRect . xy, uvRect . zw);
    return texture(sampler, vec3(clamped, uv . z), 0.0);
}

void main(void){
    vec4 Ca = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 Cb = vec4(0.0, 0.0, 0.0, 0.0);
    if(vFilterInputCount > 0){
        Ca = sampleInUvRect(sColor0, vInput1Uv, vInput1UvRect);
        if(Ca . a != 0.0){
            Ca . rgb /= Ca . a;
        }
    }
    if(vFilterInputCount > 1){
        Cb = sampleInUvRect(sColor1, vInput2Uv, vInput2UvRect);
        if(Cb . a != 0.0){
            Cb . rgb /= Cb . a;
        }
    }

    vec4 result = vec4(1.0, 0.0, 0.0, 1.0);

    bool needsPremul = true;

    switch(vFilterKind){
        case 0 :
            result = blend(Ca, Cb, vData . x);
            needsPremul = false;
            break;
        case 1 :
            result = vFilterData0;
            needsPremul = false;
            break;
        case 2 :
            result . rgb = LinearToSrgb(Ca . rgb);
            result . a = Ca . a;
            break;
        case 3 :
            result . rgb = SrgbToLinear(Ca . rgb);
            result . a = Ca . a;
            break;
        case 4 :
            result . rgb = Ca . rgb;
            result . a = Ca . a * vFloat0;
            break;
        case 5 :
            result . rgb = vColorMat * Ca . rgb + vFilterData0 . rgb;
            result . a = Ca . a;
            break;
        case 6 :
            vec4 shadow = vec4(vFilterData0 . rgb, Cb . a * vFilterData0 . a);

            result = blend(Ca, shadow, BlendMode_Normal);
            needsPremul = false;
            break;
        case 7 :
            vec2 offsetUv = vInput1Uv . xy + vFilterData0 . xy;
            result = sampleInUvRect(sColor0, vec3(offsetUv, vInput1Uv . z), vInput1UvRect);
            result *= point_inside_rect(offsetUv, vFilterData1 . xy, vFilterData1 . zw);
            needsPremul = false;
            break;
        case 8 :
            result = ComponentTransfer(Ca);
            break;
        case 9 :
            result = Ca;
            break;
        case 10 :
            result = composite(Ca, Cb, vData . x);
            needsPremul = false;
        default :
            break;
    }

    if(needsPremul){
        result . rgb *= result . a;
    }

    oFragColor = result;
}

