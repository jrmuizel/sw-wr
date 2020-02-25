template<typename S>
void textureLinearCommit4(S sampler, ivec2 i, int zoffset, uint32_t* buf) {
    assert(sampler->format == TextureFormat::RGBA8);
    ivec2 frac = i & 0x7F;
    i >>= 7;

    I32 row0 = clampCoord(i.x, sampler->width) + clampCoord(i.y, sampler->height)*sampler->stride + zoffset;
    I32 row1 = row0 + ((i.y > 0 && i.y < int32_t(sampler->height)-1) & I32(sampler->stride));
    I16 fracx = __builtin_convertvector(frac.x & (i.x > 0 && i.x < int32_t(sampler->width)-1), I16);
    I16 fracy = __builtin_convertvector(frac.y, I16);

    auto a0 = __builtin_convertvector(unaligned_load<V8<uint8_t>>(&sampler->buf[row0.x]), V8<int16_t>);
    auto a1 = __builtin_convertvector(unaligned_load<V8<uint8_t>>(&sampler->buf[row1.x]), V8<int16_t>);
    a0 += ((a1 - a0) * fracy.x) >> 7;

    auto b0 = __builtin_convertvector(unaligned_load<V8<uint8_t>>(&sampler->buf[row0.y]), V8<int16_t>);
    auto b1 = __builtin_convertvector(unaligned_load<V8<uint8_t>>(&sampler->buf[row1.y]), V8<int16_t>);
    b0 += ((b1 - b0) * fracy.y) >> 7;

    auto abl = __builtin_shufflevector(a0, b0, 0, 1, 2, 3, 8, 9, 10, 11);
    auto abh = __builtin_shufflevector(a0, b0, 4, 5, 6, 7, 12, 13, 14, 15);
    abl += ((abh - abl) * fracx.xxxxyyyy) >> 7;

    auto c0 = __builtin_convertvector(unaligned_load<V8<uint8_t>>(&sampler->buf[row0.z]), V8<int16_t>);
    auto c1 = __builtin_convertvector(unaligned_load<V8<uint8_t>>(&sampler->buf[row1.z]), V8<int16_t>);
    c0 += ((c1 - c0) * fracy.z) >> 7;

    auto d0 = __builtin_convertvector(unaligned_load<V8<uint8_t>>(&sampler->buf[row0.w]), V8<int16_t>);
    auto d1 = __builtin_convertvector(unaligned_load<V8<uint8_t>>(&sampler->buf[row1.w]), V8<int16_t>);
    d0 += ((d1 - d0) * fracy.w) >> 7;

    auto cdl = __builtin_shufflevector(c0, d0, 0, 1, 2, 3, 8, 9, 10, 11);
    auto cdh = __builtin_shufflevector(c0, d0, 4, 5, 6, 7, 12, 13, 14, 15);
    cdl += ((cdh - cdl) * fracx.zzzzwwww) >> 7;

    commit_span(buf, pack(combine(HalfRGBA8(abl), HalfRGBA8(cdl))));
}

template<typename S>
void textureLinearCommit8(S sampler, ivec2_scalar i, int zoffset, uint32_t* buf) {
    assert(sampler->format == TextureFormat::RGBA8);
    ivec2_scalar frac = i & 0x7F;
    i >>= 7;

    uint32_t* row0 = &sampler->buf[clampCoord(i.x, sampler->width) + clampCoord(i.y, sampler->height)*sampler->stride + zoffset];
    uint32_t* row1 = row0 + ((i.y > 0 && i.y < int32_t(sampler->height)-1) ? sampler->stride : 0);
    int16_t fracx = i.x > 0 && i.x < int32_t(sampler->width)-1 ? frac.x : 0;
    int16_t fracy = frac.y;

    U32 pix0 = unaligned_load<U32>(row0);
    U32 pix0n = unaligned_load<U32>(row0 + 4);
    uint32_t pix0x = row0[8];
    U32 pix1 = unaligned_load<U32>(row1);
    U32 pix1n = unaligned_load<U32>(row1 + 4);
    uint32_t pix1x = row1[8];

    {
        auto ab0 = __builtin_convertvector(bit_cast<V16<uint8_t>>(__builtin_shufflevector(pix0, pix0, 0, 1, 1, 2)), V16<int16_t>);
        auto ab1 = __builtin_convertvector(bit_cast<V16<uint8_t>>(__builtin_shufflevector(pix1, pix1, 0, 1, 1, 2)), V16<int16_t>);
        ab0 += ((ab1 - ab0) * fracy) >> 7;

        auto cd0 = __builtin_convertvector(bit_cast<V16<uint8_t>>(__builtin_shufflevector(pix0, pix0n, 2, 3, 3, 4)), V16<int16_t>);
        auto cd1 = __builtin_convertvector(bit_cast<V16<uint8_t>>(__builtin_shufflevector(pix1, pix1n, 2, 3, 3, 4)), V16<int16_t>);
        cd0 += ((cd1 - cd0) * fracy) >> 7;

        auto abcdl = __builtin_shufflevector(ab0, cd0, 0, 1, 2, 3, 4, 5, 6, 7, 16, 17, 18, 19, 20, 21, 22, 23);
        auto abcdh = __builtin_shufflevector(ab0, cd0, 8, 9, 10, 11, 12, 13, 14, 15, 24, 25, 26, 27, 28, 29, 30, 31);
        abcdl += ((abcdh - abcdl) * fracx) >> 7;

        commit_span(buf, pack(WideRGBA8(abcdl)));
    }

    {
        auto ab0 = __builtin_convertvector(bit_cast<V16<uint8_t>>(__builtin_shufflevector(pix0n, pix0n, 0, 1, 1, 2)), V16<int16_t>);
        auto ab1 = __builtin_convertvector(bit_cast<V16<uint8_t>>(__builtin_shufflevector(pix1n, pix1n, 0, 1, 1, 2)), V16<int16_t>);
        ab0 += ((ab1 - ab0) * fracy) >> 7;

        auto cd0 = __builtin_convertvector(bit_cast<V16<uint8_t>>(__builtin_shufflevector(pix0n, U32(pix0x), 2, 3, 3, 4)), V16<int16_t>);
        auto cd1 = __builtin_convertvector(bit_cast<V16<uint8_t>>(__builtin_shufflevector(pix1n, U32(pix1x), 2, 3, 3, 4)), V16<int16_t>);
        cd0 += ((cd1 - cd0) * fracy) >> 7;

        auto abcdl = __builtin_shufflevector(ab0, cd0, 0, 1, 2, 3, 4, 5, 6, 7, 16, 17, 18, 19, 20, 21, 22, 23);
        auto abcdh = __builtin_shufflevector(ab0, cd0, 8, 9, 10, 11, 12, 13, 14, 15, 24, 25, 26, 27, 28, 29, 30, 31);
        abcdl += ((abcdh - abcdl) * fracx) >> 7;

        commit_span(buf + 4, pack(WideRGBA8(abcdl)));
    }
}
