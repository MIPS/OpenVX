#include "ago_internal.h"
#include "mips_internal.h"

DECL_ALIGN(16) unsigned char dataColorConvert[16 * 26] ATTR_ALIGN(16) = {
	  1,   3,   5,   7,   9,  11,  13,  15, 255, 255, 255, 255, 255, 255, 255, 255,		// UYVY to IYUV - Y; UV12 to IUV - V (lower); NV21 to IYUV - U; UYVY to NV12 - Y; YUYV to NV12 - UV
	  0,   4,   8,  12, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,		// UYVY to IYUV - U
	  2,   6,  10,  14, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,		// UYVY to IYUV - V
	  0,   2,   4,   6,   8,  10,  12,  14, 255, 255, 255, 255, 255, 255, 255, 255,		// YUYV to IYUV - Y; UV12 to IUV - U (lower); NV21 to IYUV - V; UYVY to NV12 - UV; YUYV to NV12 - Y
	  1,   5,   9,  13, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,		// YUYV to IYUV - U
	  3,   7,  11,  15, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,		// YUYV to IYUV - V
	  0,   0,   2,   2,   4,   4,   6,   6,   8,   8,  10,  10,  12,  12,  14,  14,		// UV12 to UV - U; NV21 to YUV4 - V
	  1,   1,   3,   3,   5,   5,   7,   7,   9,   9,  11,  11,  13,  13,  15,  15,		// VV12 to UV - V; NV21 to YUV4 - U
	  0,   1,   2,   4,   5,   6,   8,   9,  10,  12,  13,  14, 255, 255, 255, 255,		// RGBX to RGB - First 16 bytes of RGBX to first 16 bytes of RGB
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,   0,   1,   2,   4,		// RGBX to RGB - Second 16 bytes of RGBX to first 16 bytes of RGB
	  5,   6,   8,   9,  10,  12,  13,  14, 255, 255, 255, 255, 255, 255, 255, 255,		// RGBX to RGB - Second 16 bytes of RGBX to second 16 bytes of RGB
	255, 255, 255, 255, 255, 255, 255, 255,   0,   1,   2,   4,   5,   6,   8,   9,		// RGBX to RGB - Third 16 bytes of RGBX to second 16 bytes of RGB
	 10,  12,  13,  14, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,		// RGBX to RGB - Third 16 bytes of RGBX to third 16 bytes of RGB
	255, 255, 255, 255,   0,   1,   2,   4,   5,   6,   8,   9,  10,  12,  13,  14,		// RGBX to RGB - Fourth 16 bytes of RGBX to third 16 bytes of RGB
	  0,   1,   2, 255,   3,   4,   5, 255,   6,   7,   8, 255,   9,  10,  11, 255,		// RGB to RGBX - First 16 bytes of RGB to first 16 bytes of RGBX
	 12,  13,  14, 255,  15, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,		// RGB to RGBX - First 16 bytes of RGB to second 16 bytes of RGBX
	255, 255, 255, 255, 255,   0,   1, 255,   2,   3,   4, 255,   5,   6,   7, 255,		// RGB to RGBX - Second 16 bytes of RGB to second 16 bytes of RGBX
	  8,   9,  10, 255,  11,  12,  13, 255,  14,  15, 255, 255, 255, 255, 255, 255,		// RGB to RGBX - Second 16 bytes of RGB to third 16 bytes of RGBX
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255,   0, 255,   1,   2,   3, 255,		// RGB to RGBX - Third 16 bytes of RGB to third 16 bytes of RGBX
	  4,   5,   6, 255,   7,   8,   9, 255,  10,  11,  12, 255,  13,  14,  15, 255,		// RGB to RGBX - Third 16 bytes of RGB to fourth 16 bytes of RGBX
	  0,   0,   0, 255,   0,   0,   0, 255,   0,   0,   0, 255,   0,   0,   0, 255,		// RGB to RGBX - Mask to fill in 255 for X positions
	  0,   3,   6,   9,  12,  15, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,		// RGB to single plane extraction
	  2,   5,   8,  11,  14, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,		// RGB to single plane extraction
	  1,   4,   7,  10,  13, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,		// RGB to single plane extraction
	255, 255, 255, 255, 255, 255, 255, 255,   1,   3,   5,   7,   9,  11,  13,  15,		// UV12 to IUV - V (upper)
	255, 255, 255, 255, 255, 255, 255, 255,   0,   2,   4,   6,   8,  10,  12,  14		// UV12 to IUV - U (upper)
};

int HafCpu_ColorConvert_Y_RGB
    (
        vx_uint32     dstWidth,
        vx_uint32     dstHeight,
        vx_uint8    * pDstYImage,
        vx_uint32     dstYImageStrideInBytes,
        vx_uint8    * pSrcImage,
        vx_uint32     srcImageStrideInBytes
    )
{
#if ENABLE_MSA
	v16i8 *tbl = (v16i8 *) dataColorConvert;
	v16u8 R, G, B;
	v4u32 pixels0, pixels1, pixels2;
	v4f32 temp, Y;
	v4u32 temp0_u, temp1_u;

    v16i8 mask1 = __builtin_msa_ld_b(tbl + 21, 0);
    v16i8 mask2 = __builtin_msa_ld_b(tbl + 22, 0);
    v16i8 mask3 = __builtin_msa_ld_b(tbl + 23, 0);

	float wR[4] = {0.2126f, 0.2126f, 0.2126f, 0.2126f};
	float wG[4] = {0.7152f, 0.7152f, 0.7152f, 0.7152f};
	float wB[4] = {0.0722f, 0.0722f, 0.0722f, 0.0722f};

	v4f32 weights_R = (v4f32) __builtin_msa_ld_w(&wR, 0);
	v4f32 weights_G = (v4f32) __builtin_msa_ld_w(&wG, 0);
	v4f32 weights_B = (v4f32) __builtin_msa_ld_w(&wB, 0);

	v16i8 zero = __builtin_msa_ldi_b(0);

	int alignedWidth = dstWidth & ~15;
	int postfixWidth = (int) dstWidth - alignedWidth;
#endif
	for (int height = 0; height < (int) dstHeight; height++)
	{
		vx_uint8 *pLocalSrc = pSrcImage;
		vx_uint8 *pLocalDst = pDstYImage;
#if ENABLE_MSA
		for (int width = 0; width < (alignedWidth >> 4); width++)
		{
			pixels0 = (v4u32) __builtin_msa_ld_b(pLocalSrc, 0);
			pixels1 = (v4u32) __builtin_msa_ld_b(pLocalSrc + 16, 0);
			pixels2 = (v4u32) __builtin_msa_ld_b(pLocalSrc + 32, 0);

			// 0 0 0 0 0 0 0 0 0 0 R5 R4 R3 R2 R1 R0
			R = (v16u8) __builtin_msa_vshf_b(mask1, (v16i8) pixels0, (v16i8) pixels0);
			// 0 0 0 0 0 0 0 0 0 0 0 G4 G3 G2 G1 G0
			G = (v16u8) __builtin_msa_vshf_b(mask3, (v16i8) pixels0, (v16i8) pixels0);
			// 0 0 0 0 0 0 0 0 0 0 0 B4 B3 B2 B1 B0
			B = (v16u8) __builtin_msa_vshf_b(mask2, (v16i8) pixels0, (v16i8) pixels0);

			// 0 0 0 0 0 0 0 0 0 0 0 0 R10 R9 R8 R7 R6
			pixels0 = (v4u32) __builtin_msa_vshf_b(mask2, (v16i8) pixels1, (v16i8) pixels1);
			pixels0 = (v4u32) __builtin_msa_sldi_b((v16i8) pixels0, (v16i8) pixels0, 10);
			// 0 0 0 0 0 R10 R9 R8 R7 R6 R5 R4 R3 R2 R1 R0
			R = __builtin_msa_or_v(R, (v16u8) pixels0);
			// 0 0 0 0 0 0 0 0 0 0 G10 G9 G8 G7 G6 G5
			pixels0 = (v4u32) __builtin_msa_vshf_b(mask1, (v16i8) pixels1, (v16i8) pixels1);
			pixels0 = (v4u32) __builtin_msa_sldi_b((v16i8) pixels0, (v16i8) pixels0, 11);
			// 0 0 0 0 0 G10 G9 G8 G7 G6 G5 G4 G3 G2 G1 G0
			G = __builtin_msa_or_v(G, (v16u8) pixels0);
			// 0 0 0 0 0 0 0 0 0 0 0 B9 B8 B7 B6 B5
			pixels0 = (v4u32) __builtin_msa_vshf_b(mask3, (v16i8) pixels1, (v16i8) pixels1);
			pixels0 = (v4u32) __builtin_msa_sldi_b((v16i8) pixels0, (v16i8) pixels0, 11);
			// 0 0 0 0 0 0 B9 B8 B7 B6 B5 B4 B3 B2 B1 B0
			B = __builtin_msa_or_v(B, (v16u8) pixels0);

			// 0 0 0 0 0 0 0 0 0 0 0 R15 R14 R13 R12 R11
			pixels0 = (v4u32) __builtin_msa_vshf_b(mask3, (v16i8) pixels2, (v16i8) pixels2);
			pixels0 = (v4u32) __builtin_msa_sldi_b((v16i8) pixels0, (v16i8) pixels0, 5);
			// R15 R14 R13 R12 R11 R10 R9 R8 R7 R6 R5 R4 R3 R2 R1 R0
			R = __builtin_msa_or_v(R, (v16u8) pixels0);
			// 0 0 0 0 0 0 0 0 0 0 0 G15 G14 G13 G12 G11
			pixels0 = (v4u32) __builtin_msa_vshf_b(mask2, (v16i8) pixels2, (v16i8) pixels2);
			pixels0 = (v4u32) __builtin_msa_sldi_b((v16i8) pixels0, (v16i8) pixels0, 5);
			// G15 G14 G13 G12 G11 G10 G9 G8 G7 G6 G5 G4 G3 G2 G1 G0
			G = __builtin_msa_or_v(G, (v16u8) pixels0);
			// 0 0 0 0 0 0 0 0 0 0 B15 B14 B13 B12 B11 B10
			pixels0 = (v4u32) __builtin_msa_vshf_b(mask1, (v16i8) pixels2, (v16i8) pixels2);
			pixels0 = (v4u32) __builtin_msa_sldi_b((v16i8) pixels0, (v16i8) pixels0, 6);
			// B15 B14 B13 B12 B11 B10 B9 B8 B7 B6 B5 B4 B3 B2 B1 B0
			B = __builtin_msa_or_v(B, (v16u8) pixels0);

			// For pixels 0..3
			pixels2 = (v4u32) __builtin_msa_ilvr_b(zero, (v16i8) R);
			pixels2 = (v4u32) __builtin_msa_ilvr_h((v8i16) zero, (v8i16) pixels2);
			// R0..R3
			temp = __builtin_msa_ffint_u_w(pixels2);
			Y = __builtin_msa_fmul_w(temp, weights_R);
			pixels2 = (v4u32) __builtin_msa_ilvr_b(zero, (v16i8) G);
			pixels2 = (v4u32) __builtin_msa_ilvr_h((v8i16) zero, (v8i16) pixels2);
			temp = __builtin_msa_ffint_u_w(pixels2);
			// G0..G3
			temp = __builtin_msa_fmul_w(temp, weights_G);
			Y = __builtin_msa_fadd_w(Y, temp);
			pixels2 = (v4u32) __builtin_msa_ilvr_b(zero, (v16i8) B);
            pixels2 = (v4u32) __builtin_msa_ilvr_h((v8i16) zero, (v8i16) pixels2);
			temp = __builtin_msa_ffint_u_w(pixels2);
			// B0..B3
			temp = __builtin_msa_fmul_w(temp, weights_B);
			Y = __builtin_msa_fadd_w(Y, temp);
			pixels0 = (v4u32) __builtin_msa_ftrunc_u_w(Y);

			// For pixels 4..7
			R = (v16u8) __builtin_msa_sldi_b((v16i8) R, (v16i8) R, 4);
			G = (v16u8) __builtin_msa_sldi_b((v16i8) G, (v16i8) G, 4);
			B = (v16u8) __builtin_msa_sldi_b((v16i8) B, (v16i8) B, 4);

			pixels2 = (v4u32) __builtin_msa_ilvr_b(zero, (v16i8) R);
            pixels2 = (v4u32) __builtin_msa_ilvr_h((v8i16) zero, (v8i16) pixels2);
			// R4..R7
			temp = __builtin_msa_ffint_u_w(pixels2);
			Y = __builtin_msa_fmul_w(temp, weights_R);
			pixels2 = (v4u32) __builtin_msa_ilvr_b(zero, (v16i8) G);
            pixels2 = (v4u32) __builtin_msa_ilvr_h((v8i16) zero, (v8i16) pixels2);
			temp = __builtin_msa_ffint_u_w(pixels2);
			// G4..G7
			temp = __builtin_msa_fmul_w(temp, weights_G);
			Y = __builtin_msa_fadd_w(Y, temp);
			pixels2 = (v4u32) __builtin_msa_ilvr_b(zero, (v16i8) B);
            pixels2 = (v4u32) __builtin_msa_ilvr_h((v8i16) zero, (v8i16) pixels2);
			temp = __builtin_msa_ffint_u_w(pixels2);
			// B4..B7
			temp = __builtin_msa_fmul_w(temp, weights_B);
			Y = __builtin_msa_fadd_w(Y, temp);
			pixels1 = __builtin_msa_ftrunc_u_w(Y);
			temp0_u = __builtin_msa_sat_u_w(pixels0, 15);
            temp1_u = __builtin_msa_sat_u_w(pixels1, 15);
            pixels0 = (v4u32) __builtin_msa_pckev_h((v8i16) temp1_u, (v8i16) temp0_u);

			// For pixels 8..11
			R = (v16u8) __builtin_msa_sldi_b((v16i8) R, (v16i8) R, 4);
			G = (v16u8) __builtin_msa_sldi_b((v16i8) G, (v16i8) G, 4);
			B = (v16u8) __builtin_msa_sldi_b((v16i8) B, (v16i8) B, 4);

			pixels2 = (v4u32) __builtin_msa_ilvr_b(zero, (v16i8) R);
            pixels2 = (v4u32) __builtin_msa_ilvr_h((v8i16) zero, (v8i16) pixels2);
			// R8..R11
			temp = __builtin_msa_ffint_u_w(pixels2);
			Y = __builtin_msa_fmul_w(temp, weights_R);
			pixels2 = (v4u32) __builtin_msa_ilvr_b(zero, (v16i8) G);
            pixels2 = (v4u32) __builtin_msa_ilvr_h((v8i16) zero, (v8i16) pixels2);
			temp = __builtin_msa_ffint_u_w(pixels2);
			// G8..G11
			temp = __builtin_msa_fmul_w(temp, weights_G);
			Y = __builtin_msa_fadd_w(Y, temp);
			pixels2 = (v4u32) __builtin_msa_ilvr_b(zero, (v16i8) B);
            pixels2 = (v4u32) __builtin_msa_ilvr_h((v8i16) zero, (v8i16) pixels2);
			temp = __builtin_msa_ffint_u_w(pixels2);
			// B8..B11
			temp = __builtin_msa_fmul_w(temp, weights_B);
			Y = __builtin_msa_fadd_w(Y, temp);
			pixels1 = __builtin_msa_ftrunc_u_w(Y);

			// For pixels 12..15
			R = (v16u8) __builtin_msa_sldi_b((v16i8) R, (v16i8) R, 4);
			G = (v16u8) __builtin_msa_sldi_b((v16i8) G, (v16i8) G, 4);
			B = (v16u8) __builtin_msa_sldi_b((v16i8) B, (v16i8) B, 4);

			pixels2 = (v4u32) __builtin_msa_ilvr_b(zero, (v16i8) R);
            pixels2 = (v4u32) __builtin_msa_ilvr_h((v8i16) zero, (v8i16) pixels2);
			// R12..R15
			temp = __builtin_msa_ffint_u_w(pixels2);
			Y = __builtin_msa_fmul_w(temp, weights_R);
			pixels2 = (v4u32) __builtin_msa_ilvr_b(zero, (v16i8) G);
            pixels2 = (v4u32) __builtin_msa_ilvr_h((v8i16) zero, (v8i16) pixels2);
			temp = __builtin_msa_ffint_u_w(pixels2);
			// G12..G15
			temp = __builtin_msa_fmul_w(temp, weights_G);
			Y = __builtin_msa_fadd_w(Y, temp);
			pixels2 = (v4u32) __builtin_msa_ilvr_b(zero, (v16i8) B);
            pixels2 = (v4u32) __builtin_msa_ilvr_h((v8i16) zero, (v8i16) pixels2);
			temp = __builtin_msa_ffint_u_w(pixels2);
			// B12..B15
			temp = __builtin_msa_fmul_w(temp, weights_B);
			Y = __builtin_msa_fadd_w(Y, temp);
			pixels2 = __builtin_msa_ftrunc_u_w(Y);
			temp0_u = __builtin_msa_sat_u_w(pixels1, 15);
            temp1_u = __builtin_msa_sat_u_w(pixels2, 15);
            pixels1 = (v4u32) __builtin_msa_pckev_h((v8i16) temp1_u, (v8i16) temp0_u);

			temp0_u = (v4u32) __builtin_msa_sat_u_h((v8u16) pixels0, 7);
            temp1_u = (v4u32) __builtin_msa_sat_u_h((v8u16) pixels1, 7);
			pixels0 = (v4u32) __builtin_msa_pckev_b((v16i8) temp1_u, (v16i8) temp0_u);
			__builtin_msa_st_b((v16i8) pixels0, (void*) pLocalDst, 0);

			pLocalSrc += 48;
			pLocalDst += 16;
		}

		for (int width = 0; width < postfixWidth; width++)
		{
			float R = (float) *pLocalSrc++;
			float G = (float) *pLocalSrc++;
			float B = (float) *pLocalSrc++;

			*pLocalDst++ = (vx_uint8) ((R * 0.2126f) + (G * 0.7152f) + (B * 0.0722f));
		}
#else
		for (int width = 0; width < dstWidth; width++)
		{
			float R = (float) *pLocalSrc++;
			float G = (float) *pLocalSrc++;
			float B = (float) *pLocalSrc++;

			*pLocalDst++ = (vx_uint8) ((R * 0.2126f) + (G * 0.7152f) + (B * 0.0722f));
		}
#endif
		pSrcImage += srcImageStrideInBytes;
		pDstYImage += dstYImageStrideInBytes;
	}
	return AGO_SUCCESS;
}
