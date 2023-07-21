#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "../include/jpeg.h"

#define JFIF_TN_PPM "jfif_thumbnail.ppm"

void parse_comment(FILE *fp) {
   char *com;
   uint16_t com_size, i;

   com_size = getc(fp) << 8;
   com_size |= getc(fp);

   printf("Comment size = %d\n", com_size);

   com = alloca(sizeof(*com) * com_size);

   for (i = 0; i < com_size; ++i) {
      *(com + i) = getc(fp);
      if (*(com + i) == 0x0A) {
         *(com + i) = '\0';
         break;
      }
   }

   printf("Comment = %s\n", com);
}

void output_jfif_tn_debug(uint16_t xthumbnail, uint16_t ythumbnail, FILE *fp) {
   size_t tn_size, i;
   FILE *ppm = NULL;
   pixel_rgb888_t *tn_pixmap = NULL;

   tn_size = xthumbnail * ythumbnail;
   if (tn_size == 0) return;

   tn_pixmap = alloca(sizeof(*tn_pixmap) * tn_size);
   if (tn_pixmap == NULL) {
      perror("alloca()");
      exit(EXIT_FAILURE);
   }

   for (i = 0; i < tn_size; ++i, ++tn_pixmap) {
      fread(tn_pixmap, sizeof(*tn_pixmap), 1, fp);
   }

   ppm = fopen(JFIF_TN_PPM, "wb");
   if (ppm == NULL) {
      perror("fopen()");
      exit(EXIT_FAILURE);
   }

   /* Write the thumbnail as PPM */
   fprintf(ppm, "P6\n%hud %hud\n255\n", xthumbnail, ythumbnail);
   fwrite(tn_pixmap, sizeof(*tn_pixmap), tn_size, ppm);

   fclose(ppm);
}

/* Convert big-endian -> little-endian */
__attribute__((always_inline)) inline
uint16_t swap_endianness_us(uint16_t val) {
   return (val << 8) | ((uint8_t)(val >> 8));
}

void parse_exif_header(FILE *fp) {


   putchar('\n');
}

void parse_jfif_header(FILE *fp) {
   char ident[5];
   uint16_t seg_len, xdensity, ydensity;
   jfif_t jfif_hdr = { 0 };
   jfxx_t jfxx_hdr = { 0 };

   seg_len = getc(fp) << 8;
   seg_len |= getc(fp);

   fgets(ident, sizeof(ident), fp);

   /* JFIF */
   if (strcmp(ident, "JFIF") == 0) {
      fseek(fp, -(sizeof(seg_len) + sizeof(ident) - 1), SEEK_CUR);
      fread(&jfif_hdr, sizeof(jfif_hdr), 1, fp);

      xdensity = swap_endianness_us(jfif_hdr.xdensity);
      ydensity = swap_endianness_us(jfif_hdr.ydensity);

      printf("segment length = %d\n", seg_len);
      printf("identifier = %s\n", jfif_hdr.ident);
      printf("JFIF version = %d.%02d\n", (uint8_t)(jfif_hdr.version), (jfif_hdr.version >> 8));

      switch (jfif_hdr.density_units) {
         case NONE: {
            printf("density units = None\n");
            break;
         }
         case PPI: {
            printf("density units = Pixels per inch\n");
            break;
         }
         case PPC: {
            printf("density units = Pixels per centimeter\n");
            break;
         }
         default: {
            fprintf(stderr, "density units = Unknown\n");
         }
      }

      printf("xdensity = %d\n", xdensity);
      assert(xdensity != 0);
      printf("ydensity = %d\n", ydensity);
      assert(ydensity != 0);
      printf("xthumbnail = %d\n", jfif_hdr.xthumbnail);
      printf("ythumbnail = %d\n", jfif_hdr.ythumbnail);

#ifdef DEBUG
      output_jfif_tn_debug(tn_size, fp);
#endif
   }
   /* JFXX */
   else if (strcmp(ident, "JFXX") == 0) {
      printf("JFXX\n");
   } else {
      fprintf(stderr, "Unrecognized APP0 segment\n");
      exit(EXIT_FAILURE);
   }

   putchar('\n');
}

void print_usage(const char *prog_name) {
   printf(
            "Usage: %s\n"
            "",
            (prog_name + 2)
         );
}

int process_args(int argc, char **argv) {
   int c;
   const char *optstr = "ht:";

   while ((c = getopt(argc, argv, optstr)) != -1) {
      switch (c) {
         case 'h': {
            print_usage(argv[0]);
            exit(EXIT_SUCCESS);
         }
         case 't': {
            printf("Selected file type: %s\n", optarg);
            break;
         }
         case '?': {
            fprintf(stderr, "Unrecognized option %s\n", optarg);
            print_usage(argv[0]);
            exit(EXIT_FAILURE);
         }
      }
   }

   return 0;
}

int main(int argc, char **argv) {
   uint16_t c;
   FILE *fp = NULL;

   if (argc < 2) {
      print_usage(argv[0]);
      exit(EXIT_FAILURE);
   }

   /* TODO: implement */
   process_args(argc, argv);

   fp = fopen(argv[1], "rb");
   if (fp == NULL) {
      perror("fopen()");
      exit(EXIT_FAILURE);
   }

   /* TODO: This will not be true for PNG */
   /* First two bytes of JPEG should be FF D8 (start of image) */
   c = getc(fp) << 8;
   c |= getc(fp);
   assert(c == SOI);

   while (true) {
      /* TODO: Put functionality of loop in another function */
      c = getc(fp) << 8;
      c |= getc(fp);

      switch (c) {
         case SOF0: {
            break;
         }
         case SOF2: {
            break;
         }
         case DHT: {
            break;
         }
         case DQT: {
            break;
         }
         case DRI: {
            break;
         }
         case SOS: {
            break;
         }
         case COM: {
            parse_comment(fp);
            break;
         }
         case EOI: {
            //cleanup();
            break;
         }
         default: {
            /* Check for RSTn or APPn */
            if ((c & RST) == RST) {
               /* Lowest nibble must be between 0..7 */
               uint8_t ln = c & 0x000F;
               assert(ln >= '0' && ln <= '7');

               switch (ln) {
                  default: {
                     fprintf(stderr, "RST%d has not been implemented\n", ln);
                     exit(EXIT_FAILURE);
                  }
               }
            } else if ((c & APP) == APP) {
               uint8_t ln = c & 0x000F;

               switch (ln) {
                  case 0: {
                     parse_jfif_header(fp);
                     break;
                  }
                  case 1: {
                     parse_exif_header(fp);
                     break;
                  }
                  default: {
                     fprintf(stderr, "APP%d has not been implemented\n", ln);
                     exit(EXIT_FAILURE);
                  }
               }
            } else {
               fprintf(stderr, "Received an unknown JPEG marker: %2x\n", c);
               exit(EXIT_FAILURE);
            }

            break;
         }
      }
   }

   fclose(fp);

   return 0;
}
