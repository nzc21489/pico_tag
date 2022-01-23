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

#include "pico_tag_mp3.h"

uint32_t get_bitrate(uint8_t bitrate_bin)
{
    uint32_t bitrate;
    switch (bitrate_bin)
    {
    case 0:
        bitrate = 0;
        break;
    case 1:
        bitrate = 32000;
        break;

    case 2:
        bitrate = 40000;
        break;

    case 3:
        bitrate = 48000;
        break;

    case 4:
        bitrate = 56000;
        break;

    case 5:
        bitrate = 64000;
        break;

    case 6:
        bitrate = 80000;
        break;

    case 7:
        bitrate = 96000;
        break;

    case 8:
        bitrate = 112000;
        break;

    case 9:
        bitrate = 128000;
        break;

    case 10:
        bitrate = 160000;
        break;

    case 11:
        bitrate = 192000;
        break;

    case 12:
        bitrate = 224000;
        break;

    case 13:
        bitrate = 256000;
        break;

    case 14:
        bitrate = 320000;
        break;

    case 15:
        bitrate = 0;
        break;

    default:
        break;
    }

    return bitrate;
}

uint32_t get_sampling_rate(uint8_t sampling_rate_bin)
{
    uint32_t sample_rate;
    switch (sampling_rate_bin)
    {
    case 0:
        sample_rate = 44100;
        break;

    case 1:
        sample_rate = 48000;
        break;

    case 2:
        sample_rate = 32000;
        break;

    case 3:
        sample_rate = 0;
        break;

    default:
        break;
    }

    return sample_rate;
}

uint32_t get_data_size_syncsafe(file &audio_file, uint8_t size)
{
    uint32_t data_size = 0;
    uint8_t read_data;
    for (int i = 0; i < size; i++)
    {
        audio_file.read_file(&read_data);
        data_size += read_data << ((size - 1 - i) * 7);
    }
    return data_size;
    return data_size;
}

void mp3_tag::get_tag(string &mp3_path)
{
    file mp3_file;
    path = mp3_path;
    mp3_file.open_file(mp3_path);

    mp3_file.seek_file(3, mp3_file.current);

    uint8_t id3_version_1 = get_data_size_big_endian(mp3_file, 1);
    uint8_t id3_version_2 = get_data_size_big_endian(mp3_file, 1);

    bool syncsafe = false;

    if (id3_version_2 == 4)
    {
        syncsafe = true;
    }

    mp3_file.seek_file(1, mp3_file.current);

    uint32_t tag_size = get_data_size_syncsafe(mp3_file, 4);

    audio_start = tag_size + 10;

    uint32_t count = 0;

    while (1)
    {
        if (count >= tag_size)
        {
            break;
        }

        string frame_id = get_data_string(mp3_file, 4);
        uint32_t frame_size;
        if (syncsafe)
        {
            frame_size = get_data_size_syncsafe(mp3_file, 4);
        }
        else
        {
            frame_size = get_data_size_big_endian(mp3_file, 4);
        }

        mp3_file.seek_file(3, mp3_file.current);

        count += 10 + frame_size;

        if (frame_id == "TIT2") // title
        {
            title = get_data_string(mp3_file, frame_size - 1);
        }
        else if (frame_id == "TPE1") //artist
        {
            artist = get_data_string(mp3_file, frame_size - 1);
        }
        else if (frame_id == "TALB") //album
        {
            album = get_data_string(mp3_file, frame_size - 1);
        }
        else if (frame_id == "TRCK") // track number
        {
            track_num = get_data_string(mp3_file, frame_size - 1);
        }
        else
        {
            mp3_file.seek_file(frame_size - 1, mp3_file.current);
        }
    }

    mp3_file.close_file();
}

uint32_t mp3_tag::get_duration(string &mp3_path)
{
    file mp3_file;
    path = mp3_path;
    mp3_file.open_file(mp3_path);

    mp3_file.seek_file(audio_start + 2, mp3_file.start);

    uint8_t data = get_data_size_big_endian(mp3_file, 1);

    uint8_t bitrate_bin = (data & 0b11110000) >> 4;
    uint8_t sampling_rate_bin = (data & 0b00000110) >> 1;

    uint8_t padding = data & 0b1;

    bitrate_bin = (data & 0b11110000) >> 4;
    sampling_rate_bin = (data & 0b00000110) >> 1;

    uint32_t bitrate = 0;

    bitrate = get_bitrate(bitrate_bin);
    sampling_rate = get_sampling_rate(sampling_rate_bin);

    if (sampling_rate > 0) // seek and get bitrate again to get true bitrate (VBR is not supported)
    {
        uint32_t seek_size = (uint32_t)(144 * bitrate / sampling_rate) + padding;

        mp3_file.seek_file(seek_size - 3, mp3_file.current);

        mp3_file.seek_file(2, mp3_file.current);

        data = get_data_size_big_endian(mp3_file, 1);

        bitrate_bin = (data & 0b11110000) >> 4;
        sampling_rate_bin = (data & 0b00000110) >> 1;

        bitrate = get_bitrate(bitrate_bin);
        sampling_rate = get_sampling_rate(sampling_rate_bin);
    }

    bits_per_sample = 16;

    audio_end = mp3_file.file_size();

    if (bitrate > 0)
    {
        duration = 8 * (audio_end - audio_start) / bitrate;
    }

    if (sampling_rate > 0)
    {
        frame_size = 144 * bitrate / sampling_rate;
    }

    mp3_file.close_file();
    return duration;
}
