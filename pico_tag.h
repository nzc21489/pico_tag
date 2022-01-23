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

#pragma once
#include <string>
#include <string.h>
#include <cstring>
using namespace std;

class pico_tag
{
public:
    uint32_t end_of_file = 0;
    uint32_t final_granule_position = 0;
    volatile uint32_t duration = 0;
    string path = "";
    string artist = "";
    string title = "";
    string album = "";
    string track_num = "";
    uint32_t bits_per_sample = 16;
    uint32_t channel = 2;
    uint32_t sampling_rate = 44100;
    uint32_t chunk_data = 0;
    uint32_t audio_start = 0;
    uint32_t audio_end = 0;
    uint32_t frame_size = 0;

    uint32_t cover_art_offset = 0;
    uint32_t cover_art_size = 0;


    virtual uint32_t get_duration(string &audio_file) = 0;
    virtual void get_tag(string &audio_file) = 0;
};

pico_tag *pico_tag_get_tag(string &audio_file);