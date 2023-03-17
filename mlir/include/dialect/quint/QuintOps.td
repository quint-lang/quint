#ifndef QUINT_OPS
#define QUINT_OPS

include "QuintDialect.td"
include "mlir/Interfaces/SideEffectInterfaces.td"

def RegisterType : OpaqueType<"quantum", "Register", "opaque register type">;

def AddRegisterOp : Quint_Op<"addreg", []> {
    let arguments = (ins StrAttr:$name, AnyI64Attr:$type, AnyI64Attr:$size);
    let results = (outs RegisterType:$reg);
    let printer = [{  auto op = *this;
      p << "quint.addreg(" << op.type() << ", " << op.size() << ") { name = " << op.name() << " }";
      p << " : "  << op.reg().getType();  }];
}

def InstOp : Quint_Op<"inst", []> {
    let arguments = (
        ins StrAttr: $name,
        Variadic<RegisterType>: $regs,
        BoolAttr: $free,
        BoolAttr: $dagger
    );
    let results = (outs Variadic<RegisterType>:$res);

    let printer = [{ auto op = *this;
            p << "quint." << op.dagger() ? "~" : "" << op.free() ? "!" : "" << op.name() << "(" << op.regs() << ")";
            p << " : " << op.res().getType();}];
}

def ConstantInstOp : Quint_Op<"constant_inst", [AttrSizedOperandSegments]> {
    let arguments = (ins StrAttr:$name, Variadic<RegisterType>:$targets, Variadic<AnyInteger>:$params, BoolAttr:$dagger);
    let results = (outs Variadic<RegisterType>:$res);

    let printer = [{ auto op = *this;
       p << "quint." << op.dagger() ? "~c_" : "c_" << op.name() << "(" << op.getOperands() << ") :" << op.res().getType();
    }];
}

def ControlOp : Quint_Op<"control", [AttrSizedOperandSegments]> {
    let arguments = (
        ins StrAttr: $name,
        Variadic<RegisterType>: $regs,
        Variadic<RegisterType>: $ctrl,
        BoolAttr: $free,
        BoolAttr: $dagger
    );
    let results = (outs Variadic<RegisterType>:$res);

    let printer = [{ auto op = *this;
            p << "quint." << op.dagger() ? "~" : "" << op.free() ? "!" : "" << op.name() << "(" << op.regs() << ")";
            p << " ctrl_by [" << op.ctrl() << "] : " << op.res().getType();}];
}

def ControlConstOp : Quint_Op<"control_const", [AttrSizedOperandSegments]> {
        let arguments = (
            ins StrAttr: $name,
            Variadic<RegisterType>: $regs,
            Variadic<AnyInteger>:$params,
            Variadic<RegisterType>: $ctrl,
            BoolAttr: $dagger
        );
        let results = (outs Variadic<RegisterType>:$res);

        let printer = [{ auto op = *this;
                p << "quint." << op.dagger() ? "~c_" : "c_" << op.name() << "(" << op.regs() << ", " << op.params() << ")";
                p << " ctrl_by [" << op.ctrl() << "] : " << op.res().getType();}];
}

def PushOp : Quint_Op<"push", []> {
    let arguments = (ins RegisterType:$target, StrAttr:$garbage_name);
    let results = (outs);

    let printer = [{ auto op = *this;
            p << "quint.push(" << op.target() << ", " << op.garbage_name << ")";
    }];
}

def PopOp : Quint_Op<"pop", []> {
    let arguments = (ins RegisterType:$target);
    let results = (outs);

    let printer = [{ auto op = *this;
            p << "quint.pop(" << op.target() << ")";
    }];
}

def RemoveRegisterOp : Quint_Op<"remove", []> {
    let arguments = (ins RegisterType: $reg);
    let results = (outs);

    let printer = [{ auto op = *this;
            p << "quint.remove(" << op.reg() << ")";
    }];
}

#endif // QUINT_OPS