/*
** Definitions for IBM z/Architecture (s390x) CPUs.
** Copyright (C) 2005-2017 Mike Pall. See Copyright Notice in luajit.h
*/

#ifndef _LJ_TARGET_S390X_H
#define _LJ_TARGET_S390X_H

/* -- Registers IDs ------------------------------------------------------- */

#define GPRDEF(_) \
  _(R0) _(R1) _(R2) _(R3) _(R4) _(R5) _(R6) _(R7) \
  _(R8) _(R9) _(R10) _(R11) _(R12) _(R13) _(R14) _(R15)
#define FPRDEF(_) \
  _(F0) _(F1) _(F2) _(F3) \
  _(F4) _(F5) _(F6) _(F7) \
  _(F8) _(F9) _(F10) _(F11) \
  _(F12) _(F13) _(F14) _(F15) 
#define VRIDDEF(_)

#define RIDENUM(name)	RID_##name,

enum {
  GPRDEF(RIDENUM)		/* General-purpose registers (GPRs). */
  FPRDEF(RIDENUM)		/* Floating-point registers (FPRs). */
  RID_MAX,

  /* r0/r1 are reserved as instruction-emission temporaries. r14 is the
  ** link register and is used as a third temporary for long branches and
  ** calls. These choices match vm_s390x.dasc and the Linux s390x ELF ABI.
  */
  RID_TMP = RID_R0,
  RID_TMP2 = RID_R1,
  RID_LR = RID_R14,
  RID_SP = RID_R15,

  /* Calling conventions. */
  RID_RET = RID_R2,
  RID_RETHI = RID_R2,
  RID_RETLO = RID_R3,
  RID_FPRET = RID_F0,

  /* These definitions must match with the *.dasc file(s): */
  RID_BASE = RID_R13,		/* Interpreter BASE. */
  RID_LPC = RID_R9,		/* Interpreter PC. */
  RID_DISPATCH = RID_R10,	/* Interpreter DISPATCH table. */
  RID_LREG = RID_R8,		/* Interpreter L (loaded on demand). */
  RID_JGL = RID_R11,		/* On-trace global_State anchor. */

  /* Register ranges [min, max) and number of registers. */
  RID_MIN_GPR = RID_R0,
  RID_MIN_FPR = RID_F0,
  RID_MAX_GPR = RID_R15+1,
  RID_MAX_FPR = RID_MAX,
  RID_NUM_GPR = RID_MAX_GPR - RID_MIN_GPR,
  RID_NUM_FPR = RID_MAX_FPR - RID_MIN_FPR
};

#define RID_NUM_KREF		RID_NUM_GPR
#define RID_MIN_KREF		RID_R0

/* -- Register sets ------------------------------------------------------- */

/* r12 is the ABI GOT register. Keep it fixed even though non-PIC code often
** leaves it unused; generated traces may call into PIC C code. r13 remains
** allocatable because BASE can be rematerialized like on the other targets.
*/
#define RSET_FIXED \
  (RID2RSET(RID_TMP)|RID2RSET(RID_TMP2)|RID2RSET(RID_R12)|\
   RID2RSET(RID_LR)|RID2RSET(RID_SP)|RID2RSET(RID_JGL))
#define RSET_GPR	(RSET_RANGE(RID_MIN_GPR, RID_MAX_GPR) - RSET_FIXED)
#define RSET_FPR	RSET_RANGE(RID_MIN_FPR, RID_MAX_FPR)
#define RSET_ALL	(RSET_GPR|RSET_FPR)
#define RSET_INIT	RSET_ALL

/* Linux s390x ELF: r0-r5 and r14, plus f0-f7, are call-clobbered. r6 is an
** argument register, but is callee-saved and therefore is not in SCRATCH.
*/
#define RSET_SCRATCH_GPR \
  (RSET_RANGE(RID_R0, RID_R5+1)|RID2RSET(RID_R14))
#define RSET_SCRATCH_FPR	RSET_RANGE(RID_F0, RID_F7+1)
#define RSET_SCRATCH		(RSET_SCRATCH_GPR|RSET_SCRATCH_FPR)

#define REGARG_FIRSTGPR	RID_R2
#define REGARG_LASTGPR		RID_R6
#define REGARG_NUMGPR		5
#define REGARG_FIRSTFPR	RID_F0
#define REGARG_LASTFPR		RID_F6
#define REGARG_NUMFPR		4

/* -- Spill slots --------------------------------------------------------- */

/* Spill slots are 32 bit wide. An even/odd pair is used for FPRs.
**
** SPS_FIXED: Available fixed spill slots in interpreter frame.
** This definition must match with the *.dasc file(s).
**
** SPS_FIRST: First spill slot for general use. Reserve min. two 32 bit slots.
*/
#define SPS_FIXED	2
#define SPS_FIRST	2

#define SPOFS_TMP	0

#define sps_scale(slot)		(4 * (int32_t)(slot))
#define sps_align(slot)		(((slot) - SPS_FIXED + 1) & ~1)

/* -- Exit state ---------------------------------------------------------- */

/* This definition must match with the *.dasc file(s). */
typedef struct {
  lua_Number fpr[RID_NUM_FPR];	/* Floating-point registers. */
  intptr_t gpr[RID_NUM_GPR];	/* General-purpose registers. */
  int32_t spill[256];		/* Spill slots. */
} ExitState;

/* Highest exit + 1 indicates stack check. */
#define EXITSTATE_CHECKEXIT	1

/* -- Instructions -------------------------------------------------------- */

/* The s390x backend emits 16-bit instruction halfwords. Instructions are
** always halfword-aligned and are two, four or six bytes long.
*/
typedef uint16_t S390XIns;

typedef enum {
  S390X_CC_O = 1,
  S390X_CC_H = 2,
  S390X_CC_L = 4,
  S390X_CC_NE = 7,
  S390X_CC_E = 8,
  S390X_CC_HE = 10,
  S390X_CC_LE = 12,
  S390X_CC_NO = 14,
  S390X_CC_ALWAYS = 15
} S390XCC;

#endif
