static void ps_mix_phase(ps_info *ps, qmf_t X_left[38][64], qmf_t X_right[38][64],
                         qmf_t X_hybrid_left[32][32], qmf_t X_hybrid_right[32][32])
{
    uint8_t n;
    uint8_t gr;
    uint8_t bk = 0;
    uint8_t sb, maxsb;
    uint8_t env;
    uint8_t nr_ipdopd_par;
    complex_t h11, h12, h21, h22;
    complex_t H11, H12, H21, H22;
    complex_t deltaH11, deltaH12, deltaH21, deltaH22;
    complex_t tempLeft;
    complex_t tempRight;
    complex_t phaseLeft;
    complex_t phaseRight;
    real_t L;
    const real_t *sf_iid;
    uint8_t no_iid_steps;

    if (ps->iid_mode >= 3)
    {
        no_iid_steps = 15;
        sf_iid = sf_iid_fine;
    } else {
        no_iid_steps = 7;
        sf_iid = sf_iid_normal;
    }

    if (ps->ipd_mode == 0 || ps->ipd_mode == 3)
    {
        nr_ipdopd_par = 11; /* resolution */
    } else {
        nr_ipdopd_par = ps->nr_ipdopd_par;
    }

    for (gr = 0; gr < ps->num_groups; gr++)
    {
        bk = (~NEGATE_IPD_MASK) & ps->map_group2bk[gr];

        /* use one channel per group in the subqmf domain */
        maxsb = (gr < ps->num_hybrid_groups) ? ps->group_border[gr] + 1 : ps->group_border[gr + 1];

        for (env = 0; env < ps->num_env; env++)
        {
            if (ps->icc_mode < 3)
            {
                /* type 'A' mixing as described in 8.6.4.6.2.1 */
                real_t c_1, c_2;
                real_t cosa, sina;
                real_t cosb, sinb;
                real_t ab1, ab2;
                real_t ab3, ab4;

                /*
                c_1 = sqrt(2.0 / (1.0 + pow(10.0, quant_iid[no_iid_steps + iid_index] / 10.0)));
                c_2 = sqrt(2.0 / (1.0 + pow(10.0, quant_iid[no_iid_steps - iid_index] / 10.0)));
                alpha = 0.5 * acos(quant_rho[icc_index]);
                beta = alpha * ( c_1 - c_2 ) / sqrt(2.0);
                */

                //printf("%d\n", ps->iid_index[env][bk]);

                /* calculate the scalefactors c_1 and c_2 from the intensity differences */
                c_1 = sf_iid[no_iid_steps + ps->iid_index[env][bk]];
                c_2 = sf_iid[no_iid_steps - ps->iid_index[env][bk]];

                /* calculate alpha and beta using the ICC parameters */
                cosa = cos_alphas[ps->icc_index[env][bk]];
                sina = sin_alphas[ps->icc_index[env][bk]];

                if (ps->iid_mode >= 3)
                {
                    if (ps->iid_index[env][bk] < 0)
                    {
                        cosb =  cos_betas_fine[-ps->iid_index[env][bk]][ps->icc_index[env][bk]];
                        sinb = -sin_betas_fine[-ps->iid_index[env][bk]][ps->icc_index[env][bk]];
                    } else {
                        cosb = cos_betas_fine[ps->iid_index[env][bk]][ps->icc_index[env][bk]];
                        sinb = sin_betas_fine[ps->iid_index[env][bk]][ps->icc_index[env][bk]];
                    }
                } else {
                    if (ps->iid_index[env][bk] < 0)
                    {
                        cosb =  cos_betas_normal[-ps->iid_index[env][bk]][ps->icc_index[env][bk]];
                        sinb = -sin_betas_normal[-ps->iid_index[env][bk]][ps->icc_index[env][bk]];
                    } else {
                        cosb = cos_betas_normal[ps->iid_index[env][bk]][ps->icc_index[env][bk]];
                        sinb = sin_betas_normal[ps->iid_index[env][bk]][ps->icc_index[env][bk]];
                    }
                }

                ab1 = MUL_C(cosb, cosa);
                ab2 = MUL_C(sinb, sina);
                ab3 = MUL_C(sinb, cosa);
                ab4 = MUL_C(cosb, sina);

                /* h_xy: COEF */
                RE(h11) = MUL_C(c_2, (ab1 - ab2));
                RE(h12) = MUL_C(c_1, (ab1 + ab2));
                RE(h21) = MUL_C(c_2, (ab3 + ab4));
                RE(h22) = MUL_C(c_1, (ab3 - ab4));
            } else {
                /* type 'B' mixing as described in 8.6.4.6.2.2 */
                real_t sina, cosa;
                real_t cosg, sing;

                /*
                real_t c, rho, mu, alpha, gamma;
                uint8_t i;

                i = ps->iid_index[env][bk];
                c = (real_t)pow(10.0, ((i)?(((i>0)?1:-1)*quant_iid[((i>0)?i:-i)-1]):0.)/20.0);
                rho = quant_rho[ps->icc_index[env][bk]];

                if (rho == 0.0f && c == 1.)
                {
                    alpha = (real_t)M_PI/4.0f;
                    rho = 0.05f;
                } else {
                    if (rho <= 0.05f)
                    {
                        rho = 0.05f;
                    }
                    alpha = 0.5f*(real_t)atan( (2.0f*c*rho) / (c*c-1.0f) );

                    if (alpha < 0.)
                    {
                        alpha += (real_t)M_PI/2.0f;
                    }
                    if (rho < 0.)
                    {
                        alpha += (real_t)M_PI;
                    }
                }
                mu = c+1.0f/c;
                mu = 1+(4.0f*rho*rho-4.0f)/(mu*mu);
                gamma = (real_t)atan(sqrt((1.0f-sqrt(mu))/(1.0f+sqrt(mu))));
                */

                if (ps->iid_mode >= 3)
                {
                    uint8_t abs_iid = abs(ps->iid_index[env][bk]);

                    cosa = sincos_alphas_B_fine[no_iid_steps + ps->iid_index[env][bk]][ps->icc_index[env][bk]];
                    sina = sincos_alphas_B_fine[30 - (no_iid_steps + ps->iid_index[env][bk])][ps->icc_index[env][bk]];
                    cosg = cos_gammas_fine[abs_iid][ps->icc_index[env][bk]];
                    sing = sin_gammas_fine[abs_iid][ps->icc_index[env][bk]];
                } else {
                    uint8_t abs_iid = abs(ps->iid_index[env][bk]);

                    cosa = sincos_alphas_B_normal[no_iid_steps + ps->iid_index[env][bk]][ps->icc_index[env][bk]];
                    sina = sincos_alphas_B_normal[14 - (no_iid_steps + ps->iid_index[env][bk])][ps->icc_index[env][bk]];
                    cosg = cos_gammas_normal[abs_iid][ps->icc_index[env][bk]];
                    sing = sin_gammas_normal[abs_iid][ps->icc_index[env][bk]];
                }

                RE(h11) = MUL_C(COEF_SQRT2, MUL_C(cosa, cosg));
                RE(h12) = MUL_C(COEF_SQRT2, MUL_C(sina, cosg));
                RE(h21) = MUL_C(COEF_SQRT2, MUL_C(-cosa, sing));
                RE(h22) = MUL_C(COEF_SQRT2, MUL_C(sina, sing));
            }

            /* calculate phase rotation parameters H_xy */
            /* note that the imaginary part of these parameters are only calculated when
               IPD and OPD are enabled
             */
            if ((ps->enable_ipdopd) && (bk < nr_ipdopd_par))
            {
                int8_t i;
                real_t xy, pq, xypq;

                /* ringbuffer index */
                i = ps->phase_hist;

                /* previous value */
#ifdef FIXED_POINT
                /* divide by 4, shift right 2 bits */
                RE(tempLeft)  = RE(ps->ipd_prev[bk][i]) >> 2;
                IM(tempLeft)  = IM(ps->ipd_prev[bk][i]) >> 2;
                RE(tempRight) = RE(ps->opd_prev[bk][i]) >> 2;
                IM(tempRight) = IM(ps->opd_prev[bk][i]) >> 2;
#else
                RE(tempLeft)  = MUL_F(RE(ps->ipd_prev[bk][i]), FRAC_CONST(0.25));
                IM(tempLeft)  = MUL_F(IM(ps->ipd_prev[bk][i]), FRAC_CONST(0.25));
                RE(tempRight) = MUL_F(RE(ps->opd_prev[bk][i]), FRAC_CONST(0.25));
                IM(tempRight) = MUL_F(IM(ps->opd_prev[bk][i]), FRAC_CONST(0.25));
#endif

                /* save current value */
                RE(ps->ipd_prev[bk][i]) = ipdopd_cos_tab[abs(ps->ipd_index[env][bk])];
                IM(ps->ipd_prev[bk][i]) = ipdopd_sin_tab[abs(ps->ipd_index[env][bk])];
                RE(ps->opd_prev[bk][i]) = ipdopd_cos_tab[abs(ps->opd_index[env][bk])];
                IM(ps->opd_prev[bk][i]) = ipdopd_sin_tab[abs(ps->opd_index[env][bk])];

                /* add current value */
                RE(tempLeft)  += RE(ps->ipd_prev[bk][i]);
                IM(tempLeft)  += IM(ps->ipd_prev[bk][i]);
                RE(tempRight) += RE(ps->opd_prev[bk][i]);
                IM(tempRight) += IM(ps->opd_prev[bk][i]);

                /* ringbuffer index */
                if (i == 0)
                {
                    i = 2;
                }
                i--;

                /* get value before previous */
#ifdef FIXED_POINT
                /* dividing by 2, shift right 1 bit */
                RE(tempLeft)  += (RE(ps->ipd_prev[bk][i]) >> 1);
                IM(tempLeft)  += (IM(ps->ipd_prev[bk][i]) >> 1);
                RE(tempRight) += (RE(ps->opd_prev[bk][i]) >> 1);
                IM(tempRight) += (IM(ps->opd_prev[bk][i]) >> 1);
#else
                RE(tempLeft)  += MUL_F(RE(ps->ipd_prev[bk][i]), FRAC_CONST(0.5));
                IM(tempLeft)  += MUL_F(IM(ps->ipd_prev[bk][i]), FRAC_CONST(0.5));
                RE(tempRight) += MUL_F(RE(ps->opd_prev[bk][i]), FRAC_CONST(0.5));
                IM(tempRight) += MUL_F(IM(ps->opd_prev[bk][i]), FRAC_CONST(0.5));
#endif

#if 0 /* original code */
                ipd = (float)atan2(IM(tempLeft), RE(tempLeft));
                opd = (float)atan2(IM(tempRight), RE(tempRight));

                /* phase rotation */
                RE(phaseLeft) = (float)cos(opd);
                IM(phaseLeft) = (float)sin(opd);
                opd -= ipd;
                RE(phaseRight) = (float)cos(opd);
                IM(phaseRight) = (float)sin(opd);
#else

                // x = IM(tempLeft)
                // y = RE(tempLeft)
                // p = IM(tempRight)
                // q = RE(tempRight)
                // cos(atan2(x,y)) = y/sqrt((x*x) + (y*y))
                // sin(atan2(x,y)) = x/sqrt((x*x) + (y*y))
                // cos(atan2(x,y)-atan2(p,q)) = (y*q + x*p) / ( sqrt((x*x) + (y*y)) * sqrt((p*p) + (q*q)) );
                // sin(atan2(x,y)-atan2(p,q)) = (x*q - y*p) / ( sqrt((x*x) + (y*y)) * sqrt((p*p) + (q*q)) );

                xy = magnitude_c(tempRight);
                pq = magnitude_c(tempLeft);

                if (xy != 0)
                {
                    RE(phaseLeft) = DIV_R(RE(tempRight), xy);
                    IM(phaseLeft) = DIV_R(IM(tempRight), xy);
                } else {
                    RE(phaseLeft) = 0;
                    IM(phaseLeft) = 0;
                }

                xypq = MUL_R(xy, pq);

                if (xypq != 0)
                {
                    real_t tmp1 = MUL_R(RE(tempRight), RE(tempLeft)) + MUL_R(IM(tempRight), IM(tempLeft));
                    real_t tmp2 = MUL_R(IM(tempRight), RE(tempLeft)) - MUL_R(RE(tempRight), IM(tempLeft));

                    RE(phaseRight) = DIV_R(tmp1, xypq);
                    IM(phaseRight) = DIV_R(tmp2, xypq);
                } else {
                    RE(phaseRight) = 0;
                    IM(phaseRight) = 0;
                }

#endif

                /* MUL_F(COEF, REAL) = COEF */
                IM(h11) = MUL_R(RE(h11), IM(phaseLeft));
                IM(h12) = MUL_R(RE(h12), IM(phaseRight));
                IM(h21) = MUL_R(RE(h21), IM(phaseLeft));
                IM(h22) = MUL_R(RE(h22), IM(phaseRight));

                RE(h11) = MUL_R(RE(h11), RE(phaseLeft));
                RE(h12) = MUL_R(RE(h12), RE(phaseRight));
                RE(h21) = MUL_R(RE(h21), RE(phaseLeft));
                RE(h22) = MUL_R(RE(h22), RE(phaseRight));
            }

            /* length of the envelope n_e+1 - n_e (in time samples) */
            /* 0 < L <= 32: integer */
            L = (real_t)(ps->border_position[env + 1] - ps->border_position[env]);

            /* obtain final H_xy by means of linear interpolation */
            RE(deltaH11) = (RE(h11) - RE(ps->h11_prev[gr])) / L;
            RE(deltaH12) = (RE(h12) - RE(ps->h12_prev[gr])) / L;
            RE(deltaH21) = (RE(h21) - RE(ps->h21_prev[gr])) / L;
            RE(deltaH22) = (RE(h22) - RE(ps->h22_prev[gr])) / L;

            RE(H11) = RE(ps->h11_prev[gr]);
            RE(H12) = RE(ps->h12_prev[gr]);
            RE(H21) = RE(ps->h21_prev[gr]);
            RE(H22) = RE(ps->h22_prev[gr]);

            RE(ps->h11_prev[gr]) = RE(h11);
            RE(ps->h12_prev[gr]) = RE(h12);
            RE(ps->h21_prev[gr]) = RE(h21);
            RE(ps->h22_prev[gr]) = RE(h22);

            /* only calculate imaginary part when needed */
            if ((ps->enable_ipdopd) && (bk < nr_ipdopd_par))
            {
                /* obtain final H_xy by means of linear interpolation */
                IM(deltaH11) = (IM(h11) - IM(ps->h11_prev[gr])) / L;
                IM(deltaH12) = (IM(h12) - IM(ps->h12_prev[gr])) / L;
                IM(deltaH21) = (IM(h21) - IM(ps->h21_prev[gr])) / L;
                IM(deltaH22) = (IM(h22) - IM(ps->h22_prev[gr])) / L;

                IM(H11) = IM(ps->h11_prev[gr]);
                IM(H12) = IM(ps->h12_prev[gr]);
                IM(H21) = IM(ps->h21_prev[gr]);
                IM(H22) = IM(ps->h22_prev[gr]);

                if ((NEGATE_IPD_MASK & ps->map_group2bk[gr]) != 0)
                {
                    IM(deltaH11) = -IM(deltaH11);
                    IM(deltaH12) = -IM(deltaH12);
                    IM(deltaH21) = -IM(deltaH21);
                    IM(deltaH22) = -IM(deltaH22);

                    IM(H11) = -IM(H11);
                    IM(H12) = -IM(H12);
                    IM(H21) = -IM(H21);
                    IM(H22) = -IM(H22);
                }

                IM(ps->h11_prev[gr]) = IM(h11);
                IM(ps->h12_prev[gr]) = IM(h12);
                IM(ps->h21_prev[gr]) = IM(h21);
                IM(ps->h22_prev[gr]) = IM(h22);
            }

            /* apply H_xy to the current envelope band of the decorrelated subband */
            for (n = ps->border_position[env]; n < ps->border_position[env + 1]; n++)
            {
                /* addition finalises the interpolation over every n */
                RE(H11) += RE(deltaH11);
                RE(H12) += RE(deltaH12);
                RE(H21) += RE(deltaH21);
                RE(H22) += RE(deltaH22);
                if ((ps->enable_ipdopd) && (bk < nr_ipdopd_par))
                {
                    IM(H11) += IM(deltaH11);
                    IM(H12) += IM(deltaH12);
                    IM(H21) += IM(deltaH21);
                    IM(H22) += IM(deltaH22);
                }

                /* channel is an alias to the subband */
                for (sb = ps->group_border[gr]; sb < maxsb; sb++)
                {
                    complex_t inLeft, inRight;

                    /* load decorrelated samples */
                    if (gr < ps->num_hybrid_groups)
                    {
                        RE(inLeft) =  RE(X_hybrid_left[n][sb]);
                        IM(inLeft) =  IM(X_hybrid_left[n][sb]);
                        RE(inRight) = RE(X_hybrid_right[n][sb]);
                        IM(inRight) = IM(X_hybrid_right[n][sb]);
                    } else {
                        RE(inLeft) =  RE(X_left[n][sb]);
                        IM(inLeft) =  IM(X_left[n][sb]);
                        RE(inRight) = RE(X_right[n][sb]);
                        IM(inRight) = IM(X_right[n][sb]);
                    }

                    /* apply mixing */
                    RE(tempLeft) =  MUL_C(RE(H11), RE(inLeft)) + MUL_C(RE(H21), RE(inRight));
                    IM(tempLeft) =  MUL_C(RE(H11), IM(inLeft)) + MUL_C(RE(H21), IM(inRight));
                    RE(tempRight) = MUL_C(RE(H12), RE(inLeft)) + MUL_C(RE(H22), RE(inRight));
                    IM(tempRight) = MUL_C(RE(H12), IM(inLeft)) + MUL_C(RE(H22), IM(inRight));

                    /* only perform imaginary operations when needed */
                    if ((ps->enable_ipdopd) && (bk < nr_ipdopd_par))
                    {
                        /* apply rotation */
                        RE(tempLeft)  -= MUL_C(IM(H11), IM(inLeft)) + MUL_C(IM(H21), IM(inRight));
                        IM(tempLeft)  += MUL_C(IM(H11), RE(inLeft)) + MUL_C(IM(H21), RE(inRight));
                        RE(tempRight) -= MUL_C(IM(H12), IM(inLeft)) + MUL_C(IM(H22), IM(inRight));
                        IM(tempRight) += MUL_C(IM(H12), RE(inLeft)) + MUL_C(IM(H22), RE(inRight));
                    }

                    /* store final samples */
                    if (gr < ps->num_hybrid_groups)
                    {
                        RE(X_hybrid_left[n][sb])  = RE(tempLeft);
                        IM(X_hybrid_left[n][sb])  = IM(tempLeft);
                        RE(X_hybrid_right[n][sb]) = RE(tempRight);
                        IM(X_hybrid_right[n][sb]) = IM(tempRight);
                    } else {
                        RE(X_left[n][sb])  = RE(tempLeft);
                        IM(X_left[n][sb])  = IM(tempLeft);
                        RE(X_right[n][sb]) = RE(tempRight);
                        IM(X_right[n][sb]) = IM(tempRight);
                    }
                }
            }

            /* shift phase smoother's circular buffer index */
            ps->phase_hist++;
            if (ps->phase_hist == 2)
            {
                ps->phase_hist = 0;
            }
        }
    }
}