//
// Created by BY210033 on 2023/3/30.
//

#ifndef QUINT_FILE_SEQUENCE_WRITER_H
#define QUINT_FILE_SEQUENCE_WRITER_H

#include "quint/util/lang_util.h"
#ifdef QUINT_WITH_LLVM
#include "quint/runtime/llvm/llvm_fwd.h"
#endif

namespace quint::lang {
    class IRNode;
}

namespace quint {

    class FileSequenceWriter {
    public:
        FileSequenceWriter(std::string filename_template, std::string file_type);

#ifdef QUINT_WITH_LLVM
        std::string write(llvm::Module *module);
#endif
        std::string write(lang::IRNode *ir);

        std::string write(const std::string &str);

    private:
        int counter_;
        std::string filename_template_;
        std::string file_type_;

        std::pair<std::ofstream, std::string> create_new_file();
    };

}

#endif //QUINT_FILE_SEQUENCE_WRITER_H
