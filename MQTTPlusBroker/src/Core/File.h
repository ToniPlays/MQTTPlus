#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace MQTTPlus
{
    enum class CopyOptions 
    {
        None = (int)std::filesystem::copy_options::none,
        OverwriteExisting = (int)std::filesystem::copy_options::overwrite_existing,
        SkipExisting = (int)std::filesystem::copy_options::skip_existing,
        UpdateExisting = (int)std::filesystem::copy_options::update_existing,
        Recursive = (int)std::filesystem::copy_options::recursive,
        DirectoriesOnly = (int)std::filesystem::copy_options::directories_only
    };

    class File {
    public:
        static bool Exists(const std::filesystem::path& path);
        static bool DirectoryExists(const std::filesystem::path& dir);
        static bool IsDirectory(const std::filesystem::path& path);
        static std::filesystem::path AppendToName(const std::filesystem::path& path, const std::string& append);

        static std::vector<char> ReadBinaryFileChar(const std::filesystem::path& path);
        static bool ReadBinaryFileUint32(const std::filesystem::path& path, std::vector<uint32_t>& buffer);
        static std::string ReadFile(const std::filesystem::path& file);

        static bool WriteBinaryFile(const std::filesystem::path& path, std::vector<uint32_t> data);
        static bool WriteBinaryFile(const std::filesystem::path& path, void* data, size_t size);
        static bool CopyFileTo(const std::filesystem::path& source, const std::filesystem::path& dest);
        static bool IsNewerThan(const std::filesystem::path& file, const std::filesystem::path& compareTo);

        static bool WriteFile(const std::filesystem::path& file, const std::string& content = "");
        static bool Move(const std::filesystem::path& src, const std::filesystem::path& dst);


        static std::filesystem::path GetFileAbsolutePath(const std::filesystem::path& file);
        static std::filesystem::path GetDirectoryOf(const std::filesystem::path& file);
        static std::string GetName(const std::filesystem::path& file);
        static std::string GetNameNoExt(const std::filesystem::path& file);
        static std::string GetPathNoExt(const std::filesystem::path& file);
        static std::string GetFileExtension(const std::filesystem::path& file);

        static std::vector<std::filesystem::path> GetAllInDirectory(const std::filesystem::path& path, bool recursive = false);

        static bool CreateDir(const std::filesystem::path& dir);
        static void Copy(const std::filesystem::path& source, const std::filesystem::path& dest, CopyOptions options);
    };
}