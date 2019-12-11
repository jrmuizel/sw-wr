/* inputs
vec3 aPosition
ivec4 aData
*/
/* outputs
vec4 vTransformBounds
vec4 vClipMaskUvBounds
vec4 vClipMaskUv
vec2 vUv
vec2 vLayerAndPerspective
vec4 vUvBounds
vec4 vUvSampleBounds
*/
struct brush_image_program : ProgramImpl {
const char *get_name() const override { return "brush_image"; }
int get_uniform(const char *name) const override {
 if (strcmp("sColor0", name) == 0) { return 7; }
 if (strcmp("sGpuCache", name) == 0) { return 2; }
 if (strcmp("sPrevPassAlpha", name) == 0) { return 8; }
 if (strcmp("sPrimitiveHeadersF", name) == 0) { return 4; }
 if (strcmp("sPrimitiveHeadersI", name) == 0) { return 5; }
 if (strcmp("sRenderTasks", name) == 0) { return 1; }
 if (strcmp("sTransformPalette", name) == 0) { return 3; }
 if (strcmp("uTransform", name) == 0) { return 6; }
 return -1;
}
struct Samplers {
 sampler2DArray_impl sColor0_impl;
 int sColor0_slot;
 sampler2D_impl sGpuCache_impl;
 int sGpuCache_slot;
 sampler2DArray_impl sPrevPassAlpha_impl;
 int sPrevPassAlpha_slot;
 sampler2D_impl sPrimitiveHeadersF_impl;
 int sPrimitiveHeadersF_slot;
 isampler2D_impl sPrimitiveHeadersI_impl;
 int sPrimitiveHeadersI_slot;
 sampler2D_impl sRenderTasks_impl;
 int sRenderTasks_slot;
 sampler2D_impl sTransformPalette_impl;
 int sTransformPalette_slot;
} samplers;
bool set_sampler(int index, int value) override {
 switch (index) {
 case 7:
  samplers.sColor0_slot = value;
  return true;
 case 2:
  samplers.sGpuCache_slot = value;
  return true;
 case 8:
  samplers.sPrevPassAlpha_slot = value;
  return true;
 case 4:
  samplers.sPrimitiveHeadersF_slot = value;
  return true;
 case 5:
  samplers.sPrimitiveHeadersI_slot = value;
  return true;
 case 1:
  samplers.sRenderTasks_slot = value;
  return true;
 case 3:
  samplers.sTransformPalette_slot = value;
  return true;
 }
 return false;
}
struct AttribLocations {
 int aPosition;
 int aData;
} attrib_locations;
void bind_attrib(const char *name, int index) override {
 if (strcmp("aPosition", name) == 0) { attrib_locations.aPosition = index; return; }
 if (strcmp("aData", name) == 0) { attrib_locations.aData = index; return; }
}
void init_shaders(void *vertex_shader, void *fragment_shader) override;
};
struct brush_image_vert : VertexShaderImpl {
typedef brush_image_vert Self;
static void set_uniform_1i(Self *self, int index, int value) {
 switch (index) {
 case 6:
  assert(0); // uTransform
  break;
 case 7:
  assert(0); // sColor0
  break;
 case 1:
  assert(0); // sRenderTasks
  break;
 case 2:
  assert(0); // sGpuCache
  break;
 case 3:
  assert(0); // sTransformPalette
  break;
 case 4:
  assert(0); // sPrimitiveHeadersF
  break;
 case 5:
  assert(0); // sPrimitiveHeadersI
  break;
 }
}
static void set_uniform_4fv(Self *self, int index, const float *value) {
 switch (index) {
 case 6:
  assert(0); // uTransform
  break;
 case 7:
  assert(0); // sColor0
  break;
 case 1:
  assert(0); // sRenderTasks
  break;
 case 2:
  assert(0); // sGpuCache
  break;
 case 3:
  assert(0); // sTransformPalette
  break;
 case 4:
  assert(0); // sPrimitiveHeadersF
  break;
 case 5:
  assert(0); // sPrimitiveHeadersI
  break;
 }
}
static void set_uniform_matrix4fv(Self *self, int index, const float *value) {
 switch (index) {
 case 6:
  self->uTransform = mat4_scalar::load_from_ptr(value);
  break;
 case 7:
  assert(0); // sColor0
  break;
 case 1:
  assert(0); // sRenderTasks
  break;
 case 2:
  assert(0); // sGpuCache
  break;
 case 3:
  assert(0); // sTransformPalette
  break;
 case 4:
  assert(0); // sPrimitiveHeadersF
  break;
 case 5:
  assert(0); // sPrimitiveHeadersI
  break;
 }
}
static void load_attribs(Self *self, brush_image_program *prog, VertexAttrib *attribs, unsigned short *indices, int start, int instance, int count) {
 load_attrib(self->aPosition, attribs[prog->attrib_locations.aPosition], indices, start, instance, count);
 load_attrib(self->aData, attribs[prog->attrib_locations.aData], indices, start, instance, count);
}
struct FlatOutputs {
vec4_scalar vTransformBounds;
vec4_scalar vClipMaskUvBounds;
vec2_scalar vLayerAndPerspective;
vec4_scalar vUvBounds;
vec4_scalar vUvSampleBounds;
};
struct InterpOutputs {
vec4_scalar vClipMaskUv;
vec2_scalar vUv;
};
ALWAYS_INLINE void store_flat_outputs(char* dest_ptr) {
  auto* dest = reinterpret_cast<FlatOutputs*>(dest_ptr);
  dest->vTransformBounds = vTransformBounds;
  dest->vClipMaskUvBounds = vClipMaskUvBounds;
  dest->vLayerAndPerspective = vLayerAndPerspective;
  dest->vUvBounds = vUvBounds;
  dest->vUvSampleBounds = vUvSampleBounds;
}
ALWAYS_INLINE void store_interp_outputs(char* dest_ptr, size_t stride) {
  for(int n = 0; n < 4; n++) {
    auto* dest = reinterpret_cast<InterpOutputs*>(dest_ptr);
    dest->vClipMaskUv = get_nth(vClipMaskUv, n);
    dest->vUv = get_nth(vUv, n);
    dest_ptr += stride;
  }
}
static void bind_textures(Self *self, brush_image_program *prog) {
 self->sColor0 = lookup_sampler_array(&prog->samplers.sColor0_impl, prog->samplers.sColor0_slot);
 self->sRenderTasks = lookup_sampler(&prog->samplers.sRenderTasks_impl, prog->samplers.sRenderTasks_slot);
 self->sGpuCache = lookup_sampler(&prog->samplers.sGpuCache_impl, prog->samplers.sGpuCache_slot);
 self->sTransformPalette = lookup_sampler(&prog->samplers.sTransformPalette_impl, prog->samplers.sTransformPalette_slot);
 self->sPrimitiveHeadersF = lookup_sampler(&prog->samplers.sPrimitiveHeadersF_impl, prog->samplers.sPrimitiveHeadersF_slot);
 self->sPrimitiveHeadersI = lookup_isampler(&prog->samplers.sPrimitiveHeadersI_impl, prog->samplers.sPrimitiveHeadersI_slot);
}
int32_t uMode;
mat4_scalar uTransform;
vec3 aPosition;
sampler2DArray sColor0;
sampler2DArray sColor1;
sampler2DArray sColor2;
struct RectWithSize_scalar {
vec2_scalar p0;
vec2_scalar size;
RectWithSize_scalar() = default;
RectWithSize_scalar(vec2_scalar p0, vec2_scalar size) : p0(p0), size(size){}
};
struct RectWithSize {
vec2 p0;
vec2 size;
RectWithSize() = default;
RectWithSize(vec2 p0, vec2 size) : p0(p0), size(size){}
RectWithSize(vec2_scalar p0, vec2_scalar size):p0(p0),size(size){
}
RectWithSize(RectWithSize_scalar s):p0(s.p0),size(s.size){
}
friend RectWithSize if_then_else(I32 c, RectWithSize t, RectWithSize e) { return RectWithSize(
if_then_else(c, t.p0, e.p0), if_then_else(c, t.size, e.size));
}};
struct RectWithEndpoint_scalar {
vec2_scalar p0;
vec2_scalar p1;
RectWithEndpoint_scalar() = default;
RectWithEndpoint_scalar(vec2_scalar p0, vec2_scalar p1) : p0(p0), p1(p1){}
};
struct RectWithEndpoint {
vec2 p0;
vec2 p1;
RectWithEndpoint() = default;
RectWithEndpoint(vec2 p0, vec2 p1) : p0(p0), p1(p1){}
RectWithEndpoint(vec2_scalar p0, vec2_scalar p1):p0(p0),p1(p1){
}
RectWithEndpoint(RectWithEndpoint_scalar s):p0(s.p0),p1(s.p1){
}
friend RectWithEndpoint if_then_else(I32 c, RectWithEndpoint t, RectWithEndpoint e) { return RectWithEndpoint(
if_then_else(c, t.p0, e.p0), if_then_else(c, t.p1, e.p1));
}};
sampler2D sRenderTasks;
struct RenderTaskCommonData_scalar {
RectWithSize_scalar task_rect;
float texture_layer_index;
RenderTaskCommonData_scalar() = default;
RenderTaskCommonData_scalar(RectWithSize_scalar task_rect, float texture_layer_index) : task_rect(task_rect), texture_layer_index(texture_layer_index){}
};
struct RenderTaskCommonData {
RectWithSize task_rect;
Float texture_layer_index;
RenderTaskCommonData() = default;
RenderTaskCommonData(RectWithSize task_rect, Float texture_layer_index) : task_rect(task_rect), texture_layer_index(texture_layer_index){}
RenderTaskCommonData(RectWithSize_scalar task_rect, float texture_layer_index):task_rect(task_rect),texture_layer_index(texture_layer_index){
}
RenderTaskCommonData(RenderTaskCommonData_scalar s):task_rect(s.task_rect),texture_layer_index(s.texture_layer_index){
}
friend RenderTaskCommonData if_then_else(I32 c, RenderTaskCommonData t, RenderTaskCommonData e) { return RenderTaskCommonData(
if_then_else(c, t.task_rect, e.task_rect), if_then_else(c, t.texture_layer_index, e.texture_layer_index));
}};
struct RenderTaskData_scalar {
RenderTaskCommonData_scalar common_data;
vec3_scalar user_data;
RenderTaskData_scalar() = default;
RenderTaskData_scalar(RenderTaskCommonData_scalar common_data, vec3_scalar user_data) : common_data(common_data), user_data(user_data){}
};
struct RenderTaskData {
RenderTaskCommonData common_data;
vec3 user_data;
RenderTaskData() = default;
RenderTaskData(RenderTaskCommonData common_data, vec3 user_data) : common_data(common_data), user_data(user_data){}
RenderTaskData(RenderTaskCommonData_scalar common_data, vec3_scalar user_data):common_data(common_data),user_data(user_data){
}
RenderTaskData(RenderTaskData_scalar s):common_data(s.common_data),user_data(s.user_data){
}
friend RenderTaskData if_then_else(I32 c, RenderTaskData t, RenderTaskData e) { return RenderTaskData(
if_then_else(c, t.common_data, e.common_data), if_then_else(c, t.user_data, e.user_data));
}};
struct PictureTask_scalar {
RenderTaskCommonData_scalar common_data;
float device_pixel_scale;
vec2_scalar content_origin;
PictureTask_scalar() = default;
PictureTask_scalar(RenderTaskCommonData_scalar common_data, float device_pixel_scale, vec2_scalar content_origin) : common_data(common_data), device_pixel_scale(device_pixel_scale), content_origin(content_origin){}
};
struct PictureTask {
RenderTaskCommonData common_data;
Float device_pixel_scale;
vec2 content_origin;
PictureTask() = default;
PictureTask(RenderTaskCommonData common_data, Float device_pixel_scale, vec2 content_origin) : common_data(common_data), device_pixel_scale(device_pixel_scale), content_origin(content_origin){}
PictureTask(RenderTaskCommonData_scalar common_data, float device_pixel_scale, vec2_scalar content_origin):common_data(common_data),device_pixel_scale(device_pixel_scale),content_origin(content_origin){
}
PictureTask(PictureTask_scalar s):common_data(s.common_data),device_pixel_scale(s.device_pixel_scale),content_origin(s.content_origin){
}
friend PictureTask if_then_else(I32 c, PictureTask t, PictureTask e) { return PictureTask(
if_then_else(c, t.common_data, e.common_data), if_then_else(c, t.device_pixel_scale, e.device_pixel_scale), if_then_else(c, t.content_origin, e.content_origin));
}};
struct ClipArea_scalar {
RenderTaskCommonData_scalar common_data;
float device_pixel_scale;
vec2_scalar screen_origin;
ClipArea_scalar() = default;
ClipArea_scalar(RenderTaskCommonData_scalar common_data, float device_pixel_scale, vec2_scalar screen_origin) : common_data(common_data), device_pixel_scale(device_pixel_scale), screen_origin(screen_origin){}
};
struct ClipArea {
RenderTaskCommonData common_data;
Float device_pixel_scale;
vec2 screen_origin;
ClipArea() = default;
ClipArea(RenderTaskCommonData common_data, Float device_pixel_scale, vec2 screen_origin) : common_data(common_data), device_pixel_scale(device_pixel_scale), screen_origin(screen_origin){}
ClipArea(RenderTaskCommonData_scalar common_data, float device_pixel_scale, vec2_scalar screen_origin):common_data(common_data),device_pixel_scale(device_pixel_scale),screen_origin(screen_origin){
}
ClipArea(ClipArea_scalar s):common_data(s.common_data),device_pixel_scale(s.device_pixel_scale),screen_origin(s.screen_origin){
}
friend ClipArea if_then_else(I32 c, ClipArea t, ClipArea e) { return ClipArea(
if_then_else(c, t.common_data, e.common_data), if_then_else(c, t.device_pixel_scale, e.device_pixel_scale), if_then_else(c, t.screen_origin, e.screen_origin));
}};
sampler2D sGpuCache;
struct ImageResource_scalar {
RectWithEndpoint_scalar uv_rect;
float layer;
vec3_scalar user_data;
ImageResource_scalar() = default;
ImageResource_scalar(RectWithEndpoint_scalar uv_rect, float layer, vec3_scalar user_data) : uv_rect(uv_rect), layer(layer), user_data(user_data){}
};
struct ImageResource {
RectWithEndpoint uv_rect;
Float layer;
vec3 user_data;
ImageResource() = default;
ImageResource(RectWithEndpoint uv_rect, Float layer, vec3 user_data) : uv_rect(uv_rect), layer(layer), user_data(user_data){}
ImageResource(RectWithEndpoint_scalar uv_rect, float layer, vec3_scalar user_data):uv_rect(uv_rect),layer(layer),user_data(user_data){
}
ImageResource(ImageResource_scalar s):uv_rect(s.uv_rect),layer(s.layer),user_data(s.user_data){
}
friend ImageResource if_then_else(I32 c, ImageResource t, ImageResource e) { return ImageResource(
if_then_else(c, t.uv_rect, e.uv_rect), if_then_else(c, t.layer, e.layer), if_then_else(c, t.user_data, e.user_data));
}};
struct ImageResourceExtra_scalar {
vec4_scalar st_tl;
vec4_scalar st_tr;
vec4_scalar st_bl;
vec4_scalar st_br;
ImageResourceExtra_scalar() = default;
ImageResourceExtra_scalar(vec4_scalar st_tl, vec4_scalar st_tr, vec4_scalar st_bl, vec4_scalar st_br) : st_tl(st_tl), st_tr(st_tr), st_bl(st_bl), st_br(st_br){}
};
struct ImageResourceExtra {
vec4 st_tl;
vec4 st_tr;
vec4 st_bl;
vec4 st_br;
ImageResourceExtra() = default;
ImageResourceExtra(vec4 st_tl, vec4 st_tr, vec4 st_bl, vec4 st_br) : st_tl(st_tl), st_tr(st_tr), st_bl(st_bl), st_br(st_br){}
ImageResourceExtra(vec4_scalar st_tl, vec4_scalar st_tr, vec4_scalar st_bl, vec4_scalar st_br):st_tl(st_tl),st_tr(st_tr),st_bl(st_bl),st_br(st_br){
}
ImageResourceExtra(ImageResourceExtra_scalar s):st_tl(s.st_tl),st_tr(s.st_tr),st_bl(s.st_bl),st_br(s.st_br){
}
friend ImageResourceExtra if_then_else(I32 c, ImageResourceExtra t, ImageResourceExtra e) { return ImageResourceExtra(
if_then_else(c, t.st_tl, e.st_tl), if_then_else(c, t.st_tr, e.st_tr), if_then_else(c, t.st_bl, e.st_bl), if_then_else(c, t.st_br, e.st_br));
}};
vec4_scalar vTransformBounds;
sampler2D sTransformPalette;
struct Transform_scalar {
mat4_scalar m;
mat4_scalar inv_m;
bool is_axis_aligned;
Transform_scalar() = default;
Transform_scalar(mat4_scalar m, mat4_scalar inv_m, bool is_axis_aligned) : m(m), inv_m(inv_m), is_axis_aligned(is_axis_aligned){}
};
struct Transform {
mat4 m;
mat4 inv_m;
Bool is_axis_aligned;
Transform() = default;
Transform(mat4 m, mat4 inv_m, Bool is_axis_aligned) : m(m), inv_m(inv_m), is_axis_aligned(is_axis_aligned){}
Transform(mat4_scalar m, mat4_scalar inv_m, bool is_axis_aligned):m(m),inv_m(inv_m),is_axis_aligned(is_axis_aligned){
}
Transform(Transform_scalar s):m(s.m),inv_m(s.inv_m),is_axis_aligned(s.is_axis_aligned){
}
friend Transform if_then_else(I32 c, Transform t, Transform e) { return Transform(
if_then_else(c, t.m, e.m), if_then_else(c, t.inv_m, e.inv_m), if_then_else(c, t.is_axis_aligned, e.is_axis_aligned));
}};
sampler2DArray sPrevPassAlpha;
sampler2DArray sPrevPassColor;
vec4_scalar vClipMaskUvBounds;
vec4 vClipMaskUv;
sampler2D sPrimitiveHeadersF;
isampler2D sPrimitiveHeadersI;
ivec4 aData;
struct PrimitiveHeader_scalar {
RectWithSize_scalar local_rect;
RectWithSize_scalar local_clip_rect;
float z;
int32_t specific_prim_address;
int32_t transform_id;
ivec4_scalar user_data;
PrimitiveHeader_scalar() = default;
PrimitiveHeader_scalar(RectWithSize_scalar local_rect, RectWithSize_scalar local_clip_rect, float z, int32_t specific_prim_address, int32_t transform_id, ivec4_scalar user_data) : local_rect(local_rect), local_clip_rect(local_clip_rect), z(z), specific_prim_address(specific_prim_address), transform_id(transform_id), user_data(user_data){}
};
struct PrimitiveHeader {
RectWithSize local_rect;
RectWithSize local_clip_rect;
Float z;
I32 specific_prim_address;
I32 transform_id;
ivec4 user_data;
PrimitiveHeader() = default;
PrimitiveHeader(RectWithSize local_rect, RectWithSize local_clip_rect, Float z, I32 specific_prim_address, I32 transform_id, ivec4 user_data) : local_rect(local_rect), local_clip_rect(local_clip_rect), z(z), specific_prim_address(specific_prim_address), transform_id(transform_id), user_data(user_data){}
PrimitiveHeader(RectWithSize_scalar local_rect, RectWithSize_scalar local_clip_rect, float z, int32_t specific_prim_address, int32_t transform_id, ivec4_scalar user_data):local_rect(local_rect),local_clip_rect(local_clip_rect),z(z),specific_prim_address(specific_prim_address),transform_id(transform_id),user_data(user_data){
}
PrimitiveHeader(PrimitiveHeader_scalar s):local_rect(s.local_rect),local_clip_rect(s.local_clip_rect),z(s.z),specific_prim_address(s.specific_prim_address),transform_id(s.transform_id),user_data(s.user_data){
}
friend PrimitiveHeader if_then_else(I32 c, PrimitiveHeader t, PrimitiveHeader e) { return PrimitiveHeader(
if_then_else(c, t.local_rect, e.local_rect), if_then_else(c, t.local_clip_rect, e.local_clip_rect), if_then_else(c, t.z, e.z), if_then_else(c, t.specific_prim_address, e.specific_prim_address), if_then_else(c, t.transform_id, e.transform_id), if_then_else(c, t.user_data, e.user_data));
}};
struct VertexInfo_scalar {
vec2_scalar local_pos;
vec2_scalar snap_offset;
vec4_scalar world_pos;
VertexInfo_scalar() = default;
VertexInfo_scalar(vec2_scalar local_pos, vec2_scalar snap_offset, vec4_scalar world_pos) : local_pos(local_pos), snap_offset(snap_offset), world_pos(world_pos){}
};
struct VertexInfo {
vec2 local_pos;
vec2 snap_offset;
vec4 world_pos;
VertexInfo() = default;
VertexInfo(vec2 local_pos, vec2 snap_offset, vec4 world_pos) : local_pos(local_pos), snap_offset(snap_offset), world_pos(world_pos){}
VertexInfo(vec2_scalar local_pos, vec2_scalar snap_offset, vec4_scalar world_pos):local_pos(local_pos),snap_offset(snap_offset),world_pos(world_pos){
}
VertexInfo(VertexInfo_scalar s):local_pos(s.local_pos),snap_offset(s.snap_offset),world_pos(s.world_pos){
}
friend VertexInfo if_then_else(I32 c, VertexInfo t, VertexInfo e) { return VertexInfo(
if_then_else(c, t.local_pos, e.local_pos), if_then_else(c, t.snap_offset, e.snap_offset), if_then_else(c, t.world_pos, e.world_pos));
}};
vec2 vUv;
vec2_scalar vLayerAndPerspective;
vec4_scalar vUvBounds;
vec4_scalar vUvSampleBounds;
struct ImageBrushData_scalar {
vec4_scalar color;
vec4_scalar background_color;
vec2_scalar stretch_size;
ImageBrushData_scalar() = default;
ImageBrushData_scalar(vec4_scalar color, vec4_scalar background_color, vec2_scalar stretch_size) : color(color), background_color(background_color), stretch_size(stretch_size){}
};
struct ImageBrushData {
vec4 color;
vec4 background_color;
vec2 stretch_size;
ImageBrushData() = default;
ImageBrushData(vec4 color, vec4 background_color, vec2 stretch_size) : color(color), background_color(background_color), stretch_size(stretch_size){}
ImageBrushData(vec4_scalar color, vec4_scalar background_color, vec2_scalar stretch_size):color(color),background_color(background_color),stretch_size(stretch_size){
}
ImageBrushData(ImageBrushData_scalar s):color(s.color),background_color(s.background_color),stretch_size(s.stretch_size){
}
friend ImageBrushData if_then_else(I32 c, ImageBrushData t, ImageBrushData e) { return ImageBrushData(
if_then_else(c, t.color, e.color), if_then_else(c, t.background_color, e.background_color), if_then_else(c, t.stretch_size, e.stretch_size));
}};
PrimitiveHeader fetch_prim_header(I32 index) {
 PrimitiveHeader ph;
 ivec2 uv_f = make_ivec2(make_int((2u)*((make_uint(index))%((1024u)/(2u)))), make_int((make_uint(index))/((1024u)/(2u))));
 vec4 local_rect = texelFetch(sPrimitiveHeadersF, (uv_f)+(make_ivec2(0, 0)), 0);
 vec4 local_clip_rect = texelFetch(sPrimitiveHeadersF, (uv_f)+(make_ivec2(1, 0)), 0);
 (ph).local_rect = RectWithSize((local_rect).sel(X, Y), (local_rect).sel(Z, W));
 (ph).local_clip_rect = RectWithSize((local_clip_rect).sel(X, Y), (local_clip_rect).sel(Z, W));
 ivec2 uv_i = make_ivec2(make_int((2u)*((make_uint(index))%((1024u)/(2u)))), make_int((make_uint(index))/((1024u)/(2u))));
 ivec4 data0 = texelFetch(sPrimitiveHeadersI, (uv_i)+(make_ivec2(0, 0)), 0);
 ivec4 data1 = texelFetch(sPrimitiveHeadersI, (uv_i)+(make_ivec2(1, 0)), 0);
 (ph).z = make_float((data0).sel(X));
 (ph).specific_prim_address = (data0).sel(Y);
 (ph).transform_id = (data0).sel(Z);
 (ph).user_data = data1;
 return ph;
}
ivec2 get_gpu_cache_uv(I32 address) {
 return make_ivec2((make_uint(address))%(1024u), (make_uint(address))/(1024u));
}
std::array<vec4,2> fetch_from_gpu_cache_2(I32 address) {
 ivec2 uv = get_gpu_cache_uv(address);
 return std::array<vec4,2>{texelFetch(sGpuCache, (uv)+(make_ivec2(0, 0)), 0), texelFetch(sGpuCache, (uv)+(make_ivec2(1, 0)), 0)};
}
RenderTaskData fetch_render_task_data(I32 index) {
 ivec2 uv = make_ivec2(make_int((2u)*((make_uint(index))%((1024u)/(2u)))), make_int((make_uint(index))/((1024u)/(2u))));
 vec4 texel0 = texelFetch(sRenderTasks, (uv)+(make_ivec2(0, 0)), 0);
 vec4 texel1 = texelFetch(sRenderTasks, (uv)+(make_ivec2(1, 0)), 0);
 RectWithSize task_rect = RectWithSize((texel0).sel(X, Y), (texel0).sel(Z, W));
 RenderTaskCommonData common_data = RenderTaskCommonData(task_rect, (texel1).sel(X));
 RenderTaskData data = RenderTaskData(common_data, (texel1).sel(Y, Z, W));
 return data;
}
PictureTask fetch_picture_task(I32 address) {
 RenderTaskData task_data = fetch_render_task_data(address);
 PictureTask task = PictureTask((task_data).common_data, ((task_data).user_data).sel(X), ((task_data).user_data).sel(Y, Z));
 return task;
}
ClipArea fetch_clip_area(I32 index) {
 ClipArea area;
 auto _c6_ = (index)>=(32767);
 {
  RectWithSize_scalar rect = RectWithSize_scalar(make_vec2(0.), make_vec2(0.));
  (area).common_data = if_then_else(_c6_,RenderTaskCommonData_scalar(rect, 0.),(area).common_data);
  (area).device_pixel_scale = if_then_else(_c6_,0.,(area).device_pixel_scale);
  (area).screen_origin = if_then_else(_c6_,make_vec2(0.),(area).screen_origin);
 }
 {
  RenderTaskData task_data = fetch_render_task_data(index);
  (area).common_data = if_then_else(~(_c6_),(task_data).common_data,(area).common_data);
  (area).device_pixel_scale = if_then_else(~(_c6_),((task_data).user_data).sel(X),(area).device_pixel_scale);
  (area).screen_origin = if_then_else(~(_c6_),((task_data).user_data).sel(Y, Z),(area).screen_origin);
 }
 return area;
}
Transform fetch_transform(I32 id) {
 Transform transform;
 (transform).is_axis_aligned = ((id)>>(24))==(0);
 I32 index = (id)&(16777215);
 ivec2 uv = make_ivec2(make_int((8u)*((make_uint(index))%((1024u)/(8u)))), make_int((make_uint(index))/((1024u)/(8u))));
 ivec2 uv0 = make_ivec2(((uv).sel(X))+(0), (uv).sel(Y));
 (transform).m[0] = texelFetch(sTransformPalette, (uv0)+(make_ivec2(0, 0)), 0);
 (transform).m[1] = texelFetch(sTransformPalette, (uv0)+(make_ivec2(1, 0)), 0);
 (transform).m[2] = texelFetch(sTransformPalette, (uv0)+(make_ivec2(2, 0)), 0);
 (transform).m[3] = texelFetch(sTransformPalette, (uv0)+(make_ivec2(3, 0)), 0);
 (transform).inv_m[0] = texelFetch(sTransformPalette, (uv0)+(make_ivec2(4, 0)), 0);
 (transform).inv_m[1] = texelFetch(sTransformPalette, (uv0)+(make_ivec2(5, 0)), 0);
 (transform).inv_m[2] = texelFetch(sTransformPalette, (uv0)+(make_ivec2(6, 0)), 0);
 (transform).inv_m[3] = texelFetch(sTransformPalette, (uv0)+(make_ivec2(7, 0)), 0);
 return transform;
}
vec2 clamp_rect(vec2 pt, RectWithSize rect) {
 return clamp(pt, (rect).p0, ((rect).p0)+((rect).size));
}
VertexInfo write_vertex(RectWithSize instance_rect, RectWithSize local_clip_rect, Float z, Transform transform, PictureTask task, I32 _cond_mask_) {
 vec2 local_pos = ((instance_rect).p0)+(((instance_rect).size)*((aPosition).sel(X, Y)));
 vec2 clamped_local_pos = clamp_rect(local_pos, local_clip_rect);
 vec4 world_pos = ((transform).m)*(make_vec4(clamped_local_pos, 0., 1.));
 vec2 device_pos = ((world_pos).sel(X, Y))*((task).device_pixel_scale);
 vec2 final_offset = (-((task).content_origin))+((((task).common_data).task_rect).p0);
 gl_Position = if_then_else(_cond_mask_,(uTransform)*(make_vec4((device_pos)+((final_offset)*((world_pos).sel(W))), (z)*((world_pos).sel(W)), (world_pos).sel(W))),gl_Position);
 VertexInfo vi = VertexInfo(clamped_local_pos, make_vec2(0., 0.), world_pos);
 return vi;
}
RectWithEndpoint to_rect_with_endpoint(RectWithSize rect) {
 RectWithEndpoint result;
 (result).p0 = (rect).p0;
 (result).p1 = ((rect).p0)+((rect).size);
 return result;
}
void init_transform_vs(vec4 local_bounds, I32 _cond_mask_) {
 if (_cond_mask_[0]) { vTransformBounds = force_scalar(local_bounds); };
}
VertexInfo write_transform_vertex(RectWithSize local_segment_rect, RectWithSize local_prim_rect, RectWithSize local_clip_rect, vec4 clip_edge_mask, Float z, Transform transform, PictureTask task, I32 _cond_mask_) {
 RectWithEndpoint clip_rect = to_rect_with_endpoint(local_clip_rect);
 RectWithEndpoint segment_rect = to_rect_with_endpoint(local_segment_rect);
 (segment_rect).p0 = if_then_else(_cond_mask_,clamp((segment_rect).p0, (clip_rect).p0, (clip_rect).p1),(segment_rect).p0);
 (segment_rect).p1 = if_then_else(_cond_mask_,clamp((segment_rect).p1, (clip_rect).p0, (clip_rect).p1),(segment_rect).p1);
 RectWithEndpoint prim_rect = to_rect_with_endpoint(local_prim_rect);
 (prim_rect).p0 = if_then_else(_cond_mask_,clamp((prim_rect).p0, (clip_rect).p0, (clip_rect).p1),(prim_rect).p0);
 (prim_rect).p1 = if_then_else(_cond_mask_,clamp((prim_rect).p1, (clip_rect).p0, (clip_rect).p1),(prim_rect).p1);
 float extrude_amount = 2.;
 vec4 extrude_distance = (make_vec4(extrude_amount))*(clip_edge_mask);
 (local_segment_rect).p0 = if_then_else(_cond_mask_,(local_segment_rect).p0-(extrude_distance).sel(X, Y),(local_segment_rect).p0);
 (local_segment_rect).size = if_then_else(_cond_mask_,(local_segment_rect).size+((extrude_distance).sel(X, Y))+((extrude_distance).sel(Z, W)),(local_segment_rect).size);
 vec2 local_pos = ((local_segment_rect).p0)+(((local_segment_rect).size)*((aPosition).sel(X, Y)));
 vec2 task_offset = ((((task).common_data).task_rect).p0)-((task).content_origin);
 vec4 world_pos = ((transform).m)*(make_vec4(local_pos, 0., 1.));
 vec4 final_pos = make_vec4((((world_pos).sel(X, Y))*((task).device_pixel_scale))+((task_offset)*((world_pos).sel(W))), (z)*((world_pos).sel(W)), (world_pos).sel(W));
 gl_Position = if_then_else(_cond_mask_,(uTransform)*(final_pos),gl_Position);
 init_transform_vs(mix(make_vec4((prim_rect).p0, (prim_rect).p1), make_vec4((segment_rect).p0, (segment_rect).p1), clip_edge_mask), _cond_mask_);
 VertexInfo vi = VertexInfo(local_pos, make_vec2(0.), world_pos);
 return vi;
}
std::array<vec4,3> fetch_from_gpu_cache_3(I32 address) {
 ivec2 uv = get_gpu_cache_uv(address);
 return std::array<vec4,3>{texelFetch(sGpuCache, (uv)+(make_ivec2(0, 0)), 0), texelFetch(sGpuCache, (uv)+(make_ivec2(1, 0)), 0), texelFetch(sGpuCache, (uv)+(make_ivec2(2, 0)), 0)};
}
ImageBrushData fetch_image_data(I32 address) {
 std::array<vec4,3> raw_data = fetch_from_gpu_cache_3(address);
 ImageBrushData data = ImageBrushData(raw_data[0], raw_data[1], (raw_data[2]).sel(X, Y));
 return data;
}
ImageResource fetch_image_resource(I32 address) {
 std::array<vec4,2> data = fetch_from_gpu_cache_2(address);
 RectWithEndpoint uv_rect = RectWithEndpoint((data[0]).sel(X, Y), (data[0]).sel(Z, W));
 return ImageResource(uv_rect, (data[1]).sel(X), (data[1]).sel(Y, Z, W));
}
void brush_vs(VertexInfo vi, I32 prim_address, RectWithSize prim_rect, RectWithSize segment_rect, ivec4 prim_user_data, I32 segment_user_data, mat4 transform, PictureTask pic_task, I32 brush_flags, vec4 segment_data) {
 ImageBrushData image_data = fetch_image_data(prim_address);
 vec2_scalar texture_size = make_vec2(textureSize(sColor0, 0));
 ImageResource res = fetch_image_resource(segment_user_data);
 vec2 uv0 = ((res).uv_rect).p0;
 vec2 uv1 = ((res).uv_rect).p1;
 RectWithSize local_rect = prim_rect;
 vec2 stretch_size = (image_data).stretch_size;
 auto _c15_ = ((stretch_size).sel(X))<(0.);
 {
  stretch_size = if_then_else(_c15_,(local_rect).size,stretch_size);
 }
 auto _c16_ = ((brush_flags)&(2))!=(0);
 {
  local_rect = if_then_else(_c16_,segment_rect,local_rect);
  stretch_size = if_then_else(_c16_,(local_rect).size,stretch_size);
    auto _c17_ = (_c16_)&(((brush_flags)&(4))!=(0));
  {
   (stretch_size).sel(X) = if_then_else(_c17_,((segment_data).sel(Z))-((segment_data).sel(X)),(stretch_size).sel(X));
  }
    auto _c18_ = (_c16_)&(((brush_flags)&(8))!=(0));
  {
   (stretch_size).sel(Y) = if_then_else(_c18_,((segment_data).sel(W))-((segment_data).sel(Y)),(stretch_size).sel(Y));
  }
    auto _c19_ = (_c16_)&(((brush_flags)&(16))!=(0));
  {
   vec2 uv_size = (((res).uv_rect).p1)-(((res).uv_rect).p0);
   uv0 = if_then_else(_c19_,(((res).uv_rect).p0)+(((segment_data).sel(X, Y))*(uv_size)),uv0);
   uv1 = if_then_else(_c19_,(((res).uv_rect).p0)+(((segment_data).sel(Z, W))*(uv_size)),uv1);
      auto _c20_ = (_c19_)&(((brush_flags)&(4))!=(0));
   {
    (stretch_size).sel(X) = if_then_else(_c20_,((stretch_size).sel(X))*((uv_size).sel(X)),(stretch_size).sel(X));
   }
      auto _c21_ = (_c19_)&(((brush_flags)&(8))!=(0));
   {
    (stretch_size).sel(Y) = if_then_else(_c21_,((stretch_size).sel(Y))*((uv_size).sel(Y)),(stretch_size).sel(Y));
   }
  }
 }
 Float perspective_interpolate = if_then_else(((brush_flags)&(1))!=(0), 1., 0.);
 vLayerAndPerspective = force_scalar(make_vec2((res).layer, perspective_interpolate));
 vec2 min_uv = min(uv0, uv1);
 vec2 max_uv = max(uv0, uv1);
 vUvSampleBounds = force_scalar((make_vec4((min_uv)+(make_vec2(0.5)), (max_uv)-(make_vec2(0.5))))/((texture_size).sel(X, Y, X, Y)));
 vec2 f = (((vi).local_pos)-((local_rect).p0))/((local_rect).size);
 vec2 repeat = ((local_rect).size)/(stretch_size);
 vUv = (mix(uv0, uv1, f))-(min_uv);
 vUv /= texture_size;
 vUv *= (repeat).sel(X, Y);
 auto _c22_ = (perspective_interpolate)==(0.);
 {
  vUv = if_then_else(_c22_,vUv*((vi).world_pos).sel(W),vUv);
 }
 vUvBounds = force_scalar((make_vec4(min_uv, max_uv))/((texture_size).sel(X, Y, X, Y)));
}
ALWAYS_INLINE void main(void) {
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
 auto _c3_ = (segment_index)==(65535);
 {
  segment_rect = if_then_else(_c3_,(ph).local_rect,segment_rect);
  segment_data = if_then_else(_c3_,make_vec4(0.),segment_data);
 }
 {
  I32 segment_address = ((ph).specific_prim_address)+((3)+((segment_index)*(2)));
  std::array<vec4,2> segment_info = fetch_from_gpu_cache_2(segment_address);
  segment_rect = if_then_else(~(_c3_),RectWithSize((segment_info[0]).sel(X, Y), (segment_info[0]).sel(Z, W)),segment_rect);
  (segment_rect).p0 = if_then_else(~(_c3_),(segment_rect).p0+((ph).local_rect).p0,(segment_rect).p0);
  segment_data = if_then_else(~(_c3_),segment_info[1],segment_data);
 }
 VertexInfo vi;
 PictureTask pic_task = fetch_picture_task(render_task_index);
 ClipArea clip_area = fetch_clip_area(clip_address);
 Transform transform = fetch_transform((ph).transform_id);
 auto _c4_ = (transform).is_axis_aligned;
 {
  vi = if_then_else(_c4_,write_vertex(segment_rect, (ph).local_clip_rect, (ph).z, transform, pic_task, _c4_),vi);
 }
 {
  bvec4 edge_mask = notEqual((edge_flags)&(make_ivec4(1, 2, 4, 8)), make_ivec4(0));
  vi = if_then_else(~(_c4_),write_transform_vertex(segment_rect, (ph).local_rect, (ph).local_clip_rect, mix(make_vec4(0.), make_vec4(1.), edge_mask), (ph).z, transform, pic_task, ~(_c4_)),vi);
 }
 brush_vs(vi, (ph).specific_prim_address, (ph).local_rect, segment_rect, (ph).user_data, segment_user_data, (transform).m, pic_task, brush_flags, segment_data);
}
static void run(Self *self, char* flats, char* interps, size_t interp_stride) {
 self->main();
 self->store_flat_outputs(flats);
 self->store_interp_outputs(interps, interp_stride);
}
void init_shader() {
 set_uniform_1i_func = (SetUniform1iFunc)&set_uniform_1i;
 set_uniform_4fv_func = (SetUniform4fvFunc)&set_uniform_4fv;
 set_uniform_matrix4fv_func = (SetUniformMatrix4fvFunc)&set_uniform_matrix4fv;
 init_batch_func = (InitBatchFunc)&bind_textures;
 load_attribs_func = (LoadAttribsFunc)&load_attribs;
 run_func = (RunFunc)&run;
}
};

/* inputs
vec4 vTransformBounds
vec4 vClipMaskUvBounds
vec4 vClipMaskUv
vec2 vUv
vec2 vLayerAndPerspective
vec4 vUvBounds
vec4 vUvSampleBounds
*/
/* outputs
vec4 oFragColor
*/
struct brush_image_frag : FragmentShaderImpl {
typedef brush_image_frag Self;
static void set_uniform_1i(Self *self, int index, int value) {
 switch (index) {
 case 7:
  assert(0); // sColor0
  break;
 case 2:
  assert(0); // sGpuCache
  break;
 case 8:
  assert(0); // sPrevPassAlpha
  break;
 }
}
static void set_uniform_4fv(Self *self, int index, const float *value) {
 switch (index) {
 case 7:
  assert(0); // sColor0
  break;
 case 2:
  assert(0); // sGpuCache
  break;
 case 8:
  assert(0); // sPrevPassAlpha
  break;
 }
}
static void set_uniform_matrix4fv(Self *self, int index, const float *value) {
 switch (index) {
 case 7:
  assert(0); // sColor0
  break;
 case 2:
  assert(0); // sGpuCache
  break;
 case 8:
  assert(0); // sPrevPassAlpha
  break;
 }
}
typedef brush_image_vert::FlatOutputs FlatInputs;
typedef brush_image_vert::InterpOutputs InterpInputs;
static void read_flat_inputs(Self *self, const FlatInputs *src) {
  self->vTransformBounds = src->vTransformBounds;
  self->vClipMaskUvBounds = src->vClipMaskUvBounds;
  self->vLayerAndPerspective = src->vLayerAndPerspective;
  self->vUvBounds = src->vUvBounds;
  self->vUvSampleBounds = src->vUvSampleBounds;
}
InterpInputs interp_step;
static void read_interp_inputs(Self *self, const InterpInputs *init, const InterpInputs *step, float step_width) {
  self->vClipMaskUv = init_interp(init->vClipMaskUv, step->vClipMaskUv);
  self->interp_step.vClipMaskUv = step->vClipMaskUv * step_width;
  self->vUv = init_interp(init->vUv, step->vUv);
  self->interp_step.vUv = step->vUv * step_width;
}
ALWAYS_INLINE void step_interp_inputs() {
  vClipMaskUv += interp_step.vClipMaskUv;
  vUv += interp_step.vUv;
}
static void bind_textures(Self *self, brush_image_program *prog) {
 self->sColor0 = lookup_sampler_array(&prog->samplers.sColor0_impl, prog->samplers.sColor0_slot);
 self->sGpuCache = lookup_sampler(&prog->samplers.sGpuCache_impl, prog->samplers.sGpuCache_slot);
 self->sPrevPassAlpha = lookup_sampler_array(&prog->samplers.sPrevPassAlpha_impl, prog->samplers.sPrevPassAlpha_slot);
}
#define oFragColor gl_FragColor
// vec4 oFragColor;
sampler2DArray sColor0;
sampler2DArray sColor1;
sampler2DArray sColor2;
struct RectWithSize_scalar {
vec2_scalar p0;
vec2_scalar size;
RectWithSize_scalar() = default;
RectWithSize_scalar(vec2_scalar p0, vec2_scalar size) : p0(p0), size(size){}
};
struct RectWithSize {
vec2 p0;
vec2 size;
RectWithSize() = default;
RectWithSize(vec2 p0, vec2 size) : p0(p0), size(size){}
RectWithSize(vec2_scalar p0, vec2_scalar size):p0(p0),size(size){
}
RectWithSize(RectWithSize_scalar s):p0(s.p0),size(s.size){
}
friend RectWithSize if_then_else(I32 c, RectWithSize t, RectWithSize e) { return RectWithSize(
if_then_else(c, t.p0, e.p0), if_then_else(c, t.size, e.size));
}};
struct RectWithEndpoint_scalar {
vec2_scalar p0;
vec2_scalar p1;
RectWithEndpoint_scalar() = default;
RectWithEndpoint_scalar(vec2_scalar p0, vec2_scalar p1) : p0(p0), p1(p1){}
};
struct RectWithEndpoint {
vec2 p0;
vec2 p1;
RectWithEndpoint() = default;
RectWithEndpoint(vec2 p0, vec2 p1) : p0(p0), p1(p1){}
RectWithEndpoint(vec2_scalar p0, vec2_scalar p1):p0(p0),p1(p1){
}
RectWithEndpoint(RectWithEndpoint_scalar s):p0(s.p0),p1(s.p1){
}
friend RectWithEndpoint if_then_else(I32 c, RectWithEndpoint t, RectWithEndpoint e) { return RectWithEndpoint(
if_then_else(c, t.p0, e.p0), if_then_else(c, t.p1, e.p1));
}};
sampler2D sGpuCache;
vec4_scalar vTransformBounds;
sampler2DArray sPrevPassAlpha;
sampler2DArray sPrevPassColor;
vec4_scalar vClipMaskUvBounds;
vec4 vClipMaskUv;
struct Fragment_scalar {
vec4_scalar color;
Fragment_scalar() = default;
Fragment_scalar(vec4_scalar color) : color(color){}
};
struct Fragment {
vec4 color;
Fragment() = default;
Fragment(vec4 color) : color(color){}
Fragment(vec4_scalar color):color(color){
}
Fragment(Fragment_scalar s):color(s.color){
}
friend Fragment if_then_else(I32 c, Fragment t, Fragment e) { return Fragment(
if_then_else(c, t.color, e.color));
}};
vec2 vUv;
vec2_scalar vLayerAndPerspective;
vec4_scalar vUvBounds;
vec4_scalar vUvSampleBounds;
Fragment brush_fs() {
 Float perspective_divisor = mix((gl_FragCoord).sel(W), 1., (vLayerAndPerspective).sel(Y));
 vec2 repeated_uv = ((vUv)*(perspective_divisor))+((vUvBounds).sel(X, Y));
 vec2 uv = clamp(repeated_uv, (vUvSampleBounds).sel(X, Y), (vUvSampleBounds).sel(Z, W));
 vec4 texel = texture(sColor0, make_vec3(uv, (vLayerAndPerspective).sel(X)));
 Fragment frag;
 (frag).color = texel;
 return frag;
}
void write_output(vec4 color) {
 oFragColor = color;
}
ALWAYS_INLINE void main(void) {
 Fragment frag = brush_fs();
 write_output((frag).color);
}
static bool use_discard(Self*) { return false; }
static void run(Self *self) {
 self->main();
 self->step_interp_inputs();
}
static void skip(Self *self) {
 self->step_interp_inputs();
}
void init_shader() {
 set_uniform_1i_func = (SetUniform1iFunc)&set_uniform_1i;
 set_uniform_4fv_func = (SetUniform4fvFunc)&set_uniform_4fv;
 set_uniform_matrix4fv_func = (SetUniformMatrix4fvFunc)&set_uniform_matrix4fv;
 init_batch_func = (InitBatchFunc)&bind_textures;
 init_primitive_func = (InitPrimitiveFunc)&read_flat_inputs;
 init_span_func = (InitSpanFunc)&read_interp_inputs;
 run_func = (RunFunc)&run;
 skip_func = (SkipFunc)&skip;
 use_discard_func = (UseDiscardFunc)&use_discard;
}
};
void brush_image_program::init_shaders(void *vertex_shader, void *fragment_shader) {
 reinterpret_cast<brush_image_vert*>(vertex_shader)->init_shader();
 reinterpret_cast<brush_image_frag*>(fragment_shader)->init_shader();
}

