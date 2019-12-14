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
vec2 vUv
vec4 vUvBounds
Float vLayer
vec4 vEdge
vec4 vUvBounds_NoClamp
Float vClipMode
*/
struct cs_clip_box_shadow_program : ProgramImpl {
const char *get_name() const override { return "cs_clip_box_shadow"; }
int get_uniform(const char *name) const override {
 if (strcmp("sColor0", name) == 0) { return 5; }
 if (strcmp("sGpuCache", name) == 0) { return 2; }
 if (strcmp("sRenderTasks", name) == 0) { return 1; }
 if (strcmp("sTransformPalette", name) == 0) { return 3; }
 if (strcmp("uTransform", name) == 0) { return 4; }
 return -1;
}
struct Samplers {
 sampler2DArray_impl sColor0_impl;
 int sColor0_slot;
 sampler2D_impl sGpuCache_impl;
 int sGpuCache_slot;
 sampler2D_impl sRenderTasks_impl;
 int sRenderTasks_slot;
 sampler2D_impl sTransformPalette_impl;
 int sTransformPalette_slot;
} samplers;
bool set_sampler(int index, int value) override {
 switch (index) {
 case 5:
  samplers.sColor0_slot = value;
  return true;
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
struct cs_clip_box_shadow_vert : VertexShaderImpl {
typedef cs_clip_box_shadow_vert Self;
static void set_uniform_1i(Self *self, int index, int value) {
 switch (index) {
 case 4:
  assert(0); // uTransform
  break;
 case 5:
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
 }
}
static void set_uniform_4fv(Self *self, int index, const float *value) {
 switch (index) {
 case 4:
  assert(0); // uTransform
  break;
 case 5:
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
 }
}
static void set_uniform_matrix4fv(Self *self, int index, const float *value) {
 switch (index) {
 case 4:
  self->uTransform = mat4_scalar::load_from_ptr(value);
  break;
 case 5:
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
 }
}
static void load_attribs(Self *self, cs_clip_box_shadow_program *prog, VertexAttrib *attribs, unsigned short *indices, int start, int instance, int count) {
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
vec4_scalar vUvBounds;
float vLayer;
vec4_scalar vEdge;
vec4_scalar vUvBounds_NoClamp;
float vClipMode;
};
struct InterpOutputs {
vec4_scalar vLocalPos;
vec2_scalar vUv;
};
ALWAYS_INLINE void store_flat_outputs(char* dest_ptr) {
  auto* dest = reinterpret_cast<FlatOutputs*>(dest_ptr);
  dest->vTransformBounds = vTransformBounds;
  dest->vUvBounds = vUvBounds;
  dest->vLayer = vLayer;
  dest->vEdge = vEdge;
  dest->vUvBounds_NoClamp = vUvBounds_NoClamp;
  dest->vClipMode = vClipMode;
}
ALWAYS_INLINE void store_interp_outputs(char* dest_ptr, size_t stride) {
  for(int n = 0; n < 4; n++) {
    auto* dest = reinterpret_cast<InterpOutputs*>(dest_ptr);
    dest->vLocalPos = get_nth(vLocalPos, n);
    dest->vUv = get_nth(vUv, n);
    dest_ptr += stride;
  }
}
static void bind_textures(Self *self, cs_clip_box_shadow_program *prog) {
 self->sColor0 = lookup_sampler_array(&prog->samplers.sColor0_impl, prog->samplers.sColor0_slot);
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
vec2 vUv;
vec4_scalar vUvBounds;
float vLayer;
vec4_scalar vEdge;
vec4_scalar vUvBounds_NoClamp;
float vClipMode;
struct BoxShadowData_scalar {
vec2_scalar src_rect_size;
float clip_mode;
int32_t stretch_mode_x;
int32_t stretch_mode_y;
RectWithSize_scalar dest_rect;
BoxShadowData_scalar() = default;
BoxShadowData_scalar(vec2_scalar src_rect_size, float clip_mode, int32_t stretch_mode_x, int32_t stretch_mode_y, RectWithSize_scalar dest_rect) : src_rect_size(src_rect_size), clip_mode(clip_mode), stretch_mode_x(stretch_mode_x), stretch_mode_y(stretch_mode_y), dest_rect(dest_rect){}
};
struct BoxShadowData {
vec2 src_rect_size;
Float clip_mode;
I32 stretch_mode_x;
I32 stretch_mode_y;
RectWithSize dest_rect;
BoxShadowData() = default;
BoxShadowData(vec2 src_rect_size, Float clip_mode, I32 stretch_mode_x, I32 stretch_mode_y, RectWithSize dest_rect) : src_rect_size(src_rect_size), clip_mode(clip_mode), stretch_mode_x(stretch_mode_x), stretch_mode_y(stretch_mode_y), dest_rect(dest_rect){}
BoxShadowData(vec2_scalar src_rect_size, float clip_mode, int32_t stretch_mode_x, int32_t stretch_mode_y, RectWithSize_scalar dest_rect):src_rect_size(src_rect_size),clip_mode(clip_mode),stretch_mode_x(stretch_mode_x),stretch_mode_y(stretch_mode_y),dest_rect(dest_rect){
}
BoxShadowData(BoxShadowData_scalar s):src_rect_size(s.src_rect_size),clip_mode(s.clip_mode),stretch_mode_x(s.stretch_mode_x),stretch_mode_y(s.stretch_mode_y),dest_rect(s.dest_rect){
}
friend BoxShadowData if_then_else(I32 c, BoxShadowData t, BoxShadowData e) { return BoxShadowData(
if_then_else(c, t.src_rect_size, e.src_rect_size), if_then_else(c, t.clip_mode, e.clip_mode), if_then_else(c, t.stretch_mode_x, e.stretch_mode_x), if_then_else(c, t.stretch_mode_y, e.stretch_mode_y), if_then_else(c, t.dest_rect, e.dest_rect));
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
std::array<vec4,3> fetch_from_gpu_cache_3_direct(ivec2 address) {
 return std::array<vec4,3>{texelFetch(sGpuCache, (address)+(make_ivec2(0, 0)), 0), texelFetch(sGpuCache, (address)+(make_ivec2(1, 0)), 0), texelFetch(sGpuCache, (address)+(make_ivec2(2, 0)), 0)};
}
BoxShadowData fetch_data(ivec2 address) {
 std::array<vec4,3> data = fetch_from_gpu_cache_3_direct(address);
 RectWithSize dest_rect = RectWithSize((data[2]).sel(X, Y), (data[2]).sel(Z, W));
 BoxShadowData bs_data = BoxShadowData((data[0]).sel(X, Y), (data[0]).sel(Z), make_int((data[1]).sel(X)), make_int((data[1]).sel(Y)), dest_rect);
 return bs_data;
}
std::array<vec4,2> fetch_from_gpu_cache_2_direct(ivec2 address) {
 return std::array<vec4,2>{texelFetch(sGpuCache, (address)+(make_ivec2(0, 0)), 0), texelFetch(sGpuCache, (address)+(make_ivec2(1, 0)), 0)};
}
ImageResource fetch_image_resource_direct(ivec2 address) {
 std::array<vec4,2> data = fetch_from_gpu_cache_2_direct(address);
 RectWithEndpoint uv_rect = RectWithEndpoint((data[0]).sel(X, Y), (data[0]).sel(Z, W));
 return ImageResource(uv_rect, (data[1]).sel(X), (data[1]).sel(Y, Z, W));
}
Bool ray_plane(vec3 normal, vec3 pt, vec3 ray_origin, vec3_scalar ray_dir, Float& t) {
 I32 ret_mask = ~0;
 Bool ret;
 Float denom = dot(normal, ray_dir);
 auto _c6_ = (abs(denom))>(0.000001f);
 {
  vec3 d = (pt)-(ray_origin);
  t = if_then_else(_c6_,(dot(d, normal))/(denom),t);
  ret = (t)>=(0.f);
  ret_mask = ~I32(_c6_);
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
ALWAYS_INLINE void main(void) {
 ClipMaskInstance cmi = fetch_clip_item();
 Transform clip_transform = fetch_transform((cmi).clip_transform_id);
 Transform prim_transform = fetch_transform((cmi).prim_transform_id);
 BoxShadowData bs_data = fetch_data((cmi).clip_data_address);
 ImageResource res = fetch_image_resource_direct((cmi).resource_address);
 RectWithSize dest_rect = (bs_data).dest_rect;
 ClipVertexInfo vi = write_clip_tile_vertex(dest_rect, prim_transform, clip_transform, (cmi).sub_rect, (cmi).task_origin, (cmi).screen_origin, (cmi).device_pixel_scale);
 vLayer = force_scalar((res).layer);
 vClipMode = force_scalar((bs_data).clip_mode);
 vec2_scalar texture_size = make_vec2(textureSize(sColor0, 0));
 vec2 local_pos = (((vi).local_pos).sel(X, Y))/(((vi).local_pos).sel(W));
 vLocalPos = (vi).local_pos;
 auto _c3_ = ((bs_data).stretch_mode_x)==(0);
 {
  if (_c3_[0]) { (vEdge).sel(X) = 0.5f; };
  if (_c3_[0]) { (vEdge).sel(Z) = force_scalar(((((dest_rect).size).sel(X))/(((bs_data).src_rect_size).sel(X)))-(0.5f)); };
  (vUv).sel(X) = if_then_else(_c3_,(((local_pos).sel(X))-(((dest_rect).p0).sel(X)))/(((bs_data).src_rect_size).sel(X)),(vUv).sel(X));
 }
 if (true) {
  {
   if (~(_c3_)[0]) { (vEdge).lsel(X, Z) = make_vec2(1.f); };
   (vUv).sel(X) = if_then_else(~(_c3_),(((local_pos).sel(X))-(((dest_rect).p0).sel(X)))/(((dest_rect).size).sel(X)),(vUv).sel(X));
  }
 }
 auto _c4_ = ((bs_data).stretch_mode_y)==(0);
 {
  if (_c4_[0]) { (vEdge).sel(Y) = 0.5f; };
  if (_c4_[0]) { (vEdge).sel(W) = force_scalar(((((dest_rect).size).sel(Y))/(((bs_data).src_rect_size).sel(Y)))-(0.5f)); };
  (vUv).sel(Y) = if_then_else(_c4_,(((local_pos).sel(Y))-(((dest_rect).p0).sel(Y)))/(((bs_data).src_rect_size).sel(Y)),(vUv).sel(Y));
 }
 if (true) {
  {
   if (~(_c4_)[0]) { (vEdge).lsel(Y, W) = make_vec2(1.f); };
   (vUv).sel(Y) = if_then_else(~(_c4_),(((local_pos).sel(Y))-(((dest_rect).p0).sel(Y)))/(((dest_rect).size).sel(Y)),(vUv).sel(Y));
  }
 }
 vUv *= ((vi).local_pos).sel(W);
 vec2 uv0 = ((res).uv_rect).p0;
 vec2 uv1 = ((res).uv_rect).p1;
 vUvBounds = force_scalar((make_vec4((uv0)+(make_vec2(0.5f)), (uv1)-(make_vec2(0.5f))))/((texture_size).sel(X, Y, X, Y)));
 vUvBounds_NoClamp = force_scalar((make_vec4(uv0, uv1))/((texture_size).sel(X, Y, X, Y)));
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
vec2 vUv
vec4 vUvBounds
Float vLayer
vec4 vEdge
vec4 vUvBounds_NoClamp
Float vClipMode
*/
/* outputs
vec4 oFragColor
*/
struct cs_clip_box_shadow_frag : FragmentShaderImpl {
typedef cs_clip_box_shadow_frag Self;
static void set_uniform_1i(Self *self, int index, int value) {
 switch (index) {
 case 5:
  assert(0); // sColor0
  break;
 case 2:
  assert(0); // sGpuCache
  break;
 }
}
static void set_uniform_4fv(Self *self, int index, const float *value) {
 switch (index) {
 case 5:
  assert(0); // sColor0
  break;
 case 2:
  assert(0); // sGpuCache
  break;
 }
}
static void set_uniform_matrix4fv(Self *self, int index, const float *value) {
 switch (index) {
 case 5:
  assert(0); // sColor0
  break;
 case 2:
  assert(0); // sGpuCache
  break;
 }
}
typedef cs_clip_box_shadow_vert::FlatOutputs FlatInputs;
typedef cs_clip_box_shadow_vert::InterpOutputs InterpInputs;
static void read_flat_inputs(Self *self, const FlatInputs *src) {
  self->vTransformBounds = src->vTransformBounds;
  self->vUvBounds = src->vUvBounds;
  self->vLayer = src->vLayer;
  self->vEdge = src->vEdge;
  self->vUvBounds_NoClamp = src->vUvBounds_NoClamp;
  self->vClipMode = src->vClipMode;
}
InterpInputs interp_step;
static void read_interp_inputs(Self *self, const InterpInputs *init, const InterpInputs *step, float step_width) {
  self->vLocalPos = init_interp(init->vLocalPos, step->vLocalPos);
  self->interp_step.vLocalPos = step->vLocalPos * step_width;
  self->vUv = init_interp(init->vUv, step->vUv);
  self->interp_step.vUv = step->vUv * step_width;
}
ALWAYS_INLINE void step_interp_inputs() {
  vLocalPos += interp_step.vLocalPos;
  vUv += interp_step.vUv;
}
static void bind_textures(Self *self, cs_clip_box_shadow_program *prog) {
 self->sColor0 = lookup_sampler_array(&prog->samplers.sColor0_impl, prog->samplers.sColor0_slot);
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
vec2 vUv;
vec4_scalar vUvBounds;
float vLayer;
vec4_scalar vEdge;
vec4_scalar vUvBounds_NoClamp;
float vClipMode;
Float point_inside_rect(vec2 p, vec2_scalar p0, vec2_scalar p1) {
 vec2 s = (step(p0, p))-(step(p1, p));
 return ((s).sel(X))*((s).sel(Y));
}
Float init_transform_rough_fs(vec2 local_pos) {
 return point_inside_rect(local_pos, (vTransformBounds).sel(X, Y), (vTransformBounds).sel(Z, W));
}
ALWAYS_INLINE void main(void) {
 vec2 uv_linear = (vUv)/((vLocalPos).sel(W));
 vec2 uv = clamp(uv_linear, make_vec2(0.f), (vEdge).sel(X, Y));
 uv += max(make_vec2(0.f), (uv_linear)-((vEdge).sel(Z, W)));
 uv = mix((vUvBounds_NoClamp).sel(X, Y), (vUvBounds_NoClamp).sel(Z, W), uv);
 uv = clamp(uv, (vUvBounds).sel(X, Y), (vUvBounds).sel(Z, W));
 Float in_shadow_rect = init_transform_rough_fs(((vLocalPos).sel(X, Y))/((vLocalPos).sel(W)));
 Float texel = (texture(sColor0, make_vec3(uv, vLayer))).sel(R);
 Float alpha = mix(texel, (1.f)-(texel), vClipMode);
 Float result = if_then_else(((vLocalPos).sel(W))>(0.f), mix(vClipMode, alpha, in_shadow_rect), 0.f);
 oFragColor = make_vec4(result);
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
void cs_clip_box_shadow_program::init_shaders(void *vertex_shader, void *fragment_shader) {
 reinterpret_cast<cs_clip_box_shadow_vert*>(vertex_shader)->init_shader();
 reinterpret_cast<cs_clip_box_shadow_frag*>(fragment_shader)->init_shader();
}

