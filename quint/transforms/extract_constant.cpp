//
// Created by BY210033 on 2023/4/11.
//
#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/ir/transforms.h"
#include "quint/ir/visitors.h"
#include "quint/program/compile_config.h"
#include "quint/system/profiler.h"

namespace quint::lang {

    class ExtractConstant : public BasicStmtVisitor {
    private:
        Block *top_level_;
        DelayedIRModifier modifier_;

    public:
        using BasicStmtVisitor::visit;

        explicit ExtractConstant(IRNode *node) : top_level_(nullptr) {
            if (node->is<Block>())
                top_level_ = node->as<Block>();
        }

        static bool run(IRNode *node) {
            ExtractConstant extractor(node);
            bool ir_modified = false;
            while (true) {
                node->accept(&extractor);
                if (extractor.modifier_.modify_ir()) {
                    ir_modified = true;
                } else {
                    break;
                }
            }
            return ir_modified;
        }
    };

    namespace irpass {
        bool extract_constant(IRNode *root, const CompileConfig &config) {
            QUINT_AUTO_PROF
            if (config.advanced_optimization) {
                return ExtractConstant::run(root);
            } else {
                return false;
            }
        }
    }

}
