/* inputs
vec3 aPosition
ivec4 aData
*/
struct brush_solid {
static int get_uniform_location(char *name) {
if (strcmp("uMode", name) == 0) { return 1; }
if (strcmp("uTransform", name) == 0) { return 2; }
if (strcmp("sColor0", name) == 0) { return 3; }
if (strcmp("sColor1", name) == 0) { return 4; }
if (strcmp("sColor2", name) == 0) { return 5; }
if (strcmp("sRenderTasks", name) == 0) { return 6; }
if (strcmp("sGpuCache", name) == 0) { return 7; }
if (strcmp("sTransformPalette", name) == 0) { return 8; }
if (strcmp("sPrevPassAlpha", name) == 0) { return 9; }
if (strcmp("sPrevPassColor", name) == 0) { return 10; }
if (strcmp("sPrimitiveHeadersF", name) == 0) { return 11; }
if (strcmp("sPrimitiveHeadersI", name) == 0) { return 12; }
return -1;
}
static inline int aPosition_location_index;
static inline int aData_location_index;
static void bind_attrib_location(char *name, int index) {
if (strcmp("aPosition", name) == 0) { aPosition_location_index = index; }
if (strcmp("aData", name) == 0) { aData_location_index = index; }
}
void load_attribs(VertexAttrib *attribs, void *buf, int count) {
{ VertexAttrib &va = attribs[aPosition_location_index];
vec3_scalar scalar;
memcpy(&scalar, (char*)buf + va.stride * count, va.size);
aPosition = vec3(scalar);
}
{ VertexAttrib &va = attribs[aData_location_index];
ivec4_scalar scalar;
memcpy(&scalar, (char*)buf + va.stride * count, va.size);
aData = ivec4(scalar);
}
}
Bool isPixelDiscarded = false;
I32 uMode;
mat4 uTransform;
vec3 aPosition;
sampler2DArray sColor0;
sampler2DArray sColor1;
sampler2DArray sColor2;
struct RectWithSize {
vec2 p0;
vec2 size;
RectWithSize() = default;
RectWithSize(vec2 p0, vec2 size) : p0(p0), size(size){}
friend RectWithSize if_then_else(I32 c, RectWithSize t, RectWithSize e) {
return RectWithSize(
if_then_else(c, t.p0, e.p0), if_then_else(c, t.size, e.size));
}};
struct RectWithEndpoint {
vec2 p0;
vec2 p1;
RectWithEndpoint() = default;
RectWithEndpoint(vec2 p0, vec2 p1) : p0(p0), p1(p1){}
friend RectWithEndpoint if_then_else(I32 c, RectWithEndpoint t, RectWithEndpoint e) {
return RectWithEndpoint(
if_then_else(c, t.p0, e.p0), if_then_else(c, t.p1, e.p1));
}};
RectWithEndpoint to_rect_with_endpoint(RectWithSize rect) {
 RectWithEndpoint result;
 (result).p0 = (rect).p0;
 (result).p1 = ((rect).p0)+((rect).size);
 return result;
}
RectWithSize to_rect_with_size(RectWithEndpoint rect) {
 RectWithSize result;
 (result).p0 = (rect).p0;
 (result).size = ((rect).p1)-((rect).p0);
 return result;
}
RectWithSize intersect_rects(RectWithSize a, RectWithSize b) {
 RectWithSize result;
 (result).p0 = max((a).p0, (b).p0);
 (result).size = (min(((a).p0)+((a).size), ((b).p0)+((b).size)))-((result).p0);
 return result;
}
Float point_inside_rect(vec2 p, vec2 p0, vec2 p1) {
 vec2 s = (step(p0, p))-(step(p1, p));
 return ((s).sel(X))*((s).sel(Y));
}
sampler2D sRenderTasks;
struct RenderTaskCommonData {
RectWithSize task_rect;
Float texture_layer_index;
RenderTaskCommonData() = default;
RenderTaskCommonData(RectWithSize task_rect, Float texture_layer_index) : task_rect(task_rect), texture_layer_index(texture_layer_index){}
friend RenderTaskCommonData if_then_else(I32 c, RenderTaskCommonData t, RenderTaskCommonData e) {
return RenderTaskCommonData(
if_then_else(c, t.task_rect, e.task_rect), if_then_else(c, t.texture_layer_index, e.texture_layer_index));
}};
struct RenderTaskData {
RenderTaskCommonData common_data;
vec3 user_data;
RenderTaskData() = default;
RenderTaskData(RenderTaskCommonData common_data, vec3 user_data) : common_data(common_data), user_data(user_data){}
friend RenderTaskData if_then_else(I32 c, RenderTaskData t, RenderTaskData e) {
return RenderTaskData(
if_then_else(c, t.common_data, e.common_data), if_then_else(c, t.user_data, e.user_data));
}};
RenderTaskData fetch_render_task_data(I32 index) {
 ivec2 uv = ivec2(I32((2u)*((uint(index))%((1024u)/(2u)))), I32((uint(index))/((1024u)/(2u))));
 vec4 texel0 = texelFetch(sRenderTasks, (uv)+(ivec2(0, 0)), 0);
 vec4 texel1 = texelFetch(sRenderTasks, (uv)+(ivec2(1, 0)), 0);
 RectWithSize task_rect = RectWithSize((texel0).sel(X, Y), (texel0).sel(Z, W));
 RenderTaskCommonData common_data = RenderTaskCommonData(task_rect, (texel1).sel(X));
 RenderTaskData data = RenderTaskData(common_data, (texel1).sel(Y, Z, W));
 return data;
}
RenderTaskCommonData fetch_render_task_common_data(I32 index) {
 ivec2 uv = ivec2(I32((2u)*((uint(index))%((1024u)/(2u)))), I32((uint(index))/((1024u)/(2u))));
 vec4 texel0 = texelFetch(sRenderTasks, (uv)+(ivec2(0, 0)), 0);
 vec4 texel1 = texelFetch(sRenderTasks, (uv)+(ivec2(1, 0)), 0);
 RectWithSize task_rect = RectWithSize((texel0).sel(X, Y), (texel0).sel(Z, W));
 RenderTaskCommonData data = RenderTaskCommonData(task_rect, (texel1).sel(X));
 return data;
}
struct PictureTask {
RenderTaskCommonData common_data;
Float device_pixel_scale;
vec2 content_origin;
PictureTask() = default;
PictureTask(RenderTaskCommonData common_data, Float device_pixel_scale, vec2 content_origin) : common_data(common_data), device_pixel_scale(device_pixel_scale), content_origin(content_origin){}
friend PictureTask if_then_else(I32 c, PictureTask t, PictureTask e) {
return PictureTask(
if_then_else(c, t.common_data, e.common_data), if_then_else(c, t.device_pixel_scale, e.device_pixel_scale), if_then_else(c, t.content_origin, e.content_origin));
}};
PictureTask fetch_picture_task(I32 address) {
 RenderTaskData task_data = fetch_render_task_data(address);
 PictureTask task = PictureTask((task_data).common_data, ((task_data).user_data).sel(X), ((task_data).user_data).sel(Y, Z));
 return task;
}
struct ClipArea {
RenderTaskCommonData common_data;
Float device_pixel_scale;
vec2 screen_origin;
ClipArea() = default;
ClipArea(RenderTaskCommonData common_data, Float device_pixel_scale, vec2 screen_origin) : common_data(common_data), device_pixel_scale(device_pixel_scale), screen_origin(screen_origin){}
friend ClipArea if_then_else(I32 c, ClipArea t, ClipArea e) {
return ClipArea(
if_then_else(c, t.common_data, e.common_data), if_then_else(c, t.device_pixel_scale, e.device_pixel_scale), if_then_else(c, t.screen_origin, e.screen_origin));
}};
ClipArea fetch_clip_area(I32 index) {
 ClipArea area;
 {
  RectWithSize rect = RectWithSize(vec2(0.), vec2(0.));
  (area).common_data = if_then_else((index)>=(32767),RenderTaskCommonData(rect, 0.),(area).common_data);
  (area).device_pixel_scale = if_then_else((index)>=(32767),0.,(area).device_pixel_scale);
  (area).screen_origin = if_then_else((index)>=(32767),vec2(0.),(area).screen_origin);
 }
 {
  RenderTaskData task_data = fetch_render_task_data(index);
  (area).common_data = if_then_else(!((index)>=(32767)),(task_data).common_data,(area).common_data);
  (area).device_pixel_scale = if_then_else(!((index)>=(32767)),((task_data).user_data).sel(X),(area).device_pixel_scale);
  (area).screen_origin = if_then_else(!((index)>=(32767)),((task_data).user_data).sel(Y, Z),(area).screen_origin);
 }
 return area;
}
sampler2D sGpuCache;
ivec2 get_gpu_cache_uv(I32 address) {
 return ivec2((uint(address))%(1024u), (uint(address))/(1024u));
}
std::array<vec4,2> fetch_from_gpu_cache_2_direct(ivec2 address) {
 return std::array<vec4,2>{texelFetch(sGpuCache, (address)+(ivec2(0, 0)), 0), texelFetch(sGpuCache, (address)+(ivec2(1, 0)), 0)};
}
std::array<vec4,2> fetch_from_gpu_cache_2(I32 address) {
 ivec2 uv = get_gpu_cache_uv(address);
 return std::array<vec4,2>{texelFetch(sGpuCache, (uv)+(ivec2(0, 0)), 0), texelFetch(sGpuCache, (uv)+(ivec2(1, 0)), 0)};
}
vec4 fetch_from_gpu_cache_1_direct(ivec2 address) {
 return texelFetch(sGpuCache, address, 0);
}
vec4 fetch_from_gpu_cache_1(I32 address) {
 ivec2 uv = get_gpu_cache_uv(address);
 return texelFetch(sGpuCache, uv, 0);
}
std::array<vec4,8> fetch_from_gpu_cache_8(I32 address) {
 ivec2 uv = get_gpu_cache_uv(address);
 return std::array<vec4,8>{texelFetch(sGpuCache, (uv)+(ivec2(0, 0)), 0), texelFetch(sGpuCache, (uv)+(ivec2(1, 0)), 0), texelFetch(sGpuCache, (uv)+(ivec2(2, 0)), 0), texelFetch(sGpuCache, (uv)+(ivec2(3, 0)), 0), texelFetch(sGpuCache, (uv)+(ivec2(4, 0)), 0), texelFetch(sGpuCache, (uv)+(ivec2(5, 0)), 0), texelFetch(sGpuCache, (uv)+(ivec2(6, 0)), 0), texelFetch(sGpuCache, (uv)+(ivec2(7, 0)), 0)};
}
std::array<vec4,3> fetch_from_gpu_cache_3(I32 address) {
 ivec2 uv = get_gpu_cache_uv(address);
 return std::array<vec4,3>{texelFetch(sGpuCache, (uv)+(ivec2(0, 0)), 0), texelFetch(sGpuCache, (uv)+(ivec2(1, 0)), 0), texelFetch(sGpuCache, (uv)+(ivec2(2, 0)), 0)};
}
std::array<vec4,3> fetch_from_gpu_cache_3_direct(ivec2 address) {
 return std::array<vec4,3>{texelFetch(sGpuCache, (address)+(ivec2(0, 0)), 0), texelFetch(sGpuCache, (address)+(ivec2(1, 0)), 0), texelFetch(sGpuCache, (address)+(ivec2(2, 0)), 0)};
}
std::array<vec4,4> fetch_from_gpu_cache_4_direct(ivec2 address) {
 return std::array<vec4,4>{texelFetch(sGpuCache, (address)+(ivec2(0, 0)), 0), texelFetch(sGpuCache, (address)+(ivec2(1, 0)), 0), texelFetch(sGpuCache, (address)+(ivec2(2, 0)), 0), texelFetch(sGpuCache, (address)+(ivec2(3, 0)), 0)};
}
std::array<vec4,4> fetch_from_gpu_cache_4(I32 address) {
 ivec2 uv = get_gpu_cache_uv(address);
 return std::array<vec4,4>{texelFetch(sGpuCache, (uv)+(ivec2(0, 0)), 0), texelFetch(sGpuCache, (uv)+(ivec2(1, 0)), 0), texelFetch(sGpuCache, (uv)+(ivec2(2, 0)), 0), texelFetch(sGpuCache, (uv)+(ivec2(3, 0)), 0)};
}
struct ImageResource {
RectWithEndpoint uv_rect;
Float layer;
vec3 user_data;
ImageResource() = default;
ImageResource(RectWithEndpoint uv_rect, Float layer, vec3 user_data) : uv_rect(uv_rect), layer(layer), user_data(user_data){}
friend ImageResource if_then_else(I32 c, ImageResource t, ImageResource e) {
return ImageResource(
if_then_else(c, t.uv_rect, e.uv_rect), if_then_else(c, t.layer, e.layer), if_then_else(c, t.user_data, e.user_data));
}};
ImageResource fetch_image_resource(I32 address) {
 std::array<vec4,2> data = fetch_from_gpu_cache_2(address);
 RectWithEndpoint uv_rect = RectWithEndpoint((data[0]).sel(X, Y), (data[0]).sel(Z, W));
 return ImageResource(uv_rect, (data[1]).sel(X), (data[1]).sel(Y, Z, W));
}
ImageResource fetch_image_resource_direct(ivec2 address) {
 std::array<vec4,2> data = fetch_from_gpu_cache_2_direct(address);
 RectWithEndpoint uv_rect = RectWithEndpoint((data[0]).sel(X, Y), (data[0]).sel(Z, W));
 return ImageResource(uv_rect, (data[1]).sel(X), (data[1]).sel(Y, Z, W));
}
struct ImageResourceExtra {
vec4 st_tl;
vec4 st_tr;
vec4 st_bl;
vec4 st_br;
ImageResourceExtra() = default;
ImageResourceExtra(vec4 st_tl, vec4 st_tr, vec4 st_bl, vec4 st_br) : st_tl(st_tl), st_tr(st_tr), st_bl(st_bl), st_br(st_br){}
friend ImageResourceExtra if_then_else(I32 c, ImageResourceExtra t, ImageResourceExtra e) {
return ImageResourceExtra(
if_then_else(c, t.st_tl, e.st_tl), if_then_else(c, t.st_tr, e.st_tr), if_then_else(c, t.st_bl, e.st_bl), if_then_else(c, t.st_br, e.st_br));
}};
ImageResourceExtra fetch_image_resource_extra(I32 address) {
 std::array<vec4,4> data = fetch_from_gpu_cache_4((address)+(2));
 return ImageResourceExtra(data[0], data[1], data[2], data[3]);
}
vec4 vTransformBounds;
sampler2D sTransformPalette;
void init_transform_vs(vec4 local_bounds) {
 vTransformBounds = local_bounds;
}
struct Transform {
mat4 m;
mat4 inv_m;
Bool is_axis_aligned;
Transform() = default;
Transform(mat4 m, mat4 inv_m, Bool is_axis_aligned) : m(m), inv_m(inv_m), is_axis_aligned(is_axis_aligned){}
friend Transform if_then_else(I32 c, Transform t, Transform e) {
return Transform(
if_then_else(c, t.m, e.m), if_then_else(c, t.inv_m, e.inv_m), if_then_else(c, t.is_axis_aligned, e.is_axis_aligned));
}};
Transform fetch_transform(I32 id) {
 Transform transform;
 (transform).is_axis_aligned = ((id)>>(24))==(0);
 I32 index = (id)&(16777215);
 ivec2 uv = ivec2(I32((8u)*((uint(index))%((1024u)/(8u)))), I32((uint(index))/((1024u)/(8u))));
 ivec2 uv0 = ivec2(((uv).sel(X))+(0), (uv).sel(Y));
 (transform).m[0] = texelFetch(sTransformPalette, (uv0)+(ivec2(0, 0)), 0);
 (transform).m[1] = texelFetch(sTransformPalette, (uv0)+(ivec2(1, 0)), 0);
 (transform).m[2] = texelFetch(sTransformPalette, (uv0)+(ivec2(2, 0)), 0);
 (transform).m[3] = texelFetch(sTransformPalette, (uv0)+(ivec2(3, 0)), 0);
 (transform).inv_m[0] = texelFetch(sTransformPalette, (uv0)+(ivec2(4, 0)), 0);
 (transform).inv_m[1] = texelFetch(sTransformPalette, (uv0)+(ivec2(5, 0)), 0);
 (transform).inv_m[2] = texelFetch(sTransformPalette, (uv0)+(ivec2(6, 0)), 0);
 (transform).inv_m[3] = texelFetch(sTransformPalette, (uv0)+(ivec2(7, 0)), 0);
 return transform;
}
Bool ray_plane(vec3 normal, vec3 pt, vec3 ray_origin, vec3 ray_dir, Float& t) {
 I32 ret_mask = ~0;
 Bool ret;
 Float denom = dot(normal, ray_dir);
 {
  vec3 d = (pt)-(ray_origin);
  t = if_then_else((abs(denom))>(0.000001),(dot(d, normal))/(denom),t);
  ret = if_then_else(ret_mask & ((abs(denom))>(0.000001)), (t)>=(0.), ret);
  ret_mask &= ~((abs(denom))>(0.000001));
 }
 ret = if_then_else(ret_mask, false, ret);
 return ret;
}
vec4 untransform(vec2 ref, vec3 n, vec3 a, mat4 inv_transform) {
 vec3 p = vec3(ref, -(10000.));
 vec3 d = vec3(0, 0, 1.);
 Float t = 0.;
 ray_plane(n, a, p, d, t);
 Float z = ((p).sel(Z))+(((d).sel(Z))*(t));
 vec4 r = (inv_transform)*(vec4(ref, z, 1.));
 return r;
}
vec4 get_node_pos(vec2 pos, Transform transform) {
 vec4 ah = ((transform).m)*(vec4(0., 0., 0., 1.));
 vec3 a = ((ah).sel(X, Y, Z))/((ah).sel(W));
 vec3 n = (transpose(mat3((transform).inv_m)))*(vec3(0., 0., 1.));
 return untransform(pos, n, a, (transform).inv_m);
}
sampler2DArray sPrevPassAlpha;
sampler2DArray sPrevPassColor;
vec2 clamp_rect(vec2 pt, RectWithSize rect) {
 return clamp(pt, (rect).p0, ((rect).p0)+((rect).size));
}
vec4 vClipMaskUvBounds;
vec4 vClipMaskUv;
sampler2D sPrimitiveHeadersF;
isampler2D sPrimitiveHeadersI;
ivec4 aData;
struct PrimitiveHeader {
RectWithSize local_rect;
RectWithSize local_clip_rect;
Float z;
I32 specific_prim_address;
I32 transform_id;
ivec4 user_data;
PrimitiveHeader() = default;
PrimitiveHeader(RectWithSize local_rect, RectWithSize local_clip_rect, Float z, I32 specific_prim_address, I32 transform_id, ivec4 user_data) : local_rect(local_rect), local_clip_rect(local_clip_rect), z(z), specific_prim_address(specific_prim_address), transform_id(transform_id), user_data(user_data){}
friend PrimitiveHeader if_then_else(I32 c, PrimitiveHeader t, PrimitiveHeader e) {
return PrimitiveHeader(
if_then_else(c, t.local_rect, e.local_rect), if_then_else(c, t.local_clip_rect, e.local_clip_rect), if_then_else(c, t.z, e.z), if_then_else(c, t.specific_prim_address, e.specific_prim_address), if_then_else(c, t.transform_id, e.transform_id), if_then_else(c, t.user_data, e.user_data));
}};
PrimitiveHeader fetch_prim_header(I32 index) {
 PrimitiveHeader ph;
 ivec2 uv_f = ivec2(I32((2u)*((uint(index))%((1024u)/(2u)))), I32((uint(index))/((1024u)/(2u))));
 vec4 local_rect = texelFetch(sPrimitiveHeadersF, (uv_f)+(ivec2(0, 0)), 0);
 vec4 local_clip_rect = texelFetch(sPrimitiveHeadersF, (uv_f)+(ivec2(1, 0)), 0);
 (ph).local_rect = RectWithSize((local_rect).sel(X, Y), (local_rect).sel(Z, W));
 (ph).local_clip_rect = RectWithSize((local_clip_rect).sel(X, Y), (local_clip_rect).sel(Z, W));
 ivec2 uv_i = ivec2(I32((2u)*((uint(index))%((1024u)/(2u)))), I32((uint(index))/((1024u)/(2u))));
 ivec4 data0 = texelFetch(sPrimitiveHeadersI, (uv_i)+(ivec2(0, 0)), 0);
 ivec4 data1 = texelFetch(sPrimitiveHeadersI, (uv_i)+(ivec2(1, 0)), 0);
 (ph).z = Float((data0).sel(X));
 (ph).specific_prim_address = (data0).sel(Y);
 (ph).transform_id = (data0).sel(Z);
 (ph).user_data = data1;
 return ph;
}
struct VertexInfo {
vec2 local_pos;
vec2 snap_offset;
vec4 world_pos;
VertexInfo() = default;
VertexInfo(vec2 local_pos, vec2 snap_offset, vec4 world_pos) : local_pos(local_pos), snap_offset(snap_offset), world_pos(world_pos){}
friend VertexInfo if_then_else(I32 c, VertexInfo t, VertexInfo e) {
return VertexInfo(
if_then_else(c, t.local_pos, e.local_pos), if_then_else(c, t.snap_offset, e.snap_offset), if_then_else(c, t.world_pos, e.world_pos));
}};
VertexInfo write_vertex(RectWithSize instance_rect, RectWithSize local_clip_rect, Float z, Transform transform, PictureTask task) {
 vec2 local_pos = ((instance_rect).p0)+(((instance_rect).size)*((aPosition).sel(X, Y)));
 vec2 clamped_local_pos = clamp_rect(local_pos, local_clip_rect);
 vec4 world_pos = ((transform).m)*(vec4(clamped_local_pos, 0., 1.));
 vec2 device_pos = ((world_pos).sel(X, Y))*((task).device_pixel_scale);
 vec2 final_offset = (-((task).content_origin))+((((task).common_data).task_rect).p0);
 gl_Position = (uTransform)*(vec4((device_pos)+((final_offset)*((world_pos).sel(W))), (z)*((world_pos).sel(W)), (world_pos).sel(W)));
 VertexInfo vi = VertexInfo(clamped_local_pos, vec2(0., 0.), world_pos);
 return vi;
}
Float cross2(vec2 v0, vec2 v1) {
 return (((v0).sel(X))*((v1).sel(Y)))-(((v0).sel(Y))*((v1).sel(X)));
}
vec2 intersect_lines(vec2 p0, vec2 p1, vec2 p2, vec2 p3) {
 vec2 d0 = (p0)-(p1);
 vec2 d1 = (p2)-(p3);
 Float s0 = cross2(p0, p1);
 Float s1 = cross2(p2, p3);
 Float d = cross2(d0, d1);
 Float nx = ((s0)*((d1).sel(X)))-(((d0).sel(X))*(s1));
 Float ny = ((s0)*((d1).sel(Y)))-(((d0).sel(Y))*(s1));
 return vec2((nx)/(d), (ny)/(d));
}
VertexInfo write_transform_vertex(RectWithSize local_segment_rect, RectWithSize local_prim_rect, RectWithSize local_clip_rect, vec4 clip_edge_mask, Float z, Transform transform, PictureTask task) {
 RectWithEndpoint clip_rect = to_rect_with_endpoint(local_clip_rect);
 RectWithEndpoint segment_rect = to_rect_with_endpoint(local_segment_rect);
 (segment_rect).p0 = clamp((segment_rect).p0, (clip_rect).p0, (clip_rect).p1);
 (segment_rect).p1 = clamp((segment_rect).p1, (clip_rect).p0, (clip_rect).p1);
 RectWithEndpoint prim_rect = to_rect_with_endpoint(local_prim_rect);
 (prim_rect).p0 = clamp((prim_rect).p0, (clip_rect).p0, (clip_rect).p1);
 (prim_rect).p1 = clamp((prim_rect).p1, (clip_rect).p0, (clip_rect).p1);
 Float extrude_amount = 2.;
 vec4 extrude_distance = (vec4(extrude_amount))*(clip_edge_mask);
 (local_segment_rect).p0 -= (extrude_distance).sel(X, Y);
 (local_segment_rect).size += ((extrude_distance).sel(X, Y))+((extrude_distance).sel(Z, W));
 vec2 local_pos = ((local_segment_rect).p0)+(((local_segment_rect).size)*((aPosition).sel(X, Y)));
 vec2 task_offset = ((((task).common_data).task_rect).p0)-((task).content_origin);
 vec4 world_pos = ((transform).m)*(vec4(local_pos, 0., 1.));
 vec4 final_pos = vec4((((world_pos).sel(X, Y))*((task).device_pixel_scale))+((task_offset)*((world_pos).sel(W))), (z)*((world_pos).sel(W)), (world_pos).sel(W));
 gl_Position = (uTransform)*(final_pos);
 init_transform_vs(mix(vec4((prim_rect).p0, (prim_rect).p1), vec4((segment_rect).p0, (segment_rect).p1), clip_edge_mask));
 VertexInfo vi = VertexInfo(local_pos, vec2(0.), world_pos);
 return vi;
}
void write_clip(vec4 world_pos, vec2 snap_offset, ClipArea area) {
 vec2 uv = (((world_pos).sel(X, Y))*((area).device_pixel_scale))+(((world_pos).sel(W))*((snap_offset)+(((((area).common_data).task_rect).p0)-((area).screen_origin))));
 vClipMaskUvBounds = vec4((((area).common_data).task_rect).p0, ((((area).common_data).task_rect).p0)+((((area).common_data).task_rect).size));
 vClipMaskUv = vec4(uv, ((area).common_data).texture_layer_index, (world_pos).sel(W));
}
vec2 get_image_quad_uv(I32 address, vec2 f) {
 ImageResourceExtra extra_data = fetch_image_resource_extra(address);
 vec4 x = mix((extra_data).st_tl, (extra_data).st_tr, (f).sel(X));
 vec4 y = mix((extra_data).st_bl, (extra_data).st_br, (f).sel(X));
 vec4 z = mix(x, y, (f).sel(Y));
 return ((z).sel(X, Y))/((z).sel(W));
}
void main(void) {
 I32 prim_header_address = (aData).sel(X);
 I32 render_task_index = ((aData).sel(Y))>>(16);
 I32 clip_address = ((aData).sel(Y))&(65535);
 I32 segment_index = ((aData).sel(Z))&(65535);
 I32 edge_flags = (((aData).sel(Z))>>(16))&(255);
 I32 brush_flags = (((aData).sel(Z))>>(24))&(255);
 I32 segment_user_data = (aData).sel(W);
 PrimitiveHeader ph = fetch_prim_header(prim_header_address);
 vec4 segment_data;
 RectWithSize segment_rect;
 {
  segment_rect = if_then_else((segment_index)==(65535),(ph).local_rect,segment_rect);
  segment_data = if_then_else((segment_index)==(65535),vec4(0.),segment_data);
 }
 {
  I32 segment_address = ((ph).specific_prim_address)+((1)+((segment_index)*(2)));
  std::array<vec4,2> segment_info = fetch_from_gpu_cache_2(segment_address);
  segment_rect = if_then_else(!((segment_index)==(65535)),RectWithSize((segment_info[0]).sel(X, Y), (segment_info[0]).sel(Z, W)),segment_rect);
  (segment_rect).p0 = if_then_else(!((segment_index)==(65535)),(segment_rect).p0+((ph).local_rect).p0,(segment_rect).p0);
  segment_data = if_then_else(!((segment_index)==(65535)),segment_info[1],segment_data);
 }
 VertexInfo vi;
 PictureTask pic_task = fetch_picture_task(render_task_index);
 ClipArea clip_area = fetch_clip_area(clip_address);
 Transform transform = fetch_transform((ph).transform_id);
 {
  vi = if_then_else((transform).is_axis_aligned,write_vertex(segment_rect, (ph).local_clip_rect, (ph).z, transform, pic_task),vi);
  init_transform_vs(vec4(vec2(-(10000000000000000.)), vec2(10000000000000000.)));
 }
 {
  bvec4 edge_mask = notEqual((edge_flags)&(ivec4(1, 2, 4, 8)), ivec4(0));
  vi = if_then_else(!((transform).is_axis_aligned),write_transform_vertex(segment_rect, (ph).local_rect, (ph).local_clip_rect, mix(vec4(0.), vec4(1.), edge_mask), (ph).z, transform, pic_task),vi);
 }
 write_clip((vi).world_pos, (vi).snap_offset, clip_area);
 brush_vs(vi, (ph).specific_prim_address, (ph).local_rect, segment_rect, (ph).user_data, segment_user_data, (transform).m, pic_task, brush_flags, segment_data);
}
vec4 vColor;
vec2 vLocalPos;
struct SolidBrush {
vec4 color;
SolidBrush() = default;
SolidBrush(vec4 color) : color(color){}
friend SolidBrush if_then_else(I32 c, SolidBrush t, SolidBrush e) {
return SolidBrush(
if_then_else(c, t.color, e.color));
}};
SolidBrush fetch_solid_primitive(I32 address) {
 vec4 data = fetch_from_gpu_cache_1(address);
 return SolidBrush(data);
}
void brush_vs(VertexInfo vi, I32 prim_address, RectWithSize local_rect, RectWithSize segment_rect, ivec4 prim_user_data, I32 segment_user_data, mat4 transform, PictureTask pic_task, I32 brush_flags, vec4 unused) {
 SolidBrush prim = fetch_solid_primitive(prim_address);
 Float opacity = (Float((prim_user_data).sel(X)))/(65535.);
 vColor = ((prim).color)*(opacity);
 vLocalPos = (vi).local_pos;
}
};
