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

#include "pico_tag_opus.h"
#include "split.h"

void opus_tag::get_tag(string &opus_path)
{
    file opus_file;
    path = opus_path;
    opus_file.open_file(opus_path);
    uint8_t read_data_8byte[8] = {0};
    uint8_t tag_count = 0;
    for (int i = 0; i < 1000; i++)
    {
        uint8_t read_data;
        uint8_t byte_read;
        byte_read = opus_file.read_file(&read_data);
        for (int j = 0; j < 7; j++)
        {
            read_data_8byte[j] = read_data_8byte[j + 1];
        }
        read_data_8byte[7] = read_data;

        if (bytes_to_string(read_data_8byte, 8) == "OpusTags")
        {
            opus_file.seek_file((get_data_size(opus_file, 4) + 4), opus_file.current);
            uint32_t count = 0;
            while (1)
            {
                string filed = get_data_string(opus_file, 4);
                if (filed == "OggS")
                {
                    audio_start = opus_file.tell_file() - 4;
                    break;
                }
                else
                {
                    //------ work around for padding before OggS ---------
                    opus_file.seek_file(-3, opus_file.current);
                    filed = get_data_string(opus_file, 4);
                    if (filed == "OggS")
                    {
                        audio_start = opus_file.tell_file() - 4;
                        break;
                    }
                    opus_file.seek_file(-1, opus_file.current);
                    //----------------------------------------------------

                    opus_file.seek_file(-4, opus_file.current);
                    uint32_t filed_size = get_data_size(opus_file, 4);
                    if (filed_size > 1000)
                    {
                        opus_file.seek_file(filed_size, opus_file.current);
                        audio_start = opus_file.tell_file();
                        break;
                    }
                    else
                    {
                        string filed = get_data_string(opus_file, filed_size);
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
                    count += 4 + filed_size;
                }

                if (count > 10000000)
                {
                    break;
                }
            }
            i = 1000;
            break;
        }
    }
    audio_end = opus_file.file_size();
    opus_file.close_file();
}

uint32_t opus_tag::get_duration(string &opus_path)
{
    file opus_file;
    path = opus_path;
    opus_file.open_file(opus_path);
    opus_file.seek_file(-64000, file::end);
    uint8_t read_data_4byte[4] = {0, 0, 0, 0};
    for (int i = 0; i < 64000; i++)
    {
        uint8_t read_data;
        uint8_t byte_read;
        byte_read = opus_file.read_file(&read_data);
        for (int j = 0; j < 3; j++)
        {
            read_data_4byte[j] = read_data_4byte[j + 1];
        }
        read_data_4byte[3] = read_data;

        if (bytes_to_string(read_data_4byte, 4) == "OggS")
        {
            final_granule_position = 0;
            // skip 2 bytes
            opus_file.read_file(&read_data);
            i++;
            opus_file.read_file(&read_data);
            i++;
            for (int k = 0; k < 8; k++)
            {
                opus_file.read_file(&read_data);
                i++;
                final_granule_position += (read_data << (8 * k));
            }
        }
    }
    opus_file.close_file();
    duration = final_granule_position / 48000;
    frame_size = 4096;
    bits_per_sample = 16;
    sampling_rate = 48000;
    return duration;
}
