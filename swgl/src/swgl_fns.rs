/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#[allow(warnings)]

use std::ffi::{CStr, CString};
use std::os::raw::{c_char, c_int, c_void};
use std::ptr;
use std::rc::Rc;
use std::str;
use gleam::gl::*;

pub struct SwGlFns {
}

extern {
}

extern "C" {
    fn ActiveTexture(texture: GLenum);
    fn BindTexture(target: GLenum, texture: GLuint);
    fn BindBuffer(target: GLenum, buffer: GLuint);
    fn BindVertexArray(vao: GLuint);
    fn BindFramebuffer(target: GLenum, fb: GLuint);
    fn BindRenderbuffer(target: GLenum, rb: GLuint);
    fn BlendFunc(srgb: GLenum, drgb: GLenum, sa: GLenum, da: GLenum);
    fn BlendColor(r: GLfloat, g: GLfloat, b: GLfloat, a: GLfloat);
    fn BlendEquation(mode: GLenum);
    fn Enable(cap: GLenum);
    fn Disable(cap: GLenum);
    fn GenQueries(n: GLsizei, result: *mut GLuint);
    fn BeginQuery(target: GLenum, id: GLuint);
    fn EndQuery(target: GLenum);
    fn GetQueryObjectui64v(id: GLuint, pname: GLenum, params: *mut GLuint64);
    fn GenBuffers(n: i32, result: *mut u32);
    fn GenTextures(n: i32, result: *mut u32);
    fn GenFramebuffers(n: i32, result: *mut u32);
    fn GenRenderbuffers(n: i32, result: *mut u32);
    fn BufferData(target: GLenum,
        size: GLsizeiptr,
        data: *const GLvoid,
        usage: GLenum);
    fn BufferSubData(
        target: GLenum,
        offset: GLintptr,
        size: GLsizeiptr,
        data: *const GLvoid);
    fn TexStorage2D(
        target: GLenum,
        levels: GLint,
        internal_format: GLenum,
        width: GLsizei,
        height: GLsizei,
    );
    fn FramebufferTexture2D(
        target: GLenum,
        attachment: GLenum,
        textarget: GLenum,
        texture: GLuint,
        level: GLint);
    fn CheckFramebufferStatus(target: GLenum) -> GLenum;
    fn TexStorage3D(
        target: GLenum,
        levels: GLint,
        internal_format: GLenum,
        width: GLsizei,
        height: GLsizei,
        depth: GLsizei,
    );
    fn TexImage2D(
        target: GLenum,
        level: GLint,
        internal_format: GLint,
        width: GLsizei,
        height: GLsizei,
        border: GLint,
        format: GLenum,
        ty: GLenum,
        data: *const c_void,
    );
    fn TexImage3D(
        target: GLenum,
        level: GLint,
        internal_format: GLint,
        width: GLsizei,
        height: GLsizei,
        depth: GLsizei,
        border: GLint,
        format: GLenum,
        ty: GLenum,
        data: *const c_void,
    );
    fn TexSubImage2D(
        target: GLenum,
        level: GLint,
        xoffset: GLint,
        yoffset: GLint,
        width: GLsizei,
        height: GLsizei,
        format: GLenum,
        ty: GLenum,
        data: *const c_void,
    );
    fn TexSubImage3D(
        target: GLenum,
        level: GLint,
        xoffset: GLint,
        yoffset: GLint,
        zoffset: GLint,
        width: GLsizei,
        height: GLsizei,
        depth: GLsizei,
        format: GLenum,
        ty: GLenum,
        data: *const c_void,
    );
    fn GetUniformLocation(program: GLuint, name: *const GLchar) -> GLint;
    fn BindAttribLocation(program: GLuint, index: GLuint, name: *const GLchar);
    fn GenVertexArrays(n: i32, result: *mut u32);
    fn VertexAttribPointer(
        index: GLuint,
        size: GLint,
        type_: GLenum,
        normalized: GLboolean,
        stride: GLsizei,
        offset: *const GLvoid,
    );
    fn VertexAttribIPointer(
        index: GLuint,
        size: GLint,
        type_: GLenum,
        stride: GLsizei,
        offset: *const GLvoid,
    );
    fn CreateShader(shader_type: GLenum) -> GLuint;
    fn AttachShader(program: GLuint, shader: GLuint);
    fn CreateProgram() -> GLuint;
    fn Uniform1i(location: GLint, v0: GLint);
    fn Uniform4fv(location: GLint, count: GLsizei, value: *const GLfloat);
    fn UniformMatrix4fv(location: GLint, count: GLsizei, transpose: GLboolean, value: *const GLfloat);

    fn DrawElementsInstanced(mode: GLenum, count: GLsizei, type_: GLenum, indices: *const c_void, instancecount: GLsizei);
    fn EnableVertexAttribArray(index: GLuint);
    fn VertexAttribDivisor(index: GLuint, divisor: GLuint);
    fn LinkProgram(program: GLuint);
    fn UseProgram(program: GLuint);
    fn SetViewport(x: GLint, y: GLint, width: GLsizei, height: GLsizei);
    fn FramebufferTextureLayer(
        target: GLenum,
        attachment: GLenum,
        texture: GLuint,
        level: GLint,
        layer: GLint,
    );
    fn FramebufferRenderbuffer(
        target: GLenum,
        attachment: GLenum,
        renderbuffertarget: GLenum,
        renderbuffer: GLuint,
    );
    fn RenderbufferStorage(
        target: GLenum,
        internalformat: GLenum,
        width: GLsizei,
        height: GLsizei,
    );
    fn DepthMask(flag: GLboolean);
    fn DepthFunc(func: GLenum);
    fn SetScissor(x: GLint, y: GLint, width: GLsizei, height: GLsizei);
    fn ClearColor(r: GLfloat, g: GLfloat, b: GLfloat, a: GLfloat);
    fn ClearDepth(depth: GLdouble);
    fn Clear(mask: GLbitfield);
    fn PixelStorei(name: GLenum, param: GLint);
    fn ReadPixels(
        x: GLint,
        y: GLint,
        width: GLsizei,
        height: GLsizei,
        format: GLenum,
        ty: GLenum,
        data: *mut c_void,
    );
    fn Finish();
    fn ShaderSourceByName(shader: GLuint, name: *const GLchar);
    fn TexParameteri(target: GLenum, pname: GLenum, param: GLint);
    fn CopyImageSubData(
        src_name: GLuint,
        src_target: GLenum,
        src_level: GLint,
        src_x: GLint,
        src_y: GLint,
        src_z: GLint,
        dst_name: GLuint,
        dst_target: GLenum,
        dst_level: GLint,
        dst_x: GLint,
        dst_y: GLint,
        dst_z: GLint,
        src_width: GLsizei,
        src_height: GLsizei,
        src_depth: GLsizei,
    );
    fn CopyTexSubImage2D(
        target: GLenum,
        level: GLint,
        xoffset: GLint,
        yoffset: GLint,
        x: GLint,
        y: GLint,
        width: GLsizei,
        height: GLsizei,
    );
    fn CopyTexSubImage3D(
        target: GLenum,
        level: GLint,
        xoffset: GLint,
        yoffset: GLint,
        zoffset: GLint,
        x: GLint,
        y: GLint,
        width: GLsizei,
        height: GLsizei,
    );
    fn BlitFramebuffer(
        src_x0: GLint,
        src_y0: GLint,
        src_x1: GLint,
        src_y1: GLint,
        dst_x0: GLint,
        dst_y0: GLint,
        dst_x1: GLint,
        dst_y1: GLint,
        mask: GLbitfield,
        filter: GLenum,
    );
    fn GetIntegerv(pname: GLenum, params: *mut GLint);
    fn GetBooleanv(pname: GLenum, params: *mut GLboolean);
    fn GetString(name: GLenum) -> *const c_char;
    fn GetStringi(name: GLenum, index: GLuint) -> *const c_char;
    fn GetError() -> GLenum;
    fn Update(width: i32, height: i32);
    fn GetColorBuffer(width: *mut i32, height: *mut i32) -> *mut c_void;
    fn DeleteTexture(n: GLuint);
    fn DeleteRenderbuffer(n: GLuint);
    fn DeleteFramebuffer(n: GLuint);
    fn DeleteBuffer(n: GLuint);
    fn DeleteVertexArray(n: GLuint);
    fn DeleteQuery(n: GLuint);
    fn DeleteShader(shader: GLuint);
    fn DeleteProgram(program: GLuint);
}

impl SwGlFns {
    pub fn load() -> Rc<Gl> {
        Rc::new(SwGlFns {}) as Rc<Gl>
    }

    pub fn update(width: i32, height: i32) {
        unsafe {
            Update(width, height);
        }
    }

    pub fn get_color_buffer() -> (*mut c_void, i32, i32) {
        unsafe {
            let mut width: i32 = 0;
            let mut height: i32 = 0;
            let data_ptr = GetColorBuffer(&mut width, &mut height);
            (data_ptr, width, height)
        }
    }
}

fn calculate_length(width: GLsizei, height: GLsizei, format: GLenum, pixel_type: GLenum) -> usize {
    let colors = match format {
        RED => 1,
        RGB => 3,
        BGR => 3,

        RGBA => 4,
        BGRA => 4,

        ALPHA => 1,
        R16 => 1,
        LUMINANCE => 1,
        DEPTH_COMPONENT => 1,
        _ => panic!("unsupported format for read_pixels: {:?}", format),
    };
    let depth = match pixel_type {
        UNSIGNED_BYTE => 1,
        UNSIGNED_SHORT => 2,
        SHORT => 2,
        FLOAT => 4,
        _ => panic!("unsupported pixel_type for read_pixels: {:?}", pixel_type),
    };

    return (width * height * colors * depth) as usize;
}

impl Gl for SwGlFns {
    fn get_type(&self) -> GlType {
        GlType::Gl
    }

    fn buffer_data_untyped(
        &self,
        target: GLenum,
        size: GLsizeiptr,
        data: *const GLvoid,
        usage: GLenum,
    ) {
        println!("buffer_data_untyped {} {} {:?} {}", target, size, data, usage);
        //panic!();
        unsafe {
                BufferData(target, size, data, usage);
        }
    }

    fn buffer_sub_data_untyped(
        &self,
        target: GLenum,
        offset: isize,
        size: GLsizeiptr,
        data: *const GLvoid,
    ) {
        println!("buffer_sub_data_untyped {} {} {} {:?}",
                target, offset, size, data);
        //panic!();
        unsafe {
                BufferSubData(target, offset, size, data);
        }
    }

    fn map_buffer(&self,
                  target: GLenum,
                  access: GLbitfield) -> *mut c_void {
        panic!();
        ptr::null_mut()
    }

    fn map_buffer_range(&self,
                        target: GLenum,
                        offset: GLintptr,
                        length: GLsizeiptr,
                        access: GLbitfield) -> *mut c_void {
        panic!();
        ptr::null_mut()
    }

    fn unmap_buffer(&self, target: GLenum) -> GLboolean {
        panic!();
        0
    }

    fn shader_source(&self, shader: GLuint, strings: &[&[u8]]) {
        //panic!();
        println!("shader_source {}", shader);
        for s in strings {
            println!("{}", str::from_utf8(s).unwrap());
        }
        //panic!();
        for s in strings {
            let u = str::from_utf8(s).unwrap();
            const prefix: &'static str = "// shader: ";
            if let Some(start) = u.find(prefix) {
                if let Some(end) = u[start..].find('\n') {
                    unsafe {
                        let c_string = CString::new(u[start + prefix.len() .. start + end].trim().replace(" ", "")).unwrap();
                        ShaderSourceByName(shader, c_string.as_ptr());
                        return;
                    }
                }
            }
        }
        panic!("unknown shader");
    }


    fn tex_buffer(&self, target: GLenum, internal_format: GLenum, buffer: GLuint) {
        panic!();
    }

    fn read_buffer(&self, mode: GLenum) {
        panic!();
    }

    fn read_pixels_into_buffer(
        &self,
        x: GLint,
        y: GLint,
        width: GLsizei,
        height: GLsizei,
        format: GLenum,
        pixel_type: GLenum,
        dst_buffer: &mut [u8],
    ) {
        // Assumes that the user properly allocated the size for dst_buffer.
        assert!(calculate_length(width, height, format, pixel_type) == dst_buffer.len());

        unsafe {
                ReadPixels(
                    x,
                    y,
                    width,
                    height,
                    format,
                    pixel_type,
                    dst_buffer.as_mut_ptr() as *mut c_void,
                );
        }
    }

    fn read_pixels(
        &self,
        x: GLint,
        y: GLint,
        width: GLsizei,
        height: GLsizei,
        format: GLenum,
        pixel_type: GLenum,
    ) -> Vec<u8> {
        let len = calculate_length(width, height, format, pixel_type);
        let mut pixels: Vec<u8> = Vec::new();
        pixels.reserve(len);
        unsafe {
            pixels.set_len(len);
        }

        self.read_pixels_into_buffer(
            x,
            y,
            width,
            height,
            format,
            pixel_type,
            pixels.as_mut_slice(),
        );

        pixels
    }

    unsafe fn read_pixels_into_pbo(&self,
                            x: GLint,
                            y: GLint,
                            width: GLsizei,
                            height: GLsizei,
                            format: GLenum,
                            pixel_type: GLenum) {
        panic!();
    }

    fn sample_coverage(&self, value: GLclampf, invert: bool) {
        panic!();
    }

    fn polygon_offset(&self, factor: GLfloat, units: GLfloat) {
        panic!();
    }

    fn pixel_store_i(&self, name: GLenum, param: GLint) {
        //panic!();
        println!("pixel_store_i {:x} {}", name, param);
        unsafe {
                PixelStorei(name, param);
        }
    }

    fn gen_buffers(&self, n: GLsizei) -> Vec<GLuint> {
        //panic!();
        let mut result = vec![0 as GLuint; n as usize];
        unsafe {
                GenBuffers(n, result.as_mut_ptr());
        }
        result
    }

    fn gen_renderbuffers(&self, n: GLsizei) -> Vec<GLuint> {
        println!("gen_renderbuffers {}", n);
        //panic!();
        let mut result = vec![0 as GLuint; n as usize];
        unsafe {
                GenRenderbuffers(n, result.as_mut_ptr());
        }
        result
    }

    fn gen_framebuffers(&self, n: GLsizei) -> Vec<GLuint> {
        //panic!();
        println!("gen_framebuffers {}", n);
        let mut result = vec![0 as GLuint; n as usize];
        unsafe {
                GenFramebuffers(n, result.as_mut_ptr());
        }
        result
    }

    fn gen_textures(&self, n: GLsizei) -> Vec<GLuint> {
        //panic!();
        let mut result = vec![0 as GLuint; n as usize];
        unsafe {
                GenTextures(n, result.as_mut_ptr());
        }
        result
    }

    fn gen_vertex_arrays(&self, n: GLsizei) -> Vec<GLuint> {
        //panic!();
        let mut result = vec![0 as GLuint; n as usize];
        unsafe {
                GenVertexArrays(n, result.as_mut_ptr());
        }
        result
    }

    fn gen_queries(&self, n: GLsizei) -> Vec<GLuint> {
        let mut result = vec![0 as GLuint; n as usize];
        unsafe {
                GenQueries(n, result.as_mut_ptr());
        }
        result
    }

    fn begin_query(&self, target: GLenum, id: GLuint) {
        unsafe {
                BeginQuery(target, id);
        }
    }

    fn end_query(&self, target: GLenum) {
        unsafe {
                EndQuery(target);
        }
    }

    fn query_counter(&self, id: GLuint, target: GLenum) {
        panic!();
    }

    fn get_query_object_iv(&self, id: GLuint, pname: GLenum) -> i32 {
        panic!();
        0
    }

    fn get_query_object_uiv(&self, id: GLuint, pname: GLenum) -> u32 {
        panic!();
        0
    }

    fn get_query_object_i64v(&self, id: GLuint, pname: GLenum) -> i64 {
        panic!();
        0
    }

    fn get_query_object_ui64v(&self, id: GLuint, pname: GLenum) -> u64 {
        let mut result = 0;
        unsafe {
                GetQueryObjectui64v(id, pname, &mut result);
        }
        result
    }

    fn delete_queries(&self, queries: &[GLuint]) {
        unsafe {
            for q in queries {
                DeleteQuery(*q);
            }
        }
    }

    fn delete_vertex_arrays(&self, vertex_arrays: &[GLuint]) {
        unsafe {
            for v in vertex_arrays {
                DeleteVertexArray(*v);
            }
        }
    }

    fn delete_buffers(&self, buffers: &[GLuint]) {
        unsafe {
            for b in buffers {
                DeleteBuffer(*b);
            }
        }
    }

    fn delete_renderbuffers(&self, renderbuffers: &[GLuint]) {
        unsafe {
            for r in renderbuffers {
                DeleteRenderbuffer(*r);
            }
        }
    }

    fn delete_framebuffers(&self, framebuffers: &[GLuint]) {
        unsafe {
            for f in framebuffers {
                DeleteFramebuffer(*f);
            }
        }
    }

    fn delete_textures(&self, textures: &[GLuint]) {
        unsafe {
            for t in textures {
                DeleteTexture(*t);
            }
        }
    }

    fn framebuffer_renderbuffer(
        &self,
        target: GLenum,
        attachment: GLenum,
        renderbuffertarget: GLenum,
        renderbuffer: GLuint,
    ) {
        println!("framebufer_renderbuffer {} {} {} {}",
                 target, attachment, renderbuffertarget,
                 renderbuffer);
        //panic!();
        unsafe {
                FramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
        }
    }

    fn renderbuffer_storage(
        &self,
        target: GLenum,
        internalformat: GLenum,
        width: GLsizei,
        height: GLsizei,
    ) {
        println!("renderbuffer_storage {} {} {} {}", target, internalformat, width, height);
        //panic!();
        unsafe {
                RenderbufferStorage(target, internalformat, width, height);
        }
    }

    fn depth_func(&self, func: GLenum) {
        println!("depth_func {}", func);
        //panic!();
        unsafe {
                DepthFunc(func);
        }
    }

    fn active_texture(&self, texture: GLenum) {
        //panic!();
        unsafe {
                ActiveTexture(texture);
        }
    }

    fn attach_shader(&self, program: GLuint, shader: GLuint) {
        println!("attach shader {} {}", program, shader);
        //panic!();
        unsafe {
                AttachShader(program, shader);
        }
    }

    fn bind_attrib_location(&self, program: GLuint, index: GLuint, name: &str) {
        println!("bind_attrib_location {} {} {}", program, index, name);
        //panic!();
        let c_string = CString::new(name).unwrap();
        unsafe {
                BindAttribLocation(program, index, c_string.as_ptr())
        }
    }

    // https://www.khronos.org/registry/OpenGL-Refpages/es2.0/xhtml/glGetUniform.xml
    unsafe fn get_uniform_iv(&self, program: GLuint, location: GLint, result: &mut [GLint]) {
        panic!();
        assert!(!result.is_empty());
    }

    // https://www.khronos.org/registry/OpenGL-Refpages/es2.0/xhtml/glGetUniform.xml
    unsafe fn get_uniform_fv(&self, program: GLuint, location: GLint, result: &mut [GLfloat]) {
        panic!();
        assert!(!result.is_empty());
    }

    fn get_uniform_block_index(&self, program: GLuint, name: &str) -> GLuint {
        panic!();
        0
    }

    fn get_uniform_indices(&self, program: GLuint, names: &[&str]) -> Vec<GLuint> {
        panic!();
        Vec::new()
    }

    fn bind_buffer_base(&self, target: GLenum, index: GLuint, buffer: GLuint) {
        panic!();
    }

    fn bind_buffer_range(
        &self,
        target: GLenum,
        index: GLuint,
        buffer: GLuint,
        offset: GLintptr,
        size: GLsizeiptr,
    ) {
        panic!();
    }

    fn uniform_block_binding(
        &self,
        program: GLuint,
        uniform_block_index: GLuint,
        uniform_block_binding: GLuint,
    ) {
        panic!();
    }

    fn bind_buffer(&self, target: GLenum, buffer: GLuint) {
        //panic!();
        unsafe {
                BindBuffer(target, buffer);
        }
    }

    fn bind_vertex_array(&self, vao: GLuint) {
        //panic!();
        unsafe {
                BindVertexArray(vao);
        }
    }

    fn bind_renderbuffer(&self, target: GLenum, renderbuffer: GLuint) {
        println!("bind_renderbuffer {} {}", target, renderbuffer);
        //panic!();
        unsafe {
                BindRenderbuffer(target, renderbuffer);
        }
    }

    fn bind_framebuffer(&self, target: GLenum, framebuffer: GLuint) {
        println!("bind_framebuffer {} {}", target, framebuffer);
        //panic!();
        unsafe {
                BindFramebuffer(target, framebuffer);
        }
    }

    fn bind_texture(&self, target: GLenum, texture: GLuint) {
        //panic!();
        unsafe {
                BindTexture(target, texture);
        }
    }

    fn draw_buffers(&self, bufs: &[GLenum]) {
        panic!();
        unsafe {
        }
    }

    // FIXME: Does not verify buffer size -- unsafe!
    fn tex_image_2d(
        &self,
        target: GLenum,
        level: GLint,
        internal_format: GLint,
        width: GLsizei,
        height: GLsizei,
        border: GLint,
        format: GLenum,
        ty: GLenum,
        opt_data: Option<&[u8]>,
    ) {
        unsafe {
            let pdata = match opt_data {
                Some(data) => data.as_ptr() as *const GLvoid,
                None => ptr::null(),
            };
                TexImage2D(
                    target,
                    level,
                    internal_format,
                    width,
                    height,
                    border,
                    format,
                    ty,
                    pdata,
                );
        }
    }

    fn compressed_tex_image_2d(
        &self,
        target: GLenum,
        level: GLint,
        internal_format: GLenum,
        width: GLsizei,
        height: GLsizei,
        border: GLint,
        data: &[u8],
    ) {
        panic!();
    }

    fn compressed_tex_sub_image_2d(
        &self,
        target: GLenum,
        level: GLint,
        xoffset: GLint,
        yoffset: GLint,
        width: GLsizei,
        height: GLsizei,
        format: GLenum,
        data: &[u8],
    ) {
        panic!();
    }

    // FIXME: Does not verify buffer size -- unsafe!
    fn tex_image_3d(
        &self,
        target: GLenum,
        level: GLint,
        internal_format: GLint,
        width: GLsizei,
        height: GLsizei,
        depth: GLsizei,
        border: GLint,
        format: GLenum,
        ty: GLenum,
        opt_data: Option<&[u8]>,
    ) {
        unsafe {
            let pdata = match opt_data {
                Some(data) => data.as_ptr() as *const GLvoid,
                None => ptr::null(),
            };
                TexImage3D(
                    target,
                    level,
                    internal_format,
                    width,
                    height,
                    depth,
                    border,
                    format,
                    ty,
                    pdata,
                );
        }
    }

    fn copy_tex_image_2d(
        &self,
        target: GLenum,
        level: GLint,
        internal_format: GLenum,
        x: GLint,
        y: GLint,
        width: GLsizei,
        height: GLsizei,
        border: GLint,
    ) {
        panic!();
    }

    fn copy_tex_sub_image_2d(
        &self,
        target: GLenum,
        level: GLint,
        xoffset: GLint,
        yoffset: GLint,
        x: GLint,
        y: GLint,
        width: GLsizei,
        height: GLsizei,
    ) {
        unsafe {
                CopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
        }
    }

    fn copy_tex_sub_image_3d(
        &self,
        target: GLenum,
        level: GLint,
        xoffset: GLint,
        yoffset: GLint,
        zoffset: GLint,
        x: GLint,
        y: GLint,
        width: GLsizei,
        height: GLsizei,
    ) {
        unsafe {
                CopyTexSubImage3D(target, level, xoffset, yoffset, zoffset, x, y, width, height);
        }
    }

    fn tex_sub_image_2d(
        &self,
        target: GLenum,
        level: GLint,
        xoffset: GLint,
        yoffset: GLint,
        width: GLsizei,
        height: GLsizei,
        format: GLenum,
        ty: GLenum,
        data: &[u8],
    ) {
        println!("tex_sub_image_2d {} {} {} {} {} {} {} {}",
                 target, level, xoffset, yoffset, width, height, format, ty);
        //panic!();
        unsafe {
                TexSubImage2D(
                    target,
                    level,
                    xoffset,
                    yoffset,
                    width,
                    height,
                    format,
                    ty,
                    data.as_ptr() as *const c_void,
                );
        }
    }

    fn tex_sub_image_2d_pbo(
        &self,
        target: GLenum,
        level: GLint,
        xoffset: GLint,
        yoffset: GLint,
        width: GLsizei,
        height: GLsizei,
        format: GLenum,
        ty: GLenum,
        offset: usize,
    ) {
        println!("tex_sub_image_2d_pbo {} {} {} {} {} {} {} {} {}",
                 target, level, xoffset, yoffset, width, height,
                 format, ty, offset);
        //panic!();
        unsafe {
            TexSubImage2D(
                target,
                level,
                xoffset,
                yoffset,
                width,
                height,
                format,
                ty,
                offset as *const c_void,
            );
        }
    }

    fn tex_sub_image_3d(
        &self,
        target: GLenum,
        level: GLint,
        xoffset: GLint,
        yoffset: GLint,
        zoffset: GLint,
        width: GLsizei,
        height: GLsizei,
        depth: GLsizei,
        format: GLenum,
        ty: GLenum,
        data: &[u8],
    ) {
        println!("tex_sub_image_3d");
        //panic!();
        unsafe {
            TexSubImage3D(
                target,
                level,
                xoffset,
                yoffset,
                zoffset,
                width,
                height,
                depth,
                format,
                ty,
                data.as_ptr() as *const c_void,
            );
        }
    }

    fn tex_sub_image_3d_pbo(
        &self,
        target: GLenum,
        level: GLint,
        xoffset: GLint,
        yoffset: GLint,
        zoffset: GLint,
        width: GLsizei,
        height: GLsizei,
        depth: GLsizei,
        format: GLenum,
        ty: GLenum,
        offset: usize,
    ) {
        unsafe {
            TexSubImage3D(
                target,
                level,
                xoffset,
                yoffset,
                zoffset,
                width,
                height,
                depth,
                format,
                ty,
                offset as *const c_void,
            );
        }
    }

    fn tex_storage_2d(
        &self,
        target: GLenum,
        levels: GLint,
        internal_format: GLenum,
        width: GLsizei,
        height: GLsizei,
    ) {
        //panic!();
            unsafe {
                    TexStorage2D(target, levels, internal_format, width, height);
            }
    }

    fn tex_storage_3d(
        &self,
        target: GLenum,
        levels: GLint,
        internal_format: GLenum,
        width: GLsizei,
        height: GLsizei,
        depth: GLsizei,
    ) {
        //panic!();
            unsafe {
                    TexStorage3D(target, levels, internal_format, width, height, depth);
            }
    }

    fn get_tex_image_into_buffer(
        &self,
        target: GLenum,
        level: GLint,
        format: GLenum,
        ty: GLenum,
        output: &mut [u8],
    ) {
        panic!();
    }

    unsafe fn copy_image_sub_data(
        &self,
        src_name: GLuint,
        src_target: GLenum,
        src_level: GLint,
        src_x: GLint,
        src_y: GLint,
        src_z: GLint,
        dst_name: GLuint,
        dst_target: GLenum,
        dst_level: GLint,
        dst_x: GLint,
        dst_y: GLint,
        dst_z: GLint,
        src_width: GLsizei,
        src_height: GLsizei,
        src_depth: GLsizei,
    ) {
            CopyImageSubData(
                src_name, src_target, src_level, src_x, src_y, src_z, dst_name, dst_target, dst_level,
                dst_x, dst_y, dst_z, src_width, src_height, src_depth,
            );
    }

    fn invalidate_framebuffer(&self, target: GLenum, attachments: &[GLenum]) {
    }

    fn invalidate_sub_framebuffer(
        &self,
        target: GLenum,
        attachments: &[GLenum],
        xoffset: GLint,
        yoffset: GLint,
        width: GLsizei,
        height: GLsizei,
    ) {
    }

    #[inline]
    unsafe fn get_integer_v(&self, name: GLenum, result: &mut [GLint]) {
        //panic!();
        assert!(!result.is_empty());
            GetIntegerv(name, result.as_mut_ptr());
    }

    #[inline]
    unsafe fn get_integer_64v(&self, name: GLenum, result: &mut [GLint64]) {
        panic!();
        assert!(!result.is_empty());
    }

    #[inline]
    unsafe fn get_integer_iv(&self, name: GLenum, index: GLuint, result: &mut [GLint]) {
        panic!();
        assert!(!result.is_empty());
    }

    #[inline]
    unsafe fn get_integer_64iv(&self, name: GLenum, index: GLuint, result: &mut [GLint64]) {
        panic!();
        assert!(!result.is_empty());
    }

    #[inline]
    unsafe fn get_boolean_v(&self, name: GLenum, result: &mut [GLboolean]) {
        println!("get_boolean_v {}", name);
        //panic!();
        assert!(!result.is_empty());
            GetBooleanv(name, result.as_mut_ptr());
    }

    #[inline]
    unsafe fn get_float_v(&self, name: GLenum, result: &mut [GLfloat]) {
        panic!();
        assert!(!result.is_empty());
    }

    fn get_framebuffer_attachment_parameter_iv(
        &self,
        target: GLenum,
        attachment: GLenum,
        pname: GLenum,
    ) -> GLint {
        panic!();
        0
    }

    fn get_renderbuffer_parameter_iv(&self, target: GLenum, pname: GLenum) -> GLint {
        panic!();
        0
    }

    fn get_tex_parameter_iv(&self, target: GLenum, pname: GLenum) -> GLint {
        panic!();
        0
    }

    fn get_tex_parameter_fv(&self, target: GLenum, pname: GLenum) -> GLfloat {
        panic!();
        0.0
    }

    fn tex_parameter_i(&self, target: GLenum, pname: GLenum, param: GLint) {
        //panic!();
        unsafe {
                TexParameteri(target, pname, param);
        }
    }

    fn tex_parameter_f(&self, target: GLenum, pname: GLenum, param: GLfloat) {
        panic!();
    }

    fn framebuffer_texture_2d(
        &self,
        target: GLenum,
        attachment: GLenum,
        textarget: GLenum,
        texture: GLuint,
        level: GLint,
    ) {
        println!("framebuffer_texture_2d {} {} {} {} {}",
                 target, attachment, textarget, texture,
                 level);
        //panic!();
        unsafe {
                FramebufferTexture2D(target, attachment, textarget, texture, level);
        }
    }

    fn framebuffer_texture_layer(
        &self,
        target: GLenum,
        attachment: GLenum,
        texture: GLuint,
        level: GLint,
        layer: GLint,
    ) {
        println!("framebuffer_texture_layer {} {} {} {} {}",
                 target, attachment, texture, level, layer);
        //panic!();
        unsafe {
                FramebufferTextureLayer(target, attachment, texture, level, layer);
        }
    }

    fn blit_framebuffer(
        &self,
        src_x0: GLint,
        src_y0: GLint,
        src_x1: GLint,
        src_y1: GLint,
        dst_x0: GLint,
        dst_y0: GLint,
        dst_x1: GLint,
        dst_y1: GLint,
        mask: GLbitfield,
        filter: GLenum,
    ) {
        unsafe {
                BlitFramebuffer(
                    src_x0, src_y0, src_x1, src_y1, dst_x0, dst_y0, dst_x1, dst_y1, mask, filter,
                );
        }
    }

    fn vertex_attrib_4f(&self, index: GLuint, x: GLfloat, y: GLfloat, z: GLfloat, w: GLfloat) {
        panic!();
    }

    fn vertex_attrib_pointer_f32(
        &self,
        index: GLuint,
        size: GLint,
        normalized: bool,
        stride: GLsizei,
        offset: GLuint,
    ) {
        panic!();
    }

    fn vertex_attrib_pointer(
        &self,
        index: GLuint,
        size: GLint,
        type_: GLenum,
        normalized: bool,
        stride: GLsizei,
        offset: GLuint,
    ) {
        println!("vertex_attrib_pointer {} {} {} {} {} {}", index, size, type_, normalized, stride, offset);
        //panic!();
        unsafe {
            VertexAttribPointer(
                index,
                size,
                type_,
                normalized as GLboolean,
                stride,
                offset as *const GLvoid);
        }
    }

    fn vertex_attrib_i_pointer(
        &self,
        index: GLuint,
        size: GLint,
        type_: GLenum,
        stride: GLsizei,
        offset: GLuint,
    ) {
        println!("vertex_attrib_i_pointer {} {} {} {} {}", index, size, type_, stride, offset);
        //panic!();
        unsafe {
                VertexAttribIPointer(index, size, type_, stride, offset as *const GLvoid);
        }
    }

    fn vertex_attrib_divisor(&self, index: GLuint, divisor: GLuint) {
        println!("vertex_attrib_divisor {} {}", index, divisor);
        //assert!(index == 0 && divisor == 0);
        //panic!();
        unsafe {
                VertexAttribDivisor(index, divisor);
        }
    }

    fn viewport(&self, x: GLint, y: GLint, width: GLsizei, height: GLsizei) {
        println!("viewport {} {} {} {}", x, y, width, height);
        //panic!();
        unsafe {
                SetViewport(x, y, width, height);
        }
    }

    fn scissor(&self, x: GLint, y: GLint, width: GLsizei, height: GLsizei) {
        //panic!();
        unsafe {
                SetScissor(x, y, width, height);
        }
    }

    fn line_width(&self, width: GLfloat) {
        panic!();
    }

    fn use_program(&self, program: GLuint) {
        //panic!();
        unsafe {
                UseProgram(program);
        }
    }

    fn validate_program(&self, program: GLuint) {
        panic!();
    }

    fn draw_arrays(&self, mode: GLenum, first: GLint, count: GLsizei) {
        panic!();
    }

    fn draw_arrays_instanced(
        &self,
        mode: GLenum,
        first: GLint,
        count: GLsizei,
        primcount: GLsizei,
    ) {
        panic!();
    }

    fn draw_elements(
        &self,
        mode: GLenum,
        count: GLsizei,
        element_type: GLenum,
        indices_offset: GLuint,
    ) {
        panic!();
    }

    fn draw_elements_instanced(
        &self,
        mode: GLenum,
        count: GLsizei,
        element_type: GLenum,
        indices_offset: GLuint,
        primcount: GLsizei,
    ) {
        println!("draw_elements_instanced {} {} {} {} {}",
                 mode, count, element_type, indices_offset, primcount);
        //panic!();
        unsafe {
            DrawElementsInstanced(
                mode,
                count,
                element_type,
                indices_offset as *const c_void,
                primcount,
            );
        }
    }

    fn blend_color(&self, r: f32, g: f32, b: f32, a: f32) {
        unsafe {
                BlendColor(r, g, b, a);
        }
    }

    fn blend_func(&self, sfactor: GLenum, dfactor: GLenum) {
        unsafe {
                BlendFunc(sfactor, dfactor, sfactor, dfactor);
        }
    }

    fn blend_func_separate(
        &self,
        src_rgb: GLenum,
        dest_rgb: GLenum,
        src_alpha: GLenum,
        dest_alpha: GLenum,
    ) {
        unsafe {
                BlendFunc(src_rgb, dest_rgb, src_alpha, dest_alpha);
        }
    }

    fn blend_equation(&self, mode: GLenum) {
        unsafe {
            BlendEquation(mode);
        }
    }

    fn blend_equation_separate(&self, mode_rgb: GLenum, mode_alpha: GLenum) {
        panic!();
    }

    fn color_mask(&self, r: bool, g: bool, b: bool, a: bool) {
        panic!();
    }

    fn cull_face(&self, mode: GLenum) {
        panic!();
    }

    fn front_face(&self, mode: GLenum) {
        panic!();
    }

    fn enable(&self, cap: GLenum) {
        println!("enable {}", cap);
        //panic!();
        unsafe {
                Enable(cap);
        }
    }

    fn disable(&self, cap: GLenum) {
        println!("disable {}", cap);
        //panic!();
        unsafe {
                Disable(cap);
        }
    }

    fn hint(&self, param_name: GLenum, param_val: GLenum) {
        panic!();
    }

    fn is_enabled(&self, cap: GLenum) -> GLboolean {
        panic!();
        0
    }

    fn is_shader(&self, shader: GLuint) -> GLboolean {
        panic!();
        0
    }

    fn is_texture(&self, texture: GLenum) -> GLboolean {
        panic!();
        0
    }

    fn is_framebuffer(&self, framebuffer: GLenum) -> GLboolean {
        panic!();
        0
    }

    fn is_renderbuffer(&self, renderbuffer: GLenum) -> GLboolean {
        panic!();
        0
    }

    fn check_frame_buffer_status(&self, target: GLenum) -> GLenum {
        println!("check_frame_buffer_status {}", target);
        //panic!();
        unsafe {
                CheckFramebufferStatus(target)
        }
    }

    fn enable_vertex_attrib_array(&self, index: GLuint) {
        //panic!();
        println!("enable_vertex_attrib_array {}", index);
        unsafe {
                EnableVertexAttribArray(index);
                //assert_eq!(index, 0);
        }
    }

    fn disable_vertex_attrib_array(&self, index: GLuint) {
        panic!();
    }

    fn uniform_1f(&self, location: GLint, v0: GLfloat) {
        panic!();
    }

    fn uniform_1fv(&self, location: GLint, values: &[f32]) {
        panic!();
    }

    fn uniform_1i(&self, location: GLint, v0: GLint) {
        println!("uniform_1i {} {}", location, v0);
        //panic!();
        unsafe {
                Uniform1i(location, v0);
        }
    }

    fn uniform_1iv(&self, location: GLint, values: &[i32]) {
        panic!();
    }

    fn uniform_1ui(&self, location: GLint, v0: GLuint) {
        panic!();
    }

    fn uniform_2f(&self, location: GLint, v0: GLfloat, v1: GLfloat) {
        panic!();
    }

    fn uniform_2fv(&self, location: GLint, values: &[f32]) {
        panic!();
    }

    fn uniform_2i(&self, location: GLint, v0: GLint, v1: GLint) {
        panic!();
    }

    fn uniform_2iv(&self, location: GLint, values: &[i32]) {
        panic!();
    }

    fn uniform_2ui(&self, location: GLint, v0: GLuint, v1: GLuint) {
        panic!();
    }

    fn uniform_3f(&self, location: GLint, v0: GLfloat, v1: GLfloat, v2: GLfloat) {
        panic!();
    }

    fn uniform_3fv(&self, location: GLint, values: &[f32]) {
        panic!();
    }

    fn uniform_3i(&self, location: GLint, v0: GLint, v1: GLint, v2: GLint) {
        panic!();
    }

    fn uniform_3iv(&self, location: GLint, values: &[i32]) {
        panic!();
    }

    fn uniform_3ui(&self, location: GLint, v0: GLuint, v1: GLuint, v2: GLuint) {
        panic!();
    }

    fn uniform_4f(&self, location: GLint, x: GLfloat, y: GLfloat, z: GLfloat, w: GLfloat) {
        panic!();
    }

    fn uniform_4i(&self, location: GLint, x: GLint, y: GLint, z: GLint, w: GLint) {
        panic!();
    }

    fn uniform_4iv(&self, location: GLint, values: &[i32]) {
        panic!();
    }

    fn uniform_4ui(&self, location: GLint, x: GLuint, y: GLuint, z: GLuint, w: GLuint) {
        panic!();
    }

    fn uniform_4fv(&self, location: GLint, values: &[f32]) {
        panic!();
        unsafe {
                Uniform4fv(location, (values.len() / 4) as GLsizei, values.as_ptr());
        }
    }

    fn uniform_matrix_2fv(&self, location: GLint, transpose: bool, value: &[f32]) {
        panic!();
    }

    fn uniform_matrix_3fv(&self, location: GLint, transpose: bool, value: &[f32]) {
        panic!();
    }

    fn uniform_matrix_4fv(&self, location: GLint, transpose: bool, value: &[f32]) {
        println!("uniform_matrix_4fv {} {} {:?}", location, transpose, value);
        //panic!();
        unsafe {
            UniformMatrix4fv(
                location,
                (value.len() / 16) as GLsizei,
                transpose as GLboolean,
                value.as_ptr(),
            );
        }
    }

    fn depth_mask(&self, flag: bool) {
        println!("depth_mask {}", flag);
        //panic!();
        unsafe {
                DepthMask(flag as GLboolean);
        }
    }

    fn depth_range(&self, near: f64, far: f64) {
        panic!();
    }

    fn get_active_attrib(&self, program: GLuint, index: GLuint) -> (i32, u32, String) {
        panic!();
        (0, 0, String::new())
    }

    fn get_active_uniform(&self, program: GLuint, index: GLuint) -> (i32, u32, String) {
        panic!();
        (0, 0, String::new())
    }

    fn get_active_uniforms_iv(
        &self,
        program: GLuint,
        indices: Vec<GLuint>,
        pname: GLenum,
    ) -> Vec<GLint> {
        panic!();
        Vec::new()
    }

    fn get_active_uniform_block_i(&self, program: GLuint, index: GLuint, pname: GLenum) -> GLint {
        panic!();
        0
    }

    fn get_active_uniform_block_iv(
        &self,
        program: GLuint,
        index: GLuint,
        pname: GLenum,
    ) -> Vec<GLint> {
        panic!();
        Vec::new()
    }

    fn get_active_uniform_block_name(&self, program: GLuint, index: GLuint) -> String {
        panic!();
        String::new()
    }

    fn get_attrib_location(&self, program: GLuint, name: &str) -> c_int {
        panic!();
        0
    }

    fn get_frag_data_location(&self, program: GLuint, name: &str) -> c_int {
        panic!();
        0
    }

    fn get_uniform_location(&self, program: GLuint, name: &str) -> c_int {
        println!("get_uniform_location {} {}", program, name);
        //panic!();
        let name = CString::new(name).unwrap();
        unsafe {
            GetUniformLocation(program, name.as_ptr())
        }
    }

    fn get_program_info_log(&self, program: GLuint) -> String {
        panic!();
        String::new()
    }

    #[inline]
    unsafe fn get_program_iv(&self, program: GLuint, pname: GLenum, result: &mut [GLint]) {
        println!("get_program_iv {}", pname);
        //panic!();
        assert!(!result.is_empty());
            //#define GL_LINK_STATUS                    0x8B82
            if pname == 0x8b82 {
                result[0] = 1;
            }
    }

    fn get_program_binary(&self, program: GLuint) -> (Vec<u8>, GLenum) {
        panic!();
        (Vec::new(), NONE)
    }

    fn program_binary(&self, program: GLuint, format: GLenum, binary: &[u8]) {
        panic!();
    }

    fn program_parameter_i(&self, program: GLuint, pname: GLenum, value: GLint) {
        panic!();
    }

    #[inline]
    unsafe fn get_vertex_attrib_iv(&self, index: GLuint, pname: GLenum, result: &mut [GLint]) {
        panic!();
        assert!(!result.is_empty());
    }

    #[inline]
    unsafe fn get_vertex_attrib_fv(&self, index: GLuint, pname: GLenum, result: &mut [GLfloat]) {
        panic!();
        assert!(!result.is_empty());
    }

    fn get_vertex_attrib_pointer_v(&self, index: GLuint, pname: GLenum) -> GLsizeiptr {
        panic!();
        0
    }

    fn get_buffer_parameter_iv(&self, target: GLuint, pname: GLenum) -> GLint {
        panic!();
        0
    }

    fn get_shader_info_log(&self, shader: GLuint) -> String {
        println!("get_shader_info_log {}", shader);
        //panic!();
        String::new()
    }

    fn get_string(&self, which: GLenum) -> String {
        // panic!();
        unsafe {
            let llstr = GetString(which);
            if !llstr.is_null() {
                return str::from_utf8_unchecked(CStr::from_ptr(llstr).to_bytes())
                    .to_string();
            } else {
                return "".to_string();
            }
        }
    }

    fn get_string_i(&self, which: GLenum, index: GLuint) -> String {
        //panic!();
        unsafe {
            let llstr = GetStringi(which, index);
            if !llstr.is_null() {
                str::from_utf8_unchecked(CStr::from_ptr(llstr).to_bytes())
                    .to_string()
            } else {
                "".to_string()
            }
        }
    }

    unsafe fn get_shader_iv(&self, shader: GLuint, pname: GLenum, result: &mut [GLint]) {
        println!("get_shader_iv");
        //panic!();
        assert!(!result.is_empty());
            if pname == 0x8B81 /*gl::COMPILE_STATUS*/ {
                result[0] = 1;
            }
    }

    fn get_shader_precision_format(
        &self,
        _shader_type: GLuint,
        precision_type: GLuint,
    ) -> (GLint, GLint, GLint) {
        panic!();
        // gl.GetShaderPrecisionFormat is not available until OpenGL 4.1.
        // Fallback to OpenGL standard precissions that most desktop hardware support.
        match precision_type {
            LOW_FLOAT | MEDIUM_FLOAT | HIGH_FLOAT => {
                // Fallback to IEEE 754 single precision
                // Range: from -2^127 to 2^127
                // Significand precision: 23 bits
                (127, 127, 23)
            }
            LOW_INT | MEDIUM_INT | HIGH_INT => {
                // Fallback to single precision integer
                // Range: from -2^24 to 2^24
                // Precision: For integer formats this value is always 0
                (24, 24, 0)
            }
            _ => (0, 0, 0),
        }
    }

    fn compile_shader(&self, shader: GLuint) {
        println!("compile_shader {}", shader);
        //panic!();
        unsafe {
        }
    }

    fn create_program(&self) -> GLuint {
        println!("create_program");
        //panic!();
        unsafe {
                CreateProgram()
        }
    }

    fn delete_program(&self, program: GLuint) {
        unsafe {
            DeleteProgram(program);
        }
    }

    fn create_shader(&self, shader_type: GLenum) -> GLuint {
        println!("create_shader {}", shader_type);
        //panic!();
        unsafe {
                CreateShader(shader_type)
        }
    }

    fn delete_shader(&self, shader: GLuint) {
        println!("delete_shader {}", shader);
        //panic!();
        unsafe {
            DeleteShader(shader);
        }
    }

    fn detach_shader(&self, program: GLuint, shader: GLuint) {
        println!("detach_shader {} {}", program, shader);
        //panic!();
        unsafe {
        }
    }

    fn link_program(&self, program: GLuint) {
        println!("link_program {}", program);
        //panic!();
        unsafe {
            LinkProgram(program);
        }
    }

    fn clear_color(&self, r: f32, g: f32, b: f32, a: f32) {
        //panic!();
        unsafe {
                ClearColor(r, g, b, a);
        }
    }

    fn clear(&self, buffer_mask: GLbitfield) {
        println!("clear {}", buffer_mask);
        //panic!();
        unsafe {
                Clear(buffer_mask);
        }
    }

    fn clear_depth(&self, depth: f64) {
        println!("clear_depth {}", depth);
        //panic!();
        unsafe {
                ClearDepth(depth as GLclampd);
        }
    }

    fn clear_stencil(&self, s: GLint) {
        panic!();
    }

    fn flush(&self) {
        unsafe {
        }
    }

    fn finish(&self) {
        unsafe {
                Finish();
        }
    }

    fn get_error(&self) -> GLenum {
        //panic!();
        unsafe {
                GetError()
        }
    }

    fn stencil_mask(&self, mask: GLuint) {
        panic!();
    }

    fn stencil_mask_separate(&self, face: GLenum, mask: GLuint) {
        panic!();
    }

    fn stencil_func(&self, func: GLenum, ref_: GLint, mask: GLuint) {
        panic!();
    }

    fn stencil_func_separate(&self, face: GLenum, func: GLenum, ref_: GLint, mask: GLuint) {
        panic!();
    }

    fn stencil_op(&self, sfail: GLenum, dpfail: GLenum, dppass: GLenum) {
        panic!();
    }

    fn stencil_op_separate(&self, face: GLenum, sfail: GLenum, dpfail: GLenum, dppass: GLenum) {
        panic!();
    }

    #[allow(unused_variables)]
    fn egl_image_target_texture2d_oes(&self, target: GLenum, image: GLeglImageOES) {
        panic!("not supported")
    }

    fn generate_mipmap(&self, target: GLenum) {
        panic!();
    }

    fn insert_event_marker_ext(&self, message: &str) {
        panic!();
    }

    fn push_group_marker_ext(&self, message: &str) {
        println!("push group {}", message);
        panic!();
    }

    fn pop_group_marker_ext(&self) {
        println!("pop group");
        panic!();
    }

    fn debug_message_insert_khr(&self, source: GLenum, type_: GLenum, id: GLuint, severity: GLenum, message: &str) {
        panic!();
    }

    fn push_debug_group_khr(&self, source: GLenum, id: GLuint, message: &str) {
        panic!();
    }

    fn pop_debug_group_khr(&self) {
        panic!();
    }

    fn fence_sync(&self, condition: GLenum, flags: GLbitfield) -> GLsync {
        panic!();
        ptr::null()
    }

    fn client_wait_sync(&self, sync: GLsync, flags: GLbitfield, timeout: GLuint64) {
        panic!();
    }

    fn wait_sync(&self, sync: GLsync, flags: GLbitfield, timeout: GLuint64) {
        panic!();
    }

    fn texture_range_apple(&self, target: GLenum, data: &[u8]) {
        panic!();
    }

    fn delete_sync(&self, sync: GLsync) {
        panic!();
    }

    fn gen_fences_apple(&self, n: GLsizei) -> Vec<GLuint> {
        panic!();
        Vec::new()
    }

    fn delete_fences_apple(&self, fences: &[GLuint]) {
        panic!();
    }

    fn set_fence_apple(&self, fence: GLuint) {
        panic!();
    }

    fn finish_fence_apple(&self, fence: GLuint) {
        panic!();
    }

    fn test_fence_apple(&self, fence: GLuint) {
        panic!();
    }

    fn test_object_apple(&self, object: GLenum, name: GLuint) -> GLboolean {
        panic!();
        0
    }

    fn finish_object_apple(&self, object: GLenum, name: GLuint) {
        panic!();
    }

    // GL_ARB_blend_func_extended
    fn bind_frag_data_location_indexed(
        &self,
        program: GLuint,
        color_number: GLuint,
        index: GLuint,
        name: &str,
    ) {
        panic!();
    }

    fn get_frag_data_index(&self, program: GLuint, name: &str) -> GLint {
        panic!();
        -1
    }

    // GL_KHR_debug
    fn get_debug_messages(&self) -> Vec<DebugMessage> {
        Vec::new()
    }

    fn provoking_vertex_angle(&self, _mode: GLenum) {
        panic!();
        unimplemented!("This extension is GLES only");
    }

    // GL_KHR_blend_equation_advanced
    fn blend_barrier_khr(&self) {
        panic!();
    }
}