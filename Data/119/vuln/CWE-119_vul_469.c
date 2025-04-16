Handle<Code> Pipeline::GenerateCode() {
  ZonePool zone_pool;
  base::SmartPointer<PipelineStatistics> pipeline_statistics;

  if (FLAG_turbo_stats) {
    pipeline_statistics.Reset(new PipelineStatistics(info(), &zone_pool));
    pipeline_statistics->BeginPhaseKind("initializing");
  }

  if (FLAG_trace_turbo) {
    FILE* json_file = OpenVisualizerLogFile(info(), nullptr, "json", "w+");
    if (json_file != nullptr) {
      OFStream json_of(json_file);
      Handle<Script> script = info()->script();
      base::SmartArrayPointer<char> function_name = info()->GetDebugName();
      int pos = info()->shared_info()->start_position();
      json_of << "{\"function\":\"" << function_name.get()
              << "\", \"sourcePosition\":" << pos << ", \"source\":\"";
      if (info()->has_literal() && !script->IsUndefined() &&
          !script->source()->IsUndefined()) {
        DisallowHeapAllocation no_allocation;
        FunctionLiteral* function = info()->literal();
        int start = function->start_position();
        int len = function->end_position() - start;
        String::SubStringRange source(String::cast(script->source()), start,
                                      len);
        for (const auto& c : source) {
          json_of << AsEscapedUC16ForJSON(c);
        }
      }
      json_of << "\",\n\"phases\":[";
      fclose(json_file);
    }
  }

  PipelineData data(&zone_pool, info(), pipeline_statistics.get());
  this->data_ = &data;

  BeginPhaseKind("graph creation");

  if (FLAG_trace_turbo) {
    OFStream os(stdout);
    os << "---------------------------------------------------\n"
       << "Begin compiling method " << info()->GetDebugName().get()
       << " using Turbofan" << std::endl;
    TurboCfgFile tcf(isolate());
    tcf << AsC1VCompilation(info());
  }

  data.source_positions()->AddDecorator();

  if (FLAG_loop_assignment_analysis) {
    Run<LoopAssignmentAnalysisPhase>();
  }

  if (info()->is_typing_enabled()) {
    Run<TypeHintAnalysisPhase>();
  }

  Run<GraphBuilderPhase>();
  if (data.compilation_failed()) return Handle<Code>::null();
  RunPrintAndVerify("Initial untyped", true);

  // Perform OSR deconstruction.
  if (info()->is_osr()) {
    Run<OsrDeconstructionPhase>();
    RunPrintAndVerify("OSR deconstruction", true);
  }

  // Perform function context specialization and inlining (if enabled).
  Run<InliningPhase>();
  RunPrintAndVerify("Inlined", true);

  // Remove dead->live edges from the graph.
  Run<EarlyGraphTrimmingPhase>();
  RunPrintAndVerify("Early trimmed", true);

  if (FLAG_print_turbo_replay) {
    // Print a replay of the initial graph.
    GraphReplayPrinter::PrintReplay(data.graph());
  }

  base::SmartPointer<Typer> typer;
  if (info()->is_typing_enabled()) {
    // Type the graph.
    typer.Reset(new Typer(isolate(), data.graph(),
                          info()->is_deoptimization_enabled()
                              ? Typer::kDeoptimizationEnabled
                              : Typer::kNoFlags,
                          info()->dependencies()));
    Run<TyperPhase>(typer.get());
    RunPrintAndVerify("Typed");
  }

  BeginPhaseKind("lowering");

  if (info()->is_typing_enabled()) {
    // Lower JSOperators where we can determine types.
    Run<TypedLoweringPhase>();
    RunPrintAndVerify("Lowered typed");

    if (FLAG_turbo_stress_loop_peeling) {
      Run<StressLoopPeelingPhase>();
      RunPrintAndVerify("Loop peeled");
    }

    if (FLAG_turbo_escape) {
      Run<EscapeAnalysisPhase>();
      RunPrintAndVerify("Escape Analysed");
    }

    // Lower simplified operators and insert changes.
    Run<SimplifiedLoweringPhase>();
    RunPrintAndVerify("Lowered simplified");

    Run<BranchEliminationPhase>();
    RunPrintAndVerify("Branch conditions eliminated");

    // Optimize control flow.
    if (FLAG_turbo_cf_optimization) {
      Run<ControlFlowOptimizationPhase>();
      RunPrintAndVerify("Control flow optimized");
    }

    // Lower changes that have been inserted before.
    Run<ChangeLoweringPhase>();
    // TODO(jarin, rossberg): Remove UNTYPED once machine typing works.
    RunPrintAndVerify("Lowered changes", true);
  }

  // Lower any remaining generic JSOperators.
  Run<GenericLoweringPhase>();
  // TODO(jarin, rossberg): Remove UNTYPED once machine typing works.
  RunPrintAndVerify("Lowered generic", true);

  Run<LateGraphTrimmingPhase>();
  // TODO(jarin, rossberg): Remove UNTYPED once machine typing works.
  RunPrintAndVerify("Late trimmed", true);

  BeginPhaseKind("block building");

  data.source_positions()->RemoveDecorator();

  // Kill the Typer and thereby uninstall the decorator (if any).
  typer.Reset(nullptr);

  return ScheduleAndGenerateCode(
      Linkage::ComputeIncoming(data.instruction_zone(), info()));
}