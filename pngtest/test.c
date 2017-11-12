#include <png.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int open_png(const char*, png_structp*, png_infop*, png_uint_32*, png_uint_32*, png_byte*);

int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Need argument\n");
		exit(EXIT_FAILURE);
	}
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_uint_32 width, height;
	png_byte channels;
	unsigned char* p_row;
	unsigned int y;
	int num_text;
	png_textp text;
	int xxfps, xxframes;
	int* fps = &xxfps;
	int* frames = &xxframes;
	int result = open_png(argv[1], &png_ptr, &info_ptr, &width, &height, &channels);

	if (result < 0) {
		fprintf(stderr, "Result %d\n", result);
		return result;
	}
	if (info_ptr == NULL) {
		fprintf(stderr, "info_ptr is null\n");
	} else {
		fprintf(stdout, "info_ptr is not null\n");
	}

	*frames = 1;
    *fps = 20;
	fprintf(stdout, "num_text %d\n", num_text);
    if (result = png_get_text(png_ptr, info_ptr, &text, &num_text)) {
        for (int i = 0; i < num_text; ++i) {
			printf("Text key %s, text %s\n", text[i].key, text[i].text);
            if (text[i].key && strcmp(text[i].key, "Frames") == 0 && text[i].text) {
                *frames = atoi(text[i].text);
            } else if (text[i].key && strcmp(text[i].key, "FPS") == 0 && text[i].text) {
                *fps = atoi(text[i].text);
            }
        }
        printf("  found frames = %d\n", *frames);
        printf("  found fps = %d\n", *fps);
	}
	printf("Total text chunks: %d\n", result);

	exit(EXIT_SUCCESS);
}

/*
 * Taken from https://github.com/LineageOS/android_bootable_recovery/blob/lineage-15.0/minui/resources.cpp
 */
/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
int open_png(const char* name, png_structp* png_ptr, png_infop* info_ptr,
                    png_uint_32* width, png_uint_32* height, png_byte* channels) {
    //char resPath[256];
    unsigned char header[8];
    int result = 0;
    int color_type, bit_depth;
    size_t bytesRead;

    //snprintf(resPath, sizeof(resPath)-1, "/res/images/%s.png", name);
    //resPath[sizeof(resPath)-1] = '\0';
	const char* resPath = name;
    FILE* fp = fopen(resPath, "rbe");
    if (fp == NULL) {
        result = -1;
		perror("fopen");
        goto exit;
    }

    bytesRead = fread(header, 1, sizeof(header), fp);
    if (bytesRead != sizeof(header)) {
        result = -2;
		perror("fread");
        goto exit;
    }

    if (png_sig_cmp(header, 0, sizeof(header))) {
        result = -3;
		perror("png_sig_cmp");
        goto exit;
    }

    *png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!*png_ptr) {
        result = -4;
		perror("png_create_read_struct");
        goto exit;
    }

    *info_ptr = png_create_info_struct(*png_ptr);
    if (!*info_ptr) {
        result = -5;
		perror("png_create_info_struct");
        goto exit;
    }

    if (setjmp(png_jmpbuf(*png_ptr))) {
        result = -6;
		perror("setjmp");
        goto exit;
    }

    png_init_io(*png_ptr, fp);
    png_set_sig_bytes(*png_ptr, sizeof(header));
    png_read_info(*png_ptr, *info_ptr);

    png_get_IHDR(*png_ptr, *info_ptr, width, height, &bit_depth,
            &color_type, NULL, NULL, NULL);

    *channels = png_get_channels(*png_ptr, *info_ptr);

    if (bit_depth == 8 && *channels == 3 && color_type == PNG_COLOR_TYPE_RGB) {
        // 8-bit RGB images: great, nothing to do.
    } else if (bit_depth <= 8 && *channels == 1 && color_type == PNG_COLOR_TYPE_GRAY) {
        // 1-, 2-, 4-, or 8-bit gray images: expand to 8-bit gray.
        png_set_expand_gray_1_2_4_to_8(*png_ptr);
    } else if (bit_depth <= 8 && *channels == 1 && color_type == PNG_COLOR_TYPE_PALETTE) {
        // paletted images: expand to 8-bit RGB.  Note that we DON'T
        // currently expand the tRNS chunk (if any) to an alpha
        // channel, because minui doesn't support alpha channels in
        // general.
        png_set_palette_to_rgb(*png_ptr);
        *channels = 3;
    } else {
        fprintf(stderr, "minui doesn't support PNG depth %d channels %d color_type %d\n",
                bit_depth, *channels, color_type);
        result = -7;
        goto exit;
    }

    return result;

  exit:
    if (result < 0) {
        png_destroy_read_struct(png_ptr, info_ptr, NULL);
    }
    if (fp != NULL) {
        fclose(fp);
    }

    return result;
}
