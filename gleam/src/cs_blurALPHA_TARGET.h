/* inputs
vec3 aPosition
I32 aBlurRenderTaskAddress
I32 aBlurSourceTaskAddress
I32 aBlurDirection
*/
/* outputs
vec4 vTransformBounds
vec4 vClipMaskUvBounds
vec4 vClipMaskUv
vec3 vUv
vec4 vUvRect
vec2 vOffsetScale
Float vSigma
I32 vSupport
*/
struct cs_blurALPHA_TARGET_program : ProgramImpl {
const char *get_name() const override { return "cs_blurALPHA_TARGET"; }
int get_uniform(const char *name) const override {
 if (strcmp("sGpuCache", name) == 0) { return 2; }
 if (strcmp("sPrevPassAlpha", name) == 0) { return 7; }
 if (strcmp("sPrimitiveHeadersF", name) == 0) { return 4; }
 if (strcmp("sPrimitiveHeadersI", name) == 0) { return 5; }
 if (strcmp("sRenderTasks", name) == 0) { return 1; }
 if (strcmp("sTransformPalette", name) == 0) { return 3; }
 if (strcmp("uTransform", name) == 0) { return 6; }
 return -1;
}
struct Samplers {
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
 case 2:
  samplers.sGpuCache_slot = value;
  return true;
 case 7:
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
 int aBlurRenderTaskAddress;
 int aBlurSourceTaskAddress;
 int aBlurDirection;
} attrib_locations;
void bind_attrib(const char *name, int index) override {
 if (strcmp("aPosition", name) == 0) { attrib_locations.aPosition = index; return; }
 if (strcmp("aBlurRenderTaskAddress", name) == 0) { attrib_locations.aBlurRenderTaskAddress = index; return; }
 if (strcmp("aBlurSourceTaskAddress", name) == 0) { attrib_locations.aBlurSourceTaskAddress = index; return; }
 if (strcmp("aBlurDirection", name) == 0) { attrib_locations.aBlurDirection = index; return; }
}
void init_shaders(void *vertex_shader, void *fragment_shader) override;
};
struct cs_blurALPHA_TARGET_vert : VertexShaderImpl {
typedef cs_blurALPHA_TARGET_vert Self;
static void set_uniform_1i(Self *self, int index, int value) {
 switch (index) {
 case 6:
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
 case 7:
  assert(0); // sPrevPassAlpha
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
 case 1:
  assert(0); // sRenderTasks
  break;
 case 2:
  assert(0); // sGpuCache
  break;
 case 3:
  assert(0); // sTransformPalette
  break;
 case 7:
  assert(0); // sPrevPassAlpha
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
 case 1:
  assert(0); // sRenderTasks
  break;
 case 2:
  assert(0); // sGpuCache
  break;
 case 3:
  assert(0); // sTransformPalette
  break;
 case 7:
  assert(0); // sPrevPassAlpha
  break;
 case 4:
  assert(0); // sPrimitiveHeadersF
  break;
 case 5:
  assert(0); // sPrimitiveHeadersI
  break;
 }
}
static void load_attribs(Self *self, cs_blurALPHA_TARGET_program *prog, VertexAttrib *attribs, unsigned short *indices, int start, int instance, int count) {
 load_attrib(self->aPosition, attribs[prog->attrib_locations.aPosition], indices, start, instance, count);
 load_attrib(self->aBlurRenderTaskAddress, attribs[prog->attrib_locations.aBlurRenderTaskAddress], indices, start, instance, count);
 load_attrib(self->aBlurSourceTaskAddress, attribs[prog->attrib_locations.aBlurSourceTaskAddress], indices, start, instance, count);
 load_attrib(self->aBlurDirection, attribs[prog->attrib_locations.aBlurDirection], indices, start, instance, count);
}
struct FlatOutputs {
vec4_scalar vTransformBounds;
vec4_scalar vClipMaskUvBounds;
vec4_scalar vUvRect;
vec2_scalar vOffsetScale;
float vSigma;
int32_t vSupport;
};
struct InterpOutputs {
vec4_scalar vClipMaskUv;
vec3_scalar vUv;
};
ALWAYS_INLINE void store_flat_outputs(char* dest_ptr) {
  auto* dest = reinterpret_cast<FlatOutputs*>(dest_ptr);
  dest->vTransformBounds = vTransformBounds;
  dest->vClipMaskUvBounds = vClipMaskUvBounds;
  dest->vUvRect = vUvRect;
  dest->vOffsetScale = vOffsetScale;
  dest->vSigma = vSigma;
  dest->vSupport = vSupport;
}
ALWAYS_INLINE void store_interp_outputs(char* dest_ptr, size_t stride) {
  for(int n = 0; n < 4; n++) {
    auto* dest = reinterpret_cast<InterpOutputs*>(dest_ptr);
    dest->vClipMaskUv = get_nth(vClipMaskUv, n);
    dest->vUv = get_nth(vUv, n);
    dest_ptr += stride;
  }
}
static void bind_textures(Self *self, cs_blurALPHA_TARGET_program *prog) {
 self->sRenderTasks = lookup_sampler(&prog->samplers.sRenderTasks_impl, prog->samplers.sRenderTasks_slot);
 self->sGpuCache = lookup_sampler(&prog->samplers.sGpuCache_impl, prog->samplers.sGpuCache_slot);
 self->sTransformPalette = lookup_sampler(&prog->samplers.sTransformPalette_impl, prog->samplers.sTransformPalette_slot);
 self->sPrevPassAlpha = lookup_sampler_array(&prog->samplers.sPrevPassAlpha_impl, prog->samplers.sPrevPassAlpha_slot);
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
vec3 vUv;
vec4_scalar vUvRect;
vec2_scalar vOffsetScale;
float vSigma;
int32_t vSupport;
I32 aBlurRenderTaskAddress;
I32 aBlurSourceTaskAddress;
I32 aBlurDirection;
struct BlurTask_scalar {
RenderTaskCommonData_scalar common_data;
float blur_radius;
vec2_scalar blur_region;
BlurTask_scalar() = default;
BlurTask_scalar(RenderTaskCommonData_scalar common_data, float blur_radius, vec2_scalar blur_region) : common_data(common_data), blur_radius(blur_radius), blur_region(blur_region){}
};
struct BlurTask {
RenderTaskCommonData common_data;
Float blur_radius;
vec2 blur_region;
BlurTask() = default;
BlurTask(RenderTaskCommonData common_data, Float blur_radius, vec2 blur_region) : common_data(common_data), blur_radius(blur_radius), blur_region(blur_region){}
BlurTask(RenderTaskCommonData_scalar common_data, float blur_radius, vec2_scalar blur_region):common_data(common_data),blur_radius(blur_radius),blur_region(blur_region){
}
BlurTask(BlurTask_scalar s):common_data(s.common_data),blur_radius(s.blur_radius),blur_region(s.blur_region){
}
friend BlurTask if_then_else(I32 c, BlurTask t, BlurTask e) { return BlurTask(
if_then_else(c, t.common_data, e.common_data), if_then_else(c, t.blur_radius, e.blur_radius), if_then_else(c, t.blur_region, e.blur_region));
}};
RenderTaskData fetch_render_task_data(I32 index) {
 ivec2 uv = make_ivec2(make_int((2u)*((make_uint(index))%((1024u)/(2u)))), make_int((make_uint(index))/((1024u)/(2u))));
 vec4 texel0 = texelFetch(sRenderTasks, (uv)+(make_ivec2(0, 0)), 0);
 vec4 texel1 = texelFetch(sRenderTasks, (uv)+(make_ivec2(1, 0)), 0);
 RectWithSize task_rect = RectWithSize((texel0).sel(X, Y), (texel0).sel(Z, W));
 RenderTaskCommonData common_data = RenderTaskCommonData(task_rect, (texel1).sel(X));
 RenderTaskData data = RenderTaskData(common_data, (texel1).sel(Y, Z, W));
 return data;
}
BlurTask fetch_blur_task(I32 address) {
 RenderTaskData task_data = fetch_render_task_data(address);
 BlurTask task = BlurTask((task_data).common_data, ((task_data).user_data).sel(X), ((task_data).user_data).sel(Y, Z));
 return task;
}
RenderTaskCommonData fetch_render_task_common_data(I32 index) {
 ivec2 uv = make_ivec2(make_int((2u)*((make_uint(index))%((1024u)/(2u)))), make_int((make_uint(index))/((1024u)/(2u))));
 vec4 texel0 = texelFetch(sRenderTasks, (uv)+(make_ivec2(0, 0)), 0);
 vec4 texel1 = texelFetch(sRenderTasks, (uv)+(make_ivec2(1, 0)), 0);
 RectWithSize task_rect = RectWithSize((texel0).sel(X, Y), (texel0).sel(Z, W));
 RenderTaskCommonData data = RenderTaskCommonData(task_rect, (texel1).sel(X));
 return data;
}
ALWAYS_INLINE void main(void) {
 BlurTask blur_task = fetch_blur_task(aBlurRenderTaskAddress);
 RenderTaskCommonData src_task = fetch_render_task_common_data(aBlurSourceTaskAddress);
 RectWithSize src_rect = (src_task).task_rect;
 RectWithSize target_rect = ((blur_task).common_data).task_rect;
 vec2_scalar texture_size = make_vec2((textureSize(sPrevPassAlpha, 0)).sel(X, Y));
 (vUv).sel(Z) = (src_task).texture_layer_index;
 vSigma = force_scalar((blur_task).blur_radius);
 vSupport = force_scalar((make_int(ceil((1.5f)*((blur_task).blur_radius))))*(2));
 auto _c4_ = (aBlurDirection)==(0);
 {
  if (_c4_[0]) { vOffsetScale = make_vec2((1.f)/((texture_size).sel(X)), 0.f); };
 }
 auto _c5_ = (aBlurDirection)==(1);
 auto _c6_ = (~(_c4_))&(_c5_);
 {
  if (_c6_[0]) { vOffsetScale = make_vec2(0.f, (1.f)/((texture_size).sel(Y))); };
 }
 _c5_ = (~(_c4_))&(~(_c5_));
 if (true) {
  {
   if (_c5_[0]) { vOffsetScale = make_vec2(0.f); };
  }
 }
 vUvRect = force_scalar(make_vec4(((src_rect).p0)+(make_vec2(0.5f)), ((src_rect).p0)+(((blur_task).blur_region)-(make_vec2(0.5f)))));
 vUvRect /= (texture_size).sel(X, Y, X, Y);
 vec2 pos = ((target_rect).p0)+(((target_rect).size)*((aPosition).sel(X, Y)));
 vec2 uv0 = ((src_rect).p0)/(texture_size);
 vec2 uv1 = (((src_rect).p0)+((src_rect).size))/(texture_size);
 (vUv).lsel(X, Y) = mix(uv0, uv1, (aPosition).sel(X, Y));
 gl_Position = (uTransform)*(make_vec4(pos, 0.f, 1.f));
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
vec3 vUv
vec4 vUvRect
vec2 vOffsetScale
Float vSigma
I32 vSupport
*/
/* outputs
vec4 oFragColor
*/
struct cs_blurALPHA_TARGET_frag : FragmentShaderImpl {
typedef cs_blurALPHA_TARGET_frag Self;
static void set_uniform_1i(Self *self, int index, int value) {
 switch (index) {
 case 2:
  assert(0); // sGpuCache
  break;
 case 7:
  assert(0); // sPrevPassAlpha
  break;
 }
}
static void set_uniform_4fv(Self *self, int index, const float *value) {
 switch (index) {
 case 2:
  assert(0); // sGpuCache
  break;
 case 7:
  assert(0); // sPrevPassAlpha
  break;
 }
}
static void set_uniform_matrix4fv(Self *self, int index, const float *value) {
 switch (index) {
 case 2:
  assert(0); // sGpuCache
  break;
 case 7:
  assert(0); // sPrevPassAlpha
  break;
 }
}
typedef cs_blurALPHA_TARGET_vert::FlatOutputs FlatInputs;
typedef cs_blurALPHA_TARGET_vert::InterpOutputs InterpInputs;
static void read_flat_inputs(Self *self, const FlatInputs *src) {
  self->vTransformBounds = src->vTransformBounds;
  self->vClipMaskUvBounds = src->vClipMaskUvBounds;
  self->vUvRect = src->vUvRect;
  self->vOffsetScale = src->vOffsetScale;
  self->vSigma = src->vSigma;
  self->vSupport = src->vSupport;
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
static void bind_textures(Self *self, cs_blurALPHA_TARGET_program *prog) {
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
vec3 vUv;
vec4_scalar vUvRect;
vec2_scalar vOffsetScale;
float vSigma;
int32_t vSupport;
ALWAYS_INLINE void main(void) {
 Float original_color = (texture(sPrevPassAlpha, vUv)).sel(R);
 if ((vSupport)==(0)) {
  {
   oFragColor = make_vec4(original_color);
   return;
  }
 }
 vec3_scalar gauss_coefficient;
 (gauss_coefficient).sel(X) = (1.f)/((sqrt((2.f)*(3.14159265f)))*(vSigma));
 (gauss_coefficient).sel(Y) = exp((-(0.5f))/((vSigma)*(vSigma)));
 (gauss_coefficient).sel(Z) = ((gauss_coefficient).sel(Y))*((gauss_coefficient).sel(Y));
 float gauss_coefficient_total = (gauss_coefficient).sel(X);
 Float avg_color = (original_color)*((gauss_coefficient).sel(X));
 (gauss_coefficient).lsel(X, Y) *= (gauss_coefficient).sel(Y, Z);
 for ( int32_t i = 1;
(i)<=(vSupport); i += 2)  {
  float gauss_coefficient_subtotal = (gauss_coefficient).sel(X);
  (gauss_coefficient).lsel(X, Y) *= (gauss_coefficient).sel(Y, Z);
  gauss_coefficient_subtotal += (gauss_coefficient).sel(X);
  float gauss_ratio = ((gauss_coefficient).sel(X))/(gauss_coefficient_subtotal);
  vec2_scalar offset = (vOffsetScale)*((make_float(i))+(gauss_ratio));
  vec2 st0 = clamp(((vUv).sel(X, Y))-(offset), (vUvRect).sel(X, Y), (vUvRect).sel(Z, W));
  avg_color += ((texture(sPrevPassAlpha, make_vec3(st0, (vUv).sel(Z)))).sel(R))*(gauss_coefficient_subtotal);
  vec2 st1 = clamp(((vUv).sel(X, Y))+(offset), (vUvRect).sel(X, Y), (vUvRect).sel(Z, W));
  avg_color += ((texture(sPrevPassAlpha, make_vec3(st1, (vUv).sel(Z)))).sel(R))*(gauss_coefficient_subtotal);
  gauss_coefficient_total += (2.f)*(gauss_coefficient_subtotal);
  (gauss_coefficient).lsel(X, Y) *= (gauss_coefficient).sel(Y, Z);
 }
 oFragColor = (make_vec4(avg_color))/(gauss_coefficient_total);
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
void cs_blurALPHA_TARGET_program::init_shaders(void *vertex_shader, void *fragment_shader) {
 reinterpret_cast<cs_blurALPHA_TARGET_vert*>(vertex_shader)->init_shader();
 reinterpret_cast<cs_blurALPHA_TARGET_frag*>(fragment_shader)->init_shader();
}

