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

#include "pico_tag_util.h"

string bytes_to_string(uint8_t *bytes, uint32_t size)
{
    string string_bytes;
    char char_bytes[300] = {0};
    for (int i = 0; i < size; i++)
    {
        char_bytes[i] = (char)bytes[i];
    }
    string_bytes = char_bytes;
    return string_bytes;
}

uint32_t get_data_size(file &audio_file, uint8_t size)
{
    uint32_t data_size = 0;
    uint8_t read_data;
    for (int i = 0; i < size; i++)
    {
        audio_file.read_file(&read_data);
        data_size += read_data << (i * 8);
    }
    return data_size;
}

uint32_t get_data_size_big_endian(file &audio_file, uint8_t size)
{
    uint32_t data_size = 0;
    uint8_t read_data;
    for (int i = 0; i < size; i++)
    {
        audio_file.read_file(&read_data);
        data_size += read_data << ((size - 1 - i) * 8);
    }
    return data_size;
}

string get_data_string(file &audio_file, uint32_t size)
{
    uint8_t data[size];
    uint8_t read_data;
    for (int j = 0; j < size; j++)
    {
        audio_file.read_file(&read_data);
        data[j] = read_data;
    }
    return bytes_to_string(data, size);
}