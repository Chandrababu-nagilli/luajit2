/*
** IBM z/Architecture instruction emitter.
** Copyright (C) 2005-2026 Mike Pall. See Copyright Notice in luajit.h
*/

/* Instructions are stored as native 16-bit halfwords. This is intentional:
** s390x is big-endian and MCode is uint16_t for this target.
*/

/* -- Instruction formats ------------------------------------------------ */

static LJ_AINLINE void emit_hw1(ASMState *as, uint16_t h0)
{
  MCode *p = as->mcp - 1;
  p[0] = h0;
  as->mcp = p;
}

static LJ_AINLINE void emit_hw2(ASMState *as, uint16_t h0, uint16_t h1)
{
  MCode *p = as->mcp - 2;
  p[0] = h0; p[1] = h1;
  as->mcp = p;
}

static LJ_AINLINE void emit_hw3(ASMState *as, uint16_t h0, uint16_t h1,
				uint16_t h2)
{
  MCode *p = as->mcp - 3;
  p[0] = h0; p[1] = h1; p[2] = h2;
  as->mcp = p;
}

/* RR: 8-bit opcode, two registers. */
static void emit_rr(ASMState *as, uint8_t op, Reg r1, Reg r2)
{
  emit_hw1(as, (uint16_t)((op << 8) | ((r1 & 15) << 4) | (r2 & 15)));
}

/* RRE: 16-bit opcode, two registers. */
static void emit_rre(ASMState *as, uint16_t op, Reg r1, Reg r2)
{
  emit_hw2(as, op, (uint16_t)(((r1 & 15) << 4) | (r2 & 15)));
}

/* RI-a: a7/r/op/imm16. */
static void emit_ri(ASMState *as, uint8_t op, Reg r1, int32_t imm)
{
  emit_hw2(as, (uint16_t)(0xa700 | ((r1 & 15) << 4) | (op & 15)),
	   (uint16_t)imm);
}

/* RIL-a/b/c: op/r/op2/imm32. */
static void emit_ril(ASMState *as, uint16_t op, Reg r1, int32_t imm)
{
  emit_hw3(as, (uint16_t)(op | ((r1 & 15) << 4)),
	   (uint16_t)((uint32_t)imm >> 16), (uint16_t)imm);
}

/* RXY-a / RXE / RSY-a: op/r1/r3-or-x2/b2/d20/op2. */
static void emit_rxy(ASMState *as, uint16_t op, Reg r1, Reg rx, Reg base,
		     int32_t ofs)
{
  lj_assertA(ofs >= -524288 && ofs <= 524287,
	     "s390x displacement %d out of range", ofs);
  emit_hw3(as,
	   (uint16_t)((op & 0xff00) | ((r1 & 15) << 4) | (rx & 15)),
	   (uint16_t)(((base & 15) << 12) | ((uint32_t)ofs & 0x0fff)),
	   (uint16_t)((((uint32_t)ofs >> 12) & 0xff) << 8 | (op & 0xff)));
}

#define emit_mem(as, op, r, base, ofs) \
  emit_rxy((as), (op), (r), 0, (base), (ofs))

/* -- Opcodes used by the assembler ------------------------------------- */

#define S390X_RR_BCR	0x07
#define S390X_RR_BASR	0x0d
#define S390X_RR_LR	0x18
#define S390X_RR_AR	0x1a
#define S390X_RR_SR	0x1b
#define S390X_RR_NR	0x14
#define S390X_RR_OR	0x16
#define S390X_RR_XR	0x17

#define S390X_RRE_LTGR	0xb902
#define S390X_RRE_LCGR	0xb903
#define S390X_RRE_LGR	0xb904
#define S390X_RRE_LGFR	0xb914
#define S390X_RRE_LLGFR	0xb916
#define S390X_RRE_AGR	0xb908
#define S390X_RRE_SGR	0xb909
#define S390X_RRE_MSGR	0xb90c
#define S390X_RRE_DSGR	0xb90d
#define S390X_RRE_CGR	0xb920
#define S390X_RRE_CLGR	0xb921
#define S390X_RRE_NGR	0xb980
#define S390X_RRE_OGR	0xb981
#define S390X_RRE_XGR	0xb982
#define S390X_RRE_FLOGR	0xb983
#define S390X_RRE_LDEBR	0xb304
#define S390X_RRE_LEDBR	0xb344
#define S390X_RRE_CEBR	0xb309
#define S390X_RRE_CDBR	0xb319
#define S390X_RRE_AEBR	0xb30a
#define S390X_RRE_ADBR	0xb31a
#define S390X_RRE_SEBR	0xb30b
#define S390X_RRE_SDBR	0xb31b
#define S390X_RRE_MEEBR	0xb317
#define S390X_RRE_MDBR	0xb31c
#define S390X_RRE_DEBR	0xb30d
#define S390X_RRE_DDBR	0xb31d
#define S390X_RRE_LCEBR	0xb303
#define S390X_RRE_LCDBR	0xb313

#define S390X_RI_LGHI	0x9
#define S390X_RI_AGHI	0xb
#define S390X_RI_BRC	0x4

#define S390X_RIL_LGFI	0xc001
#define S390X_RIL_MSGFI	0xc200
#define S390X_RIL_AGFI	0xc208
#define S390X_RIL_CGFI	0xc20c
#define S390X_RIL_CLGFI	0xc20e
#define S390X_RIL_LLILF	0xc00f
#define S390X_RIL_IIHF	0xc008
#define S390X_RIL_NIHF	0xc00a
#define S390X_RIL_NILF	0xc00b
#define S390X_RIL_OIHF	0xc00c
#define S390X_RIL_OILF	0xc00d
#define S390X_RIL_BRASL	0xc005
#define S390X_RIL_BRCL	0xc004

#define S390X_RXY_LG	0xe304
#define S390X_RXY_LGF	0xe314
#define S390X_RXY_LLGF	0xe316
#define S390X_RXY_STG	0xe324
#define S390X_RXY_LY	0xe358
#define S390X_RXY_STY	0xe350
#define S390X_RXY_LDY	0xed65
#define S390X_RXY_STDY	0xed67
#define S390X_RXY_LEY	0xed64
#define S390X_RXY_STEY	0xed66
#define S390X_RXY_LAY	0xe371

#define S390X_RSY_SRAG	0xeb0a
#define S390X_RSY_SRLG	0xeb0c
#define S390X_RSY_SLLG	0xeb0d

#if LJ_64
static intptr_t get_k64val(ASMState *as, IRRef ref)
{
  IRIns *ir = IR(ref);
  if (ir->o == IR_KINT64) return (intptr_t)ir_kint64(ir)->u64;
  if (ir->o == IR_KGC) return (intptr_t)ir_kgc(ir);
  if (ir->o == IR_KPTR || ir->o == IR_KKPTR) return (intptr_t)ir_kptr(ir);
  lj_assertA(ir->o == IR_KINT || ir->o == IR_KNULL,
	     "bad 64 bit const IR op %d", ir->o);
  return ir->i;
}
#define get_kval(as, ref) get_k64val((as), (ref))
#else
#define get_kval(as, ref) (IR((ref))->i)
#endif

/* -- Constants and addresses ------------------------------------------- */

/* Prefer rematerialization of BASE/L over spills. */
#define emit_canremat(ref) ((ref) <= REF_BASE)

static void emit_loadu64(ASMState *as, Reg r, uint64_t u)
{
  if ((int64_t)u >= -32768 && (int64_t)u <= 32767) {
    emit_ri(as, S390X_RI_LGHI, r, (int32_t)u);
  } else if ((int64_t)u >= INT32_MIN && (int64_t)u <= INT32_MAX) {
    emit_ril(as, S390X_RIL_LGFI, r, (int32_t)u);
  } else {
    /* Emission is backwards: IIHF is emitted before the initial LLILF. */
    emit_ril(as, S390X_RIL_IIHF, r, (int32_t)(u >> 32));
    emit_ril(as, S390X_RIL_LLILF, r, (int32_t)u);
  }
}

static void emit_loadi(ASMState *as, Reg r, int32_t i)
{
  emit_loadu64(as, r, (uint64_t)(int64_t)i);
}

#define emit_loada(as, r, addr) emit_loadu64((as), (r), u64ptr((addr)))

static void emit_loadk64(ASMState *as, Reg r, IRIns *ir)
{
  if (r < RID_MAX_GPR) {
    emit_loadu64(as, r, ir_k64(ir)->u64);
  } else {
    /* Loading through r0 avoids a literal pool and is endian-neutral. */
    emit_rre(as, S390X_RRE_LDEBR, r, RID_TMP);
    emit_loadu64(as, RID_TMP, ir_k64(ir)->u64);
  }
}

static void emit_lsglptr(ASMState *as, uint16_t op, Reg r, int32_t ofs)
{
  emit_mem(as, op, r, RID_JGL, ofs);
}

#define emit_getgl(as, r, field) \
  emit_lsglptr((as), S390X_RXY_LG, (r), (int32_t)offsetof(global_State, field))
#define emit_setgl(as, r, field) \
  emit_lsglptr((as), S390X_RXY_STG, (r), (int32_t)offsetof(global_State, field))
#define emit_setvmstate(as, i) UNUSED(i)

/* -- Control flow ------------------------------------------------------- */

typedef MCode *MCLabel;
#define emit_label(as) ((as)->mcp)

static void emit_branch(ASMState *as, S390XCC cc, MCode *target)
{
  MCode *p = as->mcp - 3;
  ptrdiff_t delta = target - p;
  lj_assertA(delta >= INT32_MIN && delta <= INT32_MAX,
	     "s390x branch target out of range");
  emit_ril(as, S390X_RIL_BRCL, (Reg)cc, (int32_t)delta);
}

static void emit_jmp(ASMState *as, MCode *target)
{
  emit_branch(as, S390X_CC_ALWAYS, target);
}

static void emit_call(ASMState *as, void *target)
{
  MCode *p = as->mcp - 3;
  intptr_t delta = ((char *)target - (char *)p) >> 1;
  if (delta >= INT32_MIN && delta <= INT32_MAX) {
    emit_ril(as, S390X_RIL_BRASL, RID_LR, (int32_t)delta);
  } else {
    emit_rr(as, S390X_RR_BASR, RID_LR, RID_TMP2);
    emit_loadu64(as, RID_TMP2, u64ptr(target));
  }
}

/* -- Generic register and spill operations ----------------------------- */

static void emit_movrr(ASMState *as, IRIns *ir, Reg dst, Reg src)
{
  if (dst < RID_MAX_GPR)
    emit_rre(as, S390X_RRE_LGR, dst, src);
  else
    emit_rr(as, irt_isnum(ir->t) ? 0x28 : 0x38, dst, src);
}

static void emit_loadofs(ASMState *as, IRIns *ir, Reg r, Reg base, int32_t ofs)
{
  if (r < RID_MAX_GPR) {
    emit_mem(as, irt_is64(ir->t) ? S390X_RXY_LG : S390X_RXY_LGF,
	     r, base, ofs);
  } else {
    emit_mem(as, irt_isnum(ir->t) ? S390X_RXY_LDY : S390X_RXY_LEY,
	     r, base, ofs);
  }
}

static void emit_storeofs(ASMState *as, IRIns *ir, Reg r, Reg base, int32_t ofs)
{
  if (r < RID_MAX_GPR) {
    emit_mem(as, irt_is64(ir->t) ? S390X_RXY_STG : S390X_RXY_STY,
	     r, base, ofs);
  } else {
    emit_mem(as, irt_isnum(ir->t) ? S390X_RXY_STDY : S390X_RXY_STEY,
	     r, base, ofs);
  }
}

static void emit_addptr(ASMState *as, Reg r, int32_t ofs)
{
  if (!ofs) return;
  if (ofs >= -32768 && ofs <= 32767)
    emit_ri(as, S390X_RI_AGHI, r, ofs);
  else
    emit_ril(as, S390X_RIL_AGFI, r, ofs);
}

#define emit_spsub(as, ofs) emit_addptr((as), RID_SP, -(ofs))
