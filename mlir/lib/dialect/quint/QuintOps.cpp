//
// Created by BY210033 on 2023/3/1.
//
#include "dialect/quint/QuintDialect.h"

#include <mlir/IR/OpImplementation.h>
#include <mlir/IR/Builders.h>

namespace mlir
{
    bool isOpaqueTypeWithName(mlir::Type type, std::string dialect,
                              std::string type_name) {
        if (type.isa<mlir::OpaqueType>() && dialect == "quint")
        {
            if (type_name == "Register")
            {
                return true;
            }
        }
        return false;
    }

#define GET_OPS_CLASSES
#include "dialect/quint/QuintOps.cpp.inc"
}
