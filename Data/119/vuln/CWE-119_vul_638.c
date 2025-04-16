bool SelectionOwner::ProcessTarget(XAtom target,
                                   XID requestor,
                                   XAtom property) {
  XAtom multiple_atom = atom_cache_.GetAtom(kMultiple);
  XAtom save_targets_atom = atom_cache_.GetAtom(kSaveTargets);
  XAtom targets_atom = atom_cache_.GetAtom(kTargets);

  if (target == multiple_atom || target == save_targets_atom)
    return false;

  if (target == targets_atom) {
    // We have been asked for TARGETS. Send an atom array back with the data
    // types we support.
    std::vector<XAtom> targets;
    targets.push_back(targets_atom);
    targets.push_back(save_targets_atom);
    targets.push_back(multiple_atom);
    RetrieveTargets(&targets);

    XChangeProperty(x_display_, requestor, property, XA_ATOM, 32,
                    PropModeReplace,
                    reinterpret_cast<unsigned char*>(&targets.front()),
                    targets.size());
    return true;
  } else {
    // Try to find the data type in map.
    SelectionFormatMap::const_iterator it = format_map_.find(target);
    if (it != format_map_.end()) {
      if (it->second->size() > max_request_size_) {
        // We must send the data back in several chunks due to a limitation in
        // the size of X requests. Notify the selection requestor that the data
        // will be sent incrementally by returning data of type "INCR".
        int length = it->second->size();
        XChangeProperty(x_display_,
                        requestor,
                        property,
                        atom_cache_.GetAtom(kIncr),
                        32,
                        PropModeReplace,
                        reinterpret_cast<unsigned char*>(&length),
                        1);

        // Wait for the selection requestor to indicate that it has processed
        // the selection result before sending the first chunk of data. The
        // selection requestor indicates this by deleting |property|.
        base::TimeTicks timeout =
            base::TimeTicks::Now() +
            base::TimeDelta::FromMilliseconds(kIncrementalTransferTimeoutMs);
        int foreign_window_manager_id =
            ui::XForeignWindowManager::GetInstance()->RequestEvents(
                requestor, PropertyChangeMask);
        incremental_transfers_.push_back(
            IncrementalTransfer(requestor,
                                target,
                                property,
                                it->second,
                                0,
                                timeout,
                                foreign_window_manager_id));

        // Start a timer to abort the data transfer in case that the selection
        // requestor does not support the INCR property or gets destroyed during
        // the data transfer.
        if (!incremental_transfer_abort_timer_.IsRunning()) {
          incremental_transfer_abort_timer_.Start(
              FROM_HERE,
              base::TimeDelta::FromMilliseconds(kTimerPeriodMs),
              this,
              &SelectionOwner::AbortStaleIncrementalTransfers);
        }
      } else {
        XChangeProperty(
            x_display_,
            requestor,
            property,
            target,
            8,
            PropModeReplace,
            const_cast<unsigned char*>(it->second->front()),
            it->second->size());
      }
      return true;
    }
    // I would put error logging here, but GTK ignores TARGETS and spams us
    // looking for its own internal types.
  }
  return false;
}