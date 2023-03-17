#ifndef QUINT_DIALECT
#define QUINT_DIALECT

include "mlir/IR/OpBase.td"

def Quint_Dialect : Dialect {
    let name = "quint";
    let summary = "A MLIR dialect for quantum assembly languages.";
    let description = [{
        This dialect provides operations secific to quantum assembly languages.
    }];
    let cppNamespace = "quint";
}

class Quint_Op<string mnemonic, list<OpTrait> traits = []> :
        Op<Quint_Dialect, mnemonic, traits>;

#endif // QUINT_DIALECT