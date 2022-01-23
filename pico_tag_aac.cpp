/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 nzc21489
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "pico_tag_aac.h"
void aac_tag::get_tag(string &aac_path)
{
}

uint32_t aac_tag::get_duration(string &aac_path)
{
    file aac_file;
    path = aac_path;
    aac_file.open_file(aac_path);
    aac_file.seek_file(2, aac_file.current);
    uint32_t data = get_data_size_big_endian(aac_file, 2);
    channel = (data >> 6) & 0b111;
    uint8_t sample_rate_bin = (data >>10) &0b1111;

    switch (sample_rate_bin)
    {
    case 0:
        sampling_rate = 96000;
        break;

    case 1:
        sampling_rate = 88200;
        break;

    case 2:
        sampling_rate = 64000;
        break;

    case 3:
        sampling_rate = 48000;
        break;

    case 4:
        sampling_rate = 44100;
        break;

    case 5:
        sampling_rate = 32000;
        break;

    case 6:
        sampling_rate = 24000;
        break;

    case 7:
        sampling_rate = 22050;
        break;

    case 8:
        sampling_rate = 16000;
        break;

    case 9:
        sampling_rate = 12000;
        break;

    case 10:
        sampling_rate = 11025;
        break;

    case 11:
        sampling_rate = 8000;
        break;

    default:
        break;
    }

    bits_per_sample = 16;
    audio_start = 8;
    audio_end = aac_file.file_size();
    aac_file.close_file();
    return duration;
}
