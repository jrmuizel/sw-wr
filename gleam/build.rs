extern crate cc;
extern crate glsl_to_cxx;

fn main() {

    for s in &["ps_text_run", "brush_image", "brush_solid", "ps_text_runDUAL_SOURCE_BLENDING"] {
        let v = "shaders/".to_string() + s + ".vert.pp";
        let f = "shaders/".to_string() + s + ".frag.pp";
        println!("cargo:rerun-if-changed={}", v);
        println!("cargo:rerun-if-changed={}", f);
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
