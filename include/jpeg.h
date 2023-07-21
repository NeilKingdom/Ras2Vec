/**
 * JPEG (Joint Photographic Experts Group)
 *
 * About:
 *    JPEG is a standard for compressing images using a lossy compression algorithm.
 * The compression algorithm uses many clever tactics to fool people into thinking
 * that the image has remained untouched. The primary tactic that it relies on is
 * the fact that the human eye is better at discerning luminance (brightness) than
 * it is at discerning between chroma (hue/color).
 *
 * Encoding/Decoding:
 *    The process of compressing an image using JPEG's lossy compression algorithm is
 * referred to as encoding the image. The encoded image is what is stored in the
 * actual .jpeg or .jpg file. When an application such as an image viewer needs to
 * display the image, it must first decode it. We are only interested in decoding the
 * image since we expect the user to provide a JPEG file that has already been encoded.
 *
 * The Encoding Process:
 *    The decoding process is, for the most part, just the reverse of the encoding
 * process. For this reason, it is beneficial if we understand the steps used to
 * encode a JPEG image. I will outline the general steps, then we can go into detail
 * on each individual step:
 *
 * 1. Transformation: Given an image that is stored as RGB values (the color space
 *    used is irrelevant), we first convert each pixel to YCbCr.
 * 2. Down Sampling: The Cb (chroma blue) and Cr (chroma red) are reduced in terms of
 *    spatial resolution.
 * 3. Block Splitting: Each channel is split into blocks. The size of these blocks
 *    depends upon what type of down sampling we used.
 * 4. Discrete cosine transform: Each block of each component (Y, Cb, Cr) is converted
 *    into a frequency-domain representation using a normailized discrete cosine
 *    transform (DCT).
 * 5. Quantization: Each block, now transformed by the DCT is divided by a quantization
 *    coefficient (QC) and then rounded down to an integer.
 * 6. Encoding: The quantized blocks are encoded using a combination of RLE and
 *    Huffman encoding.
 * 7. Adding a Header: The image is prepended with a header.
 *
 * Transformation:
 *
 * Down Sampling:
 *    There are two primary methods of down sampling: chroma downsampling and chroma
 * subsampling. Down sampling means that we average the pixel values in a group,
 * whereas subsampling usually indicates that we use the top left pixel the group as
 * the output color for that group. There are three common ratios used for both
 * subsampling and downsampling. 4:4:4 indicates no downsampling. 4:2:2 indicates
 * a reduction by a factor of 2 in the horizontal direction. 4:2:0 indicates a
 * reduction by a factor of 2 in both the horizontal and vertical directions. Below
 * is a visual representation of an input pixel group being chroma downsampled:
 *
 * +--+--+
 * |12|05|              +----+
 * +--+--+     ===>     |13.5|
 * |06|31|              +----+
 * +--+--+
 *
 *
 */
#ifndef JPEG_H
#define JPEG_H

#include <stdint.h>

/* JPEG Markers */

#define SOI  0xFFD8 /* Start of image */
#define SOF0 0xFFC0 /* Start of frame (baseline DCT) */
#define SOF2 0xFFC2 /* Start of frame (progressive DCT) */
#define DHT  0xFFC4 /* Huffman tables */
#define DQT  0xFFDB /* Quantization tables */
#define DRI  0xFFDD /* Define restart interval */
#define SOS  0xFFDA /* Start of scan */
#define RST  0xFFD0 /* Restart. Lowest 3 bits can be a value from 0..7 */
#define APP  0xFFE0 /* Application specific. Lowest nibble can be anything from 0-F */
#define COM  0xFFFE /* Comment */
#define EOI  0xFFD9 /* End of image */

/* Encoded JPEG image */

typedef enum Format {
   JIF,
   JFIF,
   JFXX,
   EXIF
} format_t;

/* APP0 JFIF marker segment */

#pragma pack(1)
typedef struct Pixel_RGB888 {
   uint8_t red;
   uint8_t green;
   uint8_t blue;
} pixel_rgb888_t;

typedef enum Density {
   NONE = 0x00,
   PPI,           /* Pixels per inch */
   PPC            /* Pixels per centimeter */
} density_t;

#pragma pack(1)
typedef struct Jfif {
   uint16_t    len;
   uint8_t     ident[5];
   uint16_t    version;
   uint8_t     density_units;
   uint16_t    xdensity;
   uint16_t    ydensity;
   uint8_t     xthumbnail;
   uint8_t     ythumbnail;
} jfif_t;

/* APP0 JFXX extension marker */

/* Thumbnail format */
typedef enum TnFmt {
   JPEG = 0x10,   /* JPEG format */
   BPP1 = 0x11,   /* 1 byte per pixel (bitmap format) */
   BPP3 = 0x13    /* 3 bytes per pixel (RGB format) */
} tn_fmt_t;

#pragma pack(1)
typedef struct Jfxx {
   uint16_t    marker;
   uint16_t    len;
   uint8_t     ident[5];
   uint8_t     tn_fmt;
} jfxx_t;

typedef struct Jpeg {
   const char *fname;
   format_t    fmt;
   uint16_t    width;
   uint16_t    height;
} jpeg_t;

/* APP1 Exif extension marker */

#pragma pack(1)
typedef struct Exif {
   uint16_t    len;
   uint8_t     ident[6];
   /* TODO: Finish */
} exif_t;

/* Entropy decoding functions */

/* Dequantization functions */

/* Reverse discrete cosine transform */


#endif /* JPEG_H */
