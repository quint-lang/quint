add_mlir_dialect_library(MLIRQuint
        QuintDialect.cpp
        QuintOps.cpp

        ADDITIONAL_HEADER_DIRS
        ${PROJECT_SOURCE_DIR}/include/dialect/quint

        DEPENDS
        MLIRQuintOpsIncGen

        LINK_LIBS PUBLIC
        MLIRIR
        )
