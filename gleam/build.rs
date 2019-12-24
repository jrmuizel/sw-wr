extern crate cc;
extern crate glsl_to_cxx;

fn main() {

    let shaders = &[
        "ps_text_run",
        "brush_image",
        "brush_solid",
        "cs_blurALPHA_TARGET",
        "cs_clip_rectangle",
        "ps_text_runDUAL_SOURCE_BLENDING",
    ];
    for s in shaders {
        let v = "shaders/".to_string() + s + ".vert.pp";
        let f = "shaders/".to_string() + s + ".frag.pp";
        let n = "gl_to_cxx".to_string();
        let mut args = vec![n, v, f].into_iter();
        let result = glsl_to_cxx::translate(&mut args);
        std::fs::write(std::env::var("OUT_DIR").unwrap() + "/" + s + ".h", result).unwrap();
    }
    cc::Build::new()
        .cpp(true)
        .file("src/gl.cc")
        .define("FOO", Some("bar"))
        .flag("-std=c++14")
        .include("src")
        .include(std::env::var("OUT_DIR").unwrap())
        .compile("gl_cc");
}
