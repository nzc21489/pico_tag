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

#include "pico_tag_file.h"

void file::open_file(string &path)
{
    fil = new FIL;
    FRESULT fr;
    fr = f_open(fil, path.c_str(), FA_READ);
}

void file::close_file()
{
    f_close(fil);
    delete fil;
}

uint8_t file::read_file(uint8_t *buff)
{
    UINT br;
    FRESULT fr;
    fr = f_read(fil, buff, sizeof(char), &br);
    return (uint8_t)br;
}

void file::seek_file(long offset, position pos)
{
    switch (pos)
    {
    case start:
        f_lseek(fil, offset);
        break;

    case current:
        f_lseek(fil, file::tell_file() + offset);
        break;

    case end:
        if (-offset < f_size(fil))
        {
            f_lseek(fil, f_size(fil) + offset);
        }else{
            f_lseek(fil, 0);
        }
        break;

    default:
        break;
    }
}

long file::tell_file()
{
    return f_tell(fil);
}

uint32_t file::file_size(){
    return f_size(fil);
}