//
// Created by BY210033 on 2023/3/22.
//
#include "common/zip.h"
#include "common/core.h"
#include "common/miniz.h"

namespace quint::zip {

    inline std::string get_file_name_from_whole_path(const std::string &fn) {
        std::size_t start_index = 0;
        if (fn.rfind("/") != std::string::npos) {
            start_index = fn.rfind("/") + 1;
        }
        return std::string(fn.begin() + start_index, fn.end());
    }

    void write(std::string fn, const uint8 *data, std::size_t len) {
        mz_bool status;
        QUINT_ERROR_UNLESS(ends_with(fn, ".tcb.zip"),
                        "Filename must end with .tcb.zip");

        std::string fn_uncompressed = get_file_name_from_whole_path(fn);
        fn_uncompressed = std::string(fn_uncompressed.begin(),
                                      fn_uncompressed.end() - 4);  // remove .zip
        std::remove(fn.c_str());

        auto s_pComment = "Taichi Binary File";

        status = mz_zip_add_mem_to_archive_file_in_place(
                fn.c_str(), fn_uncompressed.c_str(),
                reinterpret_cast<char *>(const_cast<uint8 *>(data)), len, s_pComment,
                (uint16)strlen(s_pComment), MZ_BEST_COMPRESSION);
        if (!status) {
            QUINT_ERROR("mz_zip_add_mem_to_archive_file_in_place failed!\n");
        }
    }

    void write(const std::string &fn, const std::string &data) {
        write(fn, reinterpret_cast<const uint8 *>(data.c_str()), data.size() + 1);
    }

    std::vector<uint8> read(const std::string fn, bool verbose) {
        QUINT_ERROR_UNLESS(ends_with(fn, ".tcb.zip"),
                        "Filename must end with .tcb.zip");

        mz_zip_archive zip_archive;
        mz_zip_archive_file_stat file_stat;
        mz_bool status;

        memset(&zip_archive, 0, sizeof(zip_archive));
        status = mz_zip_reader_init_file(&zip_archive, fn.c_str(), 0);
        if (!status) {
            QUINT_ERROR("mz_zip_reader_init_file() failed!\n");
        }
        if (!mz_zip_reader_file_stat(&zip_archive, 0, &file_stat)) {
            mz_zip_reader_end(&zip_archive);
            QUINT_ERROR("mz_zip_reader_file_stat() failed!\n");
        }

        if (verbose) {
            QUINT_TRACE(
                    "Filename: {}, Comment: {}, Uncompressed size: {}, Compressed size: "
                    "{}, Is Dir: {}\n",
                    file_stat.m_filename, file_stat.m_comment,
                    (uint)file_stat.m_uncomp_size, (uint)file_stat.m_comp_size,
                    mz_zip_reader_is_file_a_directory(&zip_archive, 0));
        }

        // Close the archive, freeing any resources it was using
        mz_zip_reader_end(&zip_archive);

        size_t uncomp_size;
        memset(&zip_archive, 0, sizeof(zip_archive));
        status = mz_zip_reader_init_file(&zip_archive, fn.c_str(), 0);
        if (!status) {
            QUINT_ERROR("mz_zip_reader_init_file() failed!\n");
        }

        std::string fn_uncompressed = get_file_name_from_whole_path(fn);
        fn_uncompressed = std::string(fn_uncompressed.begin(),
                                      fn_uncompressed.end() - 4);  // remove .zip
        auto archive_filename = fn_uncompressed;
        auto p = reinterpret_cast<const uint8 *>(mz_zip_reader_extract_file_to_heap(
                &zip_archive, archive_filename.c_str(), &uncomp_size, 0));

        if (!p) {
            mz_zip_reader_end(&zip_archive);
            QUINT_ERROR("mz_zip_reader_extract_file_to_heap() failed!");
        }

        if (verbose) {
            QUINT_TRACE("Successfully extracted file {}, size {}", archive_filename,
                     (uint)uncomp_size);
            QUINT_TRACE("File data: {}", (const char *)p);
        }

        std::vector<uint8> ret(p, p + file_stat.m_uncomp_size);
        mz_free((void *)p);
        return ret;
    }


}