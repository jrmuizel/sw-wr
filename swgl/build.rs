extern crate cc;
extern crate glsl_to_cxx;

use std::fmt::Write;

fn write_load_shader(shaders: &[&str]) {
    let mut load_shader = String::new();
    for s in shaders {
        write!(load_shader, "#include \"{}.h\"\n", s);
    }
    write!(load_shader, "ProgramImpl* load_shader(const std::string &name) {{\n");
    for s in shaders {
        write!(load_shader, "  if (name == \"{}\") {{ return new {}_program; }}\n", s, s.replace(".", "_"));
    }
    write!(load_shader, "  return nullptr;\n}}\n");
    std::fs::write(std::env::var("OUT_DIR").unwrap() + "/load_shader.h", load_shader).unwrap();
}

fn process_imports(shader_dir: &str, shader: &str, output: &mut String) {
    let source = std::fs::read_to_string(format!("{}/{}.glsl", shader_dir, shader)).unwrap();
    for line in source.lines() {
        if line.starts_with("#include ") {
            let imports = line["#include ".len() ..].split(',');
            for import in imports {
                process_imports(shader_dir, import, output);
            }
        } else if line.starts_with("#version ") || line.starts_with("#extension ") {
            // ignore
        } else {
            output.push_str(line);
            output.push('\n');
        }
    }
}

fn translate_shader(shader: &str) {
    let shader_dir = match std::env::var("MOZ_SRC") {
        Ok(dir) => dir + "/gfx/wr/webrender/res",
        Err(_) => std::env::var("CARGO_MANIFEST_DIR").unwrap() + "/../webrender/res",
    };

    let mut imported = String::new();
    write!(imported, "#define GL_ES 1\n");
    write!(imported, "#define WR_MAX_VERTEX_TEXTURE_WIDTH 1024U\n");
    let basename = if let Some(feature_start) = shader.find(char::is_uppercase) {
        let feature_end = shader.rfind(char::is_uppercase).unwrap();
        let features = shader[feature_start..feature_end+1].split('.');
        for feature in features {
            write!(imported, "#define WR_FEATURE_{}\n", feature);
        }
        &shader[0..feature_start]
    } else {
        shader
    };

    process_imports(&shader_dir, basename, &mut imported);

    let out_dir = std::env::var("OUT_DIR").unwrap();
    let imp_name = format!("{}/{}.i", out_dir, shader);
    std::fs::write(&imp_name, imported).unwrap();

    let vs = cc::Build::new()
        .flag("-xc")
        .flag("-P")
        .file(&imp_name)
        .define("WR_VERTEX_SHADER", Some(""))
        .expand();
    let fs = cc::Build::new()
        .flag("-xc")
        .flag("-P")
        .file(&imp_name)
        .define("WR_FRAGMENT_SHADER", Some(""))
        .expand();
    let vs_name = format!("{}/{}.vert", out_dir, shader);
    let fs_name = format!("{}/{}.frag", out_dir, shader);
    std::fs::write(&vs_name, vs).unwrap();
    std::fs::write(&fs_name, fs).unwrap();

    let mut args = vec![
        "glsl_to_cxx".to_string(),
        vs_name,
        fs_name,
    ].into_iter();
    let result = glsl_to_cxx::translate(&mut args);
    std::fs::write(format!("{}/{}.h", out_dir, shader), result).unwrap();
}

const WR_SHADERS: &'static [&'static str] = &[
    "brush_blendALPHA_PASS",
    "brush_blend",
    "brush_imageALPHA_PASS",
    "brush_image",
    "brush_imageREPETITION.ANTIALIASING.ALPHA_PASS",
    "brush_imageREPETITION.ANTIALIASING",
    "brush_linear_gradientALPHA_PASS",
    "brush_linear_gradientDITHERING.ALPHA_PASS",
    "brush_linear_gradientDITHERING",
    "brush_linear_gradient",
    "brush_mix_blendALPHA_PASS",
    "brush_mix_blend",
    "brush_opacityALPHA_PASS",
    "brush_radial_gradientALPHA_PASS",
    "brush_radial_gradientDITHERING.ALPHA_PASS",
    "brush_radial_gradientDITHERING",
    "brush_radial_gradient",
    "brush_solidALPHA_PASS",
    "brush_solid",
    "brush_yuv_image",
    "brush_yuv_imageTEXTURE_2D.YUV_NV12",
    "brush_yuv_imageYUV_INTERLEAVED",
    "brush_yuv_imageYUV_NV12.ALPHA_PASS",
    "brush_yuv_imageYUV_NV12",
    "brush_yuv_imageYUV_PLANAR",
    "cs_blurALPHA_TARGET",
    "cs_blurCOLOR_TARGET",
    "cs_border_segment",
    "cs_border_solid",
    "cs_clip_box_shadow",
    "cs_clip_image",
    "cs_clip_rectangleFAST_PATH",
    "cs_clip_rectangle",
    "cs_gradient",
    "cs_line_decoration",
    "cs_scale",
    "cs_svg_filter",
    "debug_color",
    "debug_font",
    "ps_split_composite",
    "ps_text_runDUAL_SOURCE_BLENDING",
    "ps_text_runGLYPH_TRANSFORM",
    "ps_text_run",
];

fn main() {
    for shader in WR_SHADERS {
        translate_shader(shader);
    }

    write_load_shader(WR_SHADERS);

    cc::Build::new()
        .cpp(true)
        .file("src/gl.cc")
        .flag("-UMOZILLA_CONFIG_H")
        .flag("-std=c++14")
        .include("src")
        .include(std::env::var("OUT_DIR").unwrap())
        .compile("gl_cc");
}
