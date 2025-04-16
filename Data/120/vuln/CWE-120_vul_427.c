STATIC SSize_t
S_study_chunk(pTHX_ RExC_state_t *pRExC_state, regnode **scanp,
                        SSize_t *minlenp, SSize_t *deltap,
			regnode *last,
			scan_data_t *data,
			I32 stopparen,
                        U32 recursed_depth,
			regnode_ssc *and_withp,
			U32 flags, U32 depth)
			/* scanp: Start here (read-write). */
			/* deltap: Write maxlen-minlen here. */
			/* last: Stop before this one. */
			/* data: string data about the pattern */
			/* stopparen: treat close N as END */
			/* recursed: which subroutines have we recursed into */
			/* and_withp: Valid if flags & SCF_DO_STCLASS_OR */
{
    dVAR;
    SSize_t final_minlen;
    /* There must be at least this number of characters to match */
    SSize_t min = 0;
    I32 pars = 0, code;
    regnode *scan = *scanp, *next;
    SSize_t delta = 0;
    int is_inf = (flags & SCF_DO_SUBSTR) && (data->flags & SF_IS_INF);
    int is_inf_internal = 0;		/* The studied chunk is infinite */
    I32 is_par = OP(scan) == OPEN ? ARG(scan) : 0;
    scan_data_t data_fake;
    SV *re_trie_maxbuff = NULL;
    regnode *first_non_open = scan;
    SSize_t stopmin = OPTIMIZE_INFTY;
    scan_frame *frame = NULL;
    DECLARE_AND_GET_RE_DEBUG_FLAGS;

    PERL_ARGS_ASSERT_STUDY_CHUNK;
    RExC_study_started= 1;

    Zero(&data_fake, 1, scan_data_t);

    if ( depth == 0 ) {
        while (first_non_open && OP(first_non_open) == OPEN)
            first_non_open=regnext(first_non_open);
    }


  fake_study_recurse:
    DEBUG_r(
        RExC_study_chunk_recursed_count++;
    );
    DEBUG_OPTIMISE_MORE_r(
    {
        Perl_re_indentf( aTHX_  "study_chunk stopparen=%ld recursed_count=%lu depth=%lu recursed_depth=%lu scan=%p last=%p",
            depth, (long)stopparen,
            (unsigned long)RExC_study_chunk_recursed_count,
            (unsigned long)depth, (unsigned long)recursed_depth,
            scan,
            last);
        if (recursed_depth) {
            U32 i;
            U32 j;
            for ( j = 0 ; j < recursed_depth ; j++ ) {
                for ( i = 0 ; i < (U32)RExC_total_parens ; i++ ) {
                    if (PAREN_TEST(j, i) && (!j || !PAREN_TEST(j - 1, i))) {
                        Perl_re_printf( aTHX_ " %d",(int)i);
                        break;
                    }
                }
                if ( j + 1 < recursed_depth ) {
                    Perl_re_printf( aTHX_  ",");
                }
            }
        }
        Perl_re_printf( aTHX_ "\n");
    }