#version 300 es















































        precision highp sampler2DArray;


















































    uniform int uMode;


    uniform mat4 uTransform;


    in vec3 aPosition;








































































































































































uniform sampler2DArray sColor0;
uniform sampler2DArray sColor1;
uniform sampler2DArray sColor2;
























      out vec2 vLocalPos;

flat out float vAxisSelect;
flat out int vStyle;
flat out vec4 vParams;






in vec4 aTaskRect;
in vec2 aLocalSize;
in int aStyle;
in int aOrientation;
in float aWavyLineThickness;

void main(void){
    vec2 size;

    switch(aOrientation){
        case 1 :
            vAxisSelect = 0.0;
            size = aLocalSize;
            break;
        case 0 :
            vAxisSelect = 1.0;
            size = aLocalSize . yx;
            break;
        default :
            vAxisSelect = 0.0;
            size = vec2(0.0);
    }

    vStyle = aStyle;

    switch(vStyle){
        case 0 : {
            break;
        }
        case 2 : {
            vParams = vec4(size . x,
                           0.5 * size . x,
                           0.0,
                           0.0);
            break;
        }
        case 1 : {
            float diameter = size . y;
            float period = diameter * 2.0;
            float center_line = 0.5 * size . y;
            vParams = vec4(period,
                           diameter / 2.0,
                           center_line,
                           0.0);
            break;
        }
        case 3 : {

            float line_thickness = max(aWavyLineThickness, 1.0);


            float slope_length = size . y - line_thickness;

            float flat_length = max((line_thickness - 1.0)* 2.0, 1.0);

            vParams = vec4(line_thickness / 2.0,
                           slope_length,
                           flat_length,
                           size . y);
            break;
        }
        default :
            vParams = vec4(0.0);
    }

    vLocalPos = aPosition . xy * aLocalSize;

    gl_Position = uTransform * vec4(aTaskRect . xy + aTaskRect . zw * aPosition . xy, 0.0, 1.0);
}

