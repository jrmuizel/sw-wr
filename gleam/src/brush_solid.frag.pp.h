/* inputs
vec4 vTransformBounds
vec4 vClipMaskUvBounds
vec4 vClipMaskUv
vec4 vColor
vec2 vLocalPos
*/
/* outputs
vec4 oFragColor
*/
struct brush_solid {
static int get_uniform_location(char *name) {
if (strcmp("sColor0", name) == 0) { return 1; }
if (strcmp("sColor1", name) == 0) { return 2; }
if (strcmp("sColor2", name) == 0) { return 3; }
if (strcmp("sGpuCache", name) == 0) { return 4; }
if (strcmp("sPrevPassAlpha", name) == 0) { return 5; }
if (strcmp("sPrevPassColor", name) == 0) { return 6; }
return -1;
}
void set_uniform_int(int index, int value) {
if (index == 1) {
sColor0 = lookup_sampler_array(value);
}
if (index == 2) {
sColor1 = lookup_sampler_array(value);
}
if (index == 3) {
sColor2 = lookup_sampler_array(value);
}
if (index == 4) {
sGpuCache = lookup_sampler(value);
}
if (index == 5) {
sPrevPassAlpha = lookup_sampler_array(value);
}
if (index == 6) {
sPrevPassColor = lookup_sampler_array(value);
}
}
void set_uniform_4f(int index, float *value) {
if (index == 1) {
assert(0); // sColor0
}
if (index == 2) {
assert(0); // sColor1
}
if (index == 3) {
assert(0); // sColor2
}
if (index == 4) {
assert(0); // sGpuCache
}
if (index == 5) {
assert(0); // sPrevPassAlpha
}
if (index == 6) {
assert(0); // sPrevPassColor
}
}
void set_uniform_matrix4fv(int index, const float *value) {
if (index == 1) {
assert(0); // sColor0
}
if (index == 2) {
assert(0); // sColor1
}
if (index == 3) {
assert(0); // sColor2
}
if (index == 4) {
assert(0); // sGpuCache
}
if (index == 5) {
assert(0); // sPrevPassAlpha
}
if (index == 6) {
assert(0); // sPrevPassColor
}
}
void read_inputs(char *src) {
  {
    vec4_scalar scalar;
    memcpy(&scalar, src, sizeof(get_nth(vTransformBounds, 0)));
    vTransformBounds = vec4_scalar(scalar);
    src += sizeof(get_nth(vTransformBounds, 0));
  }
  {
    vec4_scalar scalar;
    memcpy(&scalar, src, sizeof(get_nth(vClipMaskUvBounds, 0)));
    vClipMaskUvBounds = vec4_scalar(scalar);
    src += sizeof(get_nth(vClipMaskUvBounds, 0));
  }
  {
    vec4_scalar scalar;
    memcpy(&scalar, src, sizeof(get_nth(vClipMaskUv, 0)));
    vClipMaskUv = vec4(scalar);
    src += sizeof(get_nth(vClipMaskUv, 0));
  }
  {
    vec4_scalar scalar;
    memcpy(&scalar, src, sizeof(get_nth(vColor, 0)));
    vColor = vec4_scalar(scalar);
    src += sizeof(get_nth(vColor, 0));
  }
  {
    vec2_scalar scalar;
    memcpy(&scalar, src, sizeof(get_nth(vLocalPos, 0)));
    vLocalPos = vec2(scalar);
    src += sizeof(get_nth(vLocalPos, 0));
  }
}
Bool isPixelDiscarded = false;
vec4 oFragColor;
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
vec4_scalar vColor;
vec2 vLocalPos;
Float signed_distance_rect(vec2 pos, vec2_scalar p0, vec2_scalar p1) {
 vec2 d = max((p0)-(pos), (pos)-(p1));
 return (length(max(make_vec2(0.), d)))+(min(0., max((d).sel(X), (d).sel(Y))));
}
Float compute_aa_range(vec2 position) {
 return (0.35355)*(length(fwidth(position)));
}
Float distance_aa(Float aa_range, Float signed_distance) {
 I32 ret_mask = ~0;
 Float ret;
 Float dist = (0.5)*((signed_distance)/(aa_range));
 auto _c3_ = (dist)<=((-(0.5))+(0.0001));
 ret = if_then_else(ret_mask & (_c3_), 1., ret);
 ret_mask &= ~(_c3_);
 auto _c4_ = (dist)>=((0.5)-(0.0001));
 ret = if_then_else(ret_mask & (_c4_), 0., ret);
 ret_mask &= ~(_c4_);
 ret = if_then_else(ret_mask, (0.5)+((dist)*(((0.8431027)*((dist)*(dist)))-(1.14453603))), ret);
 return ret;
}
Float init_transform_fs(vec2 local_pos) {
 Float d = signed_distance_rect(local_pos, (vTransformBounds).sel(X, Y), (vTransformBounds).sel(Z, W));
 Float aa_range = compute_aa_range(local_pos);
 return distance_aa(aa_range, d);
}
Fragment brush_fs() {
 vec4 color = vColor;
 color *= init_transform_fs(vLocalPos);
 return Fragment(color);
}
Float do_clip() {
 I32 ret_mask = ~0;
 Float ret;
 auto _c6_ = ((vClipMaskUvBounds).sel(X, Y))==((vClipMaskUvBounds).sel(Z, W));
 {
  ret = if_then_else(ret_mask & (_c6_), 1., ret);
  ret_mask &= ~(_c6_);
 }
 vec2 mask_uv = ((vClipMaskUv).sel(X, Y))*((gl_FragCoord).sel(W));
 bvec2 left = lessThanEqual((vClipMaskUvBounds).sel(X, Y), mask_uv);
 bvec2 right = greaterThan((vClipMaskUvBounds).sel(Z, W), mask_uv);
 auto _c7_ = !(all(make_bvec4(left, right)));
 {
  ret = if_then_else(ret_mask & (_c7_), 0., ret);
  ret_mask &= ~(_c7_);
 }
 ivec3 tc = make_ivec3(mask_uv, ((vClipMaskUv).sel(Z))+(0.5));
 ret = if_then_else(ret_mask, (texelFetch(sPrevPassAlpha, tc, 0)).sel(R), ret);
 return ret;
}
void write_output(vec4 color) {
 oFragColor = color;
}
void main(void) {
 Fragment frag = brush_fs();
 Float clip_alpha = do_clip();
 (frag).color *= clip_alpha;
 write_output((frag).color);
}
};
