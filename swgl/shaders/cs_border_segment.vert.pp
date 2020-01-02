#version 300 es















































        precision highp sampler2DArray;


















































    uniform int uMode;


    uniform mat4 uTransform;


    in vec3 aPosition;








































































































































































uniform sampler2DArray sColor0;
uniform sampler2DArray sColor1;
uniform sampler2DArray sColor2;

















































































































flat out vec4 vColor00;
flat out vec4 vColor01;
flat out vec4 vColor10;
flat out vec4 vColor11;



flat out vec4 vColorLine;


flat out ivec4 vConfig;




flat out vec4 vClipCenter_Sign;



flat out vec4 vClipRadii;


flat out vec4 vEdgeReference;


flat out vec4 vPartialWidths;


flat out vec4 vClipParams1;
flat out vec4 vClipParams2;


      out vec2 vPos;





























in vec2 aTaskOrigin;
in vec4 aRect;
in vec4 aColor0;
in vec4 aColor1;
in int aFlags;
in vec2 aWidths;
in vec2 aRadii;
in vec4 aClipParams1;
in vec4 aClipParams2;

vec2 get_outer_corner_scale(int segment){
    vec2 p;

    switch(segment){
        case 0 :
            p = vec2(0.0, 0.0);
            break;
        case 1 :
            p = vec2(1.0, 0.0);
            break;
        case 2 :
            p = vec2(1.0, 1.0);
            break;
        case 3 :
            p = vec2(0.0, 1.0);
            break;
        default :

            p = vec2(0.0);
            break;
    }

    return p;
}



vec4 mod_color(vec4 color, bool is_black, bool lighter){
    const float light_black = 0.7;
    const float dark_black = 0.3;

    const float dark_scale = 0.66666666;
    const float light_scale = 1.0;

    if(is_black){
        if(lighter){
            return vec4(vec3(light_black), color . a);
        }
        return vec4(vec3(dark_black), color . a);
    }

    if(lighter){
        return vec4(color . rgb * light_scale, color . a);
    }
    return vec4(color . rgb * dark_scale, color . a);
}

vec4[2]get_colors_for_side(vec4 color, int style){
    vec4 result[2];

    bool is_black = color . rgb == vec3(0.0, 0.0, 0.0);

    switch(style){
        case 6 :
            result[0]= mod_color(color, is_black, true);
            result[1]= mod_color(color, is_black, false);
            break;
        case 7 :
            result[0]= mod_color(color, is_black, false);
            result[1]= mod_color(color, is_black, true);
            break;
        default :
            result[0]= color;
            result[1]= color;
            break;
    }

    return result;
}

void main(void){
    int segment = aFlags & 0xff;
    int style0 =(aFlags >> 8)& 0xff;
    int style1 =(aFlags >> 16)& 0xff;
    int clip_mode =(aFlags >> 24)& 0x0f;

    vec2 outer_scale = get_outer_corner_scale(segment);
    vec2 outer = outer_scale * aRect . zw;
    vec2 clip_sign = 1.0 - 2.0 * outer_scale;



    ivec2 edge_axis = ivec2(0, 0);


    vec2 edge_reference = vec2(0.0);
    switch(segment){
        case 0 :
            edge_axis = ivec2(0, 1);
            edge_reference = outer;
            break;
        case 1 :
            edge_axis = ivec2(1, 0);
            edge_reference = vec2(outer . x - aWidths . x, outer . y);
            break;
        case 2 :
            edge_axis = ivec2(0, 1);
            edge_reference = outer - aWidths;
            break;
        case 3 :
            edge_axis = ivec2(1, 0);
            edge_reference = vec2(outer . x, outer . y - aWidths . y);
            break;
        case 5 :
        case 7 :
            edge_axis = ivec2(1, 1);
            break;
        case 4 :
        case 6 :
        default :
            break;
    }

    vConfig = ivec4(
        segment,
        style0 |(style1 << 16),
        edge_axis . x |(edge_axis . y << 16),
        clip_mode
    );
    vPartialWidths = vec4(aWidths / 3.0, aWidths / 2.0);
    vPos = aRect . zw * aPosition . xy;

    vec4[2]color0 = get_colors_for_side(aColor0, style0);
    vColor00 = color0[0];
    vColor01 = color0[1];
    vec4[2]color1 = get_colors_for_side(aColor1, style1);
    vColor10 = color1[0];
    vColor11 = color1[1];
    vClipCenter_Sign = vec4(outer + clip_sign * aRadii, clip_sign);
    vClipRadii = vec4(aRadii, max(aRadii - aWidths, 0.0));
    vColorLine = vec4(outer, aWidths . y * - clip_sign . y, aWidths . x * clip_sign . x);
    vEdgeReference = vec4(edge_reference, edge_reference + aWidths);
    vClipParams1 = aClipParams1;
    vClipParams2 = aClipParams2;



    if(clip_mode == 3){
        float radius = aClipParams1 . z;



        if(radius > 0.5)
            radius += 2.0;

        vPos = vClipParams1 . xy + radius *(2.0 * aPosition . xy - 1.0);
        vPos = clamp(vPos, vec2(0.0), aRect . zw);
    } else if(clip_mode == 1){
        vec2 center =(aClipParams1 . xy + aClipParams2 . xy)* 0.5;




        float dash_length = length(aClipParams1 . xy - aClipParams2 . xy);
        float width = max(aWidths . x, aWidths . y);

        vec2 r = vec2(max(dash_length, width))+ 2.0;
        vPos = clamp(vPos, center - r, center + r);
    }

    gl_Position = uTransform * vec4(aTaskOrigin + aRect . xy + vPos, 0.0, 1.0);
}

