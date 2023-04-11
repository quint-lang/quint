//
// Created by BY210033 on 2023/4/6.
//
#include "quint/ir/expression_printer.h"
#include "quint/ir/transforms.h"

namespace quint::lang {

    std::string block_dim_info(int block_dim) {
        return "block_dim=" +
                (block_dim == 0 ? "adaptive" : std::to_string(block_dim)) + " ";
    }

    class IRPrinter : public IRVisitor {
    private:
        ExpressionPrinter *expr_printer_{nullptr};

    public:
        int current_indent{0};

        std::string *output{nullptr};
        std::stringstream ss;

        explicit IRPrinter(ExpressionPrinter *expr_printer = nullptr,
                           std::string *output = nullptr)
            : expr_printer_(expr_printer), output(output) {
        }

        template<typename... Args>
        void print(std::string f, Args &&...args) {
            print_raw(fmt::format(f, std::forward<Args>(args)...));
        }

        void print_raw(std::string f) {
            for (int i = 0; i < current_indent; ++i) {
                f.insert(0, " ");
            }
            f += "\n";
            if (output) {
                ss << f;
            } else {
                std::cout << f;
            }
        }

        static void run(ExpressionPrinter *expr_printer,
                        IRNode *node,
                        std::string *output) {
            if (node == nullptr) {
                QUINT_WARN("IRPrinter: Printing nullptr.");
                if (output) {
                    *output = std::string();
                }
                return;
            }
            auto p = IRPrinter(expr_printer, output);
            p.print("kernel {{");
            node->accept(&p);
            p.print("}}");
            if (output)
                *output = p.ss.str();
        }

        void visit(Stmt *stmt) override {

        }

        void visit(Block *stmt) override {

        }

    };

    namespace irpass {

        void print(IRNode *root, std::string *output) {
            ExpressionHumanFriendlyPrinter expr_printer;
            return IRPrinter::run(&expr_printer, root, output);
        }
    }

}