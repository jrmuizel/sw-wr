#version 300 es















































        precision highp sampler2DArray;


















































    uniform int uMode;


    uniform mat4 uTransform;


    in vec3 aPosition;








































































































































































uniform sampler2DArray sColor0;
uniform sampler2DArray sColor1;
uniform sampler2DArray sColor2;





















































































































flat out vec4 vColor0;
flat out vec4 vColor1;



flat out vec4 vColorLine;


flat out int vMixColors;




flat out vec4 vClipCenter_Sign;



flat out vec4 vClipRadii;


flat out vec4 vHorizontalClipCenter_Sign;
flat out vec2 vHorizontalClipRadii;
flat out vec4 vVerticalClipCenter_Sign;
flat out vec2 vVerticalClipRadii;


      out vec2 vPos;








in vec2 aTaskOrigin;
in vec4 aRect;
in vec4 aColor0;
in vec4 aColor1;
in int aFlags;
in vec2 aWidths;
in vec2 aRadii;
in vec4 aHorizontallyAdjacentCorner;
in vec4 aVerticallyAdjacentCorner;

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

void main(void){
    int segment = aFlags & 0xff;
    bool do_aa =((aFlags >> 24)& 0xf0)!= 0;

    vec2 outer_scale = get_outer_corner_scale(segment);
    vec2 outer = outer_scale * aRect . zw;
    vec2 clip_sign = 1.0 - 2.0 * outer_scale;

    int mix_colors;
    switch(segment){
        case 0 :
        case 1 :
        case 2 :
        case 3 : {
            mix_colors = do_aa ? 1 : 2;
            break;
        }
        default :
            mix_colors = 0;
            break;
    }

    vMixColors = mix_colors;
    vPos = aRect . zw * aPosition . xy;

    vColor0 = aColor0;
    vColor1 = aColor1;
    vClipCenter_Sign = vec4(outer + clip_sign * aRadii, clip_sign);
    vClipRadii = vec4(aRadii, max(aRadii - aWidths, 0.0));
    vColorLine = vec4(outer, aWidths . y * - clip_sign . y, aWidths . x * clip_sign . x);

    vec2 horizontal_clip_sign = vec2(- clip_sign . x, clip_sign . y);
    vHorizontalClipCenter_Sign = vec4(aHorizontallyAdjacentCorner . xy +
                                      horizontal_clip_sign * aHorizontallyAdjacentCorner . zw,
                                      horizontal_clip_sign);
    vHorizontalClipRadii = aHorizontallyAdjacentCorner . zw;

    vec2 vertical_clip_sign = vec2(clip_sign . x, - clip_sign . y);
    vVerticalClipCenter_Sign = vec4(aVerticallyAdjacentCorner . xy +
                                    vertical_clip_sign * aVerticallyAdjacentCorner . zw,
                                    vertical_clip_sign);
    vVerticalClipRadii = aVerticallyAdjacentCorner . zw;

    gl_Position = uTransform * vec4(aTaskOrigin + aRect . xy + vPos, 0.0, 1.0);
}

