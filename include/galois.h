/* Galois.h
 * James S. Plank

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

unsigned galois_single_multiply(unsigned a, unsigned b, unsigned w);
unsigned galois_single_divide(unsigned a, unsigned b, unsigned w);
unsigned galois_log(unsigned value, unsigned w);
unsigned galois_ilog(unsigned value, unsigned w);

unsigned
galois_create_log_tables(unsigned w); /* Returns 0 on success, -1 on failure */
unsigned galois_logtable_multiply(unsigned x, unsigned y, unsigned w);
unsigned galois_logtable_divide(unsigned x, unsigned y, unsigned w);

unsigned
galois_create_mult_tables(unsigned w); /* Returns 0 on success, -1 on failure */
unsigned galois_multtable_multiply(unsigned x, unsigned y, unsigned w);
unsigned galois_multtable_divide(unsigned x, unsigned y, unsigned w);

unsigned galois_shift_multiply(unsigned x, unsigned y, unsigned w);
unsigned galois_shift_divide(unsigned x, unsigned y, unsigned w);

unsigned galois_create_split_w8_tables();
unsigned galois_split_w8_multiply(unsigned x, unsigned y);

unsigned galois_inverse(unsigned x, unsigned w);
unsigned galois_shift_inverse(unsigned y, unsigned w);

unsigned *galois_get_mult_table(unsigned w);
unsigned *galois_get_div_table(unsigned w);
unsigned *galois_get_log_table(unsigned w);
unsigned *galois_get_ilog_table(unsigned w);

void galois_region_xor(
    char *r1,         /* Region 1 */
    char *r2,         /* Region 2 */
    char *r3,         /* Sum region (r3 = r1 ^ r2) -- can be r1 or r2 */
    unsigned nbytes); /* Number of bytes in region */

/* These multiply regions in w=8, w=16 and w=32.  They are much faster
   than calling galois_single_multiply.  The regions must be long word aligned.
 */

void galois_w08_region_multiply(
    char *region,    /* Region to multiply */
    unsigned multby, /* Number to multiply by */
    unsigned nbytes, /* Number of bytes in region */
    char *r2,        /* If r2 != NULL, products go here.
                        Otherwise region is overwritten */
    unsigned add);   /* If (r2 != NULL && add) the produce is XOR'd with r2 */

void galois_w16_region_multiply(
    char *region,    /* Region to multiply */
    unsigned multby, /* Number to multiply by */
    unsigned nbytes, /* Number of bytes in region */
    char *r2,        /* If r2 != NULL, products go here.
                        Otherwise region is overwritten */
    unsigned add);   /* If (r2 != NULL && add) the produce is XOR'd with r2 */

void galois_w32_region_multiply(
    char *region,    /* Region to multiply */
    unsigned multby, /* Number to multiply by */
    unsigned nbytes, /* Number of bytes in region */
    char *r2,        /* If r2 != NULL, products go here.
                        Otherwise region is overwritten */
    unsigned add);   /* If (r2 != NULL && add) the produce is XOR'd with r2 */
