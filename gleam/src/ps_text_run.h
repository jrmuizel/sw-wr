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
struct ps_text_run_samplers {
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
};
struct ps_text_run_vert : VertexShaderImpl {
typedef ps_text_run_vert Self;
typedef ps_text_run_samplers Samplers;
static void set_uniform_1i(Self *self, Samplers *samplers, int index, int value) {
 switch (index) {
 case 7:
  self->uMode = int32_t(value);
  break;
 case 6:
  assert(0); // uTransform
  break;
 case 8:
  samplers->sColor0_slot = value;
  break;
 case 1:
  samplers->sRenderTasks_slot = value;
  break;
 case 2:
  samplers->sGpuCache_slot = value;
  break;
 case 3:
  samplers->sTransformPalette_slot = value;
  break;
 case 4:
  samplers->sPrimitiveHeadersF_slot = value;
  break;
 case 5:
  samplers->sPrimitiveHeadersI_slot = value;
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
struct AttribLocations {
 int aPosition;
 int aData;
};
static void bind_attrib_location(AttribLocations* locs, const char *name, int index) {
 if (strcmp("aPosition", name) == 0) { locs->aPosition = index; return; }
 if (strcmp("aData", name) == 0) { locs->aData = index; return; }
}
static void load_attribs(Self *self, const AttribLocations *locs, VertexAttrib *attribs, unsigned short *indices, int start, int instance, int count) {
 load_attrib(self->aPosition, attribs[locs->aPosition], indices, start, instance, count);
 load_attrib(self->aData, attribs[locs->aData], indices, start, instance, count);
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
static void bind_textures(Self *self, Samplers *samplers) {
 self->sColor0 = lookup_sampler_array(&samplers->sColor0_impl, samplers->sColor0_slot);
 self->sRenderTasks = lookup_sampler(&samplers->sRenderTasks_impl, samplers->sRenderTasks_slot);
 self->sGpuCache = lookup_sampler(&samplers->sGpuCache_impl, samplers->sGpuCache_slot);
 self->sTransformPalette = lookup_sampler(&samplers->sTransformPalette_impl, samplers->sTransformPalette_slot);
 self->sPrimitiveHeadersF = lookup_sampler(&samplers->sPrimitiveHeadersF_impl, samplers->sPrimitiveHeadersF_slot);
 self->sPrimitiveHeadersI = lookup_isampler(&samplers->sPrimitiveHeadersI_impl, samplers->sPrimitiveHeadersI_slot);
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
RenderTaskData fetch_render_task_data(I32 index) {
 ivec2 uv = make_ivec2(make_int((2u)*((make_uint(index))%((1024u)/(2u)))), make_int((make_uint(index))/((1024u)/(2u))));
 vec4 texel0 = texelFetch(sRenderTasks, (uv)+(make_ivec2(0, 0)), 0);
 vec4 texel1 = texelFetch(sRenderTasks, (uv)+(make_ivec2(1, 0)), 0);
 RectWithSize task_rect = RectWithSize((texel0).sel(X, Y), (texel0).sel(Z, W));
 RenderTaskCommonData common_data = RenderTaskCommonData(task_rect, (texel1).sel(X));
 RenderTaskData data = RenderTaskData(common_data, (texel1).sel(Y, Z, W));
 return data;
}
ClipArea fetch_clip_area(I32 index) {
 ClipArea area;
 auto _c28_ = (index)>=(32767);
 {
  RectWithSize_scalar rect = RectWithSize_scalar(make_vec2(0.), make_vec2(0.));
  (area).common_data = if_then_else(_c28_,RenderTaskCommonData_scalar(rect, 0.),(area).common_data);
  (area).device_pixel_scale = if_then_else(_c28_,0.,(area).device_pixel_scale);
  (area).screen_origin = if_then_else(_c28_,make_vec2(0.),(area).screen_origin);
 }
 {
  RenderTaskData task_data = fetch_render_task_data(index);
  (area).common_data = if_then_else(~(_c28_),(task_data).common_data,(area).common_data);
  (area).device_pixel_scale = if_then_else(~(_c28_),((task_data).user_data).sel(X),(area).device_pixel_scale);
  (area).screen_origin = if_then_else(~(_c28_),((task_data).user_data).sel(Y, Z),(area).screen_origin);
 }
 return area;
}
PictureTask fetch_picture_task(I32 address) {
 RenderTaskData task_data = fetch_render_task_data(address);
 PictureTask task = PictureTask((task_data).common_data, ((task_data).user_data).sel(X), ((task_data).user_data).sel(Y, Z));
 return task;
}
ivec2 get_gpu_cache_uv(I32 address) {
 return make_ivec2((make_uint(address))%(1024u), (make_uint(address))/(1024u));
}
std::array<vec4,2> fetch_from_gpu_cache_2(I32 address) {
 ivec2 uv = get_gpu_cache_uv(address);
 return std::array<vec4,2>{texelFetch(sGpuCache, (uv)+(make_ivec2(0, 0)), 0), texelFetch(sGpuCache, (uv)+(make_ivec2(1, 0)), 0)};
}
TextRun fetch_text_run(I32 address) {
 std::array<vec4,2> data = fetch_from_gpu_cache_2(address);
 return TextRun(data[0], data[1]);
}
vec4 fetch_from_gpu_cache_1(I32 address) {
 ivec2 uv = get_gpu_cache_uv(address);
 return texelFetch(sGpuCache, uv, 0);
}
Glyph fetch_glyph(I32 specific_prim_address, I32 glyph_index) {
 I32 glyph_address = (specific_prim_address)+((2)+(make_int((make_uint(glyph_index))/(2u))));
 vec4 data = fetch_from_gpu_cache_1(glyph_address);
 vec2 glyph = mix((data).sel(X, Y), (data).sel(Z, W), make_bvec2(((make_uint(glyph_index))%(2u))!=(0u)));
 return Glyph(glyph);
}
GlyphResource fetch_glyph_resource(I32 address) {
 std::array<vec4,2> data = fetch_from_gpu_cache_2(address);
 return GlyphResource(data[0], (data[1]).sel(X), (data[1]).sel(Y, Z), (data[1]).sel(W));
}
vec2 clamp_rect(vec2 pt, RectWithSize rect) {
 return clamp(pt, (rect).p0, ((rect).p0)+((rect).size));
}
VertexInfo write_text_vertex(RectWithSize local_clip_rect, Float z, I32 raster_space, Transform transform, PictureTask task, vec2 text_offset, vec2 glyph_offset, RectWithSize glyph_rect, vec2 snap_bias) {
 vec2 snap_offset = make_vec2(0.);
 mat2 glyph_transform_inv = make_mat2(1.);
 Bool remove_subpx_offset = (transform).is_axis_aligned;
 auto _c32_ = remove_subpx_offset;
 {
  auto _c33_ = (raster_space)==(1);
  auto _c34_ = (_c32_)&(_c33_);
  {
   Float device_scale = ((task).device_pixel_scale)/(((transform).m[3]).sel(W));
   mat2 glyph_transform = (make_mat2((transform).m))*(device_scale);
   vec2 device_text_pos = ((glyph_transform)*(text_offset))+((((transform).m[3]).sel(X, Y))*(device_scale));
   snap_offset = if_then_else(_c34_,(floor((device_text_pos)+(0.5)))-(device_text_pos),snap_offset);
   vec2 device_glyph_offset = (glyph_transform)*(glyph_offset);
   snap_offset = if_then_else(_c34_,snap_offset+(floor((device_glyph_offset)+(snap_bias)))-(device_glyph_offset),snap_offset);
   glyph_transform_inv = if_then_else(_c34_,inverse(glyph_transform),glyph_transform_inv);
   snap_offset = if_then_else(_c34_,(glyph_transform_inv)*(snap_offset),snap_offset);
  }
  _c33_ = (_c32_)&(~(_c33_));
  if (true) {
   {
    snap_offset = if_then_else(_c33_,(floor((text_offset)+(0.5)))-(text_offset),snap_offset);
    snap_offset = if_then_else(_c33_,snap_offset+(floor((glyph_offset)+(snap_bias)))-(glyph_offset),snap_offset);
   }
  }
 }
 (glyph_rect).p0 += snap_offset;
 vec2 local_pos = ((glyph_rect).p0)+(((glyph_rect).size)*((aPosition).sel(X, Y)));
 local_pos = clamp_rect(local_pos, local_clip_rect);
 vec4 world_pos = ((transform).m)*(make_vec4(local_pos, 0., 1.));
 vec2 device_pos = ((world_pos).sel(X, Y))*((task).device_pixel_scale);
 vec2 final_offset = (-((task).content_origin))+((((task).common_data).task_rect).p0);
 gl_Position = (uTransform)*(make_vec4((device_pos)+((final_offset)*((world_pos).sel(W))), (z)*((world_pos).sel(W)), (world_pos).sel(W)));
 VertexInfo vi = VertexInfo(local_pos, snap_offset, world_pos);
 return vi;
}
void write_clip(vec4 world_pos, vec2 snap_offset, ClipArea area) {
 vec2 uv = (((world_pos).sel(X, Y))*((area).device_pixel_scale))+(((world_pos).sel(W))*((snap_offset)+(((((area).common_data).task_rect).p0)-((area).screen_origin))));
 vClipMaskUvBounds = force_scalar(make_vec4((((area).common_data).task_rect).p0, ((((area).common_data).task_rect).p0)+((((area).common_data).task_rect).size)));
 vClipMaskUv = make_vec4(uv, ((area).common_data).texture_layer_index, (world_pos).sel(W));
}
ALWAYS_INLINE void main(void) {
 I32 prim_header_address = (aData).sel(X);
 I32 glyph_index = ((aData).sel(Y))&(65535);
 I32 render_task_index = ((aData).sel(Y))>>(16);
 I32 resource_address = (aData).sel(Z);
 I32 raster_space = ((aData).sel(W))>>(16);
 I32 subpx_dir = (((aData).sel(W))>>(8))&(255);
 I32 color_mode = ((aData).sel(W))&(255);
 PrimitiveHeader ph = fetch_prim_header(prim_header_address);
 Transform transform = fetch_transform((ph).transform_id);
 ClipArea clip_area = fetch_clip_area(((ph).user_data).sel(W));
 PictureTask task = fetch_picture_task(render_task_index);
 TextRun text = fetch_text_run((ph).specific_prim_address);
 vec2 text_offset = (make_vec2(((ph).user_data).sel(X, Y)))/(256.);
 auto _c14_ = (color_mode)==(0);
 {
  color_mode = if_then_else(_c14_,uMode,color_mode);
 }
 Glyph glyph = fetch_glyph((ph).specific_prim_address, glyph_index);
 (glyph).offset += (((ph).local_rect).p0)-(text_offset);
 GlyphResource res = fetch_glyph_resource(resource_address);
 Float raster_scale = (make_float(((ph).user_data).sel(Z)))/(65535.);
 Float scale = ((res).scale)/((raster_scale)*((task).device_pixel_scale));
 RectWithSize glyph_rect = RectWithSize(((scale)*((res).offset))+((text_offset)+((glyph).offset)), (scale)*((((res).uv_rect).sel(Z, W))-(((res).uv_rect).sel(X, Y))));
 vec2 snap_bias;
 auto _c15_ = (subpx_dir)==(1);
 {
  snap_bias = if_then_else(_c15_,make_vec2(0.125, 0.5),snap_bias);
 }
 auto _c16_ = (subpx_dir)==(2);
 auto _c17_ = (~(_c15_))&(_c16_);
 {
  snap_bias = if_then_else(_c17_,make_vec2(0.5, 0.125),snap_bias);
 }
 _c16_ = (~(_c15_))&(~(_c16_));
 auto _c18_ = (subpx_dir)==(3);
 auto _c19_ = (_c16_)&(_c18_);
 {
  snap_bias = if_then_else(_c19_,make_vec2(0.125),snap_bias);
 }
 _c18_ = (_c16_)&(~(_c18_));
 {
  snap_bias = if_then_else(_c18_,make_vec2(0.5),snap_bias);
 }
 VertexInfo vi = write_text_vertex((ph).local_clip_rect, (ph).z, raster_space, transform, task, text_offset, (glyph).offset, glyph_rect, snap_bias);
 (glyph_rect).p0 += (vi).snap_offset;
 vec2 f = (((vi).local_pos)-((glyph_rect).p0))/((glyph_rect).size);
 write_clip((vi).world_pos, (vi).snap_offset, clip_area);
 auto _c20_ = ((color_mode)==(1))||((color_mode)==(7));
 {
  if (_c20_[0]) { vMaskSwizzle = make_vec2(0., 1.); };
  if (_c20_[0]) { vColor = force_scalar((text).color); };
 }
 auto _c21_ = ((color_mode)==(5))||((color_mode)==(6));
 auto _c22_ = (~(_c20_))&(_c21_);
 {
  if (_c22_[0]) { vMaskSwizzle = make_vec2(1., 0.); };
  if (_c22_[0]) { vColor = force_scalar((text).color); };
 }
 _c21_ = (~(_c20_))&(~(_c21_));
 auto _c23_ = (((color_mode)==(2))||((color_mode)==(3)))||((color_mode)==(8));
 auto _c24_ = (_c21_)&(_c23_);
 {
  if (_c24_[0]) { vMaskSwizzle = make_vec2(1., 0.); };
  if (_c24_[0]) { vColor = force_scalar(make_vec4(((text).color).sel(A))); };
 }
 _c23_ = (_c21_)&(~(_c23_));
 auto _c25_ = (color_mode)==(4);
 auto _c26_ = (_c23_)&(_c25_);
 {
  if (_c26_[0]) { vMaskSwizzle = make_vec2(-(1.), 1.); };
  if (_c26_[0]) { vColor = force_scalar((make_vec4(((text).color).sel(A)))*((text).bg_color)); };
 }
 _c25_ = (_c23_)&(~(_c25_));
 if (true) {
  {
   if (_c25_[0]) { vMaskSwizzle = make_vec2(0.); };
   if (_c25_[0]) { vColor = make_vec4(1.); };
  }
 }
 vec2_scalar texture_size = make_vec2(textureSize(sColor0, 0));
 vec2 st0 = (((res).uv_rect).sel(X, Y))/(texture_size);
 vec2 st1 = (((res).uv_rect).sel(Z, W))/(texture_size);
 vUv = make_vec3(mix(st0, st1, f), (res).layer);
 vUvBorder = force_scalar((((res).uv_rect)+(make_vec4(0.5, 0.5, -(0.5), -(0.5))))/((texture_size).sel(X, Y, X, Y)));
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
*/
struct ps_text_run_frag : FragmentShaderImpl {
typedef ps_text_run_frag Self;
typedef ps_text_run_samplers Samplers;
static void set_uniform_1i(Self *self, Samplers *samplers, int index, int value) {
 switch (index) {
 case 8:
  samplers->sColor0_slot = value;
  break;
 case 2:
  samplers->sGpuCache_slot = value;
  break;
 case 9:
  samplers->sPrevPassAlpha_slot = value;
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
typedef ps_text_run_vert::FlatOutputs FlatInputs;
typedef ps_text_run_vert::InterpOutputs InterpInputs;
static void read_flat_inputs(Self *self, const FlatInputs *src) {
  self->vTransformBounds = src->vTransformBounds;
  self->vClipMaskUvBounds = src->vClipMaskUvBounds;
  self->vColor = src->vColor;
  self->vUvBorder = src->vUvBorder;
  self->vMaskSwizzle = src->vMaskSwizzle;
}
static void read_interp_inputs(Self *self, const InterpInputs *init, const InterpInputs *step) {
  self->vClipMaskUv = init_interp(init->vClipMaskUv, step->vClipMaskUv);
  self->vUv = init_interp(init->vUv, step->vUv);
}
ALWAYS_INLINE void step_interp_inputs(const InterpInputs* step) {
  vClipMaskUv += step->vClipMaskUv;
  vUv += step->vUv;
}
static void bind_textures(Self *self, Samplers *samplers) {
 self->sColor0 = lookup_sampler_array(&samplers->sColor0_impl, samplers->sColor0_slot);
 self->sGpuCache = lookup_sampler(&samplers->sGpuCache_impl, samplers->sGpuCache_slot);
 self->sPrevPassAlpha = lookup_sampler_array(&samplers->sPrevPassAlpha_impl, samplers->sPrevPassAlpha_slot);
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
void write_output(vec4 color) {
 oFragColor = color;
}
ALWAYS_INLINE void main(void) {
 vec3 tc = make_vec3(clamp((vUv).sel(X, Y), (vUvBorder).sel(X, Y), (vUvBorder).sel(Z, W)), (vUv).sel(Z));
 vec4 mask = texture(sColor0, tc);
 (mask).lsel(R, G, B) = (((mask).sel(R, G, B))*((vMaskSwizzle).sel(X)))+(((mask).sel(A, A, A))*((vMaskSwizzle).sel(Y)));
 Float alpha = do_clip();
 write_output((vColor)*((mask)*(alpha)));
}
static bool use_discard(Self*) { return false; }
static void run(Self *self, const InterpInputs* step) {
 self->main();
 self->step_interp_inputs(step);
}
static void skip(Self *self, const InterpInputs* step) {
 self->step_interp_inputs(step);
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
struct ps_text_run_program : ProgramImpl {
const char *get_name() const override { return "ps_text_run"; }
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
ps_text_run_samplers samplers;
virtual void *get_samplers() override { return &samplers; }
ps_text_run_vert::AttribLocations attrib_locations;
void bind_attrib(const char *name, int index) override {
 ps_text_run_vert::bind_attrib_location(&attrib_locations, name, index);
}
const void* get_attrib_locations() const override { return &attrib_locations; }
void init_shaders(void *vertex_shader, void *fragment_shader) override {
 reinterpret_cast<ps_text_run_vert*>(vertex_shader)->init_shader();
 reinterpret_cast<ps_text_run_frag*>(fragment_shader)->init_shader();
}
};

