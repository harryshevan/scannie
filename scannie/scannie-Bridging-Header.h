//
//  Use this file to import your target's public headers that you would like to expose to Swift.
//

#ifndef MAGIC_PROCESS_IMAGE_H
#define MAGIC_PROCESS_IMAGE_H

#include <stddef.h> // For size_t

#ifdef __cplusplus
extern "C" {
#endif

unsigned char* MagicProcessImage(const unsigned char* image_data, size_t data_length, int* out_width, int* out_height, int* out_sz);

#ifdef __cplusplus
}
#endif

#endif
