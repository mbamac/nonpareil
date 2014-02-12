/*
 $Id: proc_classic.c 849 2005-06-27 20:33:01Z eric $
 Copyright 2004, 2005 Eric L. Smith <eric@brouhaha.com>
 
 Nonpareil is free software; you can redistribute it and/or modify it
 under the terms of the GNU General Public License version 2 as
 published by the Free Software Foundation.  Note that I am not
 granting permission to redistribute or modify Nonpareil under the
 terms of any later version of the General Public License.
 
 Nonpareil is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program (in the file "COPYING"); if not, write to the
 Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 MA 02111, USA.
 */

//
// any changes since nonpareil 0.77 copyright 2005-2012 Maciej Bartosiak
//

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "digit_ops.h"
#include "utils.h"
#include "proc_classic.h"

#define BIT_SET(val,bit_num)    ((val) |= (1 << (bit_num)))
#define BIT_CLEAR(val,bit_num)  ((val) &= ~(1 << (bit_num)))
#define BIT_TEST(val,bit_num)   ((val) & (1 << (bit_num)))

static inline uint8_t arithmetic_base (cpu_t *cpu_reg)
{
	return 10;  // no binary (hex) mode on Classic
}


static void bad_op (cpu_t *cpu_reg, int opcode)
{
	//printf ("illegal opcode %04o at %02o%03o\n", opcode,
	//		cpu_reg->prev_pc >> 8, cpu_reg->prev_pc & 0377);
}


static void op_arith (cpu_t *cpu_reg, int opcode)
{
	uint8_t op, field;
	int first = 0;
	int last = 0;
	
	op = opcode >> 5;
	field = (opcode >> 2) & 7;
	
	switch (field)
    {
		case 0:  /* p  */
			first =  cpu_reg->p; last =  cpu_reg->p;
			if (cpu_reg->p >= WSIZE)
			{
				//printf ("Warning! p >= WSIZE at %02o%03o\n",
				//		cpu_reg->prev_pc >> 8, cpu_reg->prev_pc & 0377);
				last = 0;  /* don't do anything */
			}
				break;
		case 1:  /* m  */  first = EXPSIZE;      last = WSIZE - 2;   break;
		case 2:  /* x  */  first = 0;            last = EXPSIZE - 1; break;
		case 3:  /* w  */  first = 0;            last = WSIZE - 1;   break;
		case 4:  /* wp */
			first =  0; last =  cpu_reg->p;
			if (cpu_reg->p >= WSIZE)
			{
				//printf ("Warning! p >= WSIZE at %02o%03o\n",
				//		cpu_reg->prev_pc >> 8, cpu_reg->prev_pc & 0377);
				last = WSIZE - 1;
			}
				break;
		case 5:  /* ms */  first =  EXPSIZE;     last = WSIZE - 1;   break;
		case 6:  /* xs */  first =  EXPSIZE - 1; last = EXPSIZE - 1; break;
		case 7:  /* s  */  first =  WSIZE - 1;   last = WSIZE - 1;   break;
    }
	
	// Note: carry was set to 0 by classic_execute_instruction before
	// we're called.
	switch (op)
    {
		case 0x00:  /* if b[f] = 0 */
			reg_test_nonequal (cpu_reg->b, NULL, first, last, & cpu_reg->carry);
			break;
		case 0x01:  /* 0 -> b[f] */
			reg_zero (cpu_reg->b, first, last);
			break;
		case 0x02:  /* if a >= c[f] */
			reg_sub (NULL, cpu_reg->a, cpu_reg->c,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x03:  /* if c[f] >= 1 */
			reg_test_equal (cpu_reg->c, NULL, first, last, & cpu_reg->carry);
			break;
		case 0x04:  /* b -> c[f] */
			reg_copy (cpu_reg->c, cpu_reg->b, first, last);
			break;
		case 0x05:  /* 0 - c -> c[f] */
			reg_sub (cpu_reg->c, NULL, cpu_reg->c,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x06:  /* 0 -> c[f] */
			reg_zero (cpu_reg->c, first, last);
			break;
		case 0x07:  /* 0 - c - 1 -> c[f] */
			cpu_reg->carry = 1;
			reg_sub (cpu_reg->c, NULL, cpu_reg->c,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x08:  /* shift left a[f] */
			reg_shift_left (cpu_reg->a, first, last);
			break;
		case 0x09:  /* a -> b[f] */
			reg_copy (cpu_reg->b, cpu_reg->a, first, last);
			break;
		case 0x0a:  /* a - c -> c[f] */
			reg_sub (cpu_reg->c, cpu_reg->a, cpu_reg->c,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x0b:  /* c - 1 -> c[f] */
			cpu_reg->carry = 1;
			reg_sub (cpu_reg->c, cpu_reg->c, NULL,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x0c:  /* c -> a[f] */
			reg_copy (cpu_reg->a, cpu_reg->c, first, last);
			break;
		case 0x0d:  /* if c[f] = 0 */
			reg_test_nonequal (cpu_reg->c, NULL, first, last, & cpu_reg->carry);
			break;
		case 0x0e:  /* a + c -> c[f] */
			reg_add (cpu_reg->c, cpu_reg->a, cpu_reg->c,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x0f:  /* c + 1 -> c[f] */
			cpu_reg->carry = 1;
			reg_add (cpu_reg->c, cpu_reg->c, NULL,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x10:  /* if a >= b[f] */
			reg_sub (NULL, cpu_reg->a, cpu_reg->b,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x11:  /* b exchange c[f] */
			reg_exch (cpu_reg->b, cpu_reg->c, first, last);
			break;
		case 0x12:  /* shift right c[f] */
			reg_shift_right (cpu_reg->c, first, last);
			break;
		case 0x13:  /* if a[f] >= 1 */
			reg_test_equal (cpu_reg->a, NULL, first, last, & cpu_reg->carry);
			break;
		case 0x14:  /* shift right b[f] */
			reg_shift_right (cpu_reg->b, first, last);
			break;
		case 0x15:  /* c + c -> c[f] */
			reg_add (cpu_reg->c, cpu_reg->c, cpu_reg->c,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x16:  /* shift right a[f] */
			reg_shift_right (cpu_reg->a, first, last);
			break;
		case 0x17:  /* 0 -> a[f] */
			reg_zero (cpu_reg->a, first, last);
			break;
		case 0x18:  /* a - b -> a[f] */
			reg_sub (cpu_reg->a, cpu_reg->a, cpu_reg->b,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x19:  /* a exchange b[f] */
			reg_exch (cpu_reg->a, cpu_reg->b, first, last);
			break;
		case 0x1a:  /* a - c -> a[f] */
			reg_sub (cpu_reg->a, cpu_reg->a, cpu_reg->c,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x1b:  /* a - 1 -> a[f] */
			cpu_reg->carry = 1;
			reg_sub (cpu_reg->a, cpu_reg->a, NULL,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x1c:  /* a + b -> a[f] */
			reg_add (cpu_reg->a, cpu_reg->a, cpu_reg->b,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x1d:  /* a exchange c[f] */
			reg_exch (cpu_reg->a, cpu_reg->c, first, last);
			break;
		case 0x1e:  /* a + c -> a[f] */
			reg_add (cpu_reg->a, cpu_reg->a, cpu_reg->c,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x1f:  /* a + 1 -> a[f] */
			cpu_reg->carry = 1;
			reg_add (cpu_reg->a, cpu_reg->a, NULL,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
    }
}


static void op_goto (cpu_t *cpu_reg, int opcode)
{
	//if (! cpu_reg->prev_carry)
    //{
		cpu_reg->pc = opcode >> 2;
		cpu_reg->rom = cpu_reg->del_rom;
		cpu_reg->group = cpu_reg->del_grp;
    //}
}


static void op_jsb (cpu_t *cpu_reg, int opcode)
{
	cpu_reg->ret_pc = cpu_reg->pc;
	cpu_reg->pc = opcode >> 2;
	cpu_reg->rom = cpu_reg->del_rom;
	cpu_reg->group = cpu_reg->del_grp;
}


static void op_return (cpu_t *cpu_reg, int opcode)
{
	cpu_reg->pc = cpu_reg->ret_pc;
}


static void op_nop (cpu_t *cpu_reg, int opcode)
{
}


static void op_dec_p (cpu_t *cpu_reg, int opcode)
{
	cpu_reg->p = (cpu_reg->p - 1) & 0xf;
	/* On the ACT (Woodstock) if P=0 before a decrement, it will be
	13 after.  Apparently the CTC (Classic) does not do this. */
}


static void op_inc_p (cpu_t *cpu_reg, int opcode)
{
	cpu_reg->p = (cpu_reg->p + 1) & 0xf;
	/* On the ACT (Woodstock) if P=13 before an increment, it will be
	0 after.  Apparently the CTC (Classic) does not do this. */
}


static void op_clear_s (cpu_t *cpu_reg, int opcode)
{
	int i;
	
	for (i = 0; i < SSIZE; i++)
        BIT_CLEAR(cpu_reg->s, i);
}


static void op_c_exch_m (cpu_t *cpu_reg, int opcode)
{
	int i, t;
	
	for (i = 0; i < WSIZE; i++)
    {
		t = cpu_reg->c [i];
		cpu_reg->c [i] = cpu_reg->m[i];
		cpu_reg->m [i] = t;
    }
}


static void op_m_to_c (cpu_t *cpu_reg, int opcode)
{
	int i;
	
	for (i = 0; i < WSIZE; i++)
		cpu_reg->c [i] = cpu_reg->m [i];
}


static void op_c_to_addr (cpu_t *cpu_reg, int opcode)
{
	if (cpu_reg->max_ram > 10)
		cpu_reg->ram_addr = cpu_reg->c [12] * 10 + cpu_reg->c [11];
	else
		cpu_reg->ram_addr = cpu_reg->c [12];
	if (cpu_reg->ram_addr >= cpu_reg->max_ram)
		printf ("c -> ram addr: address %d out of range\n", cpu_reg->ram_addr);
}


static void op_c_to_data (cpu_t *cpu_reg, int opcode)
{
	int i;
	
	if (cpu_reg->ram_addr >= cpu_reg->max_ram)
    {
		printf ("c -> data: address %d out of range\n", cpu_reg->ram_addr);
		return;
    }
	for (i = 0; i < WSIZE; i++)
		cpu_reg->ram [cpu_reg->ram_addr] [i] = cpu_reg->c [i];
}


static void op_data_to_c (cpu_t *cpu_reg, int opcode)
{
	int i;
	
	if (cpu_reg->ram_addr >= cpu_reg->max_ram)
    {
		printf ("data -> c: address %d out of range, loading 0\n", cpu_reg->ram_addr);
		for (i = 0; i < WSIZE; i++)
			cpu_reg->c [i] = 0;
		return;
    }
	for (i = 0; i < WSIZE; i++)
		cpu_reg->c [i] = cpu_reg->ram [cpu_reg->ram_addr] [i];
}


static void op_c_to_stack (cpu_t *cpu_reg, int opcode)
{
	int i;
	
	for (i = 0; i < WSIZE; i++)
    {
		cpu_reg->f [i] = cpu_reg->e [i];
		cpu_reg->e [i] = cpu_reg->d [i];
		cpu_reg->d [i] = cpu_reg->c [i];
    }
}


static void op_stack_to_a (cpu_t *cpu_reg, int opcode)
{
	int i;
	
	for (i = 0; i < WSIZE; i++)
    {
		cpu_reg->a [i] = cpu_reg->d [i];
		cpu_reg->d [i] = cpu_reg->e [i];
		cpu_reg->e [i] = cpu_reg->f [i];
    }
}


static void op_down_rotate (cpu_t *cpu_reg, int opcode)
{
	int i, t;
	
	for (i = 0; i < WSIZE; i++)
    {
		t			   = cpu_reg->c [i];
		cpu_reg->c [i] = cpu_reg->d [i];
		cpu_reg->d [i] = cpu_reg->e [i];
		cpu_reg->e [i] = cpu_reg->f [i];
		cpu_reg->f [i] = t;
    }
}


static void op_clear_reg (cpu_t *cpu_reg, int opcode)
{
	int i;
	
	for (i = 0; i < WSIZE; i++)
		cpu_reg->a [i] = cpu_reg->b [i] = cpu_reg->c [i] = cpu_reg->d [i] =
			cpu_reg->e [i] = cpu_reg->f [i] = cpu_reg->m [i] = 0;
}


static void op_load_constant (cpu_t *cpu_reg, int opcode)
{
	if (cpu_reg->p >= WSIZE)
    {
#if 0 /* HP-45 depends on load constant with p > 13 not affecting C */
		printf ("load constant w/ p >= WSIZE at %02o%03o\n",
				cpu_reg->prev_pc >> 8, cpu_reg->prev_pc & 0377)
		;
#endif
    }
	else if ((opcode >> 6) > 9)
		printf ("load constant > 9\n");
	else
		cpu_reg->c [cpu_reg->p] = opcode >> 6;
	
	cpu_reg->p = (cpu_reg->p - 1) & 0xf;
	/* On the ACT (Woodstock) if P=0 before a load constant, it will be
		13 after.  Apparently the CTC (Classic) does not do this. */
}


static void op_set_s (cpu_t *cpu_reg, int opcode)
{
	if ((opcode >> 6) >= SSIZE)
		;//printf ("stat >= SSIZE at %02o%03o\n",
		//		cpu_reg->prev_pc >> 8, cpu_reg->prev_pc & 0377);
	else
        BIT_SET(cpu_reg->s, (opcode >> 6));
}


static void op_clr_s (cpu_t *cpu_reg, int opcode)
{
	if ((opcode >> 6) >= SSIZE)
		;//printf ("stat >= SSIZE at %02o%03o\n",
		//		cpu_reg->prev_pc >> 8, cpu_reg->prev_pc & 0377);
	else
        BIT_CLEAR(cpu_reg->s, (opcode >> 6));
}


static void op_test_s (cpu_t *cpu_reg, int opcode)
{
	if ((opcode >> 6) >= SSIZE)
		;//printf ("stat >= SSIZE at %02o%03o\n",
		//		cpu_reg->prev_pc >> 8, cpu_reg->prev_pc & 0377);
	else
        cpu_reg->carry = BIT_TEST(cpu_reg->s, (opcode >> 6));
}


static void op_set_p (cpu_t *cpu_reg, int opcode)
{
	cpu_reg->p = opcode >> 6;
}


static void op_test_p (cpu_t *cpu_reg, int opcode)
{
	cpu_reg->carry = (cpu_reg->p == (opcode >> 6));
}


static void op_sel_rom (cpu_t *cpu_reg, int opcode)
{
	cpu_reg->rom = opcode >> 7;
	cpu_reg->group = cpu_reg->del_grp;
	
	cpu_reg->del_rom = cpu_reg->rom;
}


static void op_del_sel_rom (cpu_t *cpu_reg, int opcode)
{
	cpu_reg->del_rom = opcode >> 7;
}


static void op_del_sel_grp (cpu_t *cpu_reg, int opcode)
{
	cpu_reg->del_grp = (opcode >> 7) & 1;
}


static void op_keys_to_rom_addr (cpu_t *cpu_reg, int opcode)
{
	if (cpu_reg->key_buf < 0)
    {
		printf ("keys->rom address with no key pressed\n");
		cpu_reg->pc = 0;
		return;
    }
	cpu_reg->pc = cpu_reg->key_buf;
}


static void op_rom_addr_to_buf (cpu_t *cpu_reg, int opcode)
{
	/* I don't know what this instruction is supposed to do! */
#if 0
	fprintf (stderr, "rom addr to buf!!!!!!!!!!!!\n");
#endif
}


static void op_display_off (cpu_t *cpu_reg, int opcode)
{
	cpu_reg->display_enable = 0;
}


static void op_display_toggle (cpu_t *cpu_reg, int opcode)
{
	cpu_reg->display_enable = ! cpu_reg->display_enable;
}

static void printbitssimple(int n) {
	unsigned int i;
	i = 1<<(9);
    
	while (i > 0) {
		if (n & i)
			printf("1");
		else
			printf("0");
		i >>= 1;
	}
}


static void classic_execute_instruction_00(cpu_t *cpu_reg, int opcode)
{
    int f   = (opcode >> 4) & 0x3; // two bits (5,6)
    int n   = (opcode >> 6) & 0xf; // four bits(7..10)
    
    switch ((opcode >> 2) & 0x3) { // type 6
        case 0x0: // 00-00
            //op_sel_rom        nnnff10000	nn8 00 1 0000
            //op_return         nnnff10000	xxx 01 1 0000
            //op_keys2rom_adr	nnnff10000	xx1 10 1 0000
			//external entry				xx0 10 1 0000 (not implemented)
            //op_c_to_addr      nnnff10000	100 11 1 0000
            //op_c_to_data      nnnff10000	101 11 1 0000
            
            if ((opcode) >> 4 & 0x1) {
                f   = (opcode >> 5) & 0x3; // two bits (6,7)
                n   = (opcode >> 7) & 0x7; // four bits(8..10)
                
                switch (f) {
                    case 0x0:
                        op_sel_rom(cpu_reg, opcode);
                        break;
                    case 0x1:
                        op_return(cpu_reg, opcode);
                        break;
                    case 0x2:
                        if (n == 0x1)
							op_keys_to_rom_addr(cpu_reg, opcode);
						else
							bad_op(cpu_reg, opcode); // external entry from patent
						break;
					case 0x3: // not described in patent
						if (n == 0x4)
							op_c_to_addr(cpu_reg, opcode);
						else if (n == 0x5)
							op_c_to_data(cpu_reg, opcode);
						else {
							printf ("warning: op_c_to_...\n");
							printbitssimple(opcode);
							printf(" (%x) f=%d, n=%d\n", opcode, f, n);
						}
                    default:
                        break;
                }
            }
            
            //op_rom_adr2buf	xxx0000000	100000 00 00
            else if (opcode == 0x200) // 1000000000
                op_rom_addr_to_buf(cpu_reg, opcode);
            // op_nooper       xxx0000000	000 00000 00
            else if (opcode == 0) // 0000000000
                op_nop(cpu_reg, opcode);
            else
                bad_op(cpu_reg, opcode);
            break;
        case 0x1: // type 3 (01-00) status operations
            //op_set_s         nnnnff0100	nn16 00 0100
            //op_test_s        nnnnff0100	nn16 01 0100
            //op_clr_s         nnnnff0100	nn16 10 0100
            //op_clear_s       nnnnff0100	0000 11 0100
            
            //op_del_sel_grp               100 0 11 0100
            //op_del_sel_grp               101 0 11 0100
            //op_del_sel_rom               nn8 1 11 0100
            switch (f) {
                case 0x0:
                    op_set_s(cpu_reg, opcode);
                    break;
                case 0x1:
                    op_test_s(cpu_reg, opcode);
                    break;
                case 0x2:
                    op_clr_s(cpu_reg, opcode);
                    break;
                case 0x3:
                    if (n == 0)
                        op_clear_s(cpu_reg, opcode);
                    else
                        if (n & 0x1)
                            op_del_sel_rom(cpu_reg, opcode); // nn8 1 11 01 00
                        else {
                            //op_del_sel_grp			100 0 11 01 00
                            //op_del_sel_grp			101 0 11 01 00
                            if ((n == 0x8) || (n == 0xa))
                                op_del_sel_grp(cpu_reg,opcode);
                            else
                                printf ("warning: type 3 instuctions - op_del_sel_grp not executed\n");
                        }
                    break;
                default:
                    printf ("warning: 3 instuctions - status operations\n");
                    break;
            }
            break;
        case 0x2: // type 5 (10-00) data entry/display & stack instructions
            // op_load_const	nnnnff1000	nn10 01 1000
            
            if (f == 0x1) { // xxxx-01-1000
                if (n < 0xa)
                    op_load_constant(cpu_reg, opcode);
                else
                    printf ("warning:  type 5 instuctions - op_load_constant n>10\n");
            }
            //op_disp_toggle	nnn n ff1000	000 0 10 1000
            // op_c_exch_m      nnn n ff1000	001 0 10 1000
            // op_c_to_stack	nnn n ff1000	010 0 10 1000
            // op_stack_to_a	nnn n ff1000	011 0 10 1000
            //op_disp_off       nnn n ff1000	100 0 10 1000
            // op_m_to_c        nnn n ff1000	101 0 10 1000
            //op_down_rotate	nnn n ff1000	110 0 10 1000
            // op_clear_reg     nnn n ff1000	111 0 10 1000
            else if ((f == 0x2) && !(n & 0x1)) { // f == 10 and first bit of n == 0 xxx-010-1000
                switch ((n >> 1) & 0x7) {
                    case 0x0:
                        op_display_toggle(cpu_reg, opcode);
                        break;
                    case 0x1:
                        op_c_exch_m(cpu_reg, opcode);
                        break;
                    case 0x2:
                        op_c_to_stack(cpu_reg, opcode);
                        break;
                    case 0x3:
                        op_stack_to_a(cpu_reg, opcode);
                        break;
                    case 0x4:
                        op_display_off(cpu_reg, opcode);
                        break;
                    case 0x5:
                        op_m_to_c(cpu_reg, opcode);
                        break;
                    case 0x6:
                        op_down_rotate(cpu_reg, opcode);
                        break;
                    case 0x7:
                        op_clear_reg(cpu_reg, opcode);
                        break;
                    default:
                        printf ("warning: type 5 instuctions - dispaly/stack operations\n");
                        printbitssimple(opcode);
                        printf("\n");
                        break;
                }
            }            
            // op_data_to_c	nnnnff1000	n4 111 x 1000
            else if (((opcode >> 5) & 0x7) == 0x7) // second bit of f and two first bits of n == 1 (xx-111-x-1000)
                op_data_to_c(cpu_reg, opcode);
            break;
        case 0x3: // type 4 (11-00) pointer operations
            //*op_set_p     ppppff1100	nn16 00 1100
            //*op_dec_p     ppppff1100	0000 01 1100
            //*op_test_p	ppppff1100	nn16 10 1100
            //*op_inc_p     ppppff1100	0000 11 1100
            switch (f) {
                case 0x0:
                    op_set_p(cpu_reg,   opcode);
                    break;
                case 0x1:
                    if (n == 0)
                        op_dec_p(cpu_reg, opcode); else bad_op(cpu_reg, opcode);
                    break;
                case 0x02:
                    op_test_p(cpu_reg, opcode);
                    break;
                case 0x3:
                    if (n == 0)
                        op_inc_p(cpu_reg, opcode); else bad_op(cpu_reg, opcode);
                default:
                    break;
            }
            break;
        default:
            bad_op(cpu_reg, opcode);
            break;
    }
}

bool classic_execute_instruction (cpu_t *cpu_reg)
{
	int addr;
	int opcode;
	bool carry = cpu_reg->carry;
	
	addr = (cpu_reg->group << 11) | (cpu_reg->rom << 8) | cpu_reg->pc;
	opcode = cpu_reg->ucode [addr];
	
	cpu_reg->carry = 0;
	
	if (cpu_reg->key_flag)
        BIT_SET(cpu_reg->s,0);
	// copy external flags to s only for calculators with switch
	cpu_reg->s |= cpu_reg->ext_flag;
	
	cpu_reg->pc++;
	
    switch (opcode & 0x0003) {
        case 0x00:
            classic_execute_instruction_00(cpu_reg,opcode);
            break;
        case 0x01:
            op_jsb (cpu_reg, opcode);
            break;
        case 0x02:
            op_arith (cpu_reg, opcode);
            break;
        case 0x03:
			if (!carry)
				op_goto (cpu_reg, opcode);
            break;
    }
	
	return (true);  /* never sleeps */
}

void classic_press_key (cpu_t *cpu_reg, int keycode)
{
	cpu_reg->key_buf = keycode;
	cpu_reg->key_flag = true;
}

void classic_release_key (cpu_t *cpu_reg)
{
	cpu_reg->key_flag = false;
}

void classic_set_ext_flag (cpu_t *cpu_reg, int flag, bool state)
{
    if (state) {
        BIT_SET(cpu_reg->ext_flag, flag);
    } else {
        BIT_CLEAR(cpu_reg->ext_flag, flag);
    }

}

bool classic_get_ext_flag (cpu_t *cpu_reg, int flag)
{
    return BIT_TEST(cpu_reg->ext_flag,flag);
}


void classic_reset (cpu_t *cpu_reg)
{	
	cpu_reg->pc = 0;
	cpu_reg->rom = 0;
	cpu_reg->group = 0;
	cpu_reg->del_rom = 0;
	cpu_reg->del_grp = 0;
	
	op_clear_reg (cpu_reg, 0);
	op_clear_s (cpu_reg, 0);
	cpu_reg->p = 0;
	
	cpu_reg->display_enable = 0;
	
	cpu_reg->key_flag = 0;
}

void classic_clear_memory (cpu_t *cpu_reg)
{
	int addr;
	
	for (addr = 0; addr < cpu_reg->max_ram; addr++)
		reg_zero ((digit_t *)cpu_reg->ram [addr], 0, WSIZE - 1);
}

static void classic_new_rom_addr_space (cpu_t *cpu_reg,
										int max_bank,
										int max_page,
										int page_size)
{
	size_t max_words;
	
	max_words = max_bank * max_page * page_size;
	
	cpu_reg->ucode = alloc (max_words * sizeof (rom_word_t));
}


static void classic_new_ram_addr_space (cpu_t *cpu_reg, int max_ram)
{
	cpu_reg->max_ram = max_ram;
	cpu_reg->ram = malloc (max_ram * sizeof (reg_t));
    classic_clear_memory (cpu_reg);
}

cpu_t *classic_new_processor (int ram_size)
{
	cpu_t *cpu_reg;
	
	cpu_reg = malloc (sizeof (cpu_t));
	memset(cpu_reg,0,sizeof (cpu_t));
	
	classic_new_ram_addr_space (cpu_reg, ram_size);
	
	// RAM is contiguous starting from address 0.
	cpu_reg->max_ram = ram_size;
    classic_new_ram_addr_space (cpu_reg, ram_size);
	
	////cpu_reg->ucode = rom_data;
    classic_new_rom_addr_space (cpu_reg, MAX_BANK, MAX_PAGE, N_PAGE_SIZE);
	
	classic_reset (cpu_reg);
	
	return cpu_reg;
}

//
// Read rom file
//

static bool parse_octal (char *oct, int digits, int *val)
{
	*val = 0;
	
	while (digits--)
    {
		if (((*oct) < '0') || ((*oct) > '7'))
			return (false);
		(*val) = ((*val) << 3) + ((*(oct++)) - '0');
    }
	return (true);
}

static bool classic_parse_object_line (char *buf, int *bank, int *addr,
								rom_word_t *opcode)
{
	int a, o;
	
	if (buf [0] == '#')  /* comment? */
		return (false);
	
	if (strlen (buf) != 9)
		return (false);
	
	if (buf [4] != ':')
	{
		fprintf (stderr, "invalid object file format\n");
		return (false);
	}
	
	if (! parse_octal (& buf [0], 4, & a))
	{
		fprintf (stderr, "invalid address %o\n", a);
		return (false);
	}
	
	if (! parse_octal (& buf [5], 4, & o))
	{
		fprintf (stderr, "invalid opcode %o\n", o);
		return (false);
	}
	
	*bank = 0;
	*addr = a;
	*opcode = o;
	return (true);
}

bool classic_write_rom (cpu_t *cpu_reg,
						uint8_t    bank,
						addr_t     addr,
						rom_word_t *val)
{
	if ((bank >= MAX_BANK) || (addr > (MAX_PAGE * N_PAGE_SIZE)))
		return false;
	
	//cpu_reg->rom_exists [addr] = true;
	cpu_reg->ucode [addr] = *val;
	
	return true;
}


bool classic_read_object_file (cpu_t *cpu_reg, const char *fn)
{
	FILE *f;
	int bank;
	int addr;  // should change to addr_t, but will have to change
	// the parse function profiles to match.
	rom_word_t opcode;
	int count = 0;
	char buf [80];
	char magic [4];
	bool eof, error;
	
	f = fopen (fn, "rb");
	if (! f)
    {
		fprintf (stderr, "error opening object file\n");
		return (false);
    }
	
	if (fread_bytes (f, magic, sizeof (magic), & eof, & error) != sizeof (magic))
    {
		fprintf (stderr, "error reading object file\n");
		return (false);
    }
	
	//if (strncmp (magic, "MOD1", sizeof (magic)) == 0)
	//  return sim_read_mod1_file (sim, f);
	
	// switch from binary to text mode, and rewind
	f = freopen (fn, "r", f);
	
	if (! f)
    {
		fprintf (stderr, "error reopening object file\n");
		return (false);
    }
	
	while (fgets (buf, sizeof (buf), f))
    {
		trim_trailing_whitespace (buf);
		if (! buf [0])
			continue;
		if (classic_parse_object_line (buf, & bank, & addr, & opcode))
		{
			if (! classic_write_rom (cpu_reg, bank, addr, & opcode))
				fatal (3, "can't load ROM word at bank %d address %o\n", bank, addr);
			count++;
		}
    }
	
#if 0
	fprintf (stderr, "read %d words from '%s'\n", count, fn);
#endif
	return (true);
}
