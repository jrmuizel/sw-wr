extern crate cc;

fn main() {
    cc::Build::new()
        .cpp(true)
        .file("src/gl.cc")
        .define("FOO", Some("bar"))
        .flag("-std=c++14")
        .include("src")
        .compile("gl_cc");
}
