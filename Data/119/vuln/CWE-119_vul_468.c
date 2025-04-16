void VisitNode(Node* node, Truncation truncation,
                 SimplifiedLowering* lowering) {
    switch (node->opcode()) {
      //------------------------------------------------------------------
      // Common operators.
      //------------------------------------------------------------------
      case IrOpcode::kStart:
      case IrOpcode::kDead:
        return VisitLeaf(node, NodeOutputInfo::None());
      case IrOpcode::kParameter: {
        // TODO(titzer): use representation from linkage.
        Type* type = NodeProperties::GetType(node);
        ProcessInput(node, 0, UseInfo::None());
        SetOutput(node, NodeOutputInfo(MachineRepresentation::kTagged, type));
        return;
      }
      case IrOpcode::kInt32Constant:
        return VisitLeaf(node, NodeOutputInfo::Int32());
      case IrOpcode::kInt64Constant:
        return VisitLeaf(node, NodeOutputInfo::Int64());
      case IrOpcode::kFloat32Constant:
        return VisitLeaf(node, NodeOutputInfo::Float32());
      case IrOpcode::kFloat64Constant:
        return VisitLeaf(node, NodeOutputInfo::Float64());
      case IrOpcode::kExternalConstant:
        return VisitLeaf(node, NodeOutputInfo::Pointer());
      case IrOpcode::kNumberConstant:
        return VisitLeaf(node, NodeOutputInfo::NumberTagged());
      case IrOpcode::kHeapConstant:
        return VisitLeaf(node, NodeOutputInfo::AnyTagged());

      case IrOpcode::kDeoptimizeIf:
      case IrOpcode::kDeoptimizeUnless:
        ProcessInput(node, 0, UseInfo::Bool());
        ProcessInput(node, 1, UseInfo::AnyTagged());
        ProcessRemainingInputs(node, 2);
        break;
      case IrOpcode::kBranch:
        ProcessInput(node, 0, UseInfo::Bool());
        EnqueueInput(node, NodeProperties::FirstControlIndex(node));
        break;
      case IrOpcode::kSwitch:
        ProcessInput(node, 0, UseInfo::TruncatingWord32());
        EnqueueInput(node, NodeProperties::FirstControlIndex(node));
        break;
      case IrOpcode::kSelect:
        return VisitSelect(node, truncation, lowering);
      case IrOpcode::kPhi:
        return VisitPhi(node, truncation, lowering);
      case IrOpcode::kCall:
        return VisitCall(node, lowering);

//------------------------------------------------------------------
// JavaScript operators.
//------------------------------------------------------------------
// For now, we assume that all JS operators were too complex to lower
// to Simplified and that they will always require tagged value inputs
// and produce tagged value outputs.
// TODO(turbofan): it might be possible to lower some JSOperators here,
// but that responsibility really lies in the typed lowering phase.
#define DEFINE_JS_CASE(x) case IrOpcode::k##x:
        JS_OP_LIST(DEFINE_JS_CASE)
#undef DEFINE_JS_CASE
        VisitInputs(node);
        return SetOutput(node, NodeOutputInfo::AnyTagged());

      //------------------------------------------------------------------
      // Simplified operators.
      //------------------------------------------------------------------
      case IrOpcode::kBooleanNot: {
        if (lower()) {
          NodeInfo* input_info = GetInfo(node->InputAt(0));
          if (input_info->representation() == MachineRepresentation::kBit) {
            // BooleanNot(x: kRepBit) => Word32Equal(x, #0)
            node->AppendInput(jsgraph_->zone(), jsgraph_->Int32Constant(0));
            NodeProperties::ChangeOp(node, lowering->machine()->Word32Equal());
          } else {
            // BooleanNot(x: kRepTagged) => WordEqual(x, #false)
            node->AppendInput(jsgraph_->zone(), jsgraph_->FalseConstant());
            NodeProperties::ChangeOp(node, lowering->machine()->WordEqual());
          }
        } else {
          // No input representation requirement; adapt during lowering.
          ProcessInput(node, 0, UseInfo::AnyTruncatingToBool());
          SetOutput(node, NodeOutputInfo::Bool());
        }
        break;
      }
      case IrOpcode::kBooleanToNumber: {
        if (lower()) {
          NodeInfo* input_info = GetInfo(node->InputAt(0));
          if (input_info->representation() == MachineRepresentation::kBit) {
            // BooleanToNumber(x: kRepBit) => x
            DeferReplacement(node, node->InputAt(0));
          } else {
            // BooleanToNumber(x: kRepTagged) => WordEqual(x, #true)
            node->AppendInput(jsgraph_->zone(), jsgraph_->TrueConstant());
            NodeProperties::ChangeOp(node, lowering->machine()->WordEqual());
          }
        } else {
          // No input representation requirement; adapt during lowering.
          ProcessInput(node, 0, UseInfo::AnyTruncatingToBool());
          SetOutput(node, NodeOutputInfo::Int32());
        }
        break;
      }
      case IrOpcode::kNumberEqual:
      case IrOpcode::kNumberLessThan:
      case IrOpcode::kNumberLessThanOrEqual: {
        // Number comparisons reduce to integer comparisons for integer inputs.
        if (BothInputsAreSigned32(node)) {
          // => signed Int32Cmp
          VisitInt32Cmp(node);
          if (lower()) NodeProperties::ChangeOp(node, Int32Op(node));
        } else if (BothInputsAreUnsigned32(node)) {
          // => unsigned Int32Cmp
          VisitUint32Cmp(node);
          if (lower()) NodeProperties::ChangeOp(node, Uint32Op(node));
        } else {
          // => Float64Cmp
          VisitFloat64Cmp(node);
          if (lower()) NodeProperties::ChangeOp(node, Float64Op(node));
        }
        break;
      }
      case IrOpcode::kNumberAdd:
      case IrOpcode::kNumberSubtract: {
        if (BothInputsAre(node, Type::Signed32()) &&
            NodeProperties::GetType(node)->Is(Type::Signed32())) {
          // int32 + int32 = int32
          // => signed Int32Add/Sub
          VisitInt32Binop(node);
          if (lower()) NodeProperties::ChangeOp(node, Int32Op(node));
        } else if (BothInputsAre(node, type_cache_.kAdditiveSafeInteger) &&
                   truncation.TruncatesToWord32()) {
          // safe-int + safe-int = x (truncated to int32)
          // => signed Int32Add/Sub (truncated)
          VisitWord32TruncatingBinop(node);
          if (lower()) NodeProperties::ChangeOp(node, Int32Op(node));
        } else {
          // => Float64Add/Sub
          VisitFloat64Binop(node);
          if (lower()) NodeProperties::ChangeOp(node, Float64Op(node));
        }
        break;
      }
      case IrOpcode::kNumberMultiply: {
        if (BothInputsAreSigned32(node)) {
          if (NodeProperties::GetType(node)->Is(Type::Signed32())) {
            // Multiply reduces to Int32Mul if the inputs and the output
            // are integers.
            VisitInt32Binop(node);
            if (lower()) NodeProperties::ChangeOp(node, Int32Op(node));
            break;
          }
          if (truncation.TruncatesToWord32() &&
              NodeProperties::GetType(node)->Is(type_cache_.kSafeInteger)) {
            // Multiply reduces to Int32Mul if the inputs are integers,
            // the uses are truncating and the result is in the safe
            // integer range.
            VisitWord32TruncatingBinop(node);
            if (lower()) NodeProperties::ChangeOp(node, Int32Op(node));
            break;
          }
        }
        // => Float64Mul
        VisitFloat64Binop(node);
        if (lower()) NodeProperties::ChangeOp(node, Float64Op(node));
        break;
      }
      case IrOpcode::kNumberDivide: {
        if (BothInputsAreSigned32(node)) {
          if (NodeProperties::GetType(node)->Is(Type::Signed32())) {
          // => signed Int32Div
          VisitInt32Binop(node);
          if (lower()) DeferReplacement(node, lowering->Int32Div(node));
          break;
          }
          if (truncation.TruncatesToWord32()) {
            // => signed Int32Div
            VisitWord32TruncatingBinop(node);
            if (lower()) DeferReplacement(node, lowering->Int32Div(node));
            break;
          }
        }
        if (BothInputsAreUnsigned32(node) && truncation.TruncatesToWord32()) {
          // => unsigned Uint32Div
          VisitWord32TruncatingBinop(node);
          if (lower()) DeferReplacement(node, lowering->Uint32Div(node));
          break;
        }
        // => Float64Div
        VisitFloat64Binop(node);
        if (lower()) NodeProperties::ChangeOp(node, Float64Op(node));
        break;
      }
      case IrOpcode::kNumberModulus: {
        if (BothInputsAreSigned32(node)) {
          if (NodeProperties::GetType(node)->Is(Type::Signed32())) {
            // => signed Int32Mod
            VisitInt32Binop(node);
            if (lower()) DeferReplacement(node, lowering->Int32Mod(node));
            break;
          }
          if (truncation.TruncatesToWord32()) {
            // => signed Int32Mod
            VisitWord32TruncatingBinop(node);
            if (lower()) DeferReplacement(node, lowering->Int32Mod(node));
            break;
          }
        }
        if (BothInputsAreUnsigned32(node) && truncation.TruncatesToWord32()) {
          // => unsigned Uint32Mod
          VisitWord32TruncatingBinop(node);
          if (lower()) DeferReplacement(node, lowering->Uint32Mod(node));
          break;
        }
        // => Float64Mod
        VisitFloat64Binop(node);
        if (lower()) NodeProperties::ChangeOp(node, Float64Op(node));
        break;
      }
      case IrOpcode::kNumberBitwiseOr:
      case IrOpcode::kNumberBitwiseXor:
      case IrOpcode::kNumberBitwiseAnd: {
        VisitInt32Binop(node);
        if (lower()) NodeProperties::ChangeOp(node, Int32Op(node));
        break;
      }
      case IrOpcode::kNumberShiftLeft: {
        Type* rhs_type = GetInfo(node->InputAt(1))->output_type();
        VisitBinop(node, UseInfo::TruncatingWord32(),
                   UseInfo::TruncatingWord32(), NodeOutputInfo::Int32());
        if (lower()) {
          lowering->DoShift(node, lowering->machine()->Word32Shl(), rhs_type);
        }
        break;
      }
      case IrOpcode::kNumberShiftRight: {
        Type* rhs_type = GetInfo(node->InputAt(1))->output_type();
        VisitBinop(node, UseInfo::TruncatingWord32(),
                   UseInfo::TruncatingWord32(), NodeOutputInfo::Int32());
        if (lower()) {
          lowering->DoShift(node, lowering->machine()->Word32Sar(), rhs_type);
        }
        break;
      }
      case IrOpcode::kNumberShiftRightLogical: {
        Type* rhs_type = GetInfo(node->InputAt(1))->output_type();
        VisitBinop(node, UseInfo::TruncatingWord32(),
                   UseInfo::TruncatingWord32(), NodeOutputInfo::Uint32());
        if (lower()) {
          lowering->DoShift(node, lowering->machine()->Word32Shr(), rhs_type);
        }
        break;
      }
      case IrOpcode::kNumberToInt32: {
        // Just change representation if necessary.
        VisitUnop(node, UseInfo::TruncatingWord32(), NodeOutputInfo::Int32());
        if (lower()) DeferReplacement(node, node->InputAt(0));
        break;
      }
      case IrOpcode::kNumberToUint32: {
        // Just change representation if necessary.
        VisitUnop(node, UseInfo::TruncatingWord32(), NodeOutputInfo::Uint32());
        if (lower()) DeferReplacement(node, node->InputAt(0));
        break;
      }
      case IrOpcode::kNumberIsHoleNaN: {
        VisitUnop(node, UseInfo::Float64(), NodeOutputInfo::Bool());
        if (lower()) {
          // NumberIsHoleNaN(x) => Word32Equal(Float64ExtractLowWord32(x),
          //                                   #HoleNaNLower32)
          node->ReplaceInput(0,
                             jsgraph_->graph()->NewNode(
                                 lowering->machine()->Float64ExtractLowWord32(),
                                 node->InputAt(0)));
          node->AppendInput(jsgraph_->zone(),
                            jsgraph_->Int32Constant(kHoleNanLower32));
          NodeProperties::ChangeOp(node, jsgraph_->machine()->Word32Equal());
        }
        break;
      }
      case IrOpcode::kPlainPrimitiveToNumber: {
        VisitUnop(node, UseInfo::AnyTagged(), NodeOutputInfo::NumberTagged());
        if (lower()) {
          // PlainPrimitiveToNumber(x) => Call(ToNumberStub, x, no-context)
          Operator::Properties properties = node->op()->properties();
          Callable callable = CodeFactory::ToNumber(jsgraph_->isolate());
          CallDescriptor::Flags flags = CallDescriptor::kNoFlags;
          CallDescriptor* desc = Linkage::GetStubCallDescriptor(
              jsgraph_->isolate(), jsgraph_->zone(), callable.descriptor(), 0,
              flags, properties);
          node->InsertInput(jsgraph_->zone(), 0,
                            jsgraph_->HeapConstant(callable.code()));
          node->AppendInput(jsgraph_->zone(), jsgraph_->NoContextConstant());
          NodeProperties::ChangeOp(node, jsgraph_->common()->Call(desc));
        }
        break;
      }
      case IrOpcode::kReferenceEqual: {
        VisitBinop(node, UseInfo::AnyTagged(), NodeOutputInfo::Bool());
        if (lower()) {
          NodeProperties::ChangeOp(node, lowering->machine()->WordEqual());
        }
        break;
      }
      case IrOpcode::kStringEqual: {
        VisitBinop(node, UseInfo::AnyTagged(), NodeOutputInfo::Bool());
        if (lower()) lowering->DoStringEqual(node);
        break;
      }
      case IrOpcode::kStringLessThan: {
        VisitBinop(node, UseInfo::AnyTagged(), NodeOutputInfo::Bool());
        if (lower()) lowering->DoStringLessThan(node);
        break;
      }
      case IrOpcode::kStringLessThanOrEqual: {
        VisitBinop(node, UseInfo::AnyTagged(), NodeOutputInfo::Bool());
        if (lower()) lowering->DoStringLessThanOrEqual(node);
        break;
      }
      case IrOpcode::kAllocate: {
        ProcessInput(node, 0, UseInfo::AnyTagged());
        ProcessRemainingInputs(node, 1);
        SetOutput(node, NodeOutputInfo::AnyTagged());
        break;
      }
      case IrOpcode::kLoadField: {
        FieldAccess access = FieldAccessOf(node->op());
        ProcessInput(node, 0, UseInfoForBasePointer(access));
        ProcessRemainingInputs(node, 1);
        SetOutputFromMachineType(node, access.machine_type);
        break;
      }
      case IrOpcode::kStoreField: {
        FieldAccess access = FieldAccessOf(node->op());
        ProcessInput(node, 0, UseInfoForBasePointer(access));
        ProcessInput(node, 1, TruncatingUseInfoFromRepresentation(
                                  access.machine_type.representation()));
        ProcessRemainingInputs(node, 2);
        SetOutput(node, NodeOutputInfo::None());
        break;
      }
      case IrOpcode::kLoadBuffer: {
        BufferAccess access = BufferAccessOf(node->op());
        ProcessInput(node, 0, UseInfo::PointerInt());        // buffer
        ProcessInput(node, 1, UseInfo::TruncatingWord32());  // offset
        ProcessInput(node, 2, UseInfo::TruncatingWord32());  // length
        ProcessRemainingInputs(node, 3);

        NodeOutputInfo output_info;
        if (truncation.TruncatesUndefinedToZeroOrNaN()) {
          if (truncation.TruncatesNaNToZero()) {
            // If undefined is truncated to a non-NaN number, we can use
            // the load's representation.
            output_info = NodeOutputInfo(access.machine_type().representation(),
                                         NodeProperties::GetType(node));
          } else {
            // If undefined is truncated to a number, but the use can
            // observe NaN, we need to output at least the float32
            // representation.
            if (access.machine_type().representation() ==
                MachineRepresentation::kFloat32) {
              output_info =
                  NodeOutputInfo(access.machine_type().representation(),
                                 NodeProperties::GetType(node));
            } else {
              output_info = NodeOutputInfo::Float64();
            }
          }
        } else {
          // If undefined is not truncated away, we need to have the tagged
          // representation.
          output_info = NodeOutputInfo::AnyTagged();
        }
        SetOutput(node, output_info);
        if (lower())
          lowering->DoLoadBuffer(node, output_info.representation(), changer_);
        break;
      }
      case IrOpcode::kStoreBuffer: {
        BufferAccess access = BufferAccessOf(node->op());
        ProcessInput(node, 0, UseInfo::PointerInt());        // buffer
        ProcessInput(node, 1, UseInfo::TruncatingWord32());  // offset
        ProcessInput(node, 2, UseInfo::TruncatingWord32());  // length
        ProcessInput(node, 3,
                     TruncatingUseInfoFromRepresentation(
                         access.machine_type().representation()));  // value
        ProcessRemainingInputs(node, 4);
        SetOutput(node, NodeOutputInfo::None());
        if (lower()) lowering->DoStoreBuffer(node);
        break;
      }
      case IrOpcode::kLoadElement: {
        ElementAccess access = ElementAccessOf(node->op());
        ProcessInput(node, 0, UseInfoForBasePointer(access));  // base
        ProcessInput(node, 1, UseInfo::TruncatingWord32());    // index
        ProcessRemainingInputs(node, 2);
        SetOutputFromMachineType(node, access.machine_type);
        break;
      }
      case IrOpcode::kStoreElement: {
        ElementAccess access = ElementAccessOf(node->op());
        ProcessInput(node, 0, UseInfoForBasePointer(access));  // base
        ProcessInput(node, 1, UseInfo::TruncatingWord32());    // index
        ProcessInput(node, 2,
                     TruncatingUseInfoFromRepresentation(
                         access.machine_type.representation()));  // value
        ProcessRemainingInputs(node, 3);
        SetOutput(node, NodeOutputInfo::None());
        break;
      }
      case IrOpcode::kObjectIsNumber: {
        ProcessInput(node, 0, UseInfo::AnyTagged());
        SetOutput(node, NodeOutputInfo::Bool());
        break;
      }
      case IrOpcode::kObjectIsReceiver: {
        ProcessInput(node, 0, UseInfo::AnyTagged());
        SetOutput(node, NodeOutputInfo::Bool());
        break;
      }
      case IrOpcode::kObjectIsSmi: {
        ProcessInput(node, 0, UseInfo::AnyTagged());
        SetOutput(node, NodeOutputInfo::Bool());
        break;
      }

      //------------------------------------------------------------------
      // Machine-level operators.
      //------------------------------------------------------------------
      case IrOpcode::kLoad: {
        // TODO(jarin) Eventually, we should get rid of all machine stores
        // from the high-level phases, then this becomes UNREACHABLE.
        LoadRepresentation rep = LoadRepresentationOf(node->op());
        ProcessInput(node, 0, UseInfo::AnyTagged());   // tagged pointer
        ProcessInput(node, 1, UseInfo::PointerInt());  // index
        ProcessRemainingInputs(node, 2);
        SetOutputFromMachineType(node, rep);
        break;
      }
      case IrOpcode::kStore: {
        // TODO(jarin) Eventually, we should get rid of all machine stores
        // from the high-level phases, then this becomes UNREACHABLE.
        StoreRepresentation rep = StoreRepresentationOf(node->op());
        ProcessInput(node, 0, UseInfo::AnyTagged());   // tagged pointer
        ProcessInput(node, 1, UseInfo::PointerInt());  // index
        ProcessInput(node, 2,
                     TruncatingUseInfoFromRepresentation(rep.representation()));
        ProcessRemainingInputs(node, 3);
        SetOutput(node, NodeOutputInfo::None());
        break;
      }
      case IrOpcode::kWord32Shr:
        // We output unsigned int32 for shift right because JavaScript.
        return VisitBinop(node, UseInfo::TruncatingWord32(),
                          NodeOutputInfo::Uint32());
      case IrOpcode::kWord32And:
      case IrOpcode::kWord32Or:
      case IrOpcode::kWord32Xor:
      case IrOpcode::kWord32Shl:
      case IrOpcode::kWord32Sar:
        // We use signed int32 as the output type for these word32 operations,
        // though the machine bits are the same for either signed or unsigned,
        // because JavaScript considers the result from these operations signed.
        return VisitBinop(node, UseInfo::TruncatingWord32(),
                          NodeOutputInfo::Int32());
      case IrOpcode::kWord32Equal:
        return VisitBinop(node, UseInfo::TruncatingWord32(),
                          NodeOutputInfo::Bool());

      case IrOpcode::kWord32Clz:
        return VisitUnop(node, UseInfo::TruncatingWord32(),
                         NodeOutputInfo::Uint32());

      case IrOpcode::kInt32Add:
      case IrOpcode::kInt32Sub:
      case IrOpcode::kInt32Mul:
      case IrOpcode::kInt32MulHigh:
      case IrOpcode::kInt32Div:
      case IrOpcode::kInt32Mod:
        return VisitInt32Binop(node);
      case IrOpcode::kUint32Div:
      case IrOpcode::kUint32Mod:
      case IrOpcode::kUint32MulHigh:
        return VisitUint32Binop(node);
      case IrOpcode::kInt32LessThan:
      case IrOpcode::kInt32LessThanOrEqual:
        return VisitInt32Cmp(node);

      case IrOpcode::kUint32LessThan:
      case IrOpcode::kUint32LessThanOrEqual:
        return VisitUint32Cmp(node);

      case IrOpcode::kInt64Add:
      case IrOpcode::kInt64Sub:
      case IrOpcode::kInt64Mul:
      case IrOpcode::kInt64Div:
      case IrOpcode::kInt64Mod:
        return VisitInt64Binop(node);
      case IrOpcode::kInt64LessThan:
      case IrOpcode::kInt64LessThanOrEqual:
        return VisitInt64Cmp(node);

      case IrOpcode::kUint64LessThan:
        return VisitUint64Cmp(node);

      case IrOpcode::kUint64Div:
      case IrOpcode::kUint64Mod:
        return VisitUint64Binop(node);

      case IrOpcode::kWord64And:
      case IrOpcode::kWord64Or:
      case IrOpcode::kWord64Xor:
      case IrOpcode::kWord64Shl:
      case IrOpcode::kWord64Shr:
      case IrOpcode::kWord64Sar:
        return VisitBinop(node, UseInfo::TruncatingWord64(),
                          NodeOutputInfo::Int64());
      case IrOpcode::kWord64Equal:
        return VisitBinop(node, UseInfo::TruncatingWord64(),
                          NodeOutputInfo::Bool());

      case IrOpcode::kChangeInt32ToInt64:
        return VisitUnop(
            node, UseInfo::TruncatingWord32(),
            NodeOutputInfo(MachineRepresentation::kWord64, Type::Signed32()));
      case IrOpcode::kChangeUint32ToUint64:
        return VisitUnop(
            node, UseInfo::TruncatingWord32(),
            NodeOutputInfo(MachineRepresentation::kWord64, Type::Unsigned32()));
      case IrOpcode::kTruncateFloat64ToFloat32:
        return VisitUnop(node, UseInfo::Float64(), NodeOutputInfo::Float32());
      case IrOpcode::kTruncateFloat64ToInt32:
        return VisitUnop(node, UseInfo::Float64(), NodeOutputInfo::Int32());
      case IrOpcode::kTruncateInt64ToInt32:
        // TODO(titzer): Is kTypeInt32 correct here?
        return VisitUnop(node, UseInfo::Word64TruncatingToWord32(),
                         NodeOutputInfo::Int32());

      case IrOpcode::kChangeFloat32ToFloat64:
        return VisitUnop(node, UseInfo::Float32(), NodeOutputInfo::Float64());
      case IrOpcode::kChangeInt32ToFloat64:
        return VisitUnop(
            node, UseInfo::TruncatingWord32(),
            NodeOutputInfo(MachineRepresentation::kFloat64, Type::Signed32()));
      case IrOpcode::kChangeUint32ToFloat64:
        return VisitUnop(node, UseInfo::TruncatingWord32(),
                         NodeOutputInfo(MachineRepresentation::kFloat64,
                                        Type::Unsigned32()));
      case IrOpcode::kChangeFloat64ToInt32:
        return VisitUnop(node, UseInfo::Float64TruncatingToWord32(),
                         NodeOutputInfo::Int32());
      case IrOpcode::kChangeFloat64ToUint32:
        return VisitUnop(node, UseInfo::Float64TruncatingToWord32(),
                         NodeOutputInfo::Uint32());

      case IrOpcode::kFloat64Add:
      case IrOpcode::kFloat64Sub:
      case IrOpcode::kFloat64Mul:
      case IrOpcode::kFloat64Div:
      case IrOpcode::kFloat64Mod:
      case IrOpcode::kFloat64Min:
        return VisitFloat64Binop(node);
      case IrOpcode::kFloat64Abs:
      case IrOpcode::kFloat64Sqrt:
      case IrOpcode::kFloat64RoundDown:
      case IrOpcode::kFloat64RoundTruncate:
      case IrOpcode::kFloat64RoundTiesAway:
      case IrOpcode::kFloat64RoundUp:
        return VisitUnop(node, UseInfo::Float64(), NodeOutputInfo::Float64());
      case IrOpcode::kFloat64Equal:
      case IrOpcode::kFloat64LessThan:
      case IrOpcode::kFloat64LessThanOrEqual:
        return VisitFloat64Cmp(node);
      case IrOpcode::kFloat64ExtractLowWord32:
      case IrOpcode::kFloat64ExtractHighWord32:
        return VisitUnop(node, UseInfo::Float64(), NodeOutputInfo::Int32());
      case IrOpcode::kFloat64InsertLowWord32:
      case IrOpcode::kFloat64InsertHighWord32:
        return VisitBinop(node, UseInfo::Float64(), UseInfo::TruncatingWord32(),
                          NodeOutputInfo::Float64());
      case IrOpcode::kLoadStackPointer:
      case IrOpcode::kLoadFramePointer:
      case IrOpcode::kLoadParentFramePointer:
        return VisitLeaf(node, NodeOutputInfo::Pointer());
      case IrOpcode::kStateValues:
        VisitStateValues(node);
        break;
      default:
        VisitInputs(node);
        // Assume the output is tagged.
        SetOutput(node, NodeOutputInfo::AnyTagged());
        break;
    }
  }