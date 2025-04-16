static int check_alu_op(struct verifier_env *env, struct bpf_insn *insn)
{
	struct reg_state *regs = env->cur_state.regs;
	u8 opcode = BPF_OP(insn->code);
	int err;

	if (opcode == BPF_END || opcode == BPF_NEG) {
		if (opcode == BPF_NEG) {
			if (BPF_SRC(insn->code) != 0 ||
			    insn->src_reg != BPF_REG_0 ||
			    insn->off != 0 || insn->imm != 0) {
				verbose("BPF_NEG uses reserved fields\n");
				return -EINVAL;
			}
		} else {
			if (insn->src_reg != BPF_REG_0 || insn->off != 0 ||
			    (insn->imm != 16 && insn->imm != 32 && insn->imm != 64) ||
			    BPF_CLASS(insn->code) == BPF_ALU64) {
				verbose("BPF_END uses reserved fields\n");
				return -EINVAL;
			}
		}

		/* check src operand */
		err = check_reg_arg(regs, insn->dst_reg, SRC_OP);
		if (err)
			return err;

		if (is_pointer_value(env, insn->dst_reg)) {
			verbose("R%d pointer arithmetic prohibited\n",
				insn->dst_reg);
			return -EACCES;
		}

		/* check dest operand */
		err = check_reg_arg(regs, insn->dst_reg, DST_OP);
		if (err)
			return err;

	} else if (opcode == BPF_MOV) {

		if (BPF_SRC(insn->code) == BPF_X) {
			if (insn->imm != 0 || insn->off != 0) {
				verbose("BPF_MOV uses reserved fields\n");
				return -EINVAL;
			}

			/* check src operand */
			err = check_reg_arg(regs, insn->src_reg, SRC_OP);
			if (err)
				return err;
		} else {
			if (insn->src_reg != BPF_REG_0 || insn->off != 0) {
				verbose("BPF_MOV uses reserved fields\n");
				return -EINVAL;
			}
		}

		/* check dest operand */
		err = check_reg_arg(regs, insn->dst_reg, DST_OP);
		if (err)
			return err;

		if (BPF_SRC(insn->code) == BPF_X) {
			if (BPF_CLASS(insn->code) == BPF_ALU64) {
				/* case: R1 = R2
				 * copy register state to dest reg
				 */
				regs[insn->dst_reg] = regs[insn->src_reg];
			} else {
				if (is_pointer_value(env, insn->src_reg)) {
					verbose("R%d partial copy of pointer\n",
						insn->src_reg);
					return -EACCES;
				}
				regs[insn->dst_reg].type = UNKNOWN_VALUE;
				regs[insn->dst_reg].map_ptr = NULL;
			}
		} else {
			/* case: R = imm
			 * remember the value we stored into this reg
			 */
			regs[insn->dst_reg].type = CONST_IMM;
			regs[insn->dst_reg].imm = insn->imm;
		}

	} else if (opcode > BPF_END) {
		verbose("invalid BPF_ALU opcode %x\n", opcode);
		return -EINVAL;

	} else {	/* all other ALU ops: and, sub, xor, add, ... */

		bool stack_relative = false;

		if (BPF_SRC(insn->code) == BPF_X) {
			if (insn->imm != 0 || insn->off != 0) {
				verbose("BPF_ALU uses reserved fields\n");
				return -EINVAL;
			}
			/* check src1 operand */
			err = check_reg_arg(regs, insn->src_reg, SRC_OP);
			if (err)
				return err;
		} else {
			if (insn->src_reg != BPF_REG_0 || insn->off != 0) {
				verbose("BPF_ALU uses reserved fields\n");
				return -EINVAL;
			}
		}

		/* check src2 operand */
		err = check_reg_arg(regs, insn->dst_reg, SRC_OP);
		if (err)
			return err;

		if ((opcode == BPF_MOD || opcode == BPF_DIV) &&
		    BPF_SRC(insn->code) == BPF_K && insn->imm == 0) {
			verbose("div by zero\n");
			return -EINVAL;
		}

		if (opcode == BPF_ARSH && BPF_CLASS(insn->code) != BPF_ALU64) {
			verbose("BPF_ARSH not supported for 32 bit ALU\n");
			return -EINVAL;
		}

		if ((opcode == BPF_LSH || opcode == BPF_RSH ||
		     opcode == BPF_ARSH) && BPF_SRC(insn->code) == BPF_K) {
			int size = BPF_CLASS(insn->code) == BPF_ALU64 ? 64 : 32;

			if (insn->imm < 0 || insn->imm >= size) {
				verbose("invalid shift %d\n", insn->imm);
				return -EINVAL;
			}
		}

		/* pattern match 'bpf_add Rx, imm' instruction */
		if (opcode == BPF_ADD && BPF_CLASS(insn->code) == BPF_ALU64 &&
		    regs[insn->dst_reg].type == FRAME_PTR &&
		    BPF_SRC(insn->code) == BPF_K) {
			stack_relative = true;
		} else if (is_pointer_value(env, insn->dst_reg)) {
			verbose("R%d pointer arithmetic prohibited\n",
				insn->dst_reg);
			return -EACCES;
		} else if (BPF_SRC(insn->code) == BPF_X &&
			   is_pointer_value(env, insn->src_reg)) {
			verbose("R%d pointer arithmetic prohibited\n",
				insn->src_reg);
			return -EACCES;
		}

		/* check dest operand */
		err = check_reg_arg(regs, insn->dst_reg, DST_OP);
		if (err)
			return err;

		if (stack_relative) {
			regs[insn->dst_reg].type = PTR_TO_STACK;
			regs[insn->dst_reg].imm = insn->imm;
		}
	}

	return 0;
}