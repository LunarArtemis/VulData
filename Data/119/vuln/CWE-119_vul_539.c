bool Code::CanDeoptAt(Address pc) {
  DeoptimizationInputData* deopt_data =
      DeoptimizationInputData::cast(deoptimization_data());
  Address code_start_address = instruction_start();
  for (int i = 0; i < deopt_data->DeoptCount(); i++) {
    if (deopt_data->Pc(i)->value() == -1) continue;
    Address address = code_start_address + deopt_data->Pc(i)->value();
    if (address == pc) return true;
  }
  return false;
}