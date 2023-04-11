//
// Created by BY210033 on 2023/4/6.
//

#ifndef QUINT_PASS_H
#define QUINT_PASS_H

#include "quint/ir/ir.h"
#include "quint/program/compile_config.h"

#include <unordered_map>
#include <typeindex>
#include <utility>

namespace quint::lang {

    using PassID = std::string;

    class Program;

    class AnalysisManager;

    struct AnalysisResultConcept {
        virtual ~AnalysisResultConcept() = default;
    };

    template<typename ResultT>
    struct AnalysisResultModel : public AnalysisResultConcept {
        explicit AnalysisResultModel(ResultT result) : result(std::move(result)) {
        }
        ResultT result;
    };

    class Pass {
    public:
        static const PassID id;

        enum class Status {
            Failure = 0x00,
            SuccessWithChange = 0x10,
            SuccessWithoutChange = 0x11,
        };

        virtual ~Pass() = default;
    };

    class AnalysisManager {
    public:
        template<typename PassT>
        typename PassT::Result *get_pass_result() {
            auto result = result_.find(PassT::id);
            if (result == result_.end()) {
                return nullptr;
            }
            using ResultModelT = AnalysisResultModel<typename PassT::Result>;
            return &(static_cast<ResultModelT *>(result->second.get())->result);
        }

        template<typename PassT>
        void put_pass_result(typename PassT::Result &&result) {
            using ResultModelT = AnalysisResultModel<typename PassT::Result>;
            result_[PassT::id] = std::make_unique<ResultModelT>(std::move(result));
        }

    private:
        std::unordered_map<PassID, std::unique_ptr<AnalysisResultConcept>> result_;
    };

}

#endif //QUINT_PASS_H
