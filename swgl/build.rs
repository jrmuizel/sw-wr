extern crate cc;
extern crate glsl_to_cxx;

use std::fmt::Write;

fn get_shaders() -> Vec<String> {
    let mut shaders = Vec::new();
    for entry in std::fs::read_dir("shaders").unwrap() {
        let path = entry.unwrap().path();
        let s = path.strip_prefix("shaders").unwrap().to_str().unwrap();
        if let Some(idx) = s.rfind(".vert.pp") {
            if idx + ".vert.pp".len() == s.len() {
                shaders.push(s[0 .. idx].to_owned());
            }
        }
    }
    shaders
}

fn translate_shaders(shaders: &[String]) {
    for s in shaders {
        let v = "shaders/".to_string() + s + ".vert.pp";
        let f = "shaders/".to_string() + s + ".frag.pp";
        let n = "gl_to_cxx".to_string();
        let mut args = vec![n, v, f].into_iter();
        let result = glsl_to_cxx::translate(&mut args);
        std::fs::write(std::env::var("OUT_DIR").unwrap() + "/" + s + ".h", result).unwrap();
    }
}

fn write_load_shader(shaders: &[String]) {
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

fn main() {
    let shaders = get_shaders();

    translate_shaders(&shaders);

    write_load_shader(&shaders);

    cc::Build::new()
        .cpp(true)
        .file("src/gl.cc")
        .define("FOO", Some("bar"))
        .flag("-UMOZILLA_CONFIG_H")
        .flag("-std=c++14")
        .include("src")
        .include(std::env::var("OUT_DIR").unwrap())
        .compile("gl_cc");
}
