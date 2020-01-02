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


















      in float vPos;
flat in vec4 vStops;
flat in vec4 vColor0;
flat in vec4 vColor1;
flat in vec4 vColor2;
flat in vec4 vColor3;


























float linear_step(float edge0, float edge1, float x){
    if(edge0 >= edge1){
        return 0.0;
    }

    return clamp((x - edge0)/(edge1 - edge0), 0.0, 1.0);
}

void main(void){
    vec4 color = vColor0;

    color = mix(color, vColor1, linear_step(vStops . x, vStops . y, vPos));
    color = mix(color, vColor2, linear_step(vStops . y, vStops . z, vPos));
    color = mix(color, vColor3, linear_step(vStops . z, vStops . w, vPos));

    oFragColor = color;
}

