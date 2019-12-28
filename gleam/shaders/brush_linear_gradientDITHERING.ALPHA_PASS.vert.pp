#version 300 es


















































        precision highp sampler2DArray;


















































    uniform int uMode;


    uniform mat4 uTransform;


    in vec3 aPosition;








































































































































































uniform sampler2DArray sColor0;
uniform sampler2DArray sColor1;
uniform sampler2DArray sColor2;



uniform sampler2D sDither;





















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








uniform highp sampler2D sRenderTasks;

struct RenderTaskCommonData {
    RectWithSize task_rect;
    float texture_layer_index;
};

struct RenderTaskData {
    RenderTaskCommonData common_data;
    vec3 user_data;
};

RenderTaskData fetch_render_task_data(int index){
    ivec2 uv = ivec2(int(2U *(uint(index)%(1024U / 2U))), int(uint(index)/(1024U / 2U)));

    vec4 texel0 = texelFetch(sRenderTasks, uv + ivec2(0, 0), 0);
    vec4 texel1 = texelFetch(sRenderTasks, uv + ivec2(1, 0), 0);

    RectWithSize task_rect = RectWithSize(
        texel0 . xy,
        texel0 . zw
    );

    RenderTaskCommonData common_data = RenderTaskCommonData(
        task_rect,
        texel1 . x
    );

    RenderTaskData data = RenderTaskData(
        common_data,
        texel1 . yzw
    );

    return data;
}

RenderTaskCommonData fetch_render_task_common_data(int index){
    ivec2 uv = ivec2(int(2U *(uint(index)%(1024U / 2U))), int(uint(index)/(1024U / 2U)));

    vec4 texel0 = texelFetch(sRenderTasks, uv + ivec2(0, 0), 0);
    vec4 texel1 = texelFetch(sRenderTasks, uv + ivec2(1, 0), 0);

    RectWithSize task_rect = RectWithSize(
        texel0 . xy,
        texel0 . zw
    );

    RenderTaskCommonData data = RenderTaskCommonData(
        task_rect,
        texel1 . x
    );

    return data;
}









struct PictureTask {
    RenderTaskCommonData common_data;
    float device_pixel_scale;
    vec2 content_origin;
};

PictureTask fetch_picture_task(int address){
    RenderTaskData task_data = fetch_render_task_data(address);

    PictureTask task = PictureTask(
        task_data . common_data,
        task_data . user_data . x,
        task_data . user_data . yz
    );

    return task;
}



struct ClipArea {
    RenderTaskCommonData common_data;
    float device_pixel_scale;
    vec2 screen_origin;
};

ClipArea fetch_clip_area(int index){
    ClipArea area;

    if(index >= 0x7FFF){
        RectWithSize rect = RectWithSize(vec2(0.0), vec2(0.0));

        area . common_data = RenderTaskCommonData(rect, 0.0);
        area . device_pixel_scale = 0.0;
        area . screen_origin = vec2(0.0);
    } else {
        RenderTaskData task_data = fetch_render_task_data(index);

        area . common_data = task_data . common_data;
        area . device_pixel_scale = task_data . user_data . x;
        area . screen_origin = task_data . user_data . yz;
    }

    return area;
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



vec4[8]fetch_from_gpu_cache_8(int address){
    ivec2 uv = get_gpu_cache_uv(address);
    return vec4[8](
                                                 texelFetch(sGpuCache, uv + ivec2(0, 0), 0),
                                                 texelFetch(sGpuCache, uv + ivec2(1, 0), 0),
                                                 texelFetch(sGpuCache, uv + ivec2(2, 0), 0),
                                                 texelFetch(sGpuCache, uv + ivec2(3, 0), 0),
                                                 texelFetch(sGpuCache, uv + ivec2(4, 0), 0),
                                                 texelFetch(sGpuCache, uv + ivec2(5, 0), 0),
                                                 texelFetch(sGpuCache, uv + ivec2(6, 0), 0),
                                                 texelFetch(sGpuCache, uv + ivec2(7, 0), 0)
    );
}

vec4[3]fetch_from_gpu_cache_3(int address){
    ivec2 uv = get_gpu_cache_uv(address);
    return vec4[3](
                                                 texelFetch(sGpuCache, uv + ivec2(0, 0), 0),
                                                 texelFetch(sGpuCache, uv + ivec2(1, 0), 0),
                                                 texelFetch(sGpuCache, uv + ivec2(2, 0), 0)
    );
}

vec4[3]fetch_from_gpu_cache_3_direct(ivec2 address){
    return vec4[3](
                                                      texelFetch(sGpuCache, address + ivec2(0, 0), 0),
                                                      texelFetch(sGpuCache, address + ivec2(1, 0), 0),
                                                      texelFetch(sGpuCache, address + ivec2(2, 0), 0)
    );
}

vec4[4]fetch_from_gpu_cache_4_direct(ivec2 address){
    return vec4[4](
                                                      texelFetch(sGpuCache, address + ivec2(0, 0), 0),
                                                      texelFetch(sGpuCache, address + ivec2(1, 0), 0),
                                                      texelFetch(sGpuCache, address + ivec2(2, 0), 0),
                                                      texelFetch(sGpuCache, address + ivec2(3, 0), 0)
    );
}

vec4[4]fetch_from_gpu_cache_4(int address){
    ivec2 uv = get_gpu_cache_uv(address);
    return vec4[4](
                                                 texelFetch(sGpuCache, uv + ivec2(0, 0), 0),
                                                 texelFetch(sGpuCache, uv + ivec2(1, 0), 0),
                                                 texelFetch(sGpuCache, uv + ivec2(2, 0), 0),
                                                 texelFetch(sGpuCache, uv + ivec2(3, 0), 0)
    );
}



struct ImageResource {
    RectWithEndpoint uv_rect;
    float layer;
    vec3 user_data;
};

ImageResource fetch_image_resource(int address){

    vec4 data[2]= fetch_from_gpu_cache_2(address);
    RectWithEndpoint uv_rect = RectWithEndpoint(data[0]. xy, data[0]. zw);
    return ImageResource(uv_rect, data[1]. x, data[1]. yzw);
}

ImageResource fetch_image_resource_direct(ivec2 address){
    vec4 data[2]= fetch_from_gpu_cache_2_direct(address);
    RectWithEndpoint uv_rect = RectWithEndpoint(data[0]. xy, data[0]. zw);
    return ImageResource(uv_rect, data[1]. x, data[1]. yzw);
}




struct ImageResourceExtra {
    vec4 st_tl;
    vec4 st_tr;
    vec4 st_bl;
    vec4 st_br;
};

ImageResourceExtra fetch_image_resource_extra(int address){
    vec4 data[4]= fetch_from_gpu_cache_4(address + 2);
    return ImageResourceExtra(
        data[0],
        data[1],
        data[2],
        data[3]
    );
}






flat out vec4 vTransformBounds;




uniform highp sampler2D sTransformPalette;

void init_transform_vs(vec4 local_bounds){
    vTransformBounds = local_bounds;
}

struct Transform {
    mat4 m;
    mat4 inv_m;
    bool is_axis_aligned;
};

Transform fetch_transform(int id){
    Transform transform;

    transform . is_axis_aligned =(id >> 24)== 0;
    int index = id & 0x00ffffff;





    ivec2 uv = ivec2(int(8U *(uint(index)%(1024U / 8U))), int(uint(index)/(1024U / 8U)));
    ivec2 uv0 = ivec2(uv . x + 0, uv . y);

    transform . m[0]= texelFetch(sTransformPalette, uv0 + ivec2(0, 0), 0);
    transform . m[1]= texelFetch(sTransformPalette, uv0 + ivec2(1, 0), 0);
    transform . m[2]= texelFetch(sTransformPalette, uv0 + ivec2(2, 0), 0);
    transform . m[3]= texelFetch(sTransformPalette, uv0 + ivec2(3, 0), 0);

    transform . inv_m[0]= texelFetch(sTransformPalette, uv0 + ivec2(4, 0), 0);
    transform . inv_m[1]= texelFetch(sTransformPalette, uv0 + ivec2(5, 0), 0);
    transform . inv_m[2]= texelFetch(sTransformPalette, uv0 + ivec2(6, 0), 0);
    transform . inv_m[3]= texelFetch(sTransformPalette, uv0 + ivec2(7, 0), 0);

    return transform;
}




bool ray_plane(vec3 normal, vec3 pt, vec3 ray_origin, vec3 ray_dir, out float t)
{
    float denom = dot(normal, ray_dir);
    if(abs(denom)> 1e-6){
        vec3 d = pt - ray_origin;
        t = dot(d, normal)/ denom;
        return t >= 0.0;
    }

    return false;
}





vec4 untransform(vec2 ref, vec3 n, vec3 a, mat4 inv_transform){
    vec3 p = vec3(ref, - 10000.0);
    vec3 d = vec3(0, 0, 1.0);

    float t = 0.0;


    ray_plane(n, a, p, d, t);
    float z = p . z + d . z * t;

    vec4 r = inv_transform * vec4(ref, z, 1.0);
    return r;
}


vec4 get_node_pos(vec2 pos, Transform transform){

    vec4 ah = transform . m * vec4(0.0, 0.0, 0.0, 1.0);
    vec3 a = ah . xyz / ah . w;


    vec3 n = transpose(mat3(transform . inv_m))* vec3(0.0, 0.0, 1.0);
    return untransform(pos, n, a, transform . inv_m);
}














































uniform sampler2DArray sPrevPassAlpha;
uniform sampler2DArray sPrevPassColor;

vec2 clamp_rect(vec2 pt, RectWithSize rect){
    return clamp(pt, rect . p0, rect . p0 + rect . size);
}


flat out vec4 vClipMaskUvBounds;

      out vec4 vClipMaskUv;















uniform highp sampler2D sPrimitiveHeadersF;
uniform highp isampler2D sPrimitiveHeadersI;


in ivec4 aData;




struct PrimitiveHeader {
    RectWithSize local_rect;
    RectWithSize local_clip_rect;
    float z;
    int specific_prim_address;
    int transform_id;
    ivec4 user_data;
};

PrimitiveHeader fetch_prim_header(int index){
    PrimitiveHeader ph;

    ivec2 uv_f = ivec2(int(2U *(uint(index)%(1024U / 2U))), int(uint(index)/(1024U / 2U)));
    vec4 local_rect = texelFetch(sPrimitiveHeadersF, uv_f + ivec2(0, 0), 0);
    vec4 local_clip_rect = texelFetch(sPrimitiveHeadersF, uv_f + ivec2(1, 0), 0);
    ph . local_rect = RectWithSize(local_rect . xy, local_rect . zw);
    ph . local_clip_rect = RectWithSize(local_clip_rect . xy, local_clip_rect . zw);

    ivec2 uv_i = ivec2(int(2U *(uint(index)%(1024U / 2U))), int(uint(index)/(1024U / 2U)));
    ivec4 data0 = texelFetch(sPrimitiveHeadersI, uv_i + ivec2(0, 0), 0);
    ivec4 data1 = texelFetch(sPrimitiveHeadersI, uv_i + ivec2(1, 0), 0);
    ph . z = float(data0 . x);
    ph . specific_prim_address = data0 . y;
    ph . transform_id = data0 . z;
    ph . user_data = data1;

    return ph;
}

struct VertexInfo {
    vec2 local_pos;
    vec2 snap_offset;
    vec4 world_pos;
};

VertexInfo write_vertex(RectWithSize instance_rect,
                        RectWithSize local_clip_rect,
                        float z,
                        Transform transform,
                        PictureTask task){


    vec2 local_pos = instance_rect . p0 + instance_rect . size * aPosition . xy;


    vec2 clamped_local_pos = clamp_rect(local_pos, local_clip_rect);


    vec4 world_pos = transform . m * vec4(clamped_local_pos, 0.0, 1.0);


    vec2 device_pos = world_pos . xy * task . device_pixel_scale;


    vec2 final_offset = - task . content_origin + task . common_data . task_rect . p0;

    gl_Position = uTransform * vec4(device_pos + final_offset * world_pos . w, z * world_pos . w, world_pos . w);

    VertexInfo vi = VertexInfo(
        clamped_local_pos,
        vec2(0.0, 0.0),
        world_pos
    );

    return vi;
}

float cross2(vec2 v0, vec2 v1){
    return v0 . x * v1 . y - v0 . y * v1 . x;
}


vec2 intersect_lines(vec2 p0, vec2 p1, vec2 p2, vec2 p3){
    vec2 d0 = p0 - p1;
    vec2 d1 = p2 - p3;

    float s0 = cross2(p0, p1);
    float s1 = cross2(p2, p3);

    float d = cross2(d0, d1);
    float nx = s0 * d1 . x - d0 . x * s1;
    float ny = s0 * d1 . y - d0 . y * s1;

    return vec2(nx / d, ny / d);
}

VertexInfo write_transform_vertex(RectWithSize local_segment_rect,
                                  RectWithSize local_prim_rect,
                                  RectWithSize local_clip_rect,
                                  vec4 clip_edge_mask,
                                  float z,
                                  Transform transform,
                                  PictureTask task){

    RectWithEndpoint clip_rect = to_rect_with_endpoint(local_clip_rect);
    RectWithEndpoint segment_rect = to_rect_with_endpoint(local_segment_rect);
    segment_rect . p0 = clamp(segment_rect . p0, clip_rect . p0, clip_rect . p1);
    segment_rect . p1 = clamp(segment_rect . p1, clip_rect . p0, clip_rect . p1);


    RectWithEndpoint prim_rect = to_rect_with_endpoint(local_prim_rect);
    prim_rect . p0 = clamp(prim_rect . p0, clip_rect . p0, clip_rect . p1);
    prim_rect . p1 = clamp(prim_rect . p1, clip_rect . p0, clip_rect . p1);











    float extrude_amount = 2.0;
    vec4 extrude_distance = vec4(extrude_amount)* clip_edge_mask;
    local_segment_rect . p0 -= extrude_distance . xy;
    local_segment_rect . size += extrude_distance . xy + extrude_distance . zw;


    vec2 local_pos = local_segment_rect . p0 + local_segment_rect . size * aPosition . xy;


    vec2 task_offset = task . common_data . task_rect . p0 - task . content_origin;


    vec4 world_pos = transform . m * vec4(local_pos, 0.0, 1.0);
    vec4 final_pos = vec4(
        world_pos . xy * task . device_pixel_scale + task_offset * world_pos . w,
        z * world_pos . w,
        world_pos . w
    );

    gl_Position = uTransform * final_pos;

    init_transform_vs(mix(
        vec4(prim_rect . p0, prim_rect . p1),
        vec4(segment_rect . p0, segment_rect . p1),
        clip_edge_mask
    ));

    VertexInfo vi = VertexInfo(
        local_pos,
        vec2(0.0),
        world_pos
    );

    return vi;
}

void write_clip(vec4 world_pos, vec2 snap_offset, ClipArea area){
    vec2 uv = world_pos . xy * area . device_pixel_scale +
        world_pos . w *(snap_offset + area . common_data . task_rect . p0 - area . screen_origin);
    vClipMaskUvBounds = vec4(
        area . common_data . task_rect . p0,
        area . common_data . task_rect . p0 + area . common_data . task_rect . size
    );
    vClipMaskUv = vec4(uv, area . common_data . texture_layer_index, world_pos . w);
}



vec2 get_image_quad_uv(int address, vec2 f){
    ImageResourceExtra extra_data = fetch_image_resource_extra(address);
    vec4 x = mix(extra_data . st_tl, extra_data . st_tr, f . x);
    vec4 y = mix(extra_data . st_bl, extra_data . st_br, f . x);
    vec4 z = mix(x, y, f . y);
    return z . xy / z . w;
}















































































void brush_vs(
    VertexInfo vi,
    int prim_address,
    RectWithSize local_rect,
    RectWithSize segment_rect,
    ivec4 prim_user_data,
    int segment_user_data,
    mat4 transform,
    PictureTask pic_task,
    int brush_flags,
    vec4 segment_data
);











void main(void){

    int prim_header_address = aData . x;
    int render_task_index = aData . y >> 16;
    int clip_address = aData . y & 0xffff;
    int segment_index = aData . z & 0xffff;
    int edge_flags =(aData . z >> 16)& 0xff;
    int brush_flags =(aData . z >> 24)& 0xff;
    int segment_user_data = aData . w;
    PrimitiveHeader ph = fetch_prim_header(prim_header_address);


    vec4 segment_data;
    RectWithSize segment_rect;
    if(segment_index == 0xffff){
        segment_rect = ph . local_rect;
        segment_data = vec4(0.0);
    } else {
        int segment_address = ph . specific_prim_address +
                                                    2 +
                              segment_index * 2;

        vec4[2]segment_info = fetch_from_gpu_cache_2(segment_address);
        segment_rect = RectWithSize(segment_info[0]. xy, segment_info[0]. zw);
        segment_rect . p0 += ph . local_rect . p0;
        segment_data = segment_info[1];
    }

    VertexInfo vi;


    PictureTask pic_task = fetch_picture_task(render_task_index);
    ClipArea clip_area = fetch_clip_area(clip_address);

    Transform transform = fetch_transform(ph . transform_id);


    if(transform . is_axis_aligned){
        vi = write_vertex(
            segment_rect,
            ph . local_clip_rect,
            ph . z,
            transform,
            pic_task
        );








        init_transform_vs(vec4(vec2(- 1.0e16), vec2(1.0e16)));

    } else {
        bvec4 edge_mask = notEqual(edge_flags & ivec4(1, 2, 4, 8), ivec4(0));

        vi = write_transform_vertex(
            segment_rect,
            ph . local_rect,
            ph . local_clip_rect,
            mix(vec4(0.0), vec4(1.0), edge_mask),
            ph . z,
            transform,
            pic_task
        );
    }








    write_clip(
        vi . world_pos,
        vi . snap_offset,
        clip_area
    );



    brush_vs(
        vi,
        ph . specific_prim_address,
        ph . local_rect,
        segment_rect,
        ph . user_data,
        segment_user_data,
        transform . m,
        pic_task,
        brush_flags,
        segment_data
    );
}




































flat out highp int vGradientAddress;
flat out float vGradientRepeat;

flat out vec2 vScaledDir;
flat out vec2 vStartPoint;



flat out vec2 vRepeatedSize;

      out vec2 vPos;


      out vec2 vLocalPos;
flat out vec2 vTileRepeat;




struct Gradient {
    vec4 start_end_point;
    int extend_mode;
    vec2 stretch_size;
};

Gradient fetch_gradient(int address){
    vec4 data[2]= fetch_from_gpu_cache_2(address);
    return Gradient(
        data[0],
        int(data[1]. x),
        data[1]. yz
    );
}

void brush_vs(
    VertexInfo vi,
    int prim_address,
    RectWithSize local_rect,
    RectWithSize segment_rect,
    ivec4 prim_user_data,
    int segment_user_data,
    mat4 transform,
    PictureTask pic_task,
    int brush_flags,
    vec4 texel_rect
){
    Gradient gradient = fetch_gradient(prim_address);

    if((brush_flags & 2)!= 0){
        vPos =(vi . local_pos - segment_rect . p0)/ segment_rect . size;
        vPos = vPos *(texel_rect . zw - texel_rect . xy)+ texel_rect . xy;
        vPos = vPos * local_rect . size;
    } else {
        vPos = vi . local_pos - local_rect . p0;
    }

    vec2 start_point = gradient . start_end_point . xy;
    vec2 end_point = gradient . start_end_point . zw;
    vec2 dir = end_point - start_point;

    vStartPoint = start_point;
    vScaledDir = dir / dot(dir, dir);

    vec2 tile_repeat = local_rect . size / gradient . stretch_size;
    vRepeatedSize = gradient . stretch_size;

    vGradientAddress = prim_user_data . x;


    vGradientRepeat = float(gradient . extend_mode != 0);


    vTileRepeat = tile_repeat;
    vLocalPos = vi . local_pos;

}

