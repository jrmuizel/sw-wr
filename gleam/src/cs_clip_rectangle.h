/* inputs
vec3 aPosition
ivec2 aTransformIds
ivec4 aClipDataResourceAddress
vec2 aClipLocalPos
vec4 aClipTileRect
vec4 aClipDeviceArea
vec4 aClipOrigins
Float aDevicePixelScale
*/
/* outputs
vec4 vTransformBounds
vec4 vLocalPos
vec4 vClipCenter_Radius_TL
vec4 vClipCenter_Radius_TR
vec4 vClipCenter_Radius_BL
vec4 vClipCenter_Radius_BR
Float vClipMode
*/
struct cs_clip_rectangle_program : ProgramImpl {
const char *get_name() const override { return "cs_clip_rectangle"; }
int get_uniform(const char *name) const override {
 if (strcmp("sGpuCache", name) == 0) { return 2; }
 if (strcmp("sRenderTasks", name) == 0) { return 1; }
 if (strcmp("sTransformPalette", name) == 0) { return 3; }
 if (strcmp("uTransform", name) == 0) { return 4; }
 return -1;
}
struct Samplers {
 sampler2D_impl sGpuCache_impl;
 int sGpuCache_slot;
 sampler2D_impl sRenderTasks_impl;
 int sRenderTasks_slot;
 sampler2D_impl sTransformPalette_impl;
 int sTransformPalette_slot;
} samplers;
bool set_sampler(int index, int value) override {
 switch (index) {
 case 2:
  samplers.sGpuCache_slot = value;
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
 int aTransformIds;
 int aClipDataResourceAddress;
 int aClipLocalPos;
 int aClipTileRect;
 int aClipDeviceArea;
 int aClipOrigins;
 int aDevicePixelScale;
} attrib_locations;
void bind_attrib(const char *name, int index) override {
 if (strcmp("aPosition", name) == 0) { attrib_locations.aPosition = index; return; }
 if (strcmp("aTransformIds", name) == 0) { attrib_locations.aTransformIds = index; return; }
 if (strcmp("aClipDataResourceAddress", name) == 0) { attrib_locations.aClipDataResourceAddress = index; return; }
 if (strcmp("aClipLocalPos", name) == 0) { attrib_locations.aClipLocalPos = index; return; }
 if (strcmp("aClipTileRect", name) == 0) { attrib_locations.aClipTileRect = index; return; }
 if (strcmp("aClipDeviceArea", name) == 0) { attrib_locations.aClipDeviceArea = index; return; }
 if (strcmp("aClipOrigins", name) == 0) { attrib_locations.aClipOrigins = index; return; }
 if (strcmp("aDevicePixelScale", name) == 0) { attrib_locations.aDevicePixelScale = index; return; }
}
void init_shaders(void *vertex_shader, void *fragment_shader) override;
};
struct cs_clip_rectangle_vert : VertexShaderImpl {
typedef cs_clip_rectangle_vert Self;
static void set_uniform_1i(Self *self, int index, int value) {
 switch (index) {
 case 4:
  assert(0); // uTransform
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
 }
}
static void set_uniform_4fv(Self *self, int index, const float *value) {
 switch (index) {
 case 4:
  assert(0); // uTransform
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
 }
}
static void set_uniform_matrix4fv(Self *self, int index, const float *value) {
 switch (index) {
 case 4:
  self->uTransform = mat4_scalar::load_from_ptr(value);
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
 }
}
static void load_attribs(Self *self, cs_clip_rectangle_program *prog, VertexAttrib *attribs, unsigned short *indices, int start, int instance, int count) {
 load_attrib(self->aPosition, attribs[prog->attrib_locations.aPosition], indices, start, instance, count);
 load_attrib(self->aTransformIds, attribs[prog->attrib_locations.aTransformIds], indices, start, instance, count);
 load_attrib(self->aClipDataResourceAddress, attribs[prog->attrib_locations.aClipDataResourceAddress], indices, start, instance, count);
 load_attrib(self->aClipLocalPos, attribs[prog->attrib_locations.aClipLocalPos], indices, start, instance, count);
 load_attrib(self->aClipTileRect, attribs[prog->attrib_locations.aClipTileRect], indices, start, instance, count);
 load_attrib(self->aClipDeviceArea, attribs[prog->attrib_locations.aClipDeviceArea], indices, start, instance, count);
 load_attrib(self->aClipOrigins, attribs[prog->attrib_locations.aClipOrigins], indices, start, instance, count);
 load_attrib(self->aDevicePixelScale, attribs[prog->attrib_locations.aDevicePixelScale], indices, start, instance, count);
}
struct FlatOutputs {
vec4_scalar vTransformBounds;
vec4_scalar vClipCenter_Radius_TL;
vec4_scalar vClipCenter_Radius_TR;
vec4_scalar vClipCenter_Radius_BL;
vec4_scalar vClipCenter_Radius_BR;
float vClipMode;
};
struct InterpOutputs {
vec4_scalar vLocalPos;
};
ALWAYS_INLINE void store_flat_outputs(char* dest_ptr) {
  auto* dest = reinterpret_cast<FlatOutputs*>(dest_ptr);
  dest->vTransformBounds = vTransformBounds;
  dest->vClipCenter_Radius_TL = vClipCenter_Radius_TL;
  dest->vClipCenter_Radius_TR = vClipCenter_Radius_TR;
  dest->vClipCenter_Radius_BL = vClipCenter_Radius_BL;
  dest->vClipCenter_Radius_BR = vClipCenter_Radius_BR;
  dest->vClipMode = vClipMode;
}
ALWAYS_INLINE void store_interp_outputs(char* dest_ptr, size_t stride) {
  for(int n = 0; n < 4; n++) {
    auto* dest = reinterpret_cast<InterpOutputs*>(dest_ptr);
    dest->vLocalPos = get_nth(vLocalPos, n);
    dest_ptr += stride;
  }
}
static void bind_textures(Self *self, cs_clip_rectangle_program *prog) {
 self->sRenderTasks = lookup_sampler(&prog->samplers.sRenderTasks_impl, prog->samplers.sRenderTasks_slot);
 self->sGpuCache = lookup_sampler(&prog->samplers.sGpuCache_impl, prog->samplers.sGpuCache_slot);
 self->sTransformPalette = lookup_sampler(&prog->samplers.sTransformPalette_impl, prog->samplers.sTransformPalette_slot);
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
ivec2 aTransformIds;
ivec4 aClipDataResourceAddress;
vec2 aClipLocalPos;
vec4 aClipTileRect;
vec4 aClipDeviceArea;
vec4 aClipOrigins;
Float aDevicePixelScale;
struct ClipMaskInstance_scalar {
int32_t clip_transform_id;
int32_t prim_transform_id;
ivec2_scalar clip_data_address;
ivec2_scalar resource_address;
vec2_scalar local_pos;
RectWithSize_scalar tile_rect;
RectWithSize_scalar sub_rect;
vec2_scalar task_origin;
vec2_scalar screen_origin;
float device_pixel_scale;
ClipMaskInstance_scalar() = default;
ClipMaskInstance_scalar(int32_t clip_transform_id, int32_t prim_transform_id, ivec2_scalar clip_data_address, ivec2_scalar resource_address, vec2_scalar local_pos, RectWithSize_scalar tile_rect, RectWithSize_scalar sub_rect, vec2_scalar task_origin, vec2_scalar screen_origin, float device_pixel_scale) : clip_transform_id(clip_transform_id), prim_transform_id(prim_transform_id), clip_data_address(clip_data_address), resource_address(resource_address), local_pos(local_pos), tile_rect(tile_rect), sub_rect(sub_rect), task_origin(task_origin), screen_origin(screen_origin), device_pixel_scale(device_pixel_scale){}
};
struct ClipMaskInstance {
I32 clip_transform_id;
I32 prim_transform_id;
ivec2 clip_data_address;
ivec2 resource_address;
vec2 local_pos;
RectWithSize tile_rect;
RectWithSize sub_rect;
vec2 task_origin;
vec2 screen_origin;
Float device_pixel_scale;
ClipMaskInstance() = default;
ClipMaskInstance(I32 clip_transform_id, I32 prim_transform_id, ivec2 clip_data_address, ivec2 resource_address, vec2 local_pos, RectWithSize tile_rect, RectWithSize sub_rect, vec2 task_origin, vec2 screen_origin, Float device_pixel_scale) : clip_transform_id(clip_transform_id), prim_transform_id(prim_transform_id), clip_data_address(clip_data_address), resource_address(resource_address), local_pos(local_pos), tile_rect(tile_rect), sub_rect(sub_rect), task_origin(task_origin), screen_origin(screen_origin), device_pixel_scale(device_pixel_scale){}
ClipMaskInstance(int32_t clip_transform_id, int32_t prim_transform_id, ivec2_scalar clip_data_address, ivec2_scalar resource_address, vec2_scalar local_pos, RectWithSize_scalar tile_rect, RectWithSize_scalar sub_rect, vec2_scalar task_origin, vec2_scalar screen_origin, float device_pixel_scale):clip_transform_id(clip_transform_id),prim_transform_id(prim_transform_id),clip_data_address(clip_data_address),resource_address(resource_address),local_pos(local_pos),tile_rect(tile_rect),sub_rect(sub_rect),task_origin(task_origin),screen_origin(screen_origin),device_pixel_scale(device_pixel_scale){
}
ClipMaskInstance(ClipMaskInstance_scalar s):clip_transform_id(s.clip_transform_id),prim_transform_id(s.prim_transform_id),clip_data_address(s.clip_data_address),resource_address(s.resource_address),local_pos(s.local_pos),tile_rect(s.tile_rect),sub_rect(s.sub_rect),task_origin(s.task_origin),screen_origin(s.screen_origin),device_pixel_scale(s.device_pixel_scale){
}
friend ClipMaskInstance if_then_else(I32 c, ClipMaskInstance t, ClipMaskInstance e) { return ClipMaskInstance(
if_then_else(c, t.clip_transform_id, e.clip_transform_id), if_then_else(c, t.prim_transform_id, e.prim_transform_id), if_then_else(c, t.clip_data_address, e.clip_data_address), if_then_else(c, t.resource_address, e.resource_address), if_then_else(c, t.local_pos, e.local_pos), if_then_else(c, t.tile_rect, e.tile_rect), if_then_else(c, t.sub_rect, e.sub_rect), if_then_else(c, t.task_origin, e.task_origin), if_then_else(c, t.screen_origin, e.screen_origin), if_then_else(c, t.device_pixel_scale, e.device_pixel_scale));
}};
struct ClipVertexInfo_scalar {
vec4_scalar local_pos;
RectWithSize_scalar clipped_local_rect;
ClipVertexInfo_scalar() = default;
ClipVertexInfo_scalar(vec4_scalar local_pos, RectWithSize_scalar clipped_local_rect) : local_pos(local_pos), clipped_local_rect(clipped_local_rect){}
};
struct ClipVertexInfo {
vec4 local_pos;
RectWithSize clipped_local_rect;
ClipVertexInfo() = default;
ClipVertexInfo(vec4 local_pos, RectWithSize clipped_local_rect) : local_pos(local_pos), clipped_local_rect(clipped_local_rect){}
ClipVertexInfo(vec4_scalar local_pos, RectWithSize_scalar clipped_local_rect):local_pos(local_pos),clipped_local_rect(clipped_local_rect){
}
ClipVertexInfo(ClipVertexInfo_scalar s):local_pos(s.local_pos),clipped_local_rect(s.clipped_local_rect){
}
friend ClipVertexInfo if_then_else(I32 c, ClipVertexInfo t, ClipVertexInfo e) { return ClipVertexInfo(
if_then_else(c, t.local_pos, e.local_pos), if_then_else(c, t.clipped_local_rect, e.clipped_local_rect));
}};
vec4 vLocalPos;
vec4_scalar vClipCenter_Radius_TL;
vec4_scalar vClipCenter_Radius_TR;
vec4_scalar vClipCenter_Radius_BL;
vec4_scalar vClipCenter_Radius_BR;
float vClipMode;
struct ClipRect_scalar {
RectWithSize_scalar rect;
vec4_scalar mode;
ClipRect_scalar() = default;
ClipRect_scalar(RectWithSize_scalar rect, vec4_scalar mode) : rect(rect), mode(mode){}
};
struct ClipRect {
RectWithSize rect;
vec4 mode;
ClipRect() = default;
ClipRect(RectWithSize rect, vec4 mode) : rect(rect), mode(mode){}
ClipRect(RectWithSize_scalar rect, vec4_scalar mode):rect(rect),mode(mode){
}
ClipRect(ClipRect_scalar s):rect(s.rect),mode(s.mode){
}
friend ClipRect if_then_else(I32 c, ClipRect t, ClipRect e) { return ClipRect(
if_then_else(c, t.rect, e.rect), if_then_else(c, t.mode, e.mode));
}};
struct ClipCorner_scalar {
RectWithSize_scalar rect;
vec4_scalar outer_inner_radius;
ClipCorner_scalar() = default;
ClipCorner_scalar(RectWithSize_scalar rect, vec4_scalar outer_inner_radius) : rect(rect), outer_inner_radius(outer_inner_radius){}
};
struct ClipCorner {
RectWithSize rect;
vec4 outer_inner_radius;
ClipCorner() = default;
ClipCorner(RectWithSize rect, vec4 outer_inner_radius) : rect(rect), outer_inner_radius(outer_inner_radius){}
ClipCorner(RectWithSize_scalar rect, vec4_scalar outer_inner_radius):rect(rect),outer_inner_radius(outer_inner_radius){
}
ClipCorner(ClipCorner_scalar s):rect(s.rect),outer_inner_radius(s.outer_inner_radius){
}
friend ClipCorner if_then_else(I32 c, ClipCorner t, ClipCorner e) { return ClipCorner(
if_then_else(c, t.rect, e.rect), if_then_else(c, t.outer_inner_radius, e.outer_inner_radius));
}};
struct ClipData_scalar {
ClipRect_scalar rect;
ClipCorner_scalar top_left;
ClipCorner_scalar top_right;
ClipCorner_scalar bottom_left;
ClipCorner_scalar bottom_right;
ClipData_scalar() = default;
ClipData_scalar(ClipRect_scalar rect, ClipCorner_scalar top_left, ClipCorner_scalar top_right, ClipCorner_scalar bottom_left, ClipCorner_scalar bottom_right) : rect(rect), top_left(top_left), top_right(top_right), bottom_left(bottom_left), bottom_right(bottom_right){}
};
struct ClipData {
ClipRect rect;
ClipCorner top_left;
ClipCorner top_right;
ClipCorner bottom_left;
ClipCorner bottom_right;
ClipData() = default;
ClipData(ClipRect rect, ClipCorner top_left, ClipCorner top_right, ClipCorner bottom_left, ClipCorner bottom_right) : rect(rect), top_left(top_left), top_right(top_right), bottom_left(bottom_left), bottom_right(bottom_right){}
ClipData(ClipRect_scalar rect, ClipCorner_scalar top_left, ClipCorner_scalar top_right, ClipCorner_scalar bottom_left, ClipCorner_scalar bottom_right):rect(rect),top_left(top_left),top_right(top_right),bottom_left(bottom_left),bottom_right(bottom_right){
}
ClipData(ClipData_scalar s):rect(s.rect),top_left(s.top_left),top_right(s.top_right),bottom_left(s.bottom_left),bottom_right(s.bottom_right){
}
friend ClipData if_then_else(I32 c, ClipData t, ClipData e) { return ClipData(
if_then_else(c, t.rect, e.rect), if_then_else(c, t.top_left, e.top_left), if_then_else(c, t.top_right, e.top_right), if_then_else(c, t.bottom_left, e.bottom_left), if_then_else(c, t.bottom_right, e.bottom_right));
}};
ClipMaskInstance fetch_clip_item() {
 ClipMaskInstance cmi;
 (cmi).clip_transform_id = (aTransformIds).sel(X);
 (cmi).prim_transform_id = (aTransformIds).sel(Y);
 (cmi).clip_data_address = (aClipDataResourceAddress).sel(X, Y);
 (cmi).resource_address = (aClipDataResourceAddress).sel(Z, W);
 (cmi).local_pos = aClipLocalPos;
 (cmi).tile_rect = RectWithSize((aClipTileRect).sel(X, Y), (aClipTileRect).sel(Z, W));
 (cmi).sub_rect = RectWithSize((aClipDeviceArea).sel(X, Y), (aClipDeviceArea).sel(Z, W));
 (cmi).task_origin = (aClipOrigins).sel(X, Y);
 (cmi).screen_origin = (aClipOrigins).sel(Z, W);
 (cmi).device_pixel_scale = aDevicePixelScale;
 return cmi;
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
std::array<vec4,2> fetch_from_gpu_cache_2_direct(ivec2 address) {
 return std::array<vec4,2>{texelFetch(sGpuCache, (address)+(make_ivec2(0, 0)), 0), texelFetch(sGpuCache, (address)+(make_ivec2(1, 0)), 0)};
}
ClipRect fetch_clip_rect(ivec2 address) {
 std::array<vec4,2> data = fetch_from_gpu_cache_2_direct(address);
 ClipRect rect = ClipRect(RectWithSize((data[0]).sel(X, Y), (data[0]).sel(Z, W)), data[1]);
 return rect;
}
ClipCorner fetch_clip_corner(ivec2 address, float index) {
 address += make_ivec2((2)+((2)*(make_int(index))), 0);
 std::array<vec4,2> data = fetch_from_gpu_cache_2_direct(address);
 ClipCorner corner = ClipCorner(RectWithSize((data[0]).sel(X, Y), (data[0]).sel(Z, W)), data[1]);
 return corner;
}
ClipData fetch_clip(ivec2 address) {
 ClipData clip;
 (clip).rect = fetch_clip_rect(address);
 (clip).top_left = fetch_clip_corner(address, 0.f);
 (clip).top_right = fetch_clip_corner(address, 1.f);
 (clip).bottom_left = fetch_clip_corner(address, 2.f);
 (clip).bottom_right = fetch_clip_corner(address, 3.f);
 return clip;
}
Bool ray_plane(vec3 normal, vec3 pt, vec3 ray_origin, vec3_scalar ray_dir, Float& t) {
 I32 ret_mask = ~0;
 Bool ret;
 Float denom = dot(normal, ray_dir);
 auto _c2_ = (abs(denom))>(0.000001f);
 {
  vec3 d = (pt)-(ray_origin);
  t = if_then_else(_c2_,(dot(d, normal))/(denom),t);
  ret = (t)>=(0.f);
  ret_mask = ~I32(_c2_);
 }
 ret = if_then_else(ret_mask, false, ret);
 return ret;
}
vec4 untransform(vec2 ref, vec3 n, vec3 a, mat4 inv_transform) {
 vec3 p = make_vec3(ref, -(10000.f));
 vec3_scalar d = make_vec3(0, 0, 1.f);
 Float t = 0.f;
 ray_plane(n, a, p, d, t);
 Float z = ((p).sel(Z))+(((d).sel(Z))*(t));
 vec4 r = (inv_transform)*(make_vec4(ref, z, 1.f));
 return r;
}
vec4 get_node_pos(vec2 pos, Transform transform) {
 vec4 ah = ((transform).m)*(make_vec4(0.f, 0.f, 0.f, 1.f));
 vec3 a = ((ah).sel(X, Y, Z))/((ah).sel(W));
 vec3 n = (transpose(make_mat3((transform).inv_m)))*(make_vec3(0.f, 0.f, 1.f));
 return untransform(pos, n, a, (transform).inv_m);
}
void init_transform_vs(vec4 local_bounds) {
 vTransformBounds = force_scalar(local_bounds);
}
ClipVertexInfo write_clip_tile_vertex(RectWithSize local_clip_rect, Transform prim_transform, Transform clip_transform, RectWithSize sub_rect, vec2 task_origin, vec2 screen_origin, Float device_pixel_scale) {
 vec2 device_pos = (screen_origin)+(((sub_rect).p0)+(((aPosition).sel(X, Y))*((sub_rect).size)));
 vec2 world_pos = (device_pos)/(device_pixel_scale);
 vec4 pos = ((prim_transform).m)*(make_vec4(world_pos, 0.f, 1.f));
 (pos).lsel(X, Y, Z) /= (pos).sel(W);
 vec4 p = get_node_pos((pos).sel(X, Y), clip_transform);
 vec4 local_pos = (p)*((pos).sel(W));
 vec4 vertex_pos = make_vec4((task_origin)+(((sub_rect).p0)+(((aPosition).sel(X, Y))*((sub_rect).size))), 0.f, 1.f);
 gl_Position = (uTransform)*(vertex_pos);
 init_transform_vs(make_vec4((local_clip_rect).p0, ((local_clip_rect).p0)+((local_clip_rect).size)));
 ClipVertexInfo vi = ClipVertexInfo(local_pos, local_clip_rect);
 return vi;
}
RectWithEndpoint to_rect_with_endpoint(RectWithSize rect) {
 RectWithEndpoint result;
 (result).p0 = (rect).p0;
 (result).p1 = ((rect).p0)+((rect).size);
 return result;
}
ALWAYS_INLINE void main(void) {
 ClipMaskInstance cmi = fetch_clip_item();
 Transform clip_transform = fetch_transform((cmi).clip_transform_id);
 Transform prim_transform = fetch_transform((cmi).prim_transform_id);
 ClipData clip = fetch_clip((cmi).clip_data_address);
 RectWithSize local_rect = ((clip).rect).rect;
 (local_rect).p0 = (cmi).local_pos;
 ClipVertexInfo vi = write_clip_tile_vertex(local_rect, prim_transform, clip_transform, (cmi).sub_rect, (cmi).task_origin, (cmi).screen_origin, (cmi).device_pixel_scale);
 vClipMode = force_scalar((((clip).rect).mode).sel(X));
 vLocalPos = (vi).local_pos;
 RectWithEndpoint clip_rect = to_rect_with_endpoint(local_rect);
 vec2 r_tl = (((clip).top_left).outer_inner_radius).sel(X, Y);
 vec2 r_tr = (((clip).top_right).outer_inner_radius).sel(X, Y);
 vec2 r_br = (((clip).bottom_right).outer_inner_radius).sel(X, Y);
 vec2 r_bl = (((clip).bottom_left).outer_inner_radius).sel(X, Y);
 vClipCenter_Radius_TL = force_scalar(make_vec4(((clip_rect).p0)+(r_tl), r_tl));
 vClipCenter_Radius_TR = force_scalar(make_vec4((((clip_rect).p1).sel(X))-((r_tr).sel(X)), (((clip_rect).p0).sel(Y))+((r_tr).sel(Y)), r_tr));
 vClipCenter_Radius_BR = force_scalar(make_vec4(((clip_rect).p1)-(r_br), r_br));
 vClipCenter_Radius_BL = force_scalar(make_vec4((((clip_rect).p0).sel(X))+((r_bl).sel(X)), (((clip_rect).p1).sel(Y))-((r_bl).sel(Y)), r_bl));
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
vec4 vLocalPos
vec4 vClipCenter_Radius_TL
vec4 vClipCenter_Radius_TR
vec4 vClipCenter_Radius_BL
vec4 vClipCenter_Radius_BR
Float vClipMode
*/
/* outputs
vec4 oFragColor
*/
struct cs_clip_rectangle_frag : FragmentShaderImpl {
typedef cs_clip_rectangle_frag Self;
static void set_uniform_1i(Self *self, int index, int value) {
 switch (index) {
 case 2:
  assert(0); // sGpuCache
  break;
 }
}
static void set_uniform_4fv(Self *self, int index, const float *value) {
 switch (index) {
 case 2:
  assert(0); // sGpuCache
  break;
 }
}
static void set_uniform_matrix4fv(Self *self, int index, const float *value) {
 switch (index) {
 case 2:
  assert(0); // sGpuCache
  break;
 }
}
typedef cs_clip_rectangle_vert::FlatOutputs FlatInputs;
typedef cs_clip_rectangle_vert::InterpOutputs InterpInputs;
static void read_flat_inputs(Self *self, const FlatInputs *src) {
  self->vTransformBounds = src->vTransformBounds;
  self->vClipCenter_Radius_TL = src->vClipCenter_Radius_TL;
  self->vClipCenter_Radius_TR = src->vClipCenter_Radius_TR;
  self->vClipCenter_Radius_BL = src->vClipCenter_Radius_BL;
  self->vClipCenter_Radius_BR = src->vClipCenter_Radius_BR;
  self->vClipMode = src->vClipMode;
}
InterpInputs interp_step;
static void read_interp_inputs(Self *self, const InterpInputs *init, const InterpInputs *step, float step_width) {
  self->vLocalPos = init_interp(init->vLocalPos, step->vLocalPos);
  self->interp_step.vLocalPos = step->vLocalPos * step_width;
}
ALWAYS_INLINE void step_interp_inputs() {
  vLocalPos += interp_step.vLocalPos;
}
static void bind_textures(Self *self, cs_clip_rectangle_program *prog) {
 self->sGpuCache = lookup_sampler(&prog->samplers.sGpuCache_impl, prog->samplers.sGpuCache_slot);
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
vec4 vLocalPos;
vec4_scalar vClipCenter_Radius_TL;
vec4_scalar vClipCenter_Radius_TR;
vec4_scalar vClipCenter_Radius_BL;
vec4_scalar vClipCenter_Radius_BR;
float vClipMode;
Float compute_aa_range(vec2 position) {
 return (0.35355f)*(length(fwidth(position)));
}
Float signed_distance_rect(vec2 pos, vec2_scalar p0, vec2_scalar p1) {
 vec2 d = max((p0)-(pos), (pos)-(p1));
 return (length(max(make_vec2(0.f), d)))+(min(0.f, max((d).sel(X), (d).sel(Y))));
}
Float distance_aa(Float aa_range, Float signed_distance) {
 I32 ret_mask = ~0;
 Float ret;
 Float dist = (0.5f)*((signed_distance)/(aa_range));
 auto _c3_ = (dist)<=((-(0.5f))+(0.0001f));
 ret = 1.f;
 ret_mask = ~I32(_c3_);
 auto _c4_ = (dist)>=((0.5f)-(0.0001f));
 ret = if_then_else(ret_mask & I32(_c4_), 0.f, ret);
 ret_mask &= ~I32(_c4_);
 ret = if_then_else(ret_mask, (0.5f)+((dist)*(((0.8431027f)*((dist)*(dist)))-(1.14453603f))), ret);
 return ret;
}
Float init_transform_fs(vec2 local_pos) {
 Float d = signed_distance_rect(local_pos, (vTransformBounds).sel(X, Y), (vTransformBounds).sel(Z, W));
 Float aa_range = compute_aa_range(local_pos);
 return distance_aa(aa_range, d);
}
Float distance_to_ellipse(vec2 p, vec2_scalar radii, Float aa_range) {
 Float dist;
 if (any(lessThanEqual(radii, make_vec2(0.f)))) {
  {
   dist = length(p);
  }
 } else  {
  vec2_scalar invRadiiSq = (1.f)/((radii)*(radii));
  Float g = (dot((p)*((p)*(invRadiiSq)), make_vec2(1.f)))-(1.f);
  vec2 dG = (2.f)*((p)*(invRadiiSq));
  dist = (g)*(inversesqrt(dot(dG, dG)));
 }
 return clamp(dist, -(aa_range), aa_range);
}
Float clip_against_ellipse_if_needed(vec2 pos, Float current_distance, vec4_scalar ellipse_center_radius, vec2_scalar sign_modifier, Float aa_range) {
 I32 ret_mask = ~0;
 Float ret;
 auto _c6_ = !(all(lessThan((sign_modifier)*(pos), (sign_modifier)*((ellipse_center_radius).sel(X, Y)))));
 {
  ret = current_distance;
  ret_mask = ~I32(_c6_);
 }
 Float distance = distance_to_ellipse((pos)-((ellipse_center_radius).sel(X, Y)), (ellipse_center_radius).sel(Z, W), aa_range);
 ret = if_then_else(ret_mask, max(distance, current_distance), ret);
 return ret;
}
Float rounded_rect(vec2 pos, vec4_scalar clip_center_radius_tl, vec4_scalar clip_center_radius_tr, vec4_scalar clip_center_radius_br, vec4_scalar clip_center_radius_bl, Float aa_range) {
 Float current_distance = -(aa_range);
 current_distance = clip_against_ellipse_if_needed(pos, current_distance, clip_center_radius_tl, make_vec2(1.f), aa_range);
 current_distance = clip_against_ellipse_if_needed(pos, current_distance, clip_center_radius_tr, make_vec2(-(1.f), 1.f), aa_range);
 current_distance = clip_against_ellipse_if_needed(pos, current_distance, clip_center_radius_br, make_vec2(-(1.f)), aa_range);
 current_distance = clip_against_ellipse_if_needed(pos, current_distance, clip_center_radius_bl, make_vec2(1.f, -(1.f)), aa_range);
 return distance_aa(aa_range, current_distance);
}
ALWAYS_INLINE void main(void) {
 vec2 local_pos = ((vLocalPos).sel(X, Y))/((vLocalPos).sel(W));
 Float aa_range = compute_aa_range(local_pos);
 Float alpha = init_transform_fs((local_pos).sel(X, Y));
 Float clip_alpha = rounded_rect((local_pos).sel(X, Y), vClipCenter_Radius_TL, vClipCenter_Radius_TR, vClipCenter_Radius_BR, vClipCenter_Radius_BL, aa_range);
 Float combined_alpha = (alpha)*(clip_alpha);
 Float final_alpha = mix(combined_alpha, (1.f)-(combined_alpha), vClipMode);
 Float final_final_alpha = if_then_else(((vLocalPos).sel(W))>(0.f), final_alpha, 0.f);
 oFragColor = make_vec4(final_final_alpha, 0.f, 0.f, 1.f);
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
void cs_clip_rectangle_program::init_shaders(void *vertex_shader, void *fragment_shader) {
 reinterpret_cast<cs_clip_rectangle_vert*>(vertex_shader)->init_shader();
 reinterpret_cast<cs_clip_rectangle_frag*>(fragment_shader)->init_shader();
}

