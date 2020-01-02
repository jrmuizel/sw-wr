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
























      in vec2 vLocalPos;

flat in float vAxisSelect;
flat in int vStyle;
flat in vec4 vParams;

















































































void main(void){

    vec2 local_pos = vLocalPos;
    float aa_range = compute_aa_range(local_pos);
    float alpha = 1.0;


    vec2 pos = mix(local_pos . xy, local_pos . yx, vAxisSelect);

    switch(vStyle){
        case 0 : {
            break;
        }
        case 2 : {

            alpha = step(floor(pos . x + 0.5), vParams . y);
            break;
        }
        case 1 : {

            vec2 dot_relative_pos = pos - vParams . yz;
            float dot_distance = length(dot_relative_pos)- vParams . y;
            alpha = distance_aa(aa_range, dot_distance);
            break;
        }
        case 3 : {
            float half_line_thickness = vParams . x;
            float slope_length = vParams . y;
            float flat_length = vParams . z;
            float vertical_bounds = vParams . w;

            float half_period = slope_length + flat_length;

            float mid_height = vertical_bounds / 2.0;
            float peak_offset = mid_height - half_line_thickness;

            float flip = - 2.0 *(step(mod(pos . x, 2.0 * half_period), half_period)- 0.5);

            peak_offset *= flip;
            float peak_height = mid_height + peak_offset;


            pos . x = mod(pos . x, half_period);


            float dist1 = distance_to_line(vec2(0.0, peak_height),
                                           vec2(1.0, - flip),
                                           pos);
            float dist2 = distance_to_line(vec2(0.0, peak_height),
                                           vec2(0, - flip),
                                           pos);
            float dist3 = distance_to_line(vec2(flat_length, peak_height),
                                           vec2(- 1.0, - flip),
                                           pos);
            float dist = abs(max(max(dist1, dist2), dist3));


            alpha = distance_aa(aa_range, dist - half_line_thickness);


            if(half_line_thickness <= 1.0){
                alpha = 1.0 - step(alpha, 0.5);
            }

            break;
        }
        default : break;
    }

    oFragColor = vec4(alpha);
}

