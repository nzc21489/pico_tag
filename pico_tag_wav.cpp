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

#include "pico_tag_wav.h"

void wav_tag::get_tag(string &wav_path)
{
    artist = "";
    title = "";
    album = "";
    track_num = "";
    file wav_file;
    path = wav_path;
    wav_file.open_file(wav_path);
    wav_file.seek_file(12, wav_file.current);

    uint32_t file_size = wav_file.file_size();
    uint32_t wav_tell = 0;

    while (1)
    {
        string id = get_data_string(wav_file, 4);
        wav_tell += 4;
        if (id == "LIST")
        {
            uint32_t list_size = get_data_size(wav_file, 4);
            wav_tell += 4;

            long count = 0;
            string list_data = get_data_string(wav_file, 4);
            count += 4;
            wav_tell += 4;
            if (list_data == "INFO")
            {
                while (1)
                {
                    if ((count >= list_size) || (count < 0))
                    {
                        wav_tell += list_size;
                        break;
                    }

                    list_data = get_data_string(wav_file, 4);

                    count += 4;

                    if (list_data == "IART") // artist
                    {
                        uint32_t size = get_data_size(wav_file, 4);
                        count += 4 + size;
                        artist = get_data_string(wav_file, size);
                        if (get_data_size(wav_file, (uint8_t)1) != 0)
                        {
                            wav_file.seek_file(-1, wav_file.current);
                        }
                        else
                        {
                            count++;
                        }
                    }
                    else if (list_data == "INAM") // title
                    {
                        uint32_t size = get_data_size(wav_file, 4);
                        count += 4 + size;
                        title = get_data_string(wav_file, size);
                        if (get_data_size(wav_file, (uint8_t)1) != 0)
                        {
                            wav_file.seek_file(-1, wav_file.current);
                        }
                        else
                        {
                            count++;
                        }
                    }
                    else if (list_data == "IPRD") //album
                    {
                        uint32_t size = get_data_size(wav_file, 4);
                        count += 4 + size;
                        album = get_data_string(wav_file, size);
                        if (get_data_size(wav_file, (uint8_t)1) != 0)
                        {
                            wav_file.seek_file(-1, wav_file.current);
                        }
                        else
                        {
                            count++;
                        }
                    }
                    else if (list_data == "ITRK") // track number
                    {
                        uint32_t size = get_data_size(wav_file, 4);
                        count += 4 + size;
                        track_num = get_data_string(wav_file, size);
                        if (track_num.find("\0") != std::string::npos)
                            if (get_data_size(wav_file, (uint8_t)1) != 0)
                            {
                                wav_file.seek_file(-1, wav_file.current);
                            }
                            else
                            {
                                count++;
                            }
                    }
                    else
                    {
                        uint32_t size = get_data_size(wav_file, 4);
                        count += 4 + size;
                        wav_file.seek_file(size, wav_file.current);
                        if (get_data_size(wav_file, (uint8_t)1) != 0)
                        {
                            wav_file.seek_file(-1, wav_file.current);
                        }
                        else
                        {
                            count++;
                        }
                    }
                }
            }
        }
        else if (id == "data")
        {

            uint32_t seek_size = get_data_size(wav_file, 4);
            wav_tell += 4;
            audio_start = wav_file.tell_file();
            wav_file.seek_file(seek_size, wav_file.current);
            wav_tell += 4 + seek_size;
            audio_end = wav_file.tell_file();
        }
        else
        {
            uint32_t seek_size = get_data_size(wav_file, 4);
            wav_tell += 4;
            wav_file.seek_file(seek_size, wav_file.current);
            wav_tell += 4 + seek_size;
        }

        if (wav_tell >= file_size)
        {
            break;
        }
    }
    wav_file.close_file();
}

uint32_t wav_tag::get_duration(string &wav_path)
{
    file wav_file;
    path = wav_path;
    wav_file.open_file(wav_path);
    wav_file.seek_file(12, wav_file.current);

    while (1)
    {
        string id = get_data_string(wav_file, 4);
        if (id == "fmt ")
        {
            uint32_t seek_size = get_data_size(wav_file, 4);

            wav_file.seek_file(2, wav_file.current); // format code

            channel = get_data_size(wav_file, 2);
            sampling_rate = get_data_size(wav_file, 4);
            wav_file.seek_file(4, wav_file.current); // byte/sec
            frame_size = get_data_size(wav_file, 2); // block
            bits_per_sample = get_data_size(wav_file, 2); // bit/sample
            wav_file.seek_file((seek_size - 16), wav_file.current);
        }
        else if (id == "data")
        {
            chunk_data = get_data_size(wav_file, 4);
            break;
        }
        else
        {
            uint32_t seek_size = get_data_size(wav_file, 4);
            wav_file.seek_file(seek_size, wav_file.current);
        }
    }

    wav_file.close_file();

    wav_tag::duration = (wav_tag::chunk_data / (wav_tag::channel * (wav_tag::bits_per_sample / 8))) / wav_tag::sampling_rate;
    return duration;
}