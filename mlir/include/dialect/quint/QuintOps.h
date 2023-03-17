//
// Created by BY210033 on 2023/2/28.
//

#ifndef QUINT_OPS_H
#define QUINT_OPS_H

#include <mlir/IR/OpDefinition.h>
#include <mlir/IR/Dialect.h>
#include <mlir/IR/BuiltinTypes.h>
#include <mlir/Interfaces/SideEffectInterfaces.h>

namespace mlir
{
#define GET_OP_CLASSES
#include "dialect/quint/QuintOps.h.inc"
}

#endif //QUINT_OPS_H
