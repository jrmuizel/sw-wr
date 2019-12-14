/* inputs
vec3 aPosition
vec2 aTaskOrigin
vec4 aRect
vec4 aColor0
vec4 aColor1
I32 aFlags
vec2 aWidths
vec2 aRadii
vec4 aHorizontallyAdjacentCorner
vec4 aVerticallyAdjacentCorner
*/
/* outputs
vec4 vColor0
vec4 vColor1
vec4 vColorLine
I32 vMixColors
vec4 vClipCenter_Sign
vec4 vClipRadii
vec4 vHorizontalClipCenter_Sign
vec2 vHorizontalClipRadii
vec4 vVerticalClipCenter_Sign
vec2 vVerticalClipRadii
vec2 vPos
*/
struct cs_border_solid_program : ProgramImpl {
const char *get_name() const override { return "cs_border_solid"; }
int get_uniform(const char *name) const override {
 if (strcmp("uTransform", name) == 0) { return 1; }
 return -1;
}
struct Samplers {
} samplers;
bool set_sampler(int index, int value) override {
 switch (index) {
 }
 return false;
}
struct AttribLocations {
 int aPosition;
 int aTaskOrigin;
 int aRect;
 int aColor0;
 int aColor1;
 int aFlags;
 int aWidths;
 int aRadii;
 int aHorizontallyAdjacentCorner;
 int aVerticallyAdjacentCorner;
} attrib_locations;
void bind_attrib(const char *name, int index) override {
 if (strcmp("aPosition", name) == 0) { attrib_locations.aPosition = index; return; }
 if (strcmp("aTaskOrigin", name) == 0) { attrib_locations.aTaskOrigin = index; return; }
 if (strcmp("aRect", name) == 0) { attrib_locations.aRect = index; return; }
 if (strcmp("aColor0", name) == 0) { attrib_locations.aColor0 = index; return; }
 if (strcmp("aColor1", name) == 0) { attrib_locations.aColor1 = index; return; }
 if (strcmp("aFlags", name) == 0) { attrib_locations.aFlags = index; return; }
 if (strcmp("aWidths", name) == 0) { attrib_locations.aWidths = index; return; }
 if (strcmp("aRadii", name) == 0) { attrib_locations.aRadii = index; return; }
 if (strcmp("aHorizontallyAdjacentCorner", name) == 0) { attrib_locations.aHorizontallyAdjacentCorner = index; return; }
 if (strcmp("aVerticallyAdjacentCorner", name) == 0) { attrib_locations.aVerticallyAdjacentCorner = index; return; }
}
void init_shaders(void *vertex_shader, void *fragment_shader) override;
};
struct cs_border_solid_vert : VertexShaderImpl {
typedef cs_border_solid_vert Self;
static void set_uniform_1i(Self *self, int index, int value) {
 switch (index) {
 case 1:
  assert(0); // uTransform
  break;
 }
}
static void set_uniform_4fv(Self *self, int index, const float *value) {
 switch (index) {
 case 1:
  assert(0); // uTransform
  break;
 }
}
static void set_uniform_matrix4fv(Self *self, int index, const float *value) {
 switch (index) {
 case 1:
  self->uTransform = mat4_scalar::load_from_ptr(value);
  break;
 }
}
static void load_attribs(Self *self, cs_border_solid_program *prog, VertexAttrib *attribs, unsigned short *indices, int start, int instance, int count) {
 load_attrib(self->aPosition, attribs[prog->attrib_locations.aPosition], indices, start, instance, count);
 load_attrib(self->aTaskOrigin, attribs[prog->attrib_locations.aTaskOrigin], indices, start, instance, count);
 load_attrib(self->aRect, attribs[prog->attrib_locations.aRect], indices, start, instance, count);
 load_attrib(self->aColor0, attribs[prog->attrib_locations.aColor0], indices, start, instance, count);
 load_attrib(self->aColor1, attribs[prog->attrib_locations.aColor1], indices, start, instance, count);
 load_attrib(self->aFlags, attribs[prog->attrib_locations.aFlags], indices, start, instance, count);
 load_attrib(self->aWidths, attribs[prog->attrib_locations.aWidths], indices, start, instance, count);
 load_attrib(self->aRadii, attribs[prog->attrib_locations.aRadii], indices, start, instance, count);
 load_attrib(self->aHorizontallyAdjacentCorner, attribs[prog->attrib_locations.aHorizontallyAdjacentCorner], indices, start, instance, count);
 load_attrib(self->aVerticallyAdjacentCorner, attribs[prog->attrib_locations.aVerticallyAdjacentCorner], indices, start, instance, count);
}
struct FlatOutputs {
vec4_scalar vColor0;
vec4_scalar vColor1;
vec4_scalar vColorLine;
int32_t vMixColors;
vec4_scalar vClipCenter_Sign;
vec4_scalar vClipRadii;
vec4_scalar vHorizontalClipCenter_Sign;
vec2_scalar vHorizontalClipRadii;
vec4_scalar vVerticalClipCenter_Sign;
vec2_scalar vVerticalClipRadii;
};
struct InterpOutputs {
vec2_scalar vPos;
};
ALWAYS_INLINE void store_flat_outputs(char* dest_ptr) {
  auto* dest = reinterpret_cast<FlatOutputs*>(dest_ptr);
  dest->vColor0 = vColor0;
  dest->vColor1 = vColor1;
  dest->vColorLine = vColorLine;
  dest->vMixColors = vMixColors;
  dest->vClipCenter_Sign = vClipCenter_Sign;
  dest->vClipRadii = vClipRadii;
  dest->vHorizontalClipCenter_Sign = vHorizontalClipCenter_Sign;
  dest->vHorizontalClipRadii = vHorizontalClipRadii;
  dest->vVerticalClipCenter_Sign = vVerticalClipCenter_Sign;
  dest->vVerticalClipRadii = vVerticalClipRadii;
}
ALWAYS_INLINE void store_interp_outputs(char* dest_ptr, size_t stride) {
  for(int n = 0; n < 4; n++) {
    auto* dest = reinterpret_cast<InterpOutputs*>(dest_ptr);
    dest->vPos = get_nth(vPos, n);
    dest_ptr += stride;
  }
}
static void bind_textures(Self *self, cs_border_solid_program *prog) {
}
int32_t uMode;
mat4_scalar uTransform;
vec3 aPosition;
sampler2DArray sColor0;
sampler2DArray sColor1;
sampler2DArray sColor2;
vec4_scalar vColor0;
vec4_scalar vColor1;
vec4_scalar vColorLine;
int32_t vMixColors;
vec4_scalar vClipCenter_Sign;
vec4_scalar vClipRadii;
vec4_scalar vHorizontalClipCenter_Sign;
vec2_scalar vHorizontalClipRadii;
vec4_scalar vVerticalClipCenter_Sign;
vec2_scalar vVerticalClipRadii;
vec2 vPos;
vec2 aTaskOrigin;
vec4 aRect;
vec4 aColor0;
vec4 aColor1;
I32 aFlags;
vec2 aWidths;
vec2 aRadii;
vec4 aHorizontallyAdjacentCorner;
vec4 aVerticallyAdjacentCorner;
vec2 get_outer_corner_scale(I32 segment) {
 vec2 p;
 auto _c10_ = (segment)==(0);
 {
  p = if_then_else(_c10_,make_vec2(0.f, 0.f),p);
 }
 auto _c11_ = (segment)==(1);
 auto _c12_ = (~(_c10_))&(_c11_);
 {
  p = if_then_else(_c12_,make_vec2(1.f, 0.f),p);
 }
 _c11_ = (~(_c10_))&(~(_c11_));
 auto _c13_ = (segment)==(2);
 auto _c14_ = (_c11_)&(_c13_);
 {
  p = if_then_else(_c14_,make_vec2(1.f, 1.f),p);
 }
 _c13_ = (_c11_)&(~(_c13_));
 auto _c15_ = (segment)==(3);
 auto _c16_ = (_c13_)&(_c15_);
 {
  p = if_then_else(_c16_,make_vec2(0.f, 1.f),p);
 }
 _c15_ = (_c13_)&(~(_c15_));
 if (true) {
  {
   p = if_then_else(_c15_,make_vec2(0.f),p);
  }
 }
 return p;
}
ALWAYS_INLINE void main(void) {
 I32 segment = (aFlags)&(255);
 Bool do_aa = (((aFlags)>>(24))&(240))!=(0);
 vec2 outer_scale = get_outer_corner_scale(segment);
 vec2 outer = (outer_scale)*((aRect).sel(Z, W));
 vec2 clip_sign = (1.f)-((2.f)*(outer_scale));
 I32 mix_colors;
 auto _c2_ = ((((segment)==(0))||((segment)==(1)))||((segment)==(2)))||((segment)==(3));
 {
  mix_colors = if_then_else(_c2_,if_then_else(do_aa, 1, 2),mix_colors);
 }
 if (true) {
  {
   mix_colors = if_then_else(~(_c2_),0,mix_colors);
  }
 }
 vMixColors = force_scalar(mix_colors);
 vPos = ((aRect).sel(Z, W))*((aPosition).sel(X, Y));
 vColor0 = force_scalar(aColor0);
 vColor1 = force_scalar(aColor1);
 vClipCenter_Sign = force_scalar(make_vec4((outer)+((clip_sign)*(aRadii)), clip_sign));
 vClipRadii = force_scalar(make_vec4(aRadii, max((aRadii)-(aWidths), 0.f)));
 vColorLine = force_scalar(make_vec4(outer, ((aWidths).sel(Y))*(-((clip_sign).sel(Y))), ((aWidths).sel(X))*((clip_sign).sel(X))));
 vec2 horizontal_clip_sign = make_vec2(-((clip_sign).sel(X)), (clip_sign).sel(Y));
 vHorizontalClipCenter_Sign = force_scalar(make_vec4(((aHorizontallyAdjacentCorner).sel(X, Y))+((horizontal_clip_sign)*((aHorizontallyAdjacentCorner).sel(Z, W))), horizontal_clip_sign));
 vHorizontalClipRadii = force_scalar((aHorizontallyAdjacentCorner).sel(Z, W));
 vec2 vertical_clip_sign = make_vec2((clip_sign).sel(X), -((clip_sign).sel(Y)));
 vVerticalClipCenter_Sign = force_scalar(make_vec4(((aVerticallyAdjacentCorner).sel(X, Y))+((vertical_clip_sign)*((aVerticallyAdjacentCorner).sel(Z, W))), vertical_clip_sign));
 vVerticalClipRadii = force_scalar((aVerticallyAdjacentCorner).sel(Z, W));
 gl_Position = (uTransform)*(make_vec4((aTaskOrigin)+(((aRect).sel(X, Y))+(vPos)), 0.f, 1.f));
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
vec4 vColor0
vec4 vColor1
vec4 vColorLine
I32 vMixColors
vec4 vClipCenter_Sign
vec4 vClipRadii
vec4 vHorizontalClipCenter_Sign
vec2 vHorizontalClipRadii
vec4 vVerticalClipCenter_Sign
vec2 vVerticalClipRadii
vec2 vPos
*/
/* outputs
vec4 oFragColor
*/
struct cs_border_solid_frag : FragmentShaderImpl {
typedef cs_border_solid_frag Self;
static void set_uniform_1i(Self *self, int index, int value) {
 switch (index) {
 }
}
static void set_uniform_4fv(Self *self, int index, const float *value) {
 switch (index) {
 }
}
static void set_uniform_matrix4fv(Self *self, int index, const float *value) {
 switch (index) {
 }
}
typedef cs_border_solid_vert::FlatOutputs FlatInputs;
typedef cs_border_solid_vert::InterpOutputs InterpInputs;
static void read_flat_inputs(Self *self, const FlatInputs *src) {
  self->vColor0 = src->vColor0;
  self->vColor1 = src->vColor1;
  self->vColorLine = src->vColorLine;
  self->vMixColors = src->vMixColors;
  self->vClipCenter_Sign = src->vClipCenter_Sign;
  self->vClipRadii = src->vClipRadii;
  self->vHorizontalClipCenter_Sign = src->vHorizontalClipCenter_Sign;
  self->vHorizontalClipRadii = src->vHorizontalClipRadii;
  self->vVerticalClipCenter_Sign = src->vVerticalClipCenter_Sign;
  self->vVerticalClipRadii = src->vVerticalClipRadii;
}
InterpInputs interp_step;
static void read_interp_inputs(Self *self, const InterpInputs *init, const InterpInputs *step, float step_width) {
  self->vPos = init_interp(init->vPos, step->vPos);
  self->interp_step.vPos = step->vPos * step_width;
}
ALWAYS_INLINE void step_interp_inputs() {
  vPos += interp_step.vPos;
}
static void bind_textures(Self *self, cs_border_solid_program *prog) {
}
#define oFragColor gl_FragColor
// vec4 oFragColor;
sampler2DArray sColor0;
sampler2DArray sColor1;
sampler2DArray sColor2;
vec4_scalar vColor0;
vec4_scalar vColor1;
vec4_scalar vColorLine;
int32_t vMixColors;
vec4_scalar vClipCenter_Sign;
vec4_scalar vClipRadii;
vec4_scalar vHorizontalClipCenter_Sign;
vec2_scalar vHorizontalClipRadii;
vec4_scalar vVerticalClipCenter_Sign;
vec2_scalar vVerticalClipRadii;
vec2 vPos;
Float compute_aa_range(vec2 position) {
 return (0.35355f)*(length(fwidth(position)));
}
Float distance_to_line(vec2_scalar p0, vec2_scalar perp_dir, vec2 p) {
 vec2 dir_to_p0 = (p0)-(p);
 return dot(normalize(perp_dir), dir_to_p0);
}
Float distance_aa(Float aa_range, Float signed_distance) {
 I32 ret_mask = ~0;
 Float ret;
 Float dist = (0.5f)*((signed_distance)/(aa_range));
 auto _c9_ = (dist)<=((-(0.5f))+(0.0001f));
 ret = 1.f;
 ret_mask = ~I32(_c9_);
 auto _c10_ = (dist)>=((0.5f)-(0.0001f));
 ret = if_then_else(ret_mask & I32(_c10_), 0.f, ret);
 ret_mask &= ~I32(_c10_);
 ret = if_then_else(ret_mask, (0.5f)+((dist)*(((0.8431027f)*((dist)*(dist)))-(1.14453603f))), ret);
 return ret;
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
ALWAYS_INLINE void main(void) {
 Float aa_range = compute_aa_range(vPos);
 bool do_aa = (vMixColors)!=(2);
 Float mix_factor = 0.f;
 if ((vMixColors)!=(0)) {
  {
   Float d_line = distance_to_line((vColorLine).sel(X, Y), (vColorLine).sel(Z, W), vPos);
   if (do_aa) {
    {
     mix_factor = distance_aa(aa_range, -(d_line));
    }
   } else    {
    mix_factor = if_then_else(((d_line)+(0.0001f))>=(0.f), 1.f, 0.f);
   }
  }
 }
 vec2 clip_relative_pos = (vPos)-((vClipCenter_Sign).sel(X, Y));
 Bool in_clip_region = all(lessThan(((vClipCenter_Sign).sel(Z, W))*(clip_relative_pos), make_vec2(0.f)));
 Float d = -(1.f);
 auto _c4_ = in_clip_region;
 {
  Float d_radii_a = distance_to_ellipse(clip_relative_pos, (vClipRadii).sel(X, Y), aa_range);
  Float d_radii_b = distance_to_ellipse(clip_relative_pos, (vClipRadii).sel(Z, W), aa_range);
  d = if_then_else(_c4_,max(d_radii_a, -(d_radii_b)),d);
 }
 clip_relative_pos = (vPos)-((vHorizontalClipCenter_Sign).sel(X, Y));
 in_clip_region = all(lessThan(((vHorizontalClipCenter_Sign).sel(Z, W))*(clip_relative_pos), make_vec2(0.f)));
 auto _c5_ = in_clip_region;
 {
  Float d_radii = distance_to_ellipse(clip_relative_pos, (vHorizontalClipRadii).sel(X, Y), aa_range);
  d = if_then_else(_c5_,max(d_radii, d),d);
 }
 clip_relative_pos = (vPos)-((vVerticalClipCenter_Sign).sel(X, Y));
 in_clip_region = all(lessThan(((vVerticalClipCenter_Sign).sel(Z, W))*(clip_relative_pos), make_vec2(0.f)));
 auto _c6_ = in_clip_region;
 {
  Float d_radii = distance_to_ellipse(clip_relative_pos, (vVerticalClipRadii).sel(X, Y), aa_range);
  d = if_then_else(_c6_,max(d_radii, d),d);
 }
 Float alpha = do_aa ? distance_aa(aa_range, d) : 1.f;
 vec4 color = mix(vColor0, vColor1, mix_factor);
 oFragColor = (color)*(alpha);
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
void cs_border_solid_program::init_shaders(void *vertex_shader, void *fragment_shader) {
 reinterpret_cast<cs_border_solid_vert*>(vertex_shader)->init_shader();
 reinterpret_cast<cs_border_solid_frag*>(fragment_shader)->init_shader();
}

