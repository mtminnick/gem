#include <Windows.h>
#include <ShlObj.h>

#include <filesystem>
#include <iostream>
#include <stdexcept>
/*
 * Copyright (c) 2022, Michael Minnick
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 */

#include <string>

#include "WinUtil.h"

static std::filesystem::path getDocumentsDirectory()
{
    PWSTR path = nullptr;

    const HRESULT result = SHGetKnownFolderPath(
        FOLDERID_Documents,
        KF_FLAG_DEFAULT,
        nullptr,
        &path);

    if (FAILED(result))
    {
        return std::filesystem::current_path();
    }
    else
    {
        std::filesystem::path documents(path);
        CoTaskMemFree(path);
        return documents;
    }
}

std::filesystem::path getDefaultDirectory()
{
    std::filesystem::path documents = getDocumentsDirectory();
    std::filesystem::path defaultDir = documents / L"gem";
    try {
        if (!std::filesystem::exists(defaultDir)) {
            std::filesystem::create_directories(defaultDir);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error creating default directory: " << e.what() << "\n";
        return documents;
    }
    return defaultDir;
}