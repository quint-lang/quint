//
// Created by BY210033 on 2023/4/11.
//
#include "quint/ir/ir.h"
#include "quint/ir/transforms.h"
#include "quint/ir/statements.h"
#include "quint/ir/visitors.h"
#include "quint/transforms/check_out_of_bound.h"
#include "quint/system/profiler.h"

#include <set>

namespace quint::lang {

    class CheckOutOfBound : public BasicStmtVisitor {
    public:
        using BasicStmtVisitor::visit;
        std::set<int> visited;
        DelayedIRModifier modifier;
        std::string kernel_name;

        explicit CheckOutOfBound(const std::string &kernel_name)
                : kernel_name(kernel_name) {
        }

        bool is_done(Stmt *stmt) {
            return visited.find(stmt->instance_id) != visited.end();
        }

        static bool run(IRNode *node, const CompileConfig &config, const std::string &kernel_name) {
            CheckOutOfBound pass(kernel_name);
            bool modified = false;
            while (true) {
                node->accept(&pass);
                if (pass.modifier.modify_ir()) {
                    modified = true;
                } else {
                    break;
                }
            }
            if (modified)
                irpass::type_check(node, config);
            return modified;
        }
    };

    const PassID CheckOutOfBoundPass::id = "CheckOutOfBoundPass";

    namespace irpass {

        bool check_out_of_bound(IRNode *root, const CompileConfig &config, const CheckOutOfBoundPass::Args &args) {
            QUINT_AUTO_PROF
            return CheckOutOfBound::run(root, config, args.kernel_name);
        }
    }

}