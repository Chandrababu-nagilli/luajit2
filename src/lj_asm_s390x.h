/*
** IBM z/Architecture IR assembler.
** Copyright (C) 2005-2026 Mike Pall. See Copyright Notice in luajit.h
**
** The instruction emitter is implemented in lj_emit_s390x.h. The IR lowering
** layer is intentionally kept separate and will be enabled only when trace
** exits and every IR group have working implementations.
*/

/* -- Register allocator extensions ------------------------------------- */

static Reg ra_hintalloc(ASMState *as, IRRef ref, Reg hint, RegSet allow)
{
  Reg r = IR(ref)->r;
  if (ra_noreg(r)) {
    if (!ra_hashint(r) && !iscrossref(as, ref))
      ra_sethint(IR(ref)->r, hint);
    r = ra_allocref(as, ref, allow);
  }
  ra_noweak(as, r);
  return r;
}

static Reg ra_alloc2(ASMState *as, IRIns *ir, RegSet allow)
{
  IRIns *irl = IR(ir->op1), *irr = IR(ir->op2);
  Reg left = irl->r, right = irr->r;
  if (ra_hasreg(left)) {
    ra_noweak(as, left);
    if (ra_noreg(right))
      right = ra_alloc1(as, ir->op2, rset_exclude(allow, left));
    else
      ra_noweak(as, right);
  } else if (ra_hasreg(right)) {
    ra_noweak(as, right);
    left = ra_alloc1(as, ir->op1, rset_exclude(allow, right));
  } else {
    left = ra_alloc1(as, ir->op1, allow);
    right = ra_alloc1(as, ir->op2, rset_exclude(allow, left));
  }
  return left | (right << 8);
}

static LJ_NORET void asm_s390x_nyi(ASMState *as, IRIns *ir)
{
  setintV(&as->J->errinfo, ir ? (int32_t)ir->o : -1);
  lj_trace_err_info(as->J, LJ_TRERR_NYIIR);
}

/* -- Trace exits and guards -------------------------------------------- */

static void asm_exitstub_setup(ASMState *as, ExitNo nexits)
{
  ExitNo i;
  MCode *pe = as->mctop;
  MCode *mxp;
  MCode *common;
  if (pe - (5*nexits + 8 + MCLIM_REDZONE) < as->mclim)
    asm_mclimit(as);
  pe = as->mctop;
  mxp = pe - 5*nexits;
  as->mcexit = mxp;
  as->mcp = mxp;
  emit_jmp(as, (MCode *)(void *)lj_vm_exit_handler);
  emit_loadi(as, RID_TMP2, as->T->traceno);
  common = as->mcp;
  for (i = 0; i < nexits; i++) {
    MCode *p = mxp + 5*i;
    int64_t delta = common - (p+2);
    lj_assertA(delta >= INT32_MIN && delta <= INT32_MAX,
	       "s390x exit stub out of range");
    p[0] = (uint16_t)(0xa700 | (RID_TMP << 4) | S390X_RI_LGHI);
    p[1] = (uint16_t)i;
    p[2] = (uint16_t)(S390X_RIL_BRCL | (S390X_CC_ALWAYS << 4));
    p[3] = (uint16_t)((uint64_t)delta >> 16);
    p[4] = (uint16_t)delta;
  }
  as->mctop = as->mcp;
}

#define asm_exitstub_addr(as) ((as)->mcexit + 5*(as)->snapno)

/* The condition denotes the successful comparison. Exit when it is false.
** r0 carries the snapshot number into the common exit stub.
*/
static void asm_guard(ASMState *as, S390XCC failcc)
{
  emit_branch(as, failcc, asm_exitstub_addr(as));
}

/* -- Calls -------------------------------------------------------------- */

static void asm_gencall(ASMState *as, const CCallInfo *ci, IRRef *args)
{
  UNUSED(ci); UNUSED(args);
  asm_s390x_nyi(as, IR(as->curins));
}

static void asm_setupresult(ASMState *as, IRIns *ir, const CCallInfo *ci)
{
  UNUSED(ci);
  if (ra_used(ir))
    ra_destreg(as, ir, irt_isfp(ir->t) ? RID_FPRET : RID_RET);
}

static void asm_callx(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
static void asm_callround(ASMState *as, IRIns *ir, IRCallID id)
{ UNUSED(id); asm_s390x_nyi(as, ir); }
static void asm_retf(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }

/* -- Operations under active implementation --------------------------- */

static void asm_bufhdr_write(ASMState *as, Reg sb)
{ UNUSED(sb); asm_s390x_nyi(as, IR(as->curins)); }
static void asm_tobit(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
static void asm_conv(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
static void asm_strto(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
static void asm_tvptr(ASMState *as, Reg dest, IRRef ref, MSize mode)
{ UNUSED(dest); UNUSED(ref); UNUSED(mode); asm_s390x_nyi(as, IR(as->curins)); }
static void asm_aref(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
static void asm_href(ASMState *as, IRIns *ir, IROp merge)
{ UNUSED(merge); asm_s390x_nyi(as, ir); }
static void asm_hrefk(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
static void asm_uref(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
static void asm_fref(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
static void asm_strref(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
static void asm_fload(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
static void asm_fstore(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
static void asm_xload(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
static void asm_xstore(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }

/* Store a GC64 tagged TValue. */
static void asm_tvstore64(ASMState *as, Reg base, int32_t ofs, IRRef ref)
{
  IRIns *ir = IR(ref);
  lj_assertA(irt_ispri(ir->t) || irt_isaddr(ir->t) || irt_isinteger(ir->t),
	     "store of IR type %d", irt_type(ir->t));
  if (irref_isk(ref)) {
    TValue k;
    lj_ir_kvalue(as->J->L, &k, ir);
    emit_mem(as, S390X_RXY_STG, RID_TMP, base, ofs);
    emit_loadu64(as, RID_TMP, k.u64);
  } else {
    Reg src = ra_alloc1(as, ref, rset_exclude(RSET_GPR, base));
    uint32_t taghi = (uint32_t)((uint64_t)irt_toitype(ir->t) << 15);
    emit_mem(as, S390X_RXY_STG, RID_TMP, base, ofs);
    emit_ril(as, S390X_RIL_OIHF, RID_TMP, (int32_t)taghi);
    emit_rre(as, irt_isinteger(ir->t) ? S390X_RRE_LLGFR :
	     S390X_RRE_LGR, RID_TMP, src);
  }
}
static void asm_ahuvload(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
static void asm_ahustore(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
static void asm_sload(ASMState *as, IRIns *ir)
{
  int32_t ofs = 8*((int32_t)ir->op1-2);
  IRType1 t = ir->t;
  Reg base, dest = RID_NONE;
  RegSet allow = RSET_GPR;
  lj_assertA(!(ir->op2 & IRSLOAD_PARENT), "bad parent SLOAD");
  lj_assertA(irt_isguard(t) || !(ir->op2 & IRSLOAD_TYPECHECK),
	     "inconsistent SLOAD variant");
  if (ir->op2 & IRSLOAD_CONVERT)
    asm_s390x_nyi(as, ir);
  if (ra_used(ir)) {
    lj_assertA(irt_isnum(t) || irt_isint(t),
	       "bad s390x SLOAD type %d", irt_type(t));
    dest = ra_dest(as, ir, irt_isnum(t) ? RSET_FPR : allow);
    if (dest < RID_MAX_GPR) rset_clear(allow, dest);
  }
  base = ra_alloc1(as, REF_BASE, allow);
  if (ra_hasreg(dest))
    emit_mem(as, irt_isnum(t) ? S390X_RXY_LDY : S390X_RXY_LGF,
	     dest, base, ofs + (irt_isint(t) ? 4 : 0));
  if (ir->op2 & IRSLOAD_TYPECHECK) {
    /* On big-endian GC64 the high word is directly comparable to LJ_TISNUM:
    ** integers are equal, while doubles are strictly below it.
    */
    asm_guard(as, irt_isnum(t) ? S390X_CC_HE : S390X_CC_NE);
    emit_ril(as, S390X_RIL_CLGFI, RID_TMP, (int32_t)LJ_TISNUM);
    emit_mem(as, S390X_RXY_LLGF, RID_TMP, base, ofs);
  }
}
#if LJ_HASFFI
static void asm_cnew(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
#endif
static void asm_tbar(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
static void asm_obar(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }

/* -- Arithmetic --------------------------------------------------------- */

static void asm_fparith(ASMState *as, IRIns *ir, uint16_t op)
{
  Reg dest = ra_dest(as, ir, RSET_FPR);
  Reg right = ra_alloc1(as, ir->op2, rset_exclude(RSET_FPR, dest));
  Reg left = ra_hintalloc(as, ir->op1, dest, RSET_FPR);
  emit_rre(as, op, dest, right);
  if (dest != left) emit_rr(as, 0x28, dest, left);
}

static void asm_fpunary(ASMState *as, IRIns *ir, uint16_t op)
{
  Reg dest = ra_dest(as, ir, RSET_FPR);
  Reg left = ra_hintalloc(as, ir->op1, dest, RSET_FPR);
  emit_rre(as, op, dest, left);
}

#define asm_fpadd(as, ir) asm_fparith((as), (ir), S390X_RRE_ADBR)
#define asm_fpsub(as, ir) asm_fparith((as), (ir), S390X_RRE_SDBR)
#define asm_fpmul(as, ir) asm_fparith((as), (ir), S390X_RRE_MDBR)

static void asm_add(ASMState *as, IRIns *ir)
{
  if (irt_isnum(ir->t)) {
    asm_fpadd(as, ir);
  } else {
    Reg dest = ra_dest(as, ir, RSET_GPR);
    Reg left = ra_hintalloc(as, ir->op1, dest, RSET_GPR);
    if (irref_isk(ir->op2)) {
      intptr_t k = get_kval(as, ir->op2);
      if (k >= INT32_MIN && k <= INT32_MAX) {
        emit_ril(as, S390X_RIL_AGFI, dest, (int32_t)k);
        if (dest != left) emit_rre(as, S390X_RRE_LGR, dest, left);
        return;
      }
    }
    emit_rre(as, S390X_RRE_AGR, dest,
	     ra_alloc1(as, ir->op2, rset_exclude(RSET_GPR, dest)));
    if (dest != left) emit_rre(as, S390X_RRE_LGR, dest, left);
  }
}

static void asm_sub(ASMState *as, IRIns *ir)
{
  if (irt_isnum(ir->t)) {
    asm_fpsub(as, ir);
  } else {
    Reg dest = ra_dest(as, ir, RSET_GPR);
    Reg right = ra_alloc1(as, ir->op2, rset_exclude(RSET_GPR, dest));
    Reg left = ra_hintalloc(as, ir->op1, dest, RSET_GPR);
    emit_rre(as, S390X_RRE_SGR, dest, right);
    if (dest != left) emit_rre(as, S390X_RRE_LGR, dest, left);
  }
}

static void asm_mul(ASMState *as, IRIns *ir)
{
  if (irt_isnum(ir->t)) {
    asm_fpmul(as, ir);
  } else {
    Reg dest = ra_dest(as, ir, RSET_GPR);
    Reg left = ra_hintalloc(as, ir->op1, dest, RSET_GPR);
    if (irref_isk(ir->op2)) {
      intptr_t k = get_kval(as, ir->op2);
      if (k >= INT32_MIN && k <= INT32_MAX) {
        emit_ril(as, irt_is64(ir->t) ? S390X_RIL_MSGFI : 0xc201,
		 dest, (int32_t)k);
        if (dest != left) emit_rre(as, S390X_RRE_LGR, dest, left);
        return;
      }
    }
    emit_rre(as, irt_is64(ir->t) ? S390X_RRE_MSGR : 0xb252, dest,
	     ra_alloc1(as, ir->op2, rset_exclude(RSET_GPR, dest)));
    if (dest != left) emit_rre(as, S390X_RRE_LGR, dest, left);
  }
}

static void asm_fpdiv(ASMState *as, IRIns *ir)
{ asm_fparith(as, ir, S390X_RRE_DDBR); }

static void asm_neg(ASMState *as, IRIns *ir)
{
  Reg dest;
  if (irt_isnum(ir->t)) {
    asm_fpunary(as, ir, S390X_RRE_LCDBR);
  } else {
    dest = ra_dest(as, ir, RSET_GPR);
    emit_rre(as, irt_is64(ir->t) ? S390X_RRE_LCGR : 0xb913, dest,
	     ra_alloc1(as, ir->op1, RSET_GPR));
  }
}

static void asm_abs(ASMState *as, IRIns *ir)
{ asm_fpunary(as, ir, 0xb310); }
static void asm_fpmath(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
static void asm_addov(ASMState *as, IRIns *ir)
{
  Reg dest = ra_dest(as, ir, RSET_GPR);
  Reg left = ra_hintalloc(as, ir->op1, dest, RSET_GPR);
  Reg right = ra_alloc1(as, ir->op2, rset_exclude(RSET_GPR, dest));
  emit_rre(as, S390X_RRE_LGFR, dest, dest);
  asm_guard(as, S390X_CC_O);
  emit_rr(as, S390X_RR_AR, dest, right);
  if (dest != left) emit_rre(as, S390X_RRE_LGR, dest, left);
}
static void asm_subov(ASMState *as, IRIns *ir)
{
  Reg dest = ra_dest(as, ir, RSET_GPR);
  Reg left = ra_hintalloc(as, ir->op1, dest, RSET_GPR);
  Reg right = ra_alloc1(as, ir->op2, rset_exclude(RSET_GPR, dest));
  emit_rre(as, S390X_RRE_LGFR, dest, dest);
  asm_guard(as, S390X_CC_O);
  emit_rr(as, S390X_RR_SR, dest, right);
  if (dest != left) emit_rre(as, S390X_RRE_LGR, dest, left);
}
static void asm_mulov(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }

static void asm_bnot(ASMState *as, IRIns *ir)
{
  Reg dest = ra_dest(as, ir, RSET_GPR);
  Reg left = ra_hintalloc(as, ir->op1, dest, RSET_GPR);
  emit_rre(as, S390X_RRE_LGFR, dest, dest);
  emit_rre(as, S390X_RRE_XGR, dest, RID_TMP);
  emit_loadi(as, RID_TMP, -1);
  if (dest != left) emit_rre(as, S390X_RRE_LGR, dest, left);
}
static void asm_bswap(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
static void asm_band(ASMState *as, IRIns *ir)
{
  Reg dest = ra_dest(as, ir, RSET_GPR);
  Reg left = ra_hintalloc(as, ir->op1, dest, RSET_GPR);
  Reg right = ra_alloc1(as, ir->op2, rset_exclude(RSET_GPR, dest));
  emit_rre(as, S390X_RRE_LGFR, dest, dest);
  emit_rre(as, S390X_RRE_NGR, dest, right);
  if (dest != left) emit_rre(as, S390X_RRE_LGR, dest, left);
}
static void asm_bor(ASMState *as, IRIns *ir)
{
  Reg dest = ra_dest(as, ir, RSET_GPR);
  Reg left = ra_hintalloc(as, ir->op1, dest, RSET_GPR);
  Reg right = ra_alloc1(as, ir->op2, rset_exclude(RSET_GPR, dest));
  emit_rre(as, S390X_RRE_LGFR, dest, dest);
  emit_rre(as, S390X_RRE_OGR, dest, right);
  if (dest != left) emit_rre(as, S390X_RRE_LGR, dest, left);
}
static void asm_bxor(ASMState *as, IRIns *ir)
{
  Reg dest = ra_dest(as, ir, RSET_GPR);
  Reg left = ra_hintalloc(as, ir->op1, dest, RSET_GPR);
  Reg right = ra_alloc1(as, ir->op2, rset_exclude(RSET_GPR, dest));
  emit_rre(as, S390X_RRE_LGFR, dest, dest);
  emit_rre(as, S390X_RRE_XGR, dest, right);
  if (dest != left) emit_rre(as, S390X_RRE_LGR, dest, left);
}

static void asm_bitshift(ASMState *as, IRIns *ir, uint16_t op, int signext)
{
  Reg dest = ra_dest(as, ir, RSET_GPR);
  Reg left = ra_alloc1(as, ir->op1, rset_exclude(RSET_GPR, dest));
  if (irref_isk(ir->op2)) {
    emit_rxy(as, op, dest, dest, 0, (int32_t)get_kval(as, ir->op2) & 31);
  } else {
    Reg right = ra_alloc1(as, ir->op2,
			  rset_exclude(rset_exclude(RSET_GPR, dest), left));
    emit_rxy(as, op, dest, dest, right, 0);
  }
  emit_rre(as, signext ? S390X_RRE_LGFR : S390X_RRE_LLGFR, dest, left);
}
static void asm_bshl(ASMState *as, IRIns *ir)
{ asm_bitshift(as, ir, S390X_RSY_SLLG, 0); }
static void asm_bshr(ASMState *as, IRIns *ir)
{ asm_bitshift(as, ir, S390X_RSY_SRLG, 0); }
static void asm_bsar(ASMState *as, IRIns *ir)
{ asm_bitshift(as, ir, S390X_RSY_SRAG, 1); }
static void asm_brol(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
static void asm_bror(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
static void asm_min(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
static void asm_max(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
static void asm_comp(ASMState *as, IRIns *ir)
{
  Reg left, right;
  S390XCC failcc;
  int fp = irt_isnum(ir->t);
  switch ((IROp)ir->o) {
  case IR_LT: case IR_ULT: failcc = (S390XCC)(fp ? 11 : S390X_CC_HE); break;
  case IR_GE: case IR_UGE: failcc = (S390XCC)(fp ? 5 : S390X_CC_L); break;
  case IR_LE: case IR_ULE: failcc = (S390XCC)(fp ? 3 : S390X_CC_H); break;
  case IR_GT: case IR_UGT: failcc = (S390XCC)(fp ? 13 : S390X_CC_LE); break;
  case IR_EQ: failcc = S390X_CC_NE; break;
  case IR_NE: failcc = S390X_CC_E; break;
  default: asm_s390x_nyi(as, ir);
  }
  left = ra_alloc1(as, ir->op1, fp ? RSET_FPR : RSET_GPR);
  right = ra_alloc1(as, ir->op2,
		    rset_exclude(fp ? RSET_FPR : RSET_GPR, left));
  asm_guard(as, failcc);
  if (fp)
    emit_rre(as, S390X_RRE_CDBR, left, right);
  else
    emit_rre(as, ir->o >= IR_ULT && ir->o <= IR_UGT ?
	     S390X_RRE_CLGR : S390X_RRE_CGR, left, right);
}
static void asm_equal(ASMState *as, IRIns *ir)
{ asm_comp(as, ir); }
static void asm_hiop(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }
static void asm_prof(ASMState *as, IRIns *ir)
{ asm_s390x_nyi(as, ir); }

/* -- Stack, loop and trace linkage ------------------------------------- */

static void asm_stack_check(ASMState *as, BCReg topslot, IRIns *irp,
			    RegSet allow, ExitNo exitno)
{
  Reg pbase = RID_BASE;
  int temp = 0;
  if (irp) {
    pbase = irp->r;
    if (!ra_hasreg(pbase)) {
      if (allow) {
	pbase = rset_pickbot(allow);
	temp = 1;
      } else {
	pbase = RID_LR;
	temp = 1;
      }
    }
  }
  emit_branch(as, S390X_CC_L, as->mcexit + 5*exitno);
  emit_rre(as, S390X_RRE_CLGR, RID_TMP, RID_TMP2);
  emit_ril(as, S390X_RIL_AGFI, RID_TMP2, 8*(int32_t)topslot);
  emit_rre(as, S390X_RRE_LGR, RID_TMP2, pbase);
  emit_mem(as, S390X_RXY_LG, RID_TMP, RID_TMP, offsetof(lua_State, maxstack));
  emit_getgl(as, RID_TMP, cur_L);
  if (temp) emit_getgl(as, pbase, jit_base);
}

static void asm_stack_restore(ASMState *as, SnapShot *snap)
{
  SnapEntry *map = &as->T->snapmap[snap->mapofs];
#ifdef LUA_USE_ASSERT
  SnapEntry *flinks = &as->T->snapmap[snap_nextofs(as->T, snap)-1-LJ_FR2];
#endif
  MSize n;
  for (n = 0; n < snap->nent; n++) {
    SnapEntry sn = map[n];
    BCReg s = snap_slot(sn);
    int32_t ofs = 8*((int32_t)s-1-LJ_FR2);
    IRRef ref = snap_ref(sn);
    IRIns *ir = IR(ref);
    if (sn & SNAP_NORESTORE) continue;
    if (sn & SNAP_KEYINDEX) {
      if (irref_isk(ref)) {
	uint64_t tv = ((uint64_t)(uint32_t)LJ_KEYINDEX << 32) |
		      (uint32_t)ir->i;
	emit_mem(as, S390X_RXY_STG, RID_TMP, RID_BASE, ofs);
	emit_loadu64(as, RID_TMP, tv);
      } else {
	Reg src = ra_alloc1(as, ref, rset_exclude(RSET_GPR, RID_BASE));
	emit_mem(as, S390X_RXY_STG, RID_TMP, RID_BASE, ofs);
	emit_ril(as, S390X_RIL_IIHF, RID_TMP, (int32_t)LJ_KEYINDEX);
	emit_rre(as, S390X_RRE_LLGFR, RID_TMP, src);
      }
    } else if (irt_isnum(ir->t)) {
      emit_mem(as, S390X_RXY_STDY, ra_alloc1(as, ref, RSET_FPR),
	       RID_BASE, ofs);
    } else {
      asm_tvstore64(as, RID_BASE, ofs, ref);
    }
    checkmclim(as);
  }
#ifdef LUA_USE_ASSERT
  lj_assertA(map + snap->nent == flinks, "inconsistent frames in snapshot");
#endif
}
static void asm_gc_check(ASMState *as)
{ asm_s390x_nyi(as, IR(as->curins)); }

static void asm_loop_fixup(ASMState *as)
{
  MCode *p = as->mctail;
  int32_t spadj = as->T->spadjust;
  p[0] = (uint16_t)(0xc208 | (RID_SP << 4));
  p[1] = (uint16_t)((uint32_t)spadj >> 16);
  p[2] = (uint16_t)spadj;
  p[3] = (uint16_t)(S390X_RIL_BRCL | (S390X_CC_ALWAYS << 4));
  {
    int32_t delta = (int32_t)(as->mcp - (p+3));
    p[4] = (uint16_t)((uint32_t)delta >> 16);
    p[5] = (uint16_t)delta;
  }
}

static void asm_loop_tail_fixup(ASMState *as)
{ UNUSED(as); }

static void asm_head_root_base(ASMState *as)
{
  IRIns *ir = IR(REF_BASE);
  Reg r = ir->r;
  if (ra_hasreg(r)) {
    ra_free(as, r);
    if (rset_test(as->modset, r) || irt_ismarked(ir->t)) ir->r = RID_INIT;
    if (r != RID_BASE) emit_rre(as, S390X_RRE_LGR, r, RID_BASE);
  }
}

static Reg asm_head_side_base(ASMState *as, IRIns *irp)
{
  IRIns *ir = IR(REF_BASE);
  Reg r = ir->r;
  if (ra_hasreg(r)) {
    ra_free(as, r);
    if (rset_test(as->modset, r) || irt_ismarked(ir->t)) ir->r = RID_INIT;
    if (irp->r == r) return r;
    if (ra_hasreg(irp->r) && rset_test(as->freeset, irp->r)) {
      emit_rre(as, S390X_RRE_LGR, r, irp->r);
      return irp->r;
    }
    emit_getgl(as, r, jit_base);
  }
  return RID_NONE;
}

static void asm_tail_fixup(ASMState *as, TraceNo lnk)
{
  MCode *target = lnk ? traceref(as->J, lnk)->mcode :
			(MCode *)(void *)lj_vm_exit_interp;
  MCode *p = as->mctail;
  int32_t spadj = as->T->spadjust;
  p[0] = (uint16_t)(0xc208 | (RID_SP << 4));
  p[1] = (uint16_t)((uint32_t)spadj >> 16);
  p[2] = (uint16_t)spadj;
  p[3] = (uint16_t)(S390X_RIL_BRCL | (S390X_CC_ALWAYS << 4));
  {
    int32_t delta = (int32_t)(target - (p+3));
    p[4] = (uint16_t)((uint32_t)delta >> 16);
    p[5] = (uint16_t)delta;
  }
}

static void asm_tail_prep(ASMState *as, TraceNo lnk)
{
  UNUSED(lnk);
  as->mcp = as->mctop - 6;
  as->mctail = as->mcp;
  as->invmcp = NULL;
}

static Reg asm_setup_call_slots(ASMState *as, IRIns *ir,
				const CCallInfo *ci)
{
  UNUSED(as); UNUSED(ci);
  return irt_isfp(ir->t) ? REGSP_HINT(RID_FPRET) : REGSP_HINT(RID_RET);
}

static void asm_setup_target(ASMState *as)
{
  asm_exitstub_setup(as, as->T->nsnap + (as->parent ? 1 : 0));
}

void lj_asm_patchexit(jit_State *J, GCtrace *T, ExitNo exitno, MCode *target)
{
  MCode *p = T->mcode, *pe = (MCode *)((char *)p + T->szmcode);
  MCode *px = exitstub_trace_addr(T, exitno);
  MCode *mcarea = lj_mcode_patch(J, p, 0);
  for (; p + 3 <= pe; p++) {
    if ((p[0] & 0xff0f) == S390X_RIL_BRCL) {
      int32_t delta = (int32_t)(((uint32_t)p[1] << 16) | p[2]);
      if (p + delta == px) {
	int64_t ndelta = target - p;
	if (ndelta >= INT32_MIN && ndelta <= INT32_MAX) {
	  p[0] = (uint16_t)(S390X_RIL_BRCL | (S390X_CC_ALWAYS << 4));
	  p[1] = (uint16_t)((uint64_t)ndelta >> 16);
	  p[2] = (uint16_t)ndelta;
	}
      }
    }
  }
  lj_mcode_sync(T->mcode, pe);
  lj_mcode_patch(J, mcarea, 1);
}
