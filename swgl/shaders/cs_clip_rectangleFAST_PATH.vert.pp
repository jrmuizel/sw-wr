#version 300 es















































        precision highp sampler2DArray;


















































    uniform int uMode;


    uniform mat4 uTransform;


    in vec3 aPosition;








































































































































































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





































in ivec2 aTransformIds;
in ivec4 aClipDataResourceAddress;
in vec2 aClipLocalPos;
in vec4 aClipTileRect;
in vec4 aClipDeviceArea;
in vec4 aClipOrigins;
in float aDevicePixelScale;

struct ClipMaskInstance {
    int clip_transform_id;
    int prim_transform_id;
    ivec2 clip_data_address;
    ivec2 resource_address;
    vec2 local_pos;
    RectWithSize tile_rect;
    RectWithSize sub_rect;
    vec2 task_origin;
    vec2 screen_origin;
    float device_pixel_scale;
};

ClipMaskInstance fetch_clip_item(){
    ClipMaskInstance cmi;

    cmi . clip_transform_id = aTransformIds . x;
    cmi . prim_transform_id = aTransformIds . y;
    cmi . clip_data_address = aClipDataResourceAddress . xy;
    cmi . resource_address = aClipDataResourceAddress . zw;
    cmi . local_pos = aClipLocalPos;
    cmi . tile_rect = RectWithSize(aClipTileRect . xy, aClipTileRect . zw);
    cmi . sub_rect = RectWithSize(aClipDeviceArea . xy, aClipDeviceArea . zw);
    cmi . task_origin = aClipOrigins . xy;
    cmi . screen_origin = aClipOrigins . zw;
    cmi . device_pixel_scale = aDevicePixelScale;

    return cmi;
}

struct ClipVertexInfo {
    vec4 local_pos;
    RectWithSize clipped_local_rect;
};

RectWithSize intersect_rect(RectWithSize a, RectWithSize b){
    vec4 p = clamp(vec4(a . p0, a . p0 + a . size), b . p0 . xyxy, b . p0 . xyxy + b . size . xyxy);
    return RectWithSize(p . xy, max(vec2(0.0), p . zw - p . xy));
}




ClipVertexInfo write_clip_tile_vertex(RectWithSize local_clip_rect,
                                      Transform prim_transform,
                                      Transform clip_transform,
                                      RectWithSize sub_rect,
                                      vec2 task_origin,
                                      vec2 screen_origin,
                                      float device_pixel_scale){
    vec2 device_pos = screen_origin + sub_rect . p0 + aPosition . xy * sub_rect . size;
    vec2 world_pos = device_pos / device_pixel_scale;

    vec4 pos = prim_transform . m * vec4(world_pos, 0.0, 1.0);
    pos . xyz /= pos . w;

    vec4 p = get_node_pos(pos . xy, clip_transform);
    vec4 local_pos = p * pos . w;








    vec4 vertex_pos = vec4(
        task_origin + sub_rect . p0 + aPosition . xy * sub_rect . size,
        0.0,
        1.0
    );

    gl_Position = uTransform * vertex_pos;

    init_transform_vs(vec4(local_clip_rect . p0, local_clip_rect . p0 + local_clip_rect . size));

    ClipVertexInfo vi = ClipVertexInfo(local_pos, local_clip_rect);
    return vi;
}

































































































      out vec2 vLocalPos;
flat out vec3 vClipParams;








flat out float vClipMode;


struct ClipRect {
    RectWithSize rect;
    vec4 mode;
};

ClipRect fetch_clip_rect(ivec2 address){
    vec4 data[2]= fetch_from_gpu_cache_2_direct(address);
    ClipRect rect = ClipRect(RectWithSize(data[0]. xy, data[0]. zw), data[1]);
    return rect;
}

struct ClipCorner {
    RectWithSize rect;
    vec4 outer_inner_radius;
};



ClipCorner fetch_clip_corner(ivec2 address, float index){
    address += ivec2(2 + 2 * int(index), 0);
    vec4 data[2]= fetch_from_gpu_cache_2_direct(address);
    ClipCorner corner = ClipCorner(RectWithSize(data[0]. xy, data[0]. zw), data[1]);
    return corner;
}

struct ClipData {
    ClipRect rect;
    ClipCorner top_left;
    ClipCorner top_right;
    ClipCorner bottom_left;
    ClipCorner bottom_right;
};

ClipData fetch_clip(ivec2 address){
    ClipData clip;

    clip . rect = fetch_clip_rect(address);
    clip . top_left = fetch_clip_corner(address, 0.0);
    clip . top_right = fetch_clip_corner(address, 1.0);
    clip . bottom_left = fetch_clip_corner(address, 2.0);
    clip . bottom_right = fetch_clip_corner(address, 3.0);

    return clip;
}

void main(void){
    ClipMaskInstance cmi = fetch_clip_item();
    Transform clip_transform = fetch_transform(cmi . clip_transform_id);
    Transform prim_transform = fetch_transform(cmi . prim_transform_id);
    ClipData clip = fetch_clip(cmi . clip_data_address);

    RectWithSize local_rect = clip . rect . rect;
    local_rect . p0 = cmi . local_pos;

    ClipVertexInfo vi = write_clip_tile_vertex(
        local_rect,
        prim_transform,
        clip_transform,
        cmi . sub_rect,
        cmi . task_origin,
        cmi . screen_origin,
        cmi . device_pixel_scale
    );

    vClipMode = clip . rect . mode . x;




    vec2 half_size = 0.5 * local_rect . size;
    float radius = clip . top_left . outer_inner_radius . x;
    vLocalPos = vi . local_pos . xy - half_size - cmi . local_pos;
    vClipParams . xy = half_size - vec2(radius);
    vClipParams . z = radius;






















}

