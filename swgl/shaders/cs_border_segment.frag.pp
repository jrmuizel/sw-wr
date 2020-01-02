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




flat in vec4 vColor00;
flat in vec4 vColor01;
flat in vec4 vColor10;
flat in vec4 vColor11;



flat in vec4 vColorLine;


flat in ivec4 vConfig;




flat in vec4 vClipCenter_Sign;



flat in vec4 vClipRadii;


flat in vec4 vEdgeReference;


flat in vec4 vPartialWidths;


flat in vec4 vClipParams1;
flat in vec4 vClipParams2;


      in vec2 vPos;












































































































































































































vec4 evaluate_color_for_style_in_corner(
    vec2 clip_relative_pos,
    int style,
    vec4 color0,
    vec4 color1,
    vec4 clip_radii,
    float mix_factor,
    int segment,
    float aa_range
){
    switch(style){
        case 2 : {




            float d_radii_a = distance_to_ellipse(
                clip_relative_pos,
                clip_radii . xy - vPartialWidths . xy,
                aa_range
            );
            float d_radii_b = distance_to_ellipse(
                clip_relative_pos,
                clip_radii . xy - 2.0 * vPartialWidths . xy,
                aa_range
            );
            float d = min(- d_radii_a, d_radii_b);
            color0 *= distance_aa(aa_range, d);
            break;
        }
        case 6 :
        case 7 : {
            float d = distance_to_ellipse(
                clip_relative_pos,
                clip_radii . xy - vPartialWidths . zw,
                aa_range
            );
            float alpha = distance_aa(aa_range, d);
            float swizzled_factor;
            switch(segment){
                case 0 : swizzled_factor = 0.0;break;
                case 1 : swizzled_factor = mix_factor;break;
                case 2 : swizzled_factor = 1.0;break;
                case 3 : swizzled_factor = 1.0 - mix_factor;break;
                default : swizzled_factor = 0.0;break;
            };
            vec4 c0 = mix(color1, color0, swizzled_factor);
            vec4 c1 = mix(color0, color1, swizzled_factor);
            color0 = mix(c0, c1, alpha);
            break;
        }
        default :
            break;
    }

    return color0;
}

vec4 evaluate_color_for_style_in_edge(
    vec2 pos_vec,
    int style,
    vec4 color0,
    vec4 color1,
    float aa_range,
    int edge_axis_id
){
    vec2 edge_axis = edge_axis_id != 0 ? vec2(0.0, 1.0): vec2(1.0, 0.0);
    float pos = dot(pos_vec, edge_axis);
    switch(style){
        case 2 : {
            float d = - 1.0;
            float partial_width = dot(vPartialWidths . xy, edge_axis);
            if(partial_width >= 1.0){
                vec2 ref = vec2(
                    dot(vEdgeReference . xy, edge_axis)+ partial_width,
                    dot(vEdgeReference . zw, edge_axis)- partial_width
                );
                d = min(pos - ref . x, ref . y - pos);
            }
            color0 *= distance_aa(aa_range, d);
            break;
        }
        case 6 :
        case 7 : {
            float ref = dot(vEdgeReference . xy + vPartialWidths . zw, edge_axis);
            float d = pos - ref;
            float alpha = distance_aa(aa_range, d);
            color0 = mix(color0, color1, alpha);
            break;
        }
        default :
            break;
    }

    return color0;
}

void main(void){
    float aa_range = compute_aa_range(vPos);
    vec4 color0, color1;

    int segment = vConfig . x;
    ivec2 style = ivec2(vConfig . y & 0xffff, vConfig . y >> 16);
    ivec2 edge_axis = ivec2(vConfig . z & 0xffff, vConfig . z >> 16);
    int clip_mode = vConfig . w;

    float mix_factor = 0.0;
    if(edge_axis . x != edge_axis . y){
        float d_line = distance_to_line(vColorLine . xy, vColorLine . zw, vPos);
        mix_factor = distance_aa(aa_range, - d_line);
    }


    vec2 clip_relative_pos = vPos - vClipCenter_Sign . xy;
    bool in_clip_region = all(lessThan(vClipCenter_Sign . zw * clip_relative_pos, vec2(0.0)));
    float d = - 1.0;

    switch(clip_mode){
        case 3 : {

            d = distance(vClipParams1 . xy, vPos)- vClipParams1 . z;
            break;
        }
        case 2 : {
            bool is_vertical = vClipParams1 . x == 0.;
            float half_dash = is_vertical ? vClipParams1 . y : vClipParams1 . x;




            float pos = is_vertical ? vPos . y : vPos . x;
            bool in_dash = pos < half_dash || pos > 3.0 * half_dash;
            if(! in_dash){
                d = 1.;
            }
            break;
        }
        case 1 : {


            float d0 = distance_to_line(vClipParams1 . xy,
                                        vClipParams1 . zw,
                                        vPos);
            float d1 = distance_to_line(vClipParams2 . xy,
                                        vClipParams2 . zw,
                                        vPos);
            d = max(d0, - d1);
            break;
        }
        case 0 :
        default :
            break;
    }

    if(in_clip_region){
        float d_radii_a = distance_to_ellipse(clip_relative_pos, vClipRadii . xy, aa_range);
        float d_radii_b = distance_to_ellipse(clip_relative_pos, vClipRadii . zw, aa_range);
        float d_radii = max(d_radii_a, - d_radii_b);
        d = max(d, d_radii);

        color0 = evaluate_color_for_style_in_corner(
            clip_relative_pos,
            style . x,
            vColor00,
            vColor01,
            vClipRadii,
            mix_factor,
            segment,
            aa_range
        );
        color1 = evaluate_color_for_style_in_corner(
            clip_relative_pos,
            style . y,
            vColor10,
            vColor11,
            vClipRadii,
            mix_factor,
            segment,
            aa_range
        );
    } else {
        color0 = evaluate_color_for_style_in_edge(
            vPos,
            style . x,
            vColor00,
            vColor01,
            aa_range,
            edge_axis . x
        );
        color1 = evaluate_color_for_style_in_edge(
            vPos,
            style . y,
            vColor10,
            vColor11,
            aa_range,
            edge_axis . y
        );
    }

    float alpha = distance_aa(aa_range, d);
    vec4 color = mix(color0, color1, mix_factor);
    oFragColor = color * alpha;
}

