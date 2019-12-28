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








flat in vec4 vColor0;
flat in vec4 vColor1;



flat in vec4 vColorLine;


flat in int vMixColors;




flat in vec4 vClipCenter_Sign;



flat in vec4 vClipRadii;


flat in vec4 vHorizontalClipCenter_Sign;
flat in vec2 vHorizontalClipRadii;
flat in vec4 vVerticalClipCenter_Sign;
flat in vec2 vVerticalClipRadii;


      in vec2 vPos;



























































































void main(void){
    float aa_range = compute_aa_range(vPos);
    bool do_aa = vMixColors != 2;

    float mix_factor = 0.0;
    if(vMixColors != 0){
        float d_line = distance_to_line(vColorLine . xy, vColorLine . zw, vPos);
        if(do_aa){
            mix_factor = distance_aa(aa_range, - d_line);
        } else {
            mix_factor = d_line + 0.0001 >= 0. ? 1.0 : 0.0;
        }
    }


    vec2 clip_relative_pos = vPos - vClipCenter_Sign . xy;
    bool in_clip_region = all(lessThan(vClipCenter_Sign . zw * clip_relative_pos, vec2(0.0)));

    float d = - 1.0;
    if(in_clip_region){
        float d_radii_a = distance_to_ellipse(clip_relative_pos, vClipRadii . xy, aa_range);
        float d_radii_b = distance_to_ellipse(clip_relative_pos, vClipRadii . zw, aa_range);
        d = max(d_radii_a, - d_radii_b);
    }


    clip_relative_pos = vPos - vHorizontalClipCenter_Sign . xy;
    in_clip_region = all(lessThan(vHorizontalClipCenter_Sign . zw * clip_relative_pos, vec2(0.0)));
    if(in_clip_region){
        float d_radii = distance_to_ellipse(clip_relative_pos, vHorizontalClipRadii . xy, aa_range);
        d = max(d_radii, d);
    }


    clip_relative_pos = vPos - vVerticalClipCenter_Sign . xy;
    in_clip_region = all(lessThan(vVerticalClipCenter_Sign . zw * clip_relative_pos, vec2(0.0)));
    if(in_clip_region){
        float d_radii = distance_to_ellipse(clip_relative_pos, vVerticalClipRadii . xy, aa_range);
        d = max(d_radii, d);
    }

    float alpha = do_aa ? distance_aa(aa_range, d): 1.0;
    vec4 color = mix(vColor0, vColor1, mix_factor);
    oFragColor = color * alpha;
}

