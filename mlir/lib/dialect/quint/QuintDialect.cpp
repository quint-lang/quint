//
// Created by BY210033 on 2023/3/1.
//
#include "dialect/quint/QuintDialect.h"
#include "dialect/quint/QuintOps.h"

using namespace mlir;



void quint::QuintDialect::initialize() {
    addOperations<
#define GET_OP_LIST
#include "dialect/quint/QuintOps.cpp.inc"
    >();
}

