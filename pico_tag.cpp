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

#include "pico_tag.h"
#include "pico_tag_wav.h"
#include "pico_tag_opus.h"
#include "pico_tag_flac.h"
#include "pico_tag_mp3.h"
#include "pico_tag_aac.h"
#include "pico_tag_no_valid_tag.h"
pico_tag* pico_tag_get_tag(string &audio_file)
{
    pico_tag *pico_tag;
    string flac = ".flac";
    string opus = ".opus";
    string wav = ".wav";
    string mp3 = ".mp3";
    string aac = ".aac";
    if (audio_file.size() >= flac.size() &&
        audio_file.find(flac, audio_file.size() - flac.size()) != std::string::npos)
    {
        pico_tag = new flac_tag;
    }
    else if (audio_file.size() >= opus.size() &&
             audio_file.find(opus, audio_file.size() - opus.size()) != std::string::npos)
    {
        pico_tag = new opus_tag;
    }
    else if (audio_file.size() >= wav.size() &&
             audio_file.find(wav, audio_file.size() - wav.size()) != std::string::npos)
    {
        pico_tag = new wav_tag;
    }
    else if (audio_file.size() >= mp3.size() &&
             audio_file.find(mp3, audio_file.size() - mp3.size()) != std::string::npos)
    {
        pico_tag = new mp3_tag;
    }
    else if (audio_file.size() >= aac.size() &&
             audio_file.find(aac, audio_file.size() - aac.size()) != std::string::npos)
    {
        pico_tag = new aac_tag;
    }

    else
    {
        pico_tag = new no_valid_tag;
    }

    pico_tag->get_tag(audio_file);
    pico_tag->get_duration(audio_file);

    return pico_tag;
}
