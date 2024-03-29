/* Galois.c
 * James S. Plank
 * April, 2007

Galois.tar - Fast Galois Field Arithmetic Library in C/C++
Copright (C) 2007 James S. Plank

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

James S. Plank
Department of Computer Science
University of Tennessee
Knoxville, TN 37996
plank@cs.utk.edu

 */

#include <stdexcept>
#include <string>

#include "fmt/core.h"
#include "fmt/format.h"
#include "galois.h"

constexpr unsigned NONE = 10;
constexpr unsigned TABLE = 11;
constexpr unsigned SHIFT = 12;
constexpr unsigned LOGS = 13;
constexpr unsigned SPLITW8 = 14;

static unsigned prim_poly[33] = {
    0,
    /*  1 */ 1,
    /*  2 */ 07,
    /*  3 */ 013,
    /*  4 */ 023,
    /*  5 */ 045,
    /*  6 */ 0103,
    /*  7 */ 0211,
    /*  8 */ 0435,
    /*  9 */ 01021,
    /* 10 */ 02011,
    /* 11 */ 04005,
    /* 12 */ 010123,
    /* 13 */ 020033,
    /* 14 */ 042103,
    /* 15 */ 0100003,
    /* 16 */ 0210013,
    /* 17 */ 0400011,
    /* 18 */ 01000201,
    /* 19 */ 02000047,
    /* 20 */ 04000011,
    /* 21 */ 010000005,
    /* 22 */ 020000003,
    /* 23 */ 040000041,
    /* 24 */ 0100000207,
    /* 25 */ 0200000011,
    /* 26 */ 0400000107,
    /* 27 */ 01000000047,
    /* 28 */ 02000000011,
    /* 29 */ 04000000005,
    /* 30 */ 010040000007,
    /* 31 */ 020000000011,
    /* 32 */ 00020000007}; /* Really 40020000007, but we're omitting the high
                              order bit */

static unsigned mult_type[33] = {NONE,
                                 /*  1 */ TABLE,
                                 /*  2 */ TABLE,
                                 /*  3 */ TABLE,
                                 /*  4 */ TABLE,
                                 /*  5 */ TABLE,
                                 /*  6 */ TABLE,
                                 /*  7 */ TABLE,
                                 /*  8 */ TABLE,
                                 /*  9 */ TABLE,
                                 /* 10 */ LOGS,
                                 /* 11 */ LOGS,
                                 /* 12 */ LOGS,
                                 /* 13 */ LOGS,
                                 /* 14 */ LOGS,
                                 /* 15 */ LOGS,
                                 /* 16 */ LOGS,
                                 /* 17 */ LOGS,
                                 /* 18 */ LOGS,
                                 /* 19 */ LOGS,
                                 /* 20 */ LOGS,
                                 /* 21 */ LOGS,
                                 /* 22 */ LOGS,
                                 /* 23 */ SHIFT,
                                 /* 24 */ SHIFT,
                                 /* 25 */ SHIFT,
                                 /* 26 */ SHIFT,
                                 /* 27 */ SHIFT,
                                 /* 28 */ SHIFT,
                                 /* 29 */ SHIFT,
                                 /* 30 */ SHIFT,
                                 /* 31 */ SHIFT,
                                 /* 32 */ SPLITW8};

static unsigned nw[33] = {
    0,         (1 << 1),   (1 << 2),  (1 << 3),  (1 << 4),  (1 << 5),
    (1 << 6),  (1 << 7),   (1 << 8),  (1 << 9),  (1 << 10), (1 << 11),
    (1 << 12), (1 << 13),  (1 << 14), (1 << 15), (1 << 16), (1 << 17),
    (1 << 18), (1 << 19),  (1 << 20), (1 << 21), (1 << 22), (1 << 23),
    (1 << 24), (1 << 25),  (1 << 26), (1 << 27), (1 << 28), (1 << 29),
    (1 << 30), (1u << 31), 1u << 32};

static unsigned nwm1[33] = {0,
                            (1 << 1) - 1,
                            (1 << 2) - 1,
                            (1 << 3) - 1,
                            (1 << 4) - 1,
                            (1 << 5) - 1,
                            (1 << 6) - 1,
                            (1 << 7) - 1,
                            (1 << 8) - 1,
                            (1 << 9) - 1,
                            (1 << 10) - 1,
                            (1 << 11) - 1,
                            (1 << 12) - 1,
                            (1 << 13) - 1,
                            (1 << 14) - 1,
                            (1 << 15) - 1,
                            (1 << 16) - 1,
                            (1 << 17) - 1,
                            (1 << 18) - 1,
                            (1 << 19) - 1,
                            (1 << 20) - 1,
                            (1 << 21) - 1,
                            (1 << 22) - 1,
                            (1 << 23) - 1,
                            (1 << 24) - 1,
                            (1 << 25) - 1,
                            (1 << 26) - 1,
                            (1 << 27) - 1,
                            (1 << 28) - 1,
                            (1 << 29) - 1,
                            (1 << 30) - 1,
                            0x7fffffff,
                            0xffffffff};

static unsigned *galois_log_tables[33] = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static unsigned *galois_ilog_tables[33] = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static unsigned *galois_mult_tables[33] = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static unsigned *galois_div_tables[33] = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

/* Special case for w = 32 */

static unsigned *galois_split_w8[7] = {NULL, NULL, NULL, NULL,
                                       NULL, NULL, NULL};

unsigned galois_create_log_tables(unsigned w) {
    unsigned j, b;

    if (w > 30)
        return -1;
    if (galois_log_tables[w] != NULL)
        return 0;
    galois_log_tables[w] = (unsigned *)malloc(sizeof(unsigned) * nw[w]);
    if (galois_log_tables[w] == NULL)
        return -1;

    galois_ilog_tables[w] = (unsigned *)malloc(sizeof(unsigned) * nw[w] * 3);
    if (galois_ilog_tables[w] == NULL) {
        free(galois_log_tables[w]);
        galois_log_tables[w] = NULL;
        return -1;
    }

    for (j = 0; j < nw[w]; j++) {
        galois_log_tables[w][j] = nwm1[w];
        galois_ilog_tables[w][j] = 0;
    }

    b = 1;
    for (j = 0; j < nwm1[w]; j++) {
        if (galois_log_tables[w][b] != nwm1[w]) {
            throw std::logic_error(fmt::format(
                "Galois_create_log_tables Error: j={}, b={}, B->J[b]={}, "
                "J->B[j]={} (0{})",
                j, b, galois_log_tables[w][b], galois_ilog_tables[w][j],
                (b << 1) ^ prim_poly[w]));
        }
        galois_log_tables[w][b] = j;
        galois_ilog_tables[w][j] = b;
        b = b << 1;
        if (b & nw[w])
            b = (b ^ prim_poly[w]) & nwm1[w];
    }
    for (j = 0; j < nwm1[w]; j++) {
        galois_ilog_tables[w][j + nwm1[w]] = galois_ilog_tables[w][j];
        galois_ilog_tables[w][j + nwm1[w] * 2] = galois_ilog_tables[w][j];
    }
    galois_ilog_tables[w] += nwm1[w];
    return 0;
}

unsigned galois_logtable_multiply(unsigned x, unsigned y, unsigned w) {
    unsigned sum_j;

    if (x == 0 || y == 0)
        return 0;

    sum_j = galois_log_tables[w][x] + galois_log_tables[w][y];
    /* if (sum_j >= nwm1[w]) sum_j -= nwm1[w];    Don't need to do this,
                                     because we replicate the ilog table twice.
     */
    return galois_ilog_tables[w][sum_j];
}

unsigned galois_logtable_divide(unsigned x, unsigned y, unsigned w) {
    unsigned sum_j;
    unsigned z;

    if (y == 0)
        return -1;
    if (x == 0)
        return 0;
    sum_j = galois_log_tables[w][x] - galois_log_tables[w][y];
    /* if (sum_j < 0) sum_j += nwm1[w];   Don't need to do this, because we
     * replicate the ilog table twice.   */
    z = galois_ilog_tables[w][sum_j];
    return z;
}

unsigned galois_create_mult_tables(unsigned w) {
    unsigned j, x, y, logx;

    if (w >= 14)
        return -1;

    if (galois_mult_tables[w] != NULL)
        return 0;
    galois_mult_tables[w] =
        (unsigned *)malloc(sizeof(unsigned) * nw[w] * nw[w]);
    if (galois_mult_tables[w] == NULL)
        return -1;

    galois_div_tables[w] = (unsigned *)malloc(sizeof(unsigned) * nw[w] * nw[w]);
    if (galois_div_tables[w] == NULL) {
        free(galois_mult_tables[w]);
        galois_mult_tables[w] = NULL;
        return -1;
    }
    if (galois_log_tables[w] == NULL) {
        if (galois_create_log_tables(w) < 0) {
            free(galois_mult_tables[w]);
            free(galois_div_tables[w]);
            galois_mult_tables[w] = NULL;
            galois_div_tables[w] = NULL;
            return -1;
        }
    }

    /* Set mult/div tables for x = 0 */
    j = 0;
    galois_mult_tables[w][j] = 0; /* y = 0 */
    galois_div_tables[w][j] = -1;
    j++;
    for (y = 1; y < nw[w]; y++) { /* y > 0 */
        galois_mult_tables[w][j] = 0;
        galois_div_tables[w][j] = 0;
        j++;
    }

    for (x = 1; x < nw[w]; x++) {     /* x > 0 */
        galois_mult_tables[w][j] = 0; /* y = 0 */
        galois_div_tables[w][j] = -1;
        j++;
        logx = galois_log_tables[w][x];
        for (y = 1; y < nw[w]; y++) { /* y > 0 */
            galois_mult_tables[w][j] =
                galois_ilog_tables[w][logx + galois_log_tables[w][y]];
            galois_div_tables[w][j] =
                galois_ilog_tables[w][logx - galois_log_tables[w][y]];
            j++;
        }
    }
    return 0;
}

unsigned galois_ilog(unsigned value, unsigned w) {
    if (galois_ilog_tables[w] == NULL) {
        if (galois_create_log_tables(w) < 0) {
            throw std::invalid_argument("galois_ilog - w is too big");
        }
    }
    return galois_ilog_tables[w][value];
}

unsigned galois_log(unsigned value, unsigned w) {
    if (galois_log_tables[w] == NULL) {
        if (galois_create_log_tables(w) < 0) {
            throw std::invalid_argument("galois_log - w is too big");
        }
    }
    return galois_log_tables[w][value];
}

unsigned galois_shift_multiply(unsigned x, unsigned y, unsigned w) {
    unsigned prod;
    unsigned i, j, ind;
    unsigned k;
    unsigned scratch[33];

    prod = 0;
    for (i = 0; i < w; i++) {
        scratch[i] = y;
        if (y & (1 << (w - 1))) {
            y = y << 1;
            y = (y ^ prim_poly[w]) & nwm1[w];
        } else {
            y = y << 1;
        }
    }
    for (i = 0; i < w; i++) {
        ind = (1 << i);
        if (ind & x) {
            j = 1;
            for (k = 0; k < w; k++) {
                prod = prod ^ (j & scratch[i]);
                j = (j << 1);
            }
        }
    }
    return prod;
}

unsigned galois_single_multiply(unsigned x, unsigned y, unsigned w) {
    unsigned sum_j;
    unsigned z;

    if (x == 0 || y == 0)
        return 0;

    if (mult_type[w] == TABLE) {
        if (galois_mult_tables[w] == NULL) {
            if (galois_create_mult_tables(w) < 0) {
                throw std::invalid_argument(
                    "cannot make multiplication tables for w");
            }
        }
        return galois_mult_tables[w][(x << w) | y];
    } else if (mult_type[w] == LOGS) {
        if (galois_log_tables[w] == NULL) {
            if (galois_create_log_tables(w) < 0) {
                throw std::invalid_argument(
                    fmt::format("Cannot make log tables for w={}", w));
            }
        }
        sum_j = galois_log_tables[w][x] + galois_log_tables[w][y];
        z = galois_ilog_tables[w][sum_j];
        return z;
    } else if (mult_type[w] == SPLITW8) {
        if (galois_split_w8[0] == NULL) {
            if (galois_create_split_w8_tables() < 0) {
                throw std::invalid_argument(
                    fmt::format("cannot make log split_w8_tables for w={}", w));
            }
        }
        return galois_split_w8_multiply(x, y);
    } else if (mult_type[w] == SHIFT) {
        return galois_shift_multiply(x, y, w);
    }
    throw std::invalid_argument(fmt::format("no implementation for w={}", w));
}

unsigned galois_multtable_multiply(unsigned x, unsigned y, unsigned w) {
    return galois_mult_tables[w][(x << w) | y];
}

unsigned galois_single_divide(unsigned a, unsigned b, unsigned w) {
    unsigned sum_j;

    if (mult_type[w] == TABLE) {
        if (galois_div_tables[w] == NULL) {
            if (galois_create_mult_tables(w) < 0) {
                throw std::invalid_argument(fmt::format(
                    "Cannot make multiplication tables for w={}", w));
            }
        }
        return galois_div_tables[w][(a << w) | b];
    } else if (mult_type[w] == LOGS) {
        if (b == 0)
            return -1;
        if (a == 0)
            return 0;
        if (galois_log_tables[w] == NULL) {
            if (galois_create_log_tables(w) < 0) {
                std::logic_error(
                    fmt::format("Cannot make log tables for w={}", w));
            }
        }
        sum_j = galois_log_tables[w][a] - galois_log_tables[w][b];
        return galois_ilog_tables[w][sum_j];
    } else {
        if (b == 0)
            return -1;
        if (a == 0)
            return 0;
        sum_j = galois_inverse(b, w);
        return galois_single_multiply(a, sum_j, w);
    }
    std::logic_error(fmt::format("No implementation for w={}", w));
}

unsigned galois_shift_divide(unsigned a, unsigned b, unsigned w) {
    unsigned inverse;

    if (b == 0)
        return -1;
    if (a == 0)
        return 0;
    inverse = galois_shift_inverse(b, w);
    return galois_shift_multiply(a, inverse, w);
}

unsigned galois_multtable_divide(unsigned x, unsigned y, unsigned w) {
    return galois_div_tables[w][(x << w) | y];
}

void galois_w08_region_multiply(char *region,    /* Region to multiply */
                                unsigned multby, /* Number to multiply by */
                                unsigned nbytes, /* Number of bytes in region */
                                char *r2, /* If r2 != NULL, products go here */
                                unsigned add) {
    unsigned char *ur1, *ur2, *cp;
    unsigned char prod;
    unsigned i, srow, j;
    unsigned long l, *lp2;
    unsigned char *lp;
    unsigned sol;

    ur1 = (unsigned char *)region;
    ur2 = (r2 == NULL) ? ur1 : (unsigned char *)r2;

    /* This is used to test its performance with respect to just calling
      galois_single_multiply if (r2 == NULL || !add) { for (i = 0; i < nbytes;
      i++) ur2[i] = galois_single_multiply(ur1[i], multby, 8); } else { for (i =
      0; i < nbytes; i++) { ur2[i] = (ur2[i]^galois_single_multiply(ur1[i],
      multby, 8));
        }
      }
     */

    if (galois_mult_tables[8] == NULL) {
        if (galois_create_mult_tables(8) < 0) {
            std::logic_error("Could not make multiplication tables");
        }
    }
    srow = multby * nw[8];
    if (r2 == NULL || !add) {
        for (i = 0; i < nbytes; i++) {
            prod = galois_mult_tables[8][srow + ur1[i]];
            ur2[i] = prod;
        }
    } else {
        sol = sizeof(long);
        lp2 = &l;
        lp = (unsigned char *)lp2;
        for (i = 0; i < nbytes; i += sol) {
            cp = ur2 + i;
            lp2 = (unsigned long *)cp;
            for (j = 0; j < sol; j++) {
                prod = galois_mult_tables[8][srow + ur1[i + j]];
                lp[j] = prod;
            }
            *lp2 = (*lp2) ^ l;
        }
    }
    return;
}

void galois_w16_region_multiply(char *region,    /* Region to multiply */
                                unsigned multby, /* Number to multiply by */
                                unsigned nbytes, /* Number of bytes in region */
                                char *r2, /* If r2 != NULL, products go here */
                                unsigned add) {
    unsigned short *ur1, *ur2, *cp;
    unsigned prod;
    unsigned i, log1, j, log2;
    unsigned long l, *lp2, *lptop;
    unsigned short *lp;
    unsigned sol;

    ur1 = (unsigned short *)region;
    ur2 = (r2 == NULL) ? ur1 : (unsigned short *)r2;
    nbytes /= 2;

    /* This is used to test its performance with respect to just calling
     * galois_single_multiply */
    /*
      if (r2 == NULL || !add) {
        for (i = 0; i < nbytes; i++) ur2[i] = galois_single_multiply(ur1[i],
      multby, 16); } else { for (i = 0; i < nbytes; i++) { ur2[i] =
      (ur2[i]^galois_single_multiply(ur1[i], multby, 16));
        }
      }
      return;
      */

    if (multby == 0) {
        if (!add) {
            lp2 = (unsigned long *)ur2;
            ur2 += nbytes;
            lptop = (unsigned long *)ur2;
            while (lp2 < lptop) {
                *lp2 = 0;
                lp2++;
            }
        }
        return;
    }

    if (galois_log_tables[16] == NULL) {
        if (galois_create_log_tables(16) < 0) {
            std::logic_error("Could not make log tables");
        }
    }
    log1 = galois_log_tables[16][multby];

    if (r2 == NULL || !add) {
        for (i = 0; i < nbytes; i++) {
            if (ur1[i] == 0) {
                ur2[i] = 0;
            } else {
                prod = galois_log_tables[16][ur1[i]] + log1;
                ur2[i] = galois_ilog_tables[16][prod];
            }
        }
    } else {
        sol = sizeof(long) / 2;
        lp2 = &l;
        lp = (unsigned short *)lp2;
        for (i = 0; i < nbytes; i += sol) {
            cp = ur2 + i;
            lp2 = (unsigned long *)cp;
            for (j = 0; j < sol; j++) {
                if (ur1[i + j] == 0) {
                    lp[j] = 0;
                } else {
                    log2 = galois_log_tables[16][ur1[i + j]];
                    prod = log2 + log1;
                    lp[j] = galois_ilog_tables[16][prod];
                }
            }
            *lp2 = (*lp2) ^ l;
        }
    }
    return;
}

/* This will destroy mat, by the way */

void galois_invert_binary_matrix(unsigned *mat, unsigned *inv, unsigned rows) {
    unsigned cols, i, j, k;
    unsigned tmp;

    cols = rows;

    for (i = 0; i < rows; i++)
        inv[i] = (1 << i);

    /* First -- convert unsignedo upper triangular */

    for (i = 0; i < cols; i++) {

        /* Swap rows if we ave a zero i,i element.  If we can't swap, then the
           matrix was not invertible */

        if ((mat[i] & (1 << i)) == 0) {
            for (j = i + 1; j < rows && (mat[j] & (1 << i)) == 0; j++)
                ;
            if (j == rows) {
                throw std::invalid_argument("Matrix is not invertible");
            }
            tmp = mat[i];
            mat[i] = mat[j];
            mat[j] = tmp;
            tmp = inv[i];
            inv[i] = inv[j];
            inv[j] = tmp;
        }

        /* Now for each j>i, add A_ji*Ai to Aj */
        for (j = i + 1; j != rows; j++) {
            if ((mat[j] & (1 << i)) != 0) {
                mat[j] ^= mat[i];
                inv[j] ^= inv[i];
            }
        }
    }

    /* Now the matrix is upper triangular.  Start at the top and multiply down
     */

    for (i = rows - 1; i >= 0; i--) {
        for (j = 0; j < i; j++) {
            if (mat[j] & (1 << i)) {
                /*        mat[j] ^= mat[i]; */
                inv[j] ^= inv[i];
            }
        }
    }
}

unsigned galois_inverse(unsigned y, unsigned w) {

    if (y == 0)
        return -1;
    if (mult_type[w] == SHIFT || mult_type[w] == SPLITW8)
        return galois_shift_inverse(y, w);
    return galois_single_divide(1, y, w);
}

unsigned galois_shift_inverse(unsigned y, unsigned w) {
    unsigned mat[1024], mat2[32];
    unsigned inv[1024], inv2[32];
    unsigned ind, i, j, k, prod;

    for (i = 0; i < w; i++) {
        mat2[i] = y;

        if (y & nw[w - 1]) {
            y = y << 1;
            y = (y ^ prim_poly[w]) & nwm1[w];
        } else {
            y = y << 1;
        }
    }

    galois_invert_binary_matrix(mat2, inv2, w);

    return inv2[0];
}

unsigned *galois_get_mult_table(unsigned w) {
    if (galois_mult_tables[w] == NULL) {
        if (galois_create_mult_tables(w)) {
            return NULL;
        }
    }
    return galois_mult_tables[w];
}

unsigned *galois_get_div_table(unsigned w) {
    if (galois_mult_tables[w] == NULL) {
        if (galois_create_mult_tables(w)) {
            return NULL;
        }
    }
    return galois_div_tables[w];
}

unsigned *galois_get_log_table(unsigned w) {
    if (galois_log_tables[w] == NULL) {
        if (galois_create_log_tables(w)) {
            return NULL;
        }
    }
    return galois_log_tables[w];
}

unsigned *galois_get_ilog_table(unsigned w) {
    if (galois_ilog_tables[w] == NULL) {
        if (galois_create_log_tables(w)) {
            return NULL;
        }
    }
    return galois_ilog_tables[w];
}

void galois_w32_region_multiply(char *region,    /* Region to multiply */
                                unsigned multby, /* Number to multiply by */
                                unsigned nbytes, /* Number of bytes in region */
                                char *r2, /* If r2 != NULL, products go here */
                                unsigned add) {
    unsigned *ur1, *ur2, *cp, *ur2top;
    unsigned long *lp2, *lptop;
    unsigned i, j, a, b, accumulator, i8, j8, k;
    unsigned acache[4];

    ur1 = (unsigned *)region;
    ur2 = (r2 == NULL) ? ur1 : (unsigned *)r2;
    nbytes /= sizeof(unsigned);
    ur2top = ur2 + nbytes;

    if (galois_split_w8[0] == NULL) {
        if (galois_create_split_w8_tables(/*8*/) < 0) {
            throw std::logic_error(
                "galois_32_region_multiply -- couldn't make split");
        }
    }

    /* If we're overwriting r2, then we can't do better than just calling
       split_multiply.
       We'll inline it here to save on the procedure call overhead */

    i8 = 0;
    for (i = 0; i < 4; i++) {
        acache[i] = (((multby >> i8) & 255) << 8);
        i8 += 8;
    }
    if (!add) {
        for (k = 0; k < nbytes; k++) {
            accumulator = 0;
            for (i = 0; i < 4; i++) {
                a = acache[i];
                j8 = 0;
                for (j = 0; j < 4; j++) {
                    b = ((ur1[k] >> j8) & 255);
                    accumulator ^= galois_split_w8[i + j][a | b];
                    j8 += 8;
                }
            }
            ur2[k] = accumulator;
        }
    } else {
        for (k = 0; k < nbytes; k++) {
            accumulator = 0;
            for (i = 0; i < 4; i++) {
                a = acache[i];
                j8 = 0;
                for (j = 0; j < 4; j++) {
                    b = ((ur1[k] >> j8) & 255);
                    accumulator ^= galois_split_w8[i + j][a | b];
                    j8 += 8;
                }
            }
            ur2[k] = (ur2[k] ^ accumulator);
        }
    }
    return;
}

void galois_region_xor(
    char *r1,        /* Region 1 */
    char *r2,        /* Region 2 */
    char *r3,        /* Sum region (r3 = r1 ^ r2) -- can be r1 or r2 */
    unsigned nbytes) /* Number of bytes in region */
{
    long *l1;
    long *l2;
    long *l3;
    long *ltop;
    char *ctop;

    ctop = r1 + nbytes;
    ltop = (long *)ctop;
    l1 = (long *)r1;
    l2 = (long *)r2;
    l3 = (long *)r3;

    while (l1 < ltop) {
        *l3 = ((*l1) ^ (*l2));
        l1++;
        l2++;
        l3++;
    }
}

unsigned galois_create_split_w8_tables() {
    unsigned p1, p2, i, j, p1elt, p2elt, index, ishift, jshift, *table;

    if (galois_split_w8[0] != NULL)
        return 0;

    if (galois_create_mult_tables(8) < 0)
        return -1;

    for (i = 0; i < 7; i++) {
        galois_split_w8[i] = (unsigned *)malloc(sizeof(unsigned) * (1 << 16));
        if (galois_split_w8[i] == NULL) {
            for (i--; i >= 0; i--)
                free(galois_split_w8[i]);
            return -1;
        }
    }

    for (i = 0; i < 4; i += 3) {
        ishift = i * 8;
        for (j = ((i == 0) ? 0 : 1); j < 4; j++) {
            jshift = j * 8;
            table = galois_split_w8[i + j];
            index = 0;
            for (p1 = 0; p1 < 256; p1++) {
                p1elt = (p1 << ishift);
                for (p2 = 0; p2 < 256; p2++) {
                    p2elt = (p2 << jshift);
                    table[index] = galois_shift_multiply(p1elt, p2elt, 32);
                    index++;
                }
            }
        }
    }
    return 0;
}

unsigned galois_split_w8_multiply(unsigned x, unsigned y) {
    unsigned i, j, a, b, accumulator, i8, j8;

    accumulator = 0;

    i8 = 0;
    for (i = 0; i < 4; i++) {
        a = (((x >> i8) & 255) << 8);
        j8 = 0;
        for (j = 0; j < 4; j++) {
            b = ((y >> j8) & 255);
            accumulator ^= galois_split_w8[i + j][a | b];
            j8 += 8;
        }
        i8 += 8;
    }
    return accumulator;
}
