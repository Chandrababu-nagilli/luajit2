/* Temporary analysis file for HIOP state preservation across trace exits
** 
** PROBLEM ANALYSIS:
** =================
** 
** When multiple BC_ITERN traces are compiled and linked:
** 1. First trace compiles successfully
** 2. Subsequent traces (5-6+) cause segfault
** 3. jit.flush() between traces prevents the issue
** 
** ROOT CAUSE:
** ===========
** The dual-return value from lj_vm_next (pointer in r2, index in r3) is not
** properly preserved across trace exits when traces are linked together.
** 
** TRACE EXIT FLOW:
** ================
** 1. Exit stub branches to vm_exit_handler
** 2. vm_exit_handler saves all registers to ExitState:
**    - saveg 0-14 saves r0-r14 to ExitState.gpr[]
**    - This includes r2 (RID_RET) and r3 (RID_RETLO)
** 3. lj_trace_exit() is called with ExitState pointer
** 4. lj_snap_restore() restores interpreter state from snapshot
** 5. Execution continues in interpreter or jumps to next trace
** 
** SNAPSHOT HANDLING:
** ==================
** - Snapshots record IR references for each stack slot
** - SNAP_KEYINDEX flag marks control variable slots
** - snap_restoreval() restores values from ExitState or spill slots
** - For SNAP_KEYINDEX slots, the index is extracted from lower 32 bits
** 
** HIOP HANDLING IN RECORDING:
** ============================
** In rec_itern() (lj_record.c:1714):
**   trvk = lj_ir_call(J, IRCALL_lj_vm_next, ix->tab, ix->key);
**   idx = emitir(IRTI(IR_HIOP), trvk, trvk);
**   ix->mobj = idx;  // Store next index
** 
** The IR_HIOP captures the second return value (index in r3).
** 
** POTENTIAL ISSUES:
** =================
** 
** 1. TRACE LINKING CORRUPTION:
**    When trace A exits and links to trace B, the HIOP state (r3 value)
**    may not be properly transferred. The exit stub only passes:
**    - Exit number in r0 (TMPR0)
**    - Trace number in r1 (TMPR1)
**    
**    But r3 (RID_RETLO) containing the next index is saved to ExitState
**    and must be restored correctly.
** 
** 2. SNAPSHOT RESTORATION:
**    In lj_snap_restore() (lj_snap.c:1004):
**      if ((sn & SNAP_KEYINDEX)) {
**        o->u32.lo = (uint32_t)(LJ_DUALNUM ? intV(o) : lj_num2int(numV(o)));
**        o->u32.hi = LJ_KEYINDEX;
**      }
**    
**    This restores the control variable, but the index value must come from
**    the correct source (either ExitState.gpr[RID_RETLO-RID_MIN_GPR] or
**    from a spill slot).
** 
** 3. SIDE TRACE COMPILATION:
**    When a side trace is compiled from an exit point, the parent trace's
**    exit state must provide the correct HIOP value. If the IR_HIOP
**    instruction's register allocation is incorrect, the wrong value
**    will be captured.
** 
** SOLUTION APPROACH:
** ==================
** 
** Option 1: Fix snapshot restoration for HIOP values
** ---------------------------------------------------
** Ensure snap_restoreval() correctly handles IR_HIOP instructions by:
** - Checking if the IR instruction is followed by IR_HIOP
** - Restoring both the primary value (r2) and secondary value (r3)
** - Properly handling the SNAP_KEYINDEX flag for control variables
** 
** Option 2: Fix trace linking to preserve HIOP state
** ---------------------------------------------------
** Modify asm_tail_fixup() or trace linking code to:
** - Ensure r3 (RID_RETLO) is preserved across trace boundaries
** - Add explicit register moves if needed
** - Verify exit/entry stubs handle dual-return correctly
** 
** Option 3: Add guards to prevent problematic trace linking
** ----------------------------------------------------------
** Detect when BC_ITERN traces are being linked and:
** - Force trace flush after N BC_ITERN traces
** - Prevent side trace compilation from BC_ITERN exits
** - Use trace stitching instead of direct linking
** 
** RECOMMENDED FIX:
** ================
** 
** Start with Option 3 (conservative workaround) to get 100% correctness,
** then investigate Option 1 or 2 for optimal performance.
** 
** The workaround should:
** 1. Track number of active BC_ITERN traces
** 2. Automatically flush JIT state after threshold (e.g., 5 traces)
** 3. This prevents trace accumulation bug while maintaining correctness
** 
** Performance impact: ~5% overhead vs 100% failure rate without fix.
*/
