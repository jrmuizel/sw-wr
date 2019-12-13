/* inputs
vec3 aPosition
ivec4 aData
*/
/* outputs
vec4 vTransformBounds
vec4 vClipMaskUvBounds
vec4 vClipMaskUv
vec4 vColor
vec3 vUv
vec4 vUvBorder
vec2 vMaskSwizzle
*/
struct ps_text_runDUAL_SOURCE_BLENDING_program : ProgramImpl {
const char *get_name() const override { return "ps_text_runDUAL_SOURCE_BLENDING"; }
int get_uniform(const char *name) const override {
 if (strcmp("sColor0", name) == 0) { return 8; }
 if (strcmp("sGpuCache", name) == 0) { return 2; }
 if (strcmp("sPrevPassAlpha", name) == 0) { return 9; }
 if (strcmp("sPrimitiveHeadersF", name) == 0) { return 4; }
 if (strcmp("sPrimitiveHeadersI", name) == 0) { return 5; }
 if (strcmp("sRenderTasks", name) == 0) { return 1; }
 if (strcmp("sTransformPalette", name) == 0) { return 3; }
 if (strcmp("uMode", name) == 0) { return 7; }
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
 case 8:
  samplers.sColor0_slot = value;
  return true;
 case 2:
  samplers.sGpuCache_slot = value;
  return true;
 case 9:
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
struct ps_text_runDUAL_SOURCE_BLENDING_vert : VertexShaderImpl {
typedef ps_text_runDUAL_SOURCE_BLENDING_vert Self;
static void set_uniform_1i(Self *self, int index, int value) {
 switch (index) {
 case 7:
  self->uMode = int32_t(value);
  break;
 case 6:
  assert(0); // uTransform
  break;
 case 8:
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
 case 7:
  assert(0); // uMode
  break;
 case 6:
  assert(0); // uTransform
  break;
 case 8:
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
 case 7:
  assert(0); // uMode
  break;
 case 6:
  self->uTransform = mat4_scalar::load_from_ptr(value);
  break;
 case 8:
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
static void load_attribs(Self *self, ps_text_runDUAL_SOURCE_BLENDING_program *prog, VertexAttrib *attribs, unsigned short *indices, int start, int instance, int count) {
 load_attrib(self->aPosition, attribs[prog->attrib_locations.aPosition], indices, start, instance, count);
 load_flat_attrib(self->aData, attribs[prog->attrib_locations.aData], indices, start, instance, count);
}
struct FlatOutputs {
vec4_scalar vTransformBounds;
vec4_scalar vClipMaskUvBounds;
vec4_scalar vColor;
vec4_scalar vUvBorder;
vec2_scalar vMaskSwizzle;
};
struct InterpOutputs {
vec4_scalar vClipMaskUv;
vec3_scalar vUv;
};
ALWAYS_INLINE void store_flat_outputs(char* dest_ptr) {
  auto* dest = reinterpret_cast<FlatOutputs*>(dest_ptr);
  dest->vTransformBounds = vTransformBounds;
  dest->vClipMaskUvBounds = vClipMaskUvBounds;
  dest->vColor = vColor;
  dest->vUvBorder = vUvBorder;
  dest->vMaskSwizzle = vMaskSwizzle;
}
ALWAYS_INLINE void store_interp_outputs(char* dest_ptr, size_t stride) {
  for(int n = 0; n < 4; n++) {
    auto* dest = reinterpret_cast<InterpOutputs*>(dest_ptr);
    dest->vClipMaskUv = get_nth(vClipMaskUv, n);
    dest->vUv = get_nth(vUv, n);
    dest_ptr += stride;
  }
}
static void bind_textures(Self *self, ps_text_runDUAL_SOURCE_BLENDING_program *prog) {
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
ivec4_scalar aData;
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
vec4_scalar vColor;
vec3 vUv;
vec4_scalar vUvBorder;
vec2_scalar vMaskSwizzle;
struct Glyph_scalar {
vec2_scalar offset;
Glyph_scalar() = default;
Glyph_scalar(vec2_scalar offset) : offset(offset){}
};
struct Glyph {
vec2 offset;
Glyph() = default;
Glyph(vec2 offset) : offset(offset){}
Glyph(vec2_scalar offset):offset(offset){
}
Glyph(Glyph_scalar s):offset(s.offset){
}
friend Glyph if_then_else(I32 c, Glyph t, Glyph e) { return Glyph(
if_then_else(c, t.offset, e.offset));
}};
struct GlyphResource_scalar {
vec4_scalar uv_rect;
float layer;
vec2_scalar offset;
float scale;
GlyphResource_scalar() = default;
GlyphResource_scalar(vec4_scalar uv_rect, float layer, vec2_scalar offset, float scale) : uv_rect(uv_rect), layer(layer), offset(offset), scale(scale){}
};
struct GlyphResource {
vec4 uv_rect;
Float layer;
vec2 offset;
Float scale;
GlyphResource() = default;
GlyphResource(vec4 uv_rect, Float layer, vec2 offset, Float scale) : uv_rect(uv_rect), layer(layer), offset(offset), scale(scale){}
GlyphResource(vec4_scalar uv_rect, float layer, vec2_scalar offset, float scale):uv_rect(uv_rect),layer(layer),offset(offset),scale(scale){
}
GlyphResource(GlyphResource_scalar s):uv_rect(s.uv_rect),layer(s.layer),offset(s.offset),scale(s.scale){
}
friend GlyphResource if_then_else(I32 c, GlyphResource t, GlyphResource e) { return GlyphResource(
if_then_else(c, t.uv_rect, e.uv_rect), if_then_else(c, t.layer, e.layer), if_then_else(c, t.offset, e.offset), if_then_else(c, t.scale, e.scale));
}};
struct TextRun_scalar {
vec4_scalar color;
vec4_scalar bg_color;
TextRun_scalar() = default;
TextRun_scalar(vec4_scalar color, vec4_scalar bg_color) : color(color), bg_color(bg_color){}
};
struct TextRun {
vec4 color;
vec4 bg_color;
TextRun() = default;
TextRun(vec4 color, vec4 bg_color) : color(color), bg_color(bg_color){}
TextRun(vec4_scalar color, vec4_scalar bg_color):color(color),bg_color(bg_color){
}
TextRun(TextRun_scalar s):color(s.color),bg_color(s.bg_color){
}
friend TextRun if_then_else(I32 c, TextRun t, TextRun e) { return TextRun(
if_then_else(c, t.color, e.color), if_then_else(c, t.bg_color, e.bg_color));
}};
PrimitiveHeader_scalar fetch_prim_header(int32_t index) {
 PrimitiveHeader_scalar ph;
 ivec2_scalar uv_f = make_ivec2(make_int((2u)*((make_uint(index))%((1024u)/(2u)))), make_int((make_uint(index))/((1024u)/(2u))));
 vec4_scalar local_rect = texelFetch(sPrimitiveHeadersF, (uv_f)+(make_ivec2(0, 0)), 0);
 vec4_scalar local_clip_rect = texelFetch(sPrimitiveHeadersF, (uv_f)+(make_ivec2(1, 0)), 0);
 (ph).local_rect = RectWithSize_scalar((local_rect).sel(X, Y), (local_rect).sel(Z, W));
 (ph).local_clip_rect = RectWithSize_scalar((local_clip_rect).sel(X, Y), (local_clip_rect).sel(Z, W));
 ivec2_scalar uv_i = make_ivec2(make_int((2u)*((make_uint(index))%((1024u)/(2u)))), make_int((make_uint(index))/((1024u)/(2u))));
 ivec4_scalar data0 = texelFetch(sPrimitiveHeadersI, (uv_i)+(make_ivec2(0, 0)), 0);
 ivec4_scalar data1 = texelFetch(sPrimitiveHeadersI, (uv_i)+(make_ivec2(1, 0)), 0);
 (ph).z = make_float((data0).sel(X));
 (ph).specific_prim_address = (data0).sel(Y);
 (ph).transform_id = (data0).sel(Z);
 (ph).user_data = data1;
 return ph;
}
Transform_scalar fetch_transform(int32_t id) {
 Transform_scalar transform;
 (transform).is_axis_aligned = ((id)>>(24))==(0);
 int32_t index = (id)&(16777215);
 ivec2_scalar uv = make_ivec2(make_int((8u)*((make_uint(index))%((1024u)/(8u)))), make_int((make_uint(index))/((1024u)/(8u))));
 ivec2_scalar uv0 = make_ivec2(((uv).sel(X))+(0), (uv).sel(Y));
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
RenderTaskData_scalar fetch_render_task_data(int32_t index) {
 ivec2_scalar uv = make_ivec2(make_int((2u)*((make_uint(index))%((1024u)/(2u)))), make_int((make_uint(index))/((1024u)/(2u))));
 vec4_scalar texel0 = texelFetch(sRenderTasks, (uv)+(make_ivec2(0, 0)), 0);
 vec4_scalar texel1 = texelFetch(sRenderTasks, (uv)+(make_ivec2(1, 0)), 0);
 RectWithSize_scalar task_rect = RectWithSize_scalar((texel0).sel(X, Y), (texel0).sel(Z, W));
 RenderTaskCommonData_scalar common_data = RenderTaskCommonData_scalar(task_rect, (texel1).sel(X));
 RenderTaskData_scalar data = RenderTaskData_scalar(common_data, (texel1).sel(Y, Z, W));
 return data;
}
ClipArea_scalar fetch_clip_area(int32_t index) {
 ClipArea_scalar area;
 if ((index)>=(32767)) {
  {
   RectWithSize_scalar rect = RectWithSize_scalar(make_vec2(0.), make_vec2(0.));
   (area).common_data = RenderTaskCommonData_scalar(rect, 0.);
   (area).device_pixel_scale = 0.;
   (area).screen_origin = make_vec2(0.);
  }
 } else  {
  RenderTaskData_scalar task_data = fetch_render_task_data(index);
  (area).common_data = (task_data).common_data;
  (area).device_pixel_scale = ((task_data).user_data).sel(X);
  (area).screen_origin = ((task_data).user_data).sel(Y, Z);
 }
 return area;
}
PictureTask_scalar fetch_picture_task(int32_t address) {
 RenderTaskData_scalar task_data = fetch_render_task_data(address);
 PictureTask_scalar task = PictureTask_scalar((task_data).common_data, ((task_data).user_data).sel(X), ((task_data).user_data).sel(Y, Z));
 return task;
}
ivec2_scalar get_gpu_cache_uv(int32_t address) {
 return make_ivec2((make_uint(address))%(1024u), (make_uint(address))/(1024u));
}
std::array<vec4_scalar,2> fetch_from_gpu_cache_2(int32_t address) {
 ivec2_scalar uv = get_gpu_cache_uv(address);
 return std::array<vec4_scalar,2>{texelFetch(sGpuCache, (uv)+(make_ivec2(0, 0)), 0), texelFetch(sGpuCache, (uv)+(make_ivec2(1, 0)), 0)};
}
TextRun_scalar fetch_text_run(int32_t address) {
 std::array<vec4_scalar,2> data = fetch_from_gpu_cache_2(address);
 return TextRun_scalar(data[0], data[1]);
}
vec4_scalar fetch_from_gpu_cache_1(int32_t address) {
 ivec2_scalar uv = get_gpu_cache_uv(address);
 return texelFetch(sGpuCache, uv, 0);
}
Glyph_scalar fetch_glyph(int32_t specific_prim_address, int32_t glyph_index) {
 int32_t glyph_address = (specific_prim_address)+((2)+(make_int((make_uint(glyph_index))/(2u))));
 vec4_scalar data = fetch_from_gpu_cache_1(glyph_address);
 vec2_scalar glyph = mix((data).sel(X, Y), (data).sel(Z, W), make_bvec2(((make_uint(glyph_index))%(2u))!=(0u)));
 return Glyph_scalar(glyph);
}
GlyphResource_scalar fetch_glyph_resource(int32_t address) {
 std::array<vec4_scalar,2> data = fetch_from_gpu_cache_2(address);
 return GlyphResource_scalar(data[0], (data[1]).sel(X), (data[1]).sel(Y, Z), (data[1]).sel(W));
}
vec2 clamp_rect(vec2 pt, RectWithSize_scalar rect) {
 return clamp(pt, (rect).p0, ((rect).p0)+((rect).size));
}
VertexInfo write_text_vertex(RectWithSize_scalar local_clip_rect, float z, int32_t raster_space, Transform_scalar transform, PictureTask_scalar task, vec2_scalar text_offset, vec2_scalar glyph_offset, RectWithSize glyph_rect, vec2_scalar snap_bias) {
 vec2_scalar snap_offset = make_vec2(0.);
 mat2_scalar glyph_transform_inv = make_mat2(1.);
 bool remove_subpx_offset = (transform).is_axis_aligned;
 if (remove_subpx_offset) {
  {
   switch (raster_space) {
    case 1:
     {
      float device_scale = ((task).device_pixel_scale)/(((transform).m[3]).sel(W));
      mat2_scalar glyph_transform = (make_mat2((transform).m))*(device_scale);
      vec2_scalar device_text_pos = ((glyph_transform)*(text_offset))+((((transform).m[3]).sel(X, Y))*(device_scale));
      snap_offset = (floor((device_text_pos)+(0.5)))-(device_text_pos);
      vec2_scalar device_glyph_offset = (glyph_transform)*(glyph_offset);
      snap_offset += (floor((device_glyph_offset)+(snap_bias)))-(device_glyph_offset);
      glyph_transform_inv = inverse(glyph_transform);
      snap_offset = (glyph_transform_inv)*(snap_offset);
      break;
     }
    default:
     {
      snap_offset = (floor((text_offset)+(0.5)))-(text_offset);
      snap_offset += (floor((glyph_offset)+(snap_bias)))-(glyph_offset);
      break;
     }
   }
  }
 }
 (glyph_rect).p0 += snap_offset;
 vec2 local_pos = ((glyph_rect).p0)+(((glyph_rect).size)*((aPosition).sel(X, Y)));
 local_pos = clamp_rect(local_pos, local_clip_rect);
 vec4 world_pos = ((transform).m)*(make_vec4(local_pos, 0., 1.));
 vec2 device_pos = ((world_pos).sel(X, Y))*((task).device_pixel_scale);
 vec2_scalar final_offset = (-((task).content_origin))+((((task).common_data).task_rect).p0);
 gl_Position = (uTransform)*(make_vec4((device_pos)+((final_offset)*((world_pos).sel(W))), (z)*((world_pos).sel(W)), (world_pos).sel(W)));
 VertexInfo vi = VertexInfo(local_pos, snap_offset, world_pos);
 return vi;
}
void write_clip(vec4 world_pos, vec2 snap_offset, ClipArea_scalar area) {
 vec2 uv = (((world_pos).sel(X, Y))*((area).device_pixel_scale))+(((world_pos).sel(W))*((snap_offset)+(((((area).common_data).task_rect).p0)-((area).screen_origin))));
 vClipMaskUvBounds = make_vec4((((area).common_data).task_rect).p0, ((((area).common_data).task_rect).p0)+((((area).common_data).task_rect).size));
 vClipMaskUv = make_vec4(uv, ((area).common_data).texture_layer_index, (world_pos).sel(W));
}
ALWAYS_INLINE void main(void) {
 int32_t prim_header_address = (aData).sel(X);
 int32_t glyph_index = ((aData).sel(Y))&(65535);
 int32_t render_task_index = ((aData).sel(Y))>>(16);
 int32_t resource_address = (aData).sel(Z);
 int32_t raster_space = ((aData).sel(W))>>(16);
 int32_t subpx_dir = (((aData).sel(W))>>(8))&(255);
 int32_t color_mode = ((aData).sel(W))&(255);
 PrimitiveHeader_scalar ph = fetch_prim_header(prim_header_address);
 Transform_scalar transform = fetch_transform((ph).transform_id);
 ClipArea_scalar clip_area = fetch_clip_area(((ph).user_data).sel(W));
 PictureTask_scalar task = fetch_picture_task(render_task_index);
 TextRun_scalar text = fetch_text_run((ph).specific_prim_address);
 vec2_scalar text_offset = (make_vec2(((ph).user_data).sel(X, Y)))/(256.);
 if ((color_mode)==(0)) {
  {
   color_mode = uMode;
  }
 }
 Glyph_scalar glyph = fetch_glyph((ph).specific_prim_address, glyph_index);
 (glyph).offset += (((ph).local_rect).p0)-(text_offset);
 GlyphResource_scalar res = fetch_glyph_resource(resource_address);
 float raster_scale = (make_float(((ph).user_data).sel(Z)))/(65535.);
 float scale = ((res).scale)/((raster_scale)*((task).device_pixel_scale));
 RectWithSize glyph_rect = RectWithSize_scalar(((scale)*((res).offset))+((text_offset)+((glyph).offset)), (scale)*((((res).uv_rect).sel(Z, W))-(((res).uv_rect).sel(X, Y))));
 vec2_scalar snap_bias;
 switch (subpx_dir) {
  case 0:
  default:
   snap_bias = make_vec2(0.5);
   break;
  case 1:
   snap_bias = make_vec2(0.125, 0.5);
   break;
  case 2:
   snap_bias = make_vec2(0.5, 0.125);
   break;
  case 3:
   snap_bias = make_vec2(0.125);
   break;
 }
 VertexInfo vi = write_text_vertex((ph).local_clip_rect, (ph).z, raster_space, transform, task, text_offset, (glyph).offset, glyph_rect, snap_bias);
 (glyph_rect).p0 += (vi).snap_offset;
 vec2 f = (((vi).local_pos)-((glyph_rect).p0))/((glyph_rect).size);
 write_clip((vi).world_pos, (vi).snap_offset, clip_area);
 switch (color_mode) {
  case 1:
  case 7:
   vMaskSwizzle = make_vec2(0., 1.);
   vColor = (text).color;
   break;
  case 5:
  case 6:
   vMaskSwizzle = make_vec2(1., 0.);
   vColor = (text).color;
   break;
  case 2:
  case 3:
  case 8:
   vMaskSwizzle = make_vec2(1., 0.);
   vColor = make_vec4(((text).color).sel(A));
   break;
  case 4:
   vMaskSwizzle = make_vec2(-(1.), 1.);
   vColor = (make_vec4(((text).color).sel(A)))*((text).bg_color);
   break;
  default:
   vMaskSwizzle = make_vec2(0.);
   vColor = make_vec4(1.);
 }
 vec2_scalar texture_size = make_vec2(textureSize(sColor0, 0));
 vec2_scalar st0 = (((res).uv_rect).sel(X, Y))/(texture_size);
 vec2_scalar st1 = (((res).uv_rect).sel(Z, W))/(texture_size);
 vUv = make_vec3(mix(st0, st1, f), (res).layer);
 vUvBorder = (((res).uv_rect)+(make_vec4(0.5, 0.5, -(0.5), -(0.5))))/((texture_size).sel(X, Y, X, Y));
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
vec4 vColor
vec3 vUv
vec4 vUvBorder
vec2 vMaskSwizzle
*/
/* outputs
vec4 oFragColor
vec4 oFragBlend
*/
struct ps_text_runDUAL_SOURCE_BLENDING_frag : FragmentShaderImpl {
typedef ps_text_runDUAL_SOURCE_BLENDING_frag Self;
static void set_uniform_1i(Self *self, int index, int value) {
 switch (index) {
 case 8:
  assert(0); // sColor0
  break;
 case 2:
  assert(0); // sGpuCache
  break;
 case 9:
  assert(0); // sPrevPassAlpha
  break;
 }
}
static void set_uniform_4fv(Self *self, int index, const float *value) {
 switch (index) {
 case 8:
  assert(0); // sColor0
  break;
 case 2:
  assert(0); // sGpuCache
  break;
 case 9:
  assert(0); // sPrevPassAlpha
  break;
 }
}
static void set_uniform_matrix4fv(Self *self, int index, const float *value) {
 switch (index) {
 case 8:
  assert(0); // sColor0
  break;
 case 2:
  assert(0); // sGpuCache
  break;
 case 9:
  assert(0); // sPrevPassAlpha
  break;
 }
}
typedef ps_text_runDUAL_SOURCE_BLENDING_vert::FlatOutputs FlatInputs;
typedef ps_text_runDUAL_SOURCE_BLENDING_vert::InterpOutputs InterpInputs;
static void read_flat_inputs(Self *self, const FlatInputs *src) {
  self->vTransformBounds = src->vTransformBounds;
  self->vClipMaskUvBounds = src->vClipMaskUvBounds;
  self->vColor = src->vColor;
  self->vUvBorder = src->vUvBorder;
  self->vMaskSwizzle = src->vMaskSwizzle;
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
static void bind_textures(Self *self, ps_text_runDUAL_SOURCE_BLENDING_program *prog) {
 self->sColor0 = lookup_sampler_array(&prog->samplers.sColor0_impl, prog->samplers.sColor0_slot);
 self->sGpuCache = lookup_sampler(&prog->samplers.sGpuCache_impl, prog->samplers.sGpuCache_slot);
 self->sPrevPassAlpha = lookup_sampler_array(&prog->samplers.sPrevPassAlpha_impl, prog->samplers.sPrevPassAlpha_slot);
}
#define oFragColor gl_FragColor
// vec4 oFragColor;
#define oFragBlend gl_SecondaryFragColor
// vec4 oFragBlend;
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
vec4_scalar vColor;
vec3 vUv;
vec4_scalar vUvBorder;
vec2_scalar vMaskSwizzle;
Float do_clip() {
 I32 ret_mask = ~0;
 Float ret;
 if (((vClipMaskUvBounds).sel(X, Y))==((vClipMaskUvBounds).sel(Z, W))) {
  {
   return 1.;
  }
 }
 vec2 mask_uv = ((vClipMaskUv).sel(X, Y))*((gl_FragCoord).sel(W));
 bvec2 left = lessThanEqual((vClipMaskUvBounds).sel(X, Y), mask_uv);
 bvec2 right = greaterThan((vClipMaskUvBounds).sel(Z, W), mask_uv);
 auto _c2_ = !(all(make_bvec4(left, right)));
 {
  ret = 0.;
  ret_mask = ~I32(_c2_);
 }
 ivec3 tc = make_ivec3(mask_uv, ((vClipMaskUv).sel(Z))+(0.5));
 ret = if_then_else(ret_mask, (texelFetch(sPrevPassAlpha, tc, 0)).sel(R), ret);
 return ret;
}
ALWAYS_INLINE void main(void) {
 vec3 tc = make_vec3(clamp((vUv).sel(X, Y), (vUvBorder).sel(X, Y), (vUvBorder).sel(Z, W)), (vUv).sel(Z));
 vec4 mask = texture(sColor0, tc);
 (mask).lsel(R, G, B) = (((mask).sel(R, G, B))*((vMaskSwizzle).sel(X)))+(((mask).sel(A, A, A))*((vMaskSwizzle).sel(Y)));
 Float alpha = do_clip();
 vec4 alpha_mask = (mask)*(alpha);
 oFragColor = (vColor)*(alpha_mask);
 oFragBlend = (alpha_mask)*((vColor).sel(A));
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
void ps_text_runDUAL_SOURCE_BLENDING_program::init_shaders(void *vertex_shader, void *fragment_shader) {
 reinterpret_cast<ps_text_runDUAL_SOURCE_BLENDING_vert*>(vertex_shader)->init_shader();
 reinterpret_cast<ps_text_runDUAL_SOURCE_BLENDING_frag*>(fragment_shader)->init_shader();
}

