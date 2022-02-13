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

#include "pico_tag_flac.h"
#include "split.h"

void flac_tag::get_tag(string &flac_path)
{
    file flac_file;
    path = flac_path;
    flac_file.open_file(flac_path);
    flac_file.seek_file(4, flac_file.current);
    cover_art_offset = 0;
    cover_art_size = 0;

    uint8_t count = 0;

    while (1)
    {
        uint8_t block_type = get_data_size_big_endian(flac_file, 1);
        bool last_metadata = (block_type & 0b10000000) >> 7;
        block_type &= 0b01111111;
        uint32_t block_size = get_data_size_big_endian(flac_file, 3);

        if (block_type == 4) // vorbis comment
        {
            uint32_t count = get_data_size(flac_file, 4);
            flac_file.seek_file((count + 4), flac_file.current);
            count += 4;
            while (1)
            {
                if (count >= (block_size - 4))
                {
                    break;
                }

                uint32_t filed_size = get_data_size(flac_file, 4);
                if (filed_size > 1000)
                {
                    break;
                }
                else
                {
                    count += 4 + filed_size;
                }
                string filed = get_data_string(flac_file, filed_size);
                vector<string> fileds = split(filed, "=");
                if (fileds[0] == "ALBUM")
                {
                    album = fileds[1];
                }
                else if (fileds[0] == "ARTIST")
                {
                    artist = fileds[1];
                }
                else if (fileds[0] == "TITLE")
                {
                    title = fileds[1];
                }
                else if (fileds[0] == "TRACKNUMBER")
                {
                    track_num = fileds[1];
                }
            }
        }
        else if (block_type == 6) // picture
        {
            cover_art_offset = flac_file.tell_file();
            cover_art_size = block_size;
            flac_file.seek_file(block_size, flac_file.current);
        }
        else
        {
            flac_file.seek_file(block_size, flac_file.current);
        }

        if (last_metadata)
        {
            break;
        }

        count++;
        if (count > 10)
        {
            break;
        }
    }

    flac_file.close_file();
}

uint32_t flac_tag::get_duration(string &flac_path)
{
    file flac_file;
    path = flac_path;
    flac_file.open_file(flac_path);

    flac_file.seek_file(4, flac_file.current);

    while (1)
    {
        uint8_t block_type = get_data_size_big_endian(flac_file, 1);
        bool last_metadata = block_type >> 7;
        block_type &= 0b01111111;
        uint32_t block_size = get_data_size_big_endian(flac_file, 3);

        if (block_type == 0) // stream info
        {
            flac_file.seek_file(2, flac_file.current);
            frame_size = get_data_size_big_endian(flac_file, 2);
            flac_file.seek_file(6, flac_file.current);
            uint64_t meta_data_block = get_data_size_big_endian(flac_file, 4);
            meta_data_block = (meta_data_block << 32) + get_data_size_big_endian(flac_file, 4);
            sampling_rate = (meta_data_block >> 44) & 0xfffff;
            channel = 1 + (meta_data_block >> 41) & 0b111;
            bits_per_sample = 1 + (meta_data_block >> 36) & 0x1f;
            uint64_t total_sample = (meta_data_block & 0xfffffffff);
            duration = (uint32_t)(total_sample / sampling_rate);
            flac_file.seek_file((block_size - 18), flac_file.current);
        }
        else
        {
            flac_file.seek_file(block_size, flac_file.current);
        }

        if (last_metadata)
        {
            audio_start = flac_file.tell_file();
            break;
        }
    }

    flac_file.close_file();

    audio_end = flac_file.file_size();

    return duration;
}