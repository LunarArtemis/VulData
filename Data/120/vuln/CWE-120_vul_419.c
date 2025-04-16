static int
compilePassOpcode(FileInfo *nested, TranslationTableOpcode opcode,
		CharacterClass *characterClasses, TranslationTableOffset *newRuleOffset,
		TranslationTableRule **newRule, int noback, int nofor, RuleName *ruleNames,
		TranslationTableHeader **table) {
	static CharsString passRuleChars;
	static CharsString passRuleDots;
	/* Compile the operands of a pass opcode */
	widechar passSubOp;
	const CharacterClass *class;
	TranslationTableOffset ruleOffset = 0;
	TranslationTableRule *rule = NULL;
	int k;
	int kk = 0;
	pass_Codes passCode;
	int endTest = 0;
	int isScript = 1;
	widechar *passInstructions = passRuleDots.chars;
	int passIC = 0; /* Instruction counter */
	passRuleChars.length = 0;
	FileInfo *passNested = nested;
	TranslationTableOpcode passOpcode = opcode;
	CharsString passHoldString;
	widechar passHoldNumber;
	CharsString passLine;
	int passLinepos = 0;
	int passPrevLinepos;
	TranslationTableCharacterAttributes passAttributes;
	passHoldString.length = 0;
	for (k = nested->linepos; k < nested->linelen; k++)
		passHoldString.chars[passHoldString.length++] = nested->line[k];
	if (!eqasc2uni((unsigned char *)"script", passHoldString.chars, 6)) {
		isScript = 0;
#define SEPCHAR 0x0001
		for (k = 0; k < passHoldString.length && passHoldString.chars[k] > 32; k++)
			;
		if (k < passHoldString.length)
			passHoldString.chars[k] = SEPCHAR;
		else {
			compileError(passNested, "Invalid multipass operands");
			return 0;
		}
	}
	parseChars(passNested, &passLine, &passHoldString);
	if (isScript) {
		int more = 1;
		passCode = passGetScriptToken(&passLine, &passLinepos, &passPrevLinepos,
				&passHoldString, &passHoldNumber, passNested, *table);
		if (passCode != pass_script) {
			compileError(passNested, "Invalid multipass statement");
			return 0;
		}
		/* Declaratives */
		while (more) {
			passCode = passGetScriptToken(&passLine, &passLinepos, &passPrevLinepos,
					&passHoldString, &passHoldNumber, passNested, *table);
			switch (passCode) {
			case pass_define:
				if (!passIsLeftParen(&passLine, &passLinepos, &passPrevLinepos,
							&passHoldString, passNested, *table))
					return 0;
				if (!passIsName(&passLine, &passLinepos, &passPrevLinepos,
							&passHoldString, passNested, *table))
					return 0;
				if (!passIsComma(&passLine, &passLinepos, &passPrevLinepos,
							&passHoldString, passNested, *table))
					return 0;
				if (!passIsNumber(&passLine, &passLinepos, &passPrevLinepos,
							&passHoldString, &passHoldNumber, passNested, *table))
					return 0;
				if (!passIsRightParen(&passLine, &passLinepos, &passPrevLinepos,
							&passHoldString, passNested, *table))
					return 0;
				passAddName(&passHoldString, passHoldNumber, &passOpcode);
				break;
			case pass_if:
				more = 0;
				break;
			default:
				compileError(passNested, "invalid definition in declarative part");
				return 0;
			}
		}
		/* if part */
		more = 1;
		while (more) {
			passCode = passGetScriptToken(&passLine, &passLinepos, &passPrevLinepos,
					&passHoldString, &passHoldNumber, passNested, *table);
			passSubOp = passCode;
			switch (passCode) {
			case pass_not:
				passInstructions[passIC++] = pass_not;
				break;
			case pass_first:
				passInstructions[passIC++] = pass_first;
				break;
			case pass_last:
				passInstructions[passIC++] = pass_last;
				break;
			case pass_search:
				passInstructions[passIC++] = pass_search;
				break;
			case pass_string:
				if (!verifyStringOrDots(nested, opcode, 1, 0, nofor)) {
					return 0;
				}
				passInstructions[passIC++] = pass_string;
				goto ifDoCharsDots;
			case pass_dots:
				if (!verifyStringOrDots(nested, opcode, 0, 0, nofor)) {
					return 0;
				}
				passInstructions[passIC++] = pass_dots;
			ifDoCharsDots:
				passInstructions[passIC++] = passHoldString.length;
				for (kk = 0; kk < passHoldString.length; kk++)
					passInstructions[passIC++] = passHoldString.chars[kk];
				break;
			case pass_attributes:
				if (!passIsLeftParen(&passLine, &passLinepos, &passPrevLinepos,
							&passHoldString, passNested, *table))
					return 0;
				if (!passGetAttributes(
							&passLine, &passLinepos, &passAttributes, passNested))
					return 0;
				if (!passInsertAttributes(&passLine, &passLinepos, &passPrevLinepos,
							&passHoldString, &passAttributes, passNested,
							passInstructions, &passIC, *table))
					return 0;
				break;
			case pass_emphasis:
				if (!passIsLeftParen(&passLine, &passLinepos, &passPrevLinepos,
							&passHoldString, passNested, *table))
					return 0;
				if (!passGetEmphasis(&passLine, &passLinepos, passNested)) return 0;
				/* Right parenthis handled by subfunctiion */
				break;
			case pass_lookback:
				passInstructions[passIC++] = pass_lookback;
				passCode = passGetScriptToken(&passLine, &passLinepos, &passPrevLinepos,
						&passHoldString, &passHoldNumber, passNested, *table);
				if (passCode != pass_leftParen) {
					passInstructions[passIC++] = 1;
					passLinepos = passPrevLinepos;
					break;
				}
				if (!passIsNumber(&passLine, &passLinepos, &passPrevLinepos,
							&passHoldString, &passHoldNumber, passNested, *table))
					return 0;
				if (!passIsRightParen(&passLine, &passLinepos, &passPrevLinepos,
							&passHoldString, passNested, *table))
					return 0;
				passInstructions[passIC] = passHoldNumber;
				break;
			case pass_group:
				if (!passIsLeftParen(&passLine, &passLinepos, &passPrevLinepos,
							&passHoldString, passNested, *table))
					return 0;
				break;
			case pass_mark:
				passInstructions[passIC++] = pass_startReplace;
				passInstructions[passIC++] = pass_endReplace;
				break;
			case pass_replace:
				passInstructions[passIC++] = pass_startReplace;
				if (!passIsLeftParen(&passLine, &passLinepos, &passPrevLinepos,
							&passHoldString, passNested, *table))
					return 0;
				break;
			case pass_rightParen:
				passInstructions[passIC++] = pass_endReplace;
				break;
			case pass_groupstart:
			case pass_groupend:
				if (!passIsLeftParen(&passLine, &passLinepos, &passPrevLinepos,
							&passHoldString, passNested, *table))
					return 0;
				if (!passGetName(
							&passLine, &passLinepos, &passHoldString, passNested, *table))
					return 0;
				if (!passIsRightParen(&passLine, &passLinepos, &passPrevLinepos,
							&passHoldString, passNested, *table))
					return 0;
				ruleOffset = findRuleName(&passHoldString, ruleNames);
				if (ruleOffset)
					rule = (TranslationTableRule *)&(*table)->ruleArea[ruleOffset];
				if (rule && rule->opcode == CTO_Grouping) {
					passInstructions[passIC++] = passSubOp;
					passInstructions[passIC++] = ruleOffset >> 16;
					passInstructions[passIC++] = ruleOffset & 0xffff;
					break;
				} else {
					compileError(passNested, "%s is not a grouping name",
							_lou_showString(
									&passHoldString.chars[0], passHoldString.length));
					return 0;
				}
				break;
			case pass_class:
				if (!passIsLeftParen(&passLine, &passLinepos, &passPrevLinepos,
							&passHoldString, passNested, *table))
					return 0;
				if (!passGetName(
							&passLine, &passLinepos, &passHoldString, passNested, *table))
					return 0;
				if (!passIsRightParen(&passLine, &passLinepos, &passPrevLinepos,
							&passHoldString, passNested, *table))
					return 0;
				if (!(class = findCharacterClass(&passHoldString, characterClasses)))
					return 0;
				passAttributes = class->attribute;
				passInsertAttributes(&passLine, &passLinepos, &passPrevLinepos,
						&passHoldString, &passAttributes, passNested, passInstructions,
						&passIC, *table);
				break;
			case pass_swap:
				ruleOffset = findRuleName(&passHoldString, ruleNames);
				if (!passIsLeftParen(&passLine, &passLinepos, &passPrevLinepos,
							&passHoldString, passNested, *table))
					return 0;
				if (!passGetName(
							&passLine, &passLinepos, &passHoldString, passNested, *table))
					return 0;
				if (!passIsRightParen(&passLine, &passLinepos, &passPrevLinepos,
							&passHoldString, passNested, *table))
					return 0;
				ruleOffset = findRuleName(&passHoldString, ruleNames);
				if (ruleOffset)
					rule = (TranslationTableRule *)&(*table)->ruleArea[ruleOffset];
				if (rule && (rule->opcode == CTO_SwapCc || rule->opcode == CTO_SwapCd ||
									rule->opcode == CTO_SwapDd)) {
					passInstructions[passIC++] = pass_swap;
					passInstructions[passIC++] = ruleOffset >> 16;
					passInstructions[passIC++] = ruleOffset & 0xffff;
					if (!passGetRange(&passLine, &passLinepos, &passPrevLinepos,
								&passHoldString, passNested, passInstructions, &passIC,
								*table))
						return 0;
					break;
				}
				compileError(passNested, "%s is not a swap name.",
						_lou_showString(&passHoldString.chars[0], passHoldString.length));
				return 0;
			case pass_nameFound:
				passHoldNumber = passFindName(&passHoldString, passNested, &passOpcode);
				passCode = passGetScriptToken(&passLine, &passLinepos, &passPrevLinepos,
						&passHoldString, &passHoldNumber, passNested, *table);
				if (!(passCode == pass_eq || passCode == pass_lt || passCode == pass_gt ||
							passCode == pass_noteq || passCode == pass_lteq ||
							passCode == pass_gteq)) {
					compileError(nested, "invalid comparison operator in if part");
					return 0;
				}
				passInstructions[passIC++] = passCode;
				passInstructions[passIC++] = passHoldNumber;
				if (!passIsNumber(&passLine, &passLinepos, &passPrevLinepos,
							&passHoldString, &passHoldNumber, passNested, *table))
					return 0;
				passInstructions[passIC++] = passHoldNumber;
				break;
			case pass_then:
				passInstructions[passIC++] = pass_endTest;
				more = 0;
				break;
			default:
				compileError(passNested, "invalid choice in if part");
				return 0;
			}
		}

		/* then part */
		more = 1;
		while (more) {
			passCode = passGetScriptToken(&passLine, &passLinepos, &passPrevLinepos,
					&passHoldString, &passHoldNumber, passNested, *table);
			passSubOp = passCode;
			switch (passCode) {
			case pass_string:
				if (!verifyStringOrDots(nested, opcode, 1, 1, nofor)) {
					return 0;
				}
				passInstructions[passIC++] = pass_string;
				goto thenDoCharsDots;
			case pass_dots:
				if (!verifyStringOrDots(nested, opcode, 0, 1, nofor)) {
					return 0;
				}
				passInstructions[passIC++] = pass_dots;
			thenDoCharsDots:
				passInstructions[passIC++] = passHoldString.length;
				for (kk = 0; kk < passHoldString.length; kk++)
					passInstructions[passIC++] = passHoldString.chars[kk];
				break;
			case pass_nameFound:
				passHoldNumber = passFindName(&passHoldString, passNested, &passOpcode);
				passCode = passGetScriptToken(&passLine, &passLinepos, &passPrevLinepos,
						&passHoldString, &passHoldNumber, passNested, *table);
				if (!(passCode == pass_plus || passCode == pass_hyphen ||
							passCode == pass_eq)) {
					compileError(nested, "Invalid variable operator in then part");
					return 0;
				}
				passInstructions[passIC++] = passCode;
				passInstructions[passIC++] = passHoldNumber;
				if (!passIsNumber(&passLine, &passLinepos, &passPrevLinepos,
							&passHoldString, &passHoldNumber, passNested, *table))
					return 0;
				passInstructions[passIC++] = passHoldNumber;
				break;
			case pass_copy:
				passInstructions[passIC++] = pass_copy;
				break;
			case pass_omit:
				passInstructions[passIC++] = pass_omit;
				break;
			case pass_swap:
				ruleOffset = findRuleName(&passHoldString, ruleNames);
				if (!passIsLeftParen(&passLine, &passLinepos, &passPrevLinepos,
							&passHoldString, passNested, *table))
					return 0;
				if (!passGetName(
							&passLine, &passLinepos, &passHoldString, passNested, *table))
					return 0;
				if (!passIsRightParen(&passLine, &passLinepos, &passPrevLinepos,
							&passHoldString, passNested, *table))
					return 0;
				ruleOffset = findRuleName(&passHoldString, ruleNames);
				if (ruleOffset)
					rule = (TranslationTableRule *)&(*table)->ruleArea[ruleOffset];
				if (rule && (rule->opcode == CTO_SwapCc || rule->opcode == CTO_SwapCd ||
									rule->opcode == CTO_SwapDd)) {
					passInstructions[passIC++] = pass_swap;
					passInstructions[passIC++] = ruleOffset >> 16;
					passInstructions[passIC++] = ruleOffset & 0xffff;
					if (!passGetRange(&passLine, &passLinepos, &passPrevLinepos,
								&passHoldString, passNested, passInstructions, &passIC,
								*table))
						return 0;
					break;
				}
				compileError(passNested, "%s is not a swap name.",
						_lou_showString(&passHoldString.chars[0], passHoldString.length));
				return 0;
			case pass_noMoreTokens:
				more = 0;
				break;
			default:
				compileError(passNested, "invalid action in then part");
				return 0;
			}
		}
	} else {
		/* Older machine-language-like "assembler". */

		/* Compile test part */
		for (k = 0; k < passLine.length && passLine.chars[k] != SEPCHAR; k++)
			;
		endTest = k;
		passLine.chars[endTest] = pass_endTest;
		passLinepos = 0;
		while (passLinepos <= endTest) {
			switch ((passSubOp = passLine.chars[passLinepos])) {
			case pass_lookback:
				passInstructions[passIC++] = pass_lookback;
				passLinepos++;
				passGetNumber(&passLine, &passLinepos, &passHoldNumber);
				if (passHoldNumber == 0) passHoldNumber = 1;
				passInstructions[passIC++] = passHoldNumber;
				break;
			case pass_not:
				passInstructions[passIC++] = pass_not;
				passLinepos++;
				break;
			case pass_first:
				passInstructions[passIC++] = pass_first;
				passLinepos++;
				break;
			case pass_last:
				passInstructions[passIC++] = pass_last;
				passLinepos++;
				break;
			case pass_search:
				passInstructions[passIC++] = pass_search;
				passLinepos++;
				break;
			case pass_string:
				if (!verifyStringOrDots(nested, opcode, 1, 0, nofor)) {
					return 0;
				}
				passLinepos++;
				passInstructions[passIC++] = pass_string;
				passGetString(&passLine, &passLinepos, &passHoldString, passNested);
				goto testDoCharsDots;
			case pass_dots:
				if (!verifyStringOrDots(nested, opcode, 0, 0, nofor)) {
					return 0;
				}
				passLinepos++;
				passInstructions[passIC++] = pass_dots;
				passGetDots(&passLine, &passLinepos, &passHoldString, passNested);
			testDoCharsDots:
				if (passHoldString.length == 0) return 0;
				passInstructions[passIC++] = passHoldString.length;
				for (kk = 0; kk < passHoldString.length; kk++)
					passInstructions[passIC++] = passHoldString.chars[kk];
				break;
			case pass_startReplace:
				passInstructions[passIC++] = pass_startReplace;
				passLinepos++;
				break;
			case pass_endReplace:
				passInstructions[passIC++] = pass_endReplace;
				passLinepos++;
				break;
			case pass_variable:
				passLinepos++;
				if (!passGetVariableNumber(
							nested, &passLine, &passLinepos, &passHoldNumber))
					return 0;
				switch (passLine.chars[passLinepos]) {
				case pass_eq:
					passInstructions[passIC++] = pass_eq;
					goto doComp;
				case pass_lt:
					if (passLine.chars[passLinepos + 1] == pass_eq) {
						passLinepos++;
						passInstructions[passIC++] = pass_lteq;
					} else
						passInstructions[passIC++] = pass_lt;
					goto doComp;
				case pass_gt:
					if (passLine.chars[passLinepos + 1] == pass_eq) {
						passLinepos++;
						passInstructions[passIC++] = pass_gteq;
					} else
						passInstructions[passIC++] = pass_gt;
				doComp:
					passInstructions[passIC++] = passHoldNumber;
					passLinepos++;
					passGetNumber(&passLine, &passLinepos, &passHoldNumber);
					passInstructions[passIC++] = passHoldNumber;
					break;
				default:
					compileError(passNested, "incorrect comparison operator");
					return 0;
				}
				break;
			case pass_attributes:
				passLinepos++;
				if (!passGetAttributes(
							&passLine, &passLinepos, &passAttributes, passNested))
					return 0;
			insertAttributes:
				passInstructions[passIC++] = pass_attributes;
				passInstructions[passIC++] = passAttributes >> 16;
				passInstructions[passIC++] = passAttributes & 0xffff;
			getRange:
				if (passLine.chars[passLinepos] == pass_until) {
					passLinepos++;
					passInstructions[passIC++] = 1;
					passInstructions[passIC++] = 0xffff;
					break;
				}
				passGetNumber(&passLine, &passLinepos, &passHoldNumber);
				if (passHoldNumber == 0) {
					passHoldNumber = passInstructions[passIC++] = 1;
					passInstructions[passIC++] = 1; /* This is not an error */
					break;
				}
				passInstructions[passIC++] = passHoldNumber;
				if (passLine.chars[passLinepos] != pass_hyphen) {
					passInstructions[passIC++] = passHoldNumber;
					break;
				}
				passLinepos++;
				passGetNumber(&passLine, &passLinepos, &passHoldNumber);
				if (passHoldNumber == 0) {
					compileError(passNested, "invalid range");
					return 0;
				}
				passInstructions[passIC++] = passHoldNumber;
				break;
			case pass_groupstart:
			case pass_groupend:
				passLinepos++;
				passGetName(&passLine, &passLinepos, &passHoldString, passNested, *table);
				ruleOffset = findRuleName(&passHoldString, ruleNames);
				if (ruleOffset)
					rule = (TranslationTableRule *)&(*table)->ruleArea[ruleOffset];
				if (rule && rule->opcode == CTO_Grouping) {
					passInstructions[passIC++] = passSubOp;
					passInstructions[passIC++] = ruleOffset >> 16;
					passInstructions[passIC++] = ruleOffset & 0xffff;
					break;
				} else {
					compileError(passNested, "%s is not a grouping name",
							_lou_showString(
									&passHoldString.chars[0], passHoldString.length));
					return 0;
				}
				break;
			case pass_swap:
				passGetName(&passLine, &passLinepos, &passHoldString, passNested, *table);
				if ((class = findCharacterClass(&passHoldString, characterClasses))) {
					passAttributes = class->attribute;
					goto insertAttributes;
				}
				ruleOffset = findRuleName(&passHoldString, ruleNames);
				if (ruleOffset)
					rule = (TranslationTableRule *)&(*table)->ruleArea[ruleOffset];
				if (rule && (rule->opcode == CTO_SwapCc || rule->opcode == CTO_SwapCd ||
									rule->opcode == CTO_SwapDd)) {
					passInstructions[passIC++] = pass_swap;
					passInstructions[passIC++] = ruleOffset >> 16;
					passInstructions[passIC++] = ruleOffset & 0xffff;
					goto getRange;
				}
				compileError(passNested, "%s is neither a class name nor a swap name.",
						_lou_showString(&passHoldString.chars[0], passHoldString.length));
				return 0;
			case pass_endTest:
				passInstructions[passIC++] = pass_endTest;
				passLinepos++;
				break;
			default:
				compileError(passNested, "incorrect operator '%c ' in test part",
						passLine.chars[passLinepos]);
				return 0;
			}

		} /* Compile action part */

		/* Compile action part */
		while (passLinepos < passLine.length && passLine.chars[passLinepos] <= 32)
			passLinepos++;
		while (passLinepos < passLine.length && passLine.chars[passLinepos] > 32) {
			switch ((passSubOp = passLine.chars[passLinepos])) {
			case pass_string:
				if (!verifyStringOrDots(nested, opcode, 1, 1, nofor)) {
					return 0;
				}
				passLinepos++;
				passInstructions[passIC++] = pass_string;
				passGetString(&passLine, &passLinepos, &passHoldString, passNested);
				goto actionDoCharsDots;
			case pass_dots:
				if (!verifyStringOrDots(nested, opcode, 0, 1, nofor)) {
					return 0;
				}
				passLinepos++;
				passGetDots(&passLine, &passLinepos, &passHoldString, passNested);
				passInstructions[passIC++] = pass_dots;
			actionDoCharsDots:
				if (passHoldString.length == 0) return 0;
				passInstructions[passIC++] = passHoldString.length;
				for (kk = 0; kk < passHoldString.length; kk++)
					passInstructions[passIC++] = passHoldString.chars[kk];
				break;
			case pass_variable:
				passLinepos++;
				if (!passGetVariableNumber(
							nested, &passLine, &passLinepos, &passHoldNumber))
					return 0;
				switch (passLine.chars[passLinepos]) {
				case pass_eq:
					passInstructions[passIC++] = pass_eq;
					passInstructions[passIC++] = passHoldNumber;
					passLinepos++;
					passGetNumber(&passLine, &passLinepos, &passHoldNumber);
					passInstructions[passIC++] = passHoldNumber;
					break;
				case pass_plus:
				case pass_hyphen:
					passInstructions[passIC++] = passLine.chars[passLinepos++];
					passInstructions[passIC++] = passHoldNumber;
					break;
				default:
					compileError(
							passNested, "incorrect variable operator in action part");
					return 0;
				}
				break;
			case pass_copy:
				passInstructions[passIC++] = pass_copy;
				passLinepos++;
				break;
			case pass_omit:
				passInstructions[passIC++] = pass_omit;
				passLinepos++;
				break;
			case pass_groupreplace:
			case pass_groupstart:
			case pass_groupend:
				passLinepos++;
				passGetName(&passLine, &passLinepos, &passHoldString, passNested, *table);
				ruleOffset = findRuleName(&passHoldString, ruleNames);
				if (ruleOffset)
					rule = (TranslationTableRule *)&(*table)->ruleArea[ruleOffset];
				if (rule && rule->opcode == CTO_Grouping) {
					passInstructions[passIC++] = passSubOp;
					passInstructions[passIC++] = ruleOffset >> 16;
					passInstructions[passIC++] = ruleOffset & 0xffff;
					break;
				}
				compileError(passNested, "%s is not a grouping name",
						_lou_showString(&passHoldString.chars[0], passHoldString.length));
				return 0;
			case pass_swap:
				passLinepos++;
				passGetName(&passLine, &passLinepos, &passHoldString, passNested, *table);
				ruleOffset = findRuleName(&passHoldString, ruleNames);
				if (ruleOffset)
					rule = (TranslationTableRule *)&(*table)->ruleArea[ruleOffset];
				if (rule && (rule->opcode == CTO_SwapCc || rule->opcode == CTO_SwapCd ||
									rule->opcode == CTO_SwapDd)) {
					passInstructions[passIC++] = pass_swap;
					passInstructions[passIC++] = ruleOffset >> 16;
					passInstructions[passIC++] = ruleOffset & 0xffff;
					break;
				}
				compileError(passNested, "%s is not a swap name.",
						_lou_showString(&passHoldString.chars[0], passHoldString.length));
				return 0;
				break;
			default:
				compileError(passNested, "incorrect operator in action part");
				return 0;
			}
		}
	}

	/* Analyze and add rule */
	passRuleDots.length = passIC;

	{
		widechar *characters;
		int length;
		int found = passFindCharacters(
				passNested, passInstructions, passRuleDots.length, &characters, &length);

		if (!found) return 0;

		if (characters) {
			for (k = 0; k < length; k += 1) passRuleChars.chars[k] = characters[k];
			passRuleChars.length = k;
		}
	}

	if (!addRule(passNested, opcode, &passRuleChars, &passRuleDots, 0, 0, newRuleOffset,
				newRule, noback, nofor, table))
		return 0;
	return 1;
}