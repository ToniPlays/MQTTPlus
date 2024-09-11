
#include "File.h"

#include <fstream>
#include <iostream>

namespace MQTTPlus {

    bool File::Exists(const std::filesystem::path& path)
    {
        return std::filesystem::exists(path);
    }
    bool File::DirectoryExists(const std::filesystem::path& dir)
    {
        return std::filesystem::exists(dir);
    }
    bool File::IsDirectory(const std::filesystem::path& path)
    {
        return std::filesystem::is_directory(path);
    }
    std::filesystem::path File::AppendToName(const std::filesystem::path& path, const std::string& append)
    {
        auto parentPath = GetDirectoryOf(path);
        auto name = GetNameNoExt(path);
        auto extension = IsDirectory(path) ? "" : "." + GetFileExtension(path);

        return parentPath / std::filesystem::path(name + append + extension);
    }

    bool File::WriteBinaryFile(const std::filesystem::path& path, std::vector<uint32_t> data)
    {
        std::ofstream out(path, std::ios::out | std::ios::binary);

        if (out.is_open())
        {
            out.write((char*)data.data(), data.size() * sizeof(uint32_t));
            out.flush();
            out.close();
            return true;
        }
        return false;
    }
    bool File::WriteBinaryFile(const std::filesystem::path& path, void* data, size_t size)
    {
        std::ofstream out(path, std::ios::out | std::ios::binary);

        if (out.is_open())
        {
            out.write((char*)data, size);
            out.flush();
            out.close();
            return true;
        }
        return false;
    }
    bool File::CopyFileTo(const std::filesystem::path& source, const std::filesystem::path& dest)
    {
        std::filesystem::path destFolder = GetDirectoryOf(dest);
        if (!DirectoryExists(destFolder)) {
            File::CreateDir(destFolder);
        }
        return std::filesystem::copy_file(source, dest);
    }
    bool File::IsNewerThan(const std::filesystem::path& file, const std::filesystem::path& compareTo)
    {
        if (!Exists(file)) return false;
        if (!Exists(compareTo)) return true;
        auto fTime = std::filesystem::last_write_time(file);
        auto sTime = std::filesystem::last_write_time(compareTo);
        return fTime > sTime;
    }

    bool File::WriteFile(const std::filesystem::path& file, const std::string& content)
    {
        std::ofstream f(file);
        f << content;
        f.close();
        return true;
    }

    bool File::Move(const std::filesystem::path& src, const std::filesystem::path& dst) {
        if (!File::Exists(dst))
            return false;

        std::filesystem::rename(std::filesystem::canonical(src), std::filesystem::canonical(dst));
        return true;
    }

    std::string File::ReadFile(const std::filesystem::path& file)
    {
        std::string result;
        std::ifstream ifs(File::GetFileAbsolutePath(file), std::ios::in | std::ios::binary | std::ios::ate);

        size_t size = ifs.tellg();
        result.resize(size);
        ifs.seekg(0, std::ios::beg);
        ifs.read(&result[0], size);

        return result;
    }
    std::vector<char> File::ReadBinaryFileChar(const std::filesystem::path& path)
    {
        std::ifstream stream(path, std::ios::binary | std::ios::ate);

        auto end = stream.tellg();
        stream.seekg(0, std::ios::beg);

        auto size = std::size_t(end - stream.tellg());
        if (size == 0) return {};

        std::vector<char> buffer(size);
        if (!stream.read((char*)buffer.data(), buffer.size())) {}
        return buffer;
    }
    
    bool File::ReadBinaryFileUint32(const std::filesystem::path& path, std::vector<uint32_t>& buffer)
    {
        std::ifstream stream(path, std::ios::binary | std::ios::ate);

        if (!stream.is_open())
            return false;

        auto size = stream.tellg();
        stream.seekg(0, std::ios::beg);

        if (size == 0)
            return false;

        buffer.resize(size / sizeof(uint32_t));

        if (!stream.read((char*)buffer.data(), size))
        {
        }

        return true;
    }

    std::filesystem::path File::GetFileAbsolutePath(const std::filesystem::path& file)
    {
        return std::filesystem::absolute(file);
    }
    std::filesystem::path File::GetDirectoryOf(const std::filesystem::path& file)
    {
        return file.parent_path();
    }
    std::string File::GetName(const std::filesystem::path& file)
    {
        std::filesystem::path path(file);
        return path.filename().string();
    }
    std::string File::GetNameNoExt(const std::filesystem::path& file)
    {
        std::string name = GetName(file);
        return name.substr(0, name.find_last_of('.'));
    }
    std::string File::GetPathNoExt(const std::filesystem::path& file)
    {
        return file.string().substr(0, file.string().find_last_of('.'));
    }
    std::string File::GetFileExtension(const std::filesystem::path& file) {
        return file.string().substr(file.string().find_last_of('.') + 1);
    }

    std::vector<std::filesystem::path> File::GetAllInDirectory(const std::filesystem::path& path, bool recursive)
    {
        std::vector<std::filesystem::path> result;
        if (recursive)
        {
            for (const auto& iter : std::filesystem::recursive_directory_iterator(path))
                result.emplace_back(iter);
            return result;
        }

        for (const auto& iter : std::filesystem::directory_iterator(path))
            result.emplace_back(iter);
        return result;
    }

    bool File::CreateDir(const std::filesystem::path& dir)
    {
        return std::filesystem::create_directories(dir);
    }
    void File::Copy(const std::filesystem::path& source, const std::filesystem::path& dest, CopyOptions options)
    {
        std::filesystem::copy(source, dest, (std::filesystem::copy_options)options);
    }
}