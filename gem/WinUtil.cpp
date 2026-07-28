#include <Windows.h>
#include <ShlObj.h>

#include <filesystem>
#include <iostream>
#include <stdexcept>
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