// nnkkff.cpp
//
#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <memory>

namespace {
    int nnkkff(std::wstring const& fileName_source, std::wstring const& fileName_target) noexcept;
}

int _tmain(int argc, wchar_t *argv[])
{
    if (argc<3)
    {
        std::cout << "引数：対象ファイル名 出力ファイル名" << std::endl;
        return 0;
    }
    std::wstring source{argv[1]};
    std::wstring target{argv[2]};
    if (source == target)
    {
        std::cout << "対象ファイル名と出力ファイル名は異なっている必要があります" << std::endl;
        return 0;
    }
    auto size = nnkkff(source, target);
    std::cout << "出力文字数:" << size << std::endl;
    return 0;
}

namespace {
    struct file_closer {
        void operator ()(FILE* pF) const  noexcept { if (pF) ::fclose(pF); }
    };

    using fileCloseRAII = std::unique_ptr<FILE, file_closer>;

    auto WideCharToMultiByte_b(unsigned int codepage, std::wstring const& s, std::string& buf)
    {
        auto len = ::WideCharToMultiByte(codepage, 0, s.data(), -1, nullptr, 0, nullptr, nullptr);
        buf.resize(len? len: 0, '\0');
        return ::WideCharToMultiByte(codepage, 0, s.data(), -1, &buf[0], len, nullptr, nullptr) - 1;
    }

    auto MultiByteToWideChar_b(unsigned int codepage, std::string const& s, std::wstring& buf)
    {
        auto len = ::MultiByteToWideChar(codepage, 0, s.data(), -1, nullptr, 0);
        buf.resize(len? len: 0, L'\0');
        return ::MultiByteToWideChar(codepage, 0, s.data(), -1, &buf[0], len) - 1;
    }

    int nnkkff(std::wstring const& fileName_source, std::wstring const& fileName_target) noexcept
    {
        FILE* fp_source = nullptr;
        if ( 0 != ::_wfopen_s(&fp_source, fileName_source.data(), L"rb") )   //
            return 0;
        fileCloseRAII fcr_source{fp_source};
        FILE* fp_target = nullptr;
        if ( 0 != ::_wfopen_s(&fp_target, fileName_target.data(), L"wb") )   //ANSI
            return 0;
        fileCloseRAII fcr_target{fp_target};
        std::string read_buf, out_buf;
        read_buf.resize(16384);
        out_buf.resize(256);
        std::wstring wide_buf;
        wide_buf.resize(256);
        int size{0};
        while (std::fgets(&read_buf[0], static_cast<int>(read_buf.capacity()), fp_source))
        {
            MultiByteToWideChar_b(CP_ACP, read_buf, wide_buf);
            size += WideCharToMultiByte_b(CP_UTF8, wide_buf, out_buf);
            auto b = std::fputs(out_buf.data(), fp_target);
        }
        return size;
    }
}

/*
    _wfopen_s
    https://docs.microsoft.com/ja-jp/cpp/c-runtime-library/reference/fopen-s-wfopen-s?view=msvc-170

    fgetws
    https://docs.microsoft.com/ja-jp/cpp/c-runtime-library/reference/fgets-fgetws?view=msvc-170

    テキスト モードとバイナリ モードの Unicode ストリーム入出力
    https://docs.microsoft.com/ja-jp/cpp/c-runtime-library/unicode-stream-i-o-in-text-and-binary-modes?view=msvc-170

    mbtowc、_mbtowc_l
    https://docs.microsoft.com/ja-jp/cpp/c-runtime-library/reference/mbtowc-mbtowc-l?view=msvc-170

    fputs, fputws
    https://docs.microsoft.com/ja-jp/cpp/c-runtime-library/reference/fputs-fputws?view=msvc-170
*/
