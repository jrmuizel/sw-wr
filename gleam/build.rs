extern crate gl_generator;
extern crate cc;

use std::env;
use std::fs::File;
use std::path::Path;

fn main() {
    cc::Build::new()
        .cpp(true)
        .file("src/gl.cc")
        .file("src/main.cc")
        .file("src/model.cc")
        .file("src/our_gl.cc")
        .file("src/tgaimage.cc")
        .file("src/geometry.cc")
        .define("FOO", Some("bar"))
        .include("src")
        .compile("gl_cc");
}
