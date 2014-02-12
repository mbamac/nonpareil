/*
 $Id: proc_woodstock.c 846 2005-06-27 06:08:22Z eric $
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
// any changes since 0.77 copyright 2005-2012 Maciej Bartosiak
//

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "digit_ops.h"
#include "utils.h"
#include "proc_woodstock.h"

/* If defined, print warnings about stack overflow or underflow. */
#undef STACK_WARNING

#define BIT_SET(val,bit_num)    ((val) |= (1 << (bit_num)))
#define BIT_CLEAR(val,bit_num)  ((val) &= ~(1 << (bit_num)))
#define BIT_TEST(val,bit_num)   ((val) & (1 << (bit_num)))


static inline uint8_t get_effective_bank (cpu_t *act_reg, rom_addr_t addr)
{
	uint8_t page = addr / N_PAGE_SIZE;
	uint8_t bank;
	
	bank = act_reg->bank;
	if (! (act_reg->bank_exists [page] & (1 << bank)))
		bank = 0;
	
	return bank;
}


static rom_word_t woodstock_get_ucode (cpu_t *act_reg, rom_addr_t addr)
{
	uint8_t bank;
	//uint16_t index;
	
	bank = get_effective_bank (act_reg, addr);
	
	// $$$ check for non-existent memory?
	//index = bank * (MAX_PAGE * N_PAGE_SIZE) + addr;
	
	//if (index > (sizeof rom_data / sizeof(rom_data[0]))) {
	//	printf("index too big: %d\n", index);
	//}
	
	
	return act_reg->rom [bank * (MAX_PAGE * N_PAGE_SIZE) + addr];
}

static inline uint8_t arithmetic_base (cpu_t *act_reg)
{
	return act_reg->decimal ? 10 : 16;
}


//static void woodstock_print_state (cpu_t *act_reg);


static void bad_op (cpu_t *act_reg, int opcode)
{	
	printf ("illegal opcode %04o at %05o\n", opcode, act_reg->pc - 1);
}


static void op_arith (cpu_t *act_reg, int opcode)
{
	uint8_t op, field;
	int first = 0;
	int last = 0;
	
	op = opcode >> 5;
	field = (opcode >> 2) & 7;
	
	switch (field)
    {
		case 0:  /* p  */
			first = act_reg->p; last = act_reg->p;
			if (act_reg->p >= WSIZE)
			{
				printf ("Warning! p >= WSIZE at %05o\n", act_reg->pc - 1);
				//woodstock_print_state (act_reg);
				last = 0;  /* don't do anything */
			}
				break;
		case 1:  /* wp */
			first = 0; last = act_reg->p;
			if (act_reg->p >= WSIZE)
			{
				printf ("Warning! p >= WSIZE at %05o\n", act_reg->pc - 1);
				//woodstock_print_state (act_reg);
				last = WSIZE - 1;
			}
				break;
		case 2:  /* xs */  first = EXPSIZE - 1; last = EXPSIZE - 1; break;
		case 3:  /* x  */  first = 0;           last = EXPSIZE - 1; break;
		case 4:  /* s  */  first = WSIZE - 1;   last = WSIZE - 1;   break;
		case 5:  /* m  */  first = EXPSIZE;     last = WSIZE - 2;   break;
		case 6:  /* w  */  first = 0;           last = WSIZE - 1;   break;
		case 7:  /* ms */  first = EXPSIZE;     last = WSIZE - 1;   break;
    }
	
	act_reg->carry = 0;
	
	switch (op)
    {
		case 0x00:  /* 0 -> a[f] */
			reg_zero (act_reg->a, first, last);
			break;
		case 0x01:  /* 0 -> b[f] */
			reg_zero (act_reg->b, first, last);
			break;
		case 0x02:  /* a exchange b[f] */
			reg_exch (act_reg->a, act_reg->b, first, last);
			break;
		case 0x03:  /* a -> b[f] */
			reg_copy (act_reg->b, act_reg->a, first, last);
			break;
		case 0x04:  /* a exchange c[f] */
			reg_exch (act_reg->a, act_reg->c, first, last);
			break;
		case 0x05:  /* c -> a[f] */
			reg_copy (act_reg->a, act_reg->c, first, last);
			break;
		case 0x06:  /* b -> c[f] */
			reg_copy (act_reg->c, act_reg->b, first, last);
			break;
		case 0x07:  /* b exchange c[f] */
			reg_exch (act_reg->b, act_reg->c, first, last);
			break;
		case 0x08:  /* 0 -> c[f] */
			reg_zero (act_reg->c, first, last);
			break;
		case 0x09:  /* a + b -> a[f] */
			reg_add (act_reg->a, act_reg->a, act_reg->b,
					 first, last,
					 & act_reg->carry, arithmetic_base (act_reg));
			break;
		case 0x0a:  /* a + c -> a[f] */
			reg_add (act_reg->a, act_reg->a, act_reg->c,
					 first, last,
					 & act_reg->carry, arithmetic_base (act_reg));
			break;
		case 0x0b:  /* c + c -> c[f] */
			reg_add (act_reg->c, act_reg->c, act_reg->c,
					 first, last,
					 & act_reg->carry, arithmetic_base (act_reg));
			break;
		case 0x0c:  /* a + c -> c[f] */
			reg_add (act_reg->c, act_reg->a, act_reg->c,
					 first, last,
					 & act_reg->carry, arithmetic_base (act_reg));
			break;
		case 0x0d:  /* a + 1 -> a[f] */
			act_reg->carry = 1;
			reg_add (act_reg->a, act_reg->a, NULL,
					 first, last,
					 & act_reg->carry, arithmetic_base (act_reg));
			break;
		case 0x0e:  /* shift left a[f] */
			reg_shift_left (act_reg->a, first, last);
			break;
		case 0x0f:  /* c + 1 -> c[f] */
			act_reg->carry = 1;
			reg_add (act_reg->c, act_reg->c, NULL,
					 first, last,
					 & act_reg->carry, arithmetic_base (act_reg));
			break;
		case 0x10:  /* a - b -> a[f] */
			reg_sub (act_reg->a, act_reg->a, act_reg->b,
					 first, last,
					 & act_reg->carry, arithmetic_base (act_reg));
			break;
		case 0x11:  /* a - c -> c[f] */
			reg_sub (act_reg->c, act_reg->a, act_reg->c,
					 first, last,
					 & act_reg->carry, arithmetic_base (act_reg));
			break;
		case 0x12:  /* a - 1 -> a[f] */
			act_reg->carry = 1;
			reg_sub (act_reg->a, act_reg->a, NULL,
					 first, last,
					 & act_reg->carry, arithmetic_base (act_reg));
			break;
		case 0x13:  /* c - 1 -> c[f] */
			act_reg->carry = 1;
			reg_sub (act_reg->c, act_reg->c, NULL,
					 first, last,
					 & act_reg->carry, arithmetic_base (act_reg));
			break;
		case 0x14:  /* 0 - c -> c[f] */
			reg_sub (act_reg->c, NULL, act_reg->c,
					 first, last,
					 & act_reg->carry, arithmetic_base (act_reg));
			break;
		case 0x15:  /* 0 - c - 1 -> c[f] */
			act_reg->carry = 1;
			reg_sub (act_reg->c, NULL, act_reg->c,
					 first, last,
					 & act_reg->carry, arithmetic_base (act_reg));
			break;
		case 0x16:  /* if b[f] = 0 */
			act_reg->inst_state = branch;
			reg_test_nonequal (act_reg->b, NULL, first, last, & act_reg->carry);
			break;
		case 0x17:  /* if c[f] = 0 */
			act_reg->inst_state = branch;
			reg_test_nonequal (act_reg->c, NULL, first, last, & act_reg->carry);
			break;
		case 0x18:  /* if a >= c[f] */
			act_reg->inst_state = branch;
			reg_sub (NULL, act_reg->a, act_reg->c,
					 first, last,
					 & act_reg->carry, arithmetic_base (act_reg));
			break;
		case 0x19:  /* if a >= b[f] */
			act_reg->inst_state = branch;
			reg_sub (NULL, act_reg->a, act_reg->b,
					 first, last,
					 & act_reg->carry, arithmetic_base (act_reg));
			break;
		case 0x1a:  /* if a[f] # 0 */
			act_reg->inst_state = branch;
			reg_test_equal (act_reg->a, NULL, first, last, & act_reg->carry);
			break;
		case 0x1b:  /* if c[f] # 0 */
			act_reg->inst_state = branch;
			reg_test_equal (act_reg->c, NULL, first, last, & act_reg->carry);
			break;
		case 0x1c:  /* a - c -> a[f] */
			reg_sub (act_reg->a, act_reg->a, act_reg->c,
					 first, last,
					 & act_reg->carry, arithmetic_base (act_reg));
			break;
		case 0x1d:  /* shift right a[f] */
			reg_shift_right (act_reg->a, first, last);
			break;
		case 0x1e:  /* shift right b[f] */
			reg_shift_right (act_reg->b, first, last);
			break;
		case 0x1f:  /* shift right c[f] */
			reg_shift_right (act_reg->c, first, last);
			break;
    }
}


static void handle_del_rom (cpu_t *act_reg)
{
	if (act_reg->del_rom_flag)
    {
		act_reg->pc = (act_reg->del_rom << 8) + (act_reg->pc & 0377);
		act_reg->del_rom_flag = 0;
    }
}


static void op_goto (cpu_t *act_reg, int opcode)
{	
	if (! act_reg->prev_carry)
    {
		act_reg->pc = (act_reg->pc & ~0377) | (opcode >> 2);
		handle_del_rom (act_reg);
    }
}


static void op_jsb (cpu_t *act_reg, int opcode)
{	
	act_reg->stack [act_reg->sp] = act_reg->pc;
	act_reg->sp++;
	if (act_reg->sp >= STACK_SIZE)
    {
#ifdef STACK_WARNING
		printf ("stack overflow\n");
#endif
		act_reg->sp = 0;
    }
	act_reg->pc = (act_reg->pc & ~0377) | (opcode >> 2);
	handle_del_rom (act_reg);
}


static void op_return (cpu_t *act_reg, int opcode)
{	
	act_reg->sp--;
	if (act_reg->sp < 0)
    {
#ifdef STACK_WARNING
		printf ("stack underflow\n");
#endif
		act_reg->sp = STACK_SIZE - 1;
    }
	act_reg->pc = act_reg->stack [act_reg->sp];
}


static void op_nop (cpu_t *act_reg, int opcode)
{
}


static void op_binary (cpu_t *act_reg, int opcode)
{
	act_reg->decimal = false;
}


static void op_decimal (cpu_t *act_reg, int opcode)
{	
	act_reg->decimal = true;
}


/* $$$ woodstock doc says when increment or decrement P wraps,
* P "disappears for one word time". */

static void op_dec_p (cpu_t *act_reg, int opcode)
{	
	if (act_reg->p)
		act_reg->p--;
	else
		act_reg->p = WSIZE - 1;
}


static void op_inc_p (cpu_t *act_reg, int opcode)
{	
	act_reg->p++;
	if (act_reg->p >= WSIZE)
		act_reg->p = 0;
}


static void op_clear_s (cpu_t *act_reg, int opcode)
{
	int i;
	
	for (i = 0; i < SSIZE; i++)
		if ((i != 1) && (i != 2) && (i != 5) && (i != 15))
			//act_reg->s [i] = 0;
            BIT_CLEAR(act_reg->s, i);
}


static void op_m1_exch_c (cpu_t *act_reg, int opcode)
{	
	reg_exch (act_reg->c, act_reg->m1, 0, WSIZE - 1);
}


static void op_m1_to_c (cpu_t *act_reg, int opcode)
{	
	reg_copy (act_reg->c, act_reg->m1, 0, WSIZE - 1);
}


static void op_m2_exch_c (cpu_t *act_reg, int opcode)
{	
	reg_exch (act_reg->c, act_reg->m2, 0, WSIZE - 1);
}


static void op_m2_to_c (cpu_t *act_reg, int opcode)
{	
	reg_copy (act_reg->c, act_reg->m2, 0, WSIZE - 1);
}


static void op_f_to_a (cpu_t *act_reg, int opcode)
{
	act_reg->a [0] = act_reg->f;
}


static void op_f_exch_a (cpu_t *act_reg, int opcode)
{
	int t;
	
	t = act_reg->a [0];
	act_reg->a [0] = act_reg->f;
	act_reg->f = t;
}


static void op_circulate_a_left (cpu_t *act_reg, int opcode)
{
	int i, t;
	
	t = act_reg->a [WSIZE - 1];
	for (i = WSIZE - 1; i >= 1; i--)
		act_reg->a [i] = act_reg->a [i - 1];
	act_reg->a [0] = t;
}


static void op_bank_switch (cpu_t *act_reg, int opcode)
{	
	act_reg->bank ^= 1;
}


static void op_rom_selftest (cpu_t *act_reg, int opcode)
{	
	act_reg->crc = 01777;
	act_reg->inst_state = selftest;
	act_reg->pc &= ~ 01777;  // start from beginning of current 1K ROM bank
	printf ("starting ROM CRC of bank %d addr %04o\n", act_reg->bank, act_reg->pc);
}


static void rom_selftest_done (cpu_t *act_reg)
{	
	// ROM self-test completed, return and set S5 if error
	printf ("ROM CRC done, crc = %03x: %s\n", act_reg->crc,
			act_reg->crc == 0x078 ? "good" : "bad");
	if (act_reg->crc != 0x078)
		//act_reg->s [5] = 1;  // indicate error
        BIT_SET(act_reg->s, 5);
	act_reg->inst_state = norm;
	op_return (act_reg, 0);
}


static void crc_update (cpu_t *act_reg, int word)
{
	int i;
	int b;
	
	for (i = 0; i < 10; i++)
    {
		b = act_reg->crc & 1;
		act_reg->crc >>= 1;
		if (b ^ (word & 1))
			act_reg->crc ^= 0x331;
		word >>= 1;
    }
}


static void op_c_to_addr (cpu_t *act_reg, int opcode)
{	
	act_reg->ram_addr = (act_reg->c [1] << 4) + act_reg->c [0];

	if (act_reg->ram_addr >= act_reg->max_ram)
		printf ("c -> ram addr: address %d out of range\n", act_reg->ram_addr);
}


static void op_c_to_data (cpu_t *act_reg, int opcode)
{
	int i;
	
	if (act_reg->ram_addr >= act_reg->max_ram)
    {
		printf ("c -> data: address %02x out of range\n", act_reg->ram_addr);
		return;
    }
	for (i = 0; i < WSIZE; i++)
		act_reg->ram [act_reg->ram_addr] [i] = act_reg->c [i];
}


static void op_data_to_c (cpu_t *act_reg, int opcode)
{
	int i;
	
	if (act_reg->ram_addr >= act_reg->max_ram)
    {
		printf ("data -> c: address %d out of range, loading 0\n", act_reg->ram_addr);
		for (i = 0; i < WSIZE; i++)
			act_reg->c [i] = 0;
		return;
    }
	for (i = 0; i < WSIZE; i++)
		act_reg->c [i] = act_reg->ram [act_reg->ram_addr] [i];
}


static void op_c_to_register (cpu_t *act_reg, int opcode)
{
	int i;
	
	act_reg->ram_addr &= ~017;
	act_reg->ram_addr += (opcode >> 6);
	
	if (act_reg->ram_addr >= act_reg->max_ram)
    {
		printf ("c -> register: address %d out of range\n", act_reg->ram_addr);
		return;
    }

	for (i = 0; i < WSIZE; i++)
		act_reg->ram [act_reg->ram_addr] [i] = act_reg->c [i];
}


static void op_register_to_c (cpu_t *act_reg, int opcode)
{
	int i;
	
	act_reg->ram_addr &= ~017;
	act_reg->ram_addr += (opcode >> 6);
	
	if (act_reg->ram_addr >= act_reg->max_ram)
    {
		printf ("register -> c: address %d out of range, loading 0\n", act_reg->ram_addr);
		for (i = 0; i < WSIZE; i++)
			act_reg->c [i] = 0;
		return;
    }

	for (i = 0; i < WSIZE; i++)
		act_reg->c [i] = act_reg->ram [act_reg->ram_addr] [i];
}


static void op_clear_data_regs (cpu_t *act_reg, int opcode)
{
	int base;
	int i, j;
	
	base = act_reg->ram_addr & ~ 017;
	for (i = base; i <= base + 15; i++)
		for (j = 0; j < WSIZE; j++)
			act_reg->ram [i] [j] = 0;
}


static void op_c_to_stack (cpu_t *act_reg, int opcode)
{
	int i;
	
	for (i = 0; i < WSIZE; i++)
    {
		act_reg->t [i] = act_reg->z [i];
		act_reg->z [i] = act_reg->y [i];
		act_reg->y [i] = act_reg->c [i];
    }
}


static void op_stack_to_a (cpu_t *act_reg, int opcode)
{
	int i;
	
	for (i = 0; i < WSIZE; i++)
    {
		act_reg->a [i] = act_reg->y [i];
		act_reg->y [i] = act_reg->z [i];
		act_reg->z [i] = act_reg->t [i];
    }
}


static void op_y_to_a (cpu_t *act_reg, int opcode)
{
	int i;
	
	for (i = 0; i < WSIZE; i++)
    {
		act_reg->a [i] = act_reg->y [i];
    }
}


static void op_down_rotate (cpu_t *act_reg, int opcode)
{
	int i, t;
	
	for (i = 0; i < WSIZE; i++)
    {
		t = act_reg->c [i];
		act_reg->c [i] = act_reg->y [i];
		act_reg->y [i] = act_reg->z [i];
		act_reg->z [i] = act_reg->t [i];
		act_reg->t [i] = t;
    }
}


static void op_clear_reg (cpu_t *act_reg, int opcode)
{
	int i;
	
	for (i = 0; i < WSIZE; i++)
		act_reg->a [i] = act_reg->b [i] = act_reg->c [i] = act_reg->y [i] =
			act_reg->z [i] = act_reg->t [i] = 0;
	// Apparently we're not supposed to clear F, or the HP-21 CLR function
	// resets the display format.
	// Should this clear P?  Probably not.
}


static void op_load_constant (cpu_t *act_reg, int opcode)
{	
	if (act_reg->p >= WSIZE)
		printf ("load constant w/ p >= WSIZE at %05o\n", act_reg->pc - 1);
	else
		act_reg->c [act_reg->p] = opcode >> 6;
	if (act_reg->p)
		act_reg->p--;
	else
		act_reg->p = WSIZE - 1;
}


static void op_set_s (cpu_t *act_reg, int opcode)
{	
	if ((opcode >> 6) >= SSIZE)
		printf ("stat >= SSIZE at %05o\n", act_reg->pc - 1);
	else
        BIT_SET(act_reg->s, (opcode >> 6));
}


static void op_clr_s (cpu_t *act_reg, int opcode)
{	
	if ((opcode >> 6) >= SSIZE)
		printf ("stat >= SSIZE at %05o\n", act_reg->pc - 1);
	else
        BIT_CLEAR(act_reg->s, (opcode >> 6));
}


static void op_test_s_eq_0 (cpu_t *act_reg, int opcode)
{	
	act_reg->inst_state = branch;
    act_reg->carry = BIT_TEST(act_reg->s, (opcode >> 6));
}


static void op_test_s_eq_1 (cpu_t *act_reg, int opcode)
{	
	act_reg->inst_state = branch;
	act_reg->carry = ! BIT_TEST(act_reg->s, (opcode >> 6));
}


static uint8_t p_set_map [16] =
{ 14,  4,  7,  8, 11,  2, 10, 12,  1,  3, 13,  6,  0,  9,  5, 14 };

static uint8_t p_test_map [16] =
{  4,  8, 12,  2,  9,  1,  6,  3,  1, 13,  5,  0, 11, 10,  7,  4 };


static void op_set_p (cpu_t *act_reg, int opcode)
{	
	act_reg->p = p_set_map [opcode >> 6];
	if (act_reg->p >= WSIZE)
		printf ("invalid set p, operand encoding is %02o\n", opcode > 6);
}


static void op_test_p_eq (cpu_t *act_reg, int opcode)
{	
	act_reg->inst_state = branch;
	act_reg->carry = ! (act_reg->p == p_test_map [opcode >> 6]);
}


static void op_test_p_ne (cpu_t *act_reg, int opcode)
{
	act_reg->inst_state = branch;
	act_reg->carry = ! (act_reg->p != p_test_map [opcode >> 6]);
}


static void op_sel_rom (cpu_t *act_reg, int opcode)
{
	act_reg->pc = ((opcode & 01700) << 2) + (act_reg->pc & 0377);
}


static void op_del_sel_rom (cpu_t *act_reg, int opcode)
{	
	act_reg->del_rom = opcode >> 6;
	act_reg->del_rom_flag = 1;
}


static void op_keys_to_rom_addr (cpu_t *act_reg, int opcode)
{
	act_reg->pc = act_reg->pc & ~0377;
	handle_del_rom (act_reg);
	if (act_reg->key_buf < 0)
    {
		printf ("keys->rom address with no key pressed\n");
		return;
    }
	act_reg->pc += act_reg->key_buf;
}


static void op_keys_to_a (cpu_t *act_reg, int opcode)
{	
	if (act_reg->key_buf < 0)
    {
		printf ("keys->a with no key pressed\n");
		act_reg->a [2] = 0;
		act_reg->a [1] = 0;
		return;
    }
	act_reg->a [2] = act_reg->key_buf >> 4;
	act_reg->a [1] = act_reg->key_buf & 0x0f;
}


static void op_a_to_rom_addr (cpu_t *act_reg, int opcode)
{	
	act_reg->pc = act_reg->pc & ~0377;
	handle_del_rom (act_reg);
	act_reg->pc += ((act_reg->a [2] << 4) + act_reg->a [1]);
}


static void op_display_off (cpu_t *act_reg, int opcode)
{	
	act_reg->display_enable = 0;
}


static void op_display_toggle (cpu_t *act_reg, int opcode)
{	
	act_reg->display_enable = ! act_reg->display_enable;
}


static void op_display_reset_twf (cpu_t *act_reg, int opcode)
{
    // is this correct?
	//act_reg->display_14_digit = true;
	//act_reg->right_scan = 0;
}


static void op_crc_clear_f1 (cpu_t *act_reg, int opcode)
{
	// don't do anything, as CRC F1 is controlled by hardware
	// (in our case, ext_flag [1])
	;  
}


static void op_crc_test_f1 (cpu_t *act_reg, int opcode)
{
    if (BIT_TEST(act_reg->ext_flag, 1))
        BIT_SET(act_reg->s, 3);
}


static void init_ops (cpu_t *act_reg)
{
	int i;
	
	for (i = 0; i < 1024; i += 4)
    {
		act_reg->op_fcn [i + 0] = bad_op;
		act_reg->op_fcn [i + 1] = op_jsb;    // type 1: aaaaaaaa01
		act_reg->op_fcn [i + 2] = op_arith;  // type 2: ooooowww10
		act_reg->op_fcn [i + 3] = op_goto;   // type 1: aaaaaaaa11
    }
	
	for (i = 0; i <= 15; i ++)
    {
		// xx00 uassigned
		act_reg->op_fcn [00004 + (i << 6)] = op_set_s;
		// xx10 misc
		act_reg->op_fcn [00014 + (i << 6)] = op_clr_s;
		// xx20 misc
		act_reg->op_fcn [00024 + (i << 6)] = op_test_s_eq_1;
		act_reg->op_fcn [00030 + (i << 6)] = op_load_constant;
		act_reg->op_fcn [00034 + (i << 6)] = op_test_s_eq_0;
		act_reg->op_fcn [00040 + (i << 6)] = op_sel_rom;
		act_reg->op_fcn [00044 + (i << 6)] = op_test_p_eq;
		act_reg->op_fcn [00050 + (i << 6)] = op_c_to_register;
		act_reg->op_fcn [00054 + (i << 6)] = op_test_p_ne;
		// xx60 misc
		act_reg->op_fcn [00064 + (i << 6)] = op_del_sel_rom;
		act_reg->op_fcn [00070 + (i << 6)] = op_register_to_c;
		act_reg->op_fcn [00074 + (i << 6)] = op_set_p;
    }
	
	act_reg->op_fcn [00000] = op_nop;
	act_reg->op_fcn [00070] = op_data_to_c;
	
	act_reg->op_fcn [00010] = op_clear_reg;
	act_reg->op_fcn [00110] = op_clear_s;
	act_reg->op_fcn [00210] = op_display_toggle;
	act_reg->op_fcn [00310] = op_display_off;
	act_reg->op_fcn [00410] = op_m1_exch_c;
	act_reg->op_fcn [00510] = op_m1_to_c;
	act_reg->op_fcn [00610] = op_m2_exch_c;
	act_reg->op_fcn [00710] = op_m2_to_c;
	act_reg->op_fcn [01010] = op_stack_to_a;
	act_reg->op_fcn [01110] = op_down_rotate;
	act_reg->op_fcn [01210] = op_y_to_a;
	act_reg->op_fcn [01310] = op_c_to_stack;
	act_reg->op_fcn [01410] = op_decimal;
	// 1510 unassigned
	act_reg->op_fcn [01610] = op_f_to_a;
	act_reg->op_fcn [01710] = op_f_exch_a;
	
	act_reg->op_fcn [00020] = op_keys_to_rom_addr;
	act_reg->op_fcn [00120] = op_keys_to_a;
	act_reg->op_fcn [00220] = op_a_to_rom_addr;
	act_reg->op_fcn [00320] = op_display_reset_twf;
	act_reg->op_fcn [00420] = op_binary;
	act_reg->op_fcn [00520] = op_circulate_a_left;
	act_reg->op_fcn [00620] = op_dec_p;
	act_reg->op_fcn [00720] = op_inc_p;
	act_reg->op_fcn [01020] = op_return;
	// 1120..1720 unknown, probably printer
	
	// 0060 unknown
	// 0160..0760 unassigned/unknown
	act_reg->op_fcn [01060] = op_bank_switch;
	act_reg->op_fcn [01160] = op_c_to_addr;
	act_reg->op_fcn [01260] = op_clear_data_regs;
	act_reg->op_fcn [01360] = op_c_to_data;
	act_reg->op_fcn [01460] = op_rom_selftest;  // Only on Spice series
	// 1560..1660 unassigned/unknown
	act_reg->op_fcn [01760] = op_nop;  // "HI I'M WOODSTOCK"
	
	// CRC chip in 67/97
	act_reg->op_fcn [00300] = op_crc_test_f1;
	act_reg->op_fcn [01500] = op_crc_clear_f1;
	
	act_reg->op_fcn [00560] = op_nop;
	
	act_reg->op_fcn [00400] = op_nop;
	act_reg->op_fcn [01000] = op_nop;
	act_reg->op_fcn [01300] = op_nop;
	
	//
	// Instruction codings unknown (probably 1160..1760):
	//    PRINT 0
	//    PRINT 1
	//    PRINT 2
	//    PRINT 3
	//    PRINT 6
	//    HOME?
	//    CR?
	//
}


static bool woodstock_execute_cycle (cpu_t *act_reg)
{
	rom_word_t opcode;
	inst_state_t prev_inst_state;
	
	//act_reg->prev_pc = act_reg->pc;
	opcode = woodstock_get_ucode (act_reg, act_reg->pc);
	
	if (/*(sim->platform != PLATFORM_SPICE) &&*/
		(act_reg->pc < 02000) &&
		(act_reg->bank == 1))
	{
		act_reg->bank = 0;
	}
		
	prev_inst_state = act_reg->inst_state;
	if (act_reg->inst_state == branch)
		act_reg->inst_state = norm;
	
	act_reg->prev_carry = act_reg->carry;
	act_reg->carry = 0;
	
	if (act_reg->key_flag)
        BIT_SET(act_reg->s, 15);
	    
    if (BIT_TEST(act_reg->ext_flag, 3))
        BIT_SET(act_reg->s, 3);
    if (BIT_TEST(act_reg->ext_flag, 5))
        BIT_SET(act_reg->s, 5);
	
	act_reg->pc++;
	
	switch (prev_inst_state)
    {
		case norm:
			(* act_reg->op_fcn [opcode]) (act_reg, opcode);
			break;
		case branch:
			if (! act_reg->prev_carry)
				act_reg->pc = (act_reg->pc & ~01777) | opcode;
			break;
		case selftest:
			crc_update (act_reg, opcode);
			if (opcode == 01060)
				op_bank_switch (act_reg, opcode);  // bank switch even in self-test
				if (! (act_reg->pc & 01777))    // end of 1K ROM bank?
					rom_selftest_done (act_reg);
					break;
    }
	
	return (true);  /* never sleeps */
}


bool woodstock_execute_instruction (cpu_t *act_reg)
{
	do
    {
		if (! woodstock_execute_cycle (act_reg))
			return false;
    }
	while (act_reg->inst_state != norm);
	return true;
}

static bool spice_execute_cycle (cpu_t *act_reg)
{
	rom_word_t opcode;
	inst_state_t prev_inst_state;
	
	//act_reg->prev_pc = act_reg->pc;
	opcode = woodstock_get_ucode (act_reg, act_reg->pc);
	
	prev_inst_state = act_reg->inst_state;
	if (act_reg->inst_state == branch)
		act_reg->inst_state = norm;
	
	act_reg->prev_carry = act_reg->carry;
	act_reg->carry = 0;
	
	if (act_reg->key_flag)
        BIT_SET(act_reg->s, 15);
	    
    if (BIT_TEST(act_reg->ext_flag, 3))
        BIT_SET(act_reg->s, 3);
    if (BIT_TEST(act_reg->ext_flag, 5))
        BIT_SET(act_reg->s, 5);
	
	act_reg->pc++;
	
	switch (prev_inst_state)
    {
		case norm:
			(* act_reg->op_fcn [opcode]) (act_reg, opcode);
			break;
		case branch:
			if (! act_reg->prev_carry)
				act_reg->pc = (act_reg->pc & ~01777) | opcode;
			break;
		case selftest:
			crc_update (act_reg, opcode);
			if (opcode == 01060)
				op_bank_switch (act_reg, opcode);  // bank switch even in self-test
				if (! (act_reg->pc & 01777))    // end of 1K ROM bank?
					rom_selftest_done (act_reg);
					break;
    }
	
	return (true);  /* never sleeps */
}

bool spice_execute_instruction (cpu_t *act_reg)
{
	do
    {
		if (! spice_execute_cycle (act_reg))
			return false;
    }
	while (act_reg->inst_state != norm);
	return true;
}


void woodstock_press_key (cpu_t *act_reg, int keycode)
{
	act_reg->key_buf = keycode;
	act_reg->key_flag = true;
}


void woodstock_release_key (cpu_t *act_reg)
{
	act_reg->key_flag = false;
}


void woodstock_set_ext_flag (cpu_t *act_reg, int flag, bool state)
{	
	//act_reg->ext_flag [flag] = state;
    if (state) {
        BIT_SET(act_reg->ext_flag, flag);
    } else {
        BIT_CLEAR(act_reg->ext_flag, flag);
    }
}

bool woodstock_get_ext_flag (cpu_t *act_reg, int flag)
{
    return BIT_TEST(act_reg->ext_flag,flag);
}

void woodstock_reset (cpu_t *act_reg)
{		
	act_reg->decimal = true;
	
	act_reg->pc = 0;
	act_reg->del_rom_flag = 0;
	
	act_reg->inst_state = norm;
	
	act_reg->sp = 0;
	
	op_clear_reg (act_reg, 0);
	op_clear_s (act_reg, 0);
	act_reg->p = 0;
	
	act_reg->display_enable = 0;
	
	act_reg->key_buf = -1;  // no key has been pressed
	act_reg->key_flag = 0;
	
    BIT_SET(act_reg->ext_flag, 5);
}

void spice_reset (cpu_t *act_reg)
{		
	act_reg->decimal = true;
	
	act_reg->pc = 0;
	act_reg->del_rom_flag = 0;
	
	act_reg->inst_state = norm;
	
	act_reg->sp = 0;
	
	op_clear_reg (act_reg, 0);
	op_clear_s (act_reg, 0);
	act_reg->p = 0;
	
	act_reg->display_enable = 0;
	
	act_reg->key_buf = -1;  // no key has been pressed
	act_reg->key_flag = 0;
}

void woodstock_clear_memory (cpu_t *act_reg)
{
	int addr;
	
	for (addr = 0; addr < act_reg->max_ram; addr++)
		reg_zero ((digit_t *)act_reg->ram [addr], 0, WSIZE - 1);
}

static void woodstock_new_rom_addr_space (cpu_t *act_reg,
										  int max_bank,
										  int max_page,
										  int page_size)
{
	size_t max_words;
	
	max_words = max_bank * max_page * page_size;
	
	act_reg->rom = alloc (max_words * sizeof (rom_word_t));
}

static void woodstock_new_ram_addr_space (cpu_t *act_reg, int max_ram)
{
	act_reg->max_ram = max_ram;
	act_reg->ram = alloc (max_ram * sizeof (reg_t));
}


cpu_t *woodstock_new_processor (int ram_size)
{
	cpu_t *act_reg;
	
	act_reg = alloc (sizeof (cpu_t));
	
    woodstock_new_rom_addr_space (act_reg, MAX_BANK, MAX_PAGE, N_PAGE_SIZE);
	woodstock_new_ram_addr_space (act_reg, ram_size);
	
	init_ops (act_reg);
    woodstock_reset (act_reg);
	
	return act_reg;
}

cpu_t *spice_new_processor (int ram_size)
{	
	cpu_t *act_reg;
	
	act_reg = malloc (sizeof (cpu_t));
	memset(act_reg,0,sizeof (cpu_t));
	
    woodstock_new_rom_addr_space (act_reg, MAX_BANK, MAX_PAGE, N_PAGE_SIZE);
	woodstock_new_ram_addr_space (act_reg, ram_size);
    
    init_ops (act_reg);
	spice_reset (act_reg);
	
	return act_reg;
}

//
// Read ROM
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

static bool woodstock_parse_object_line (char *buf, int *bank, int *addr,
                                  rom_word_t *opcode)
{
	bool has_bank;
	int b = 0;
	int a, o;
	
	if (buf [0] == '#')  /* comment? */
		return (false);
    
    if ((strlen (buf) < 9) || (strlen (buf) > 10))
		return (false);
    
    if (buf [4] == ':')
		has_bank = false;
    else if (buf [5] == ':')
		has_bank = true;
    else
    {
        fprintf (stderr, "invalid object file format\n");
        return (false);
    }
    
    if (has_bank && ! parse_octal (& buf [0], 1, & b))
    {
        fprintf (stderr, "invalid bank in object line '%s'\n", buf);
        return (false);
    }
    
    if (! parse_octal (& buf [has_bank ? 1 : 0], 4, & a))
    {
        fprintf (stderr, "invalid address in object line '%s'\n", buf);
        return (false);
    }
    
    if (! parse_octal (& buf [has_bank ? 6 : 5], 4, & o))
    {
        fprintf (stderr, "invalid opcode in object line '%s'\n", buf);
        return (false);
    }
    
    *bank = b;
    *addr = a;
    *opcode = o;
    return (true);
}

static bool woodstock_write_rom (cpu_t *act_reg,
                          uint8_t    bank,
                          addr_t     addr,
                          rom_word_t *val)
{
	uint8_t page;
	uint16_t rom_index;
	
	if (addr >= (MAX_PAGE * N_PAGE_SIZE))
		return false;
	
	page = addr / N_PAGE_SIZE;
	
	act_reg->bank_exists [page] |= (1 << bank);
	
	rom_index = bank * (MAX_PAGE * N_PAGE_SIZE) + addr;
	
	//act_reg->rom_exists [rom_index] = true;
	act_reg->rom [rom_index] = *val;
	
	return true;
}

bool woodstock_read_object_file (cpu_t *act_reg, const char *fn)
{
	FILE *f;
	int bank;
	int addr;  // should change to addr_t, but will have to change
	// the parse function profiles to match.
	rom_word_t opcode;
	int count = 0;
	char buf [80];
	//char magic [4];
	//bool eof, error;
	
	f = fopen (fn, "rb");
	if (! f)
    {
		fprintf (stderr, "error opening object file\n");
		return (false);
    }
	
	//if (fread_bytes (f, magic, sizeof (magic), & eof, & error) != sizeof (magic))
    //{
	//	fprintf (stderr, "error reading object file\n");
	//	return (false);
    //}
	
	//f = freopen (fn, "r", f);
	
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
		if (woodstock_parse_object_line (buf, & bank, & addr, & opcode))
		{
			if (! woodstock_write_rom (act_reg, bank, addr, & opcode))
				fatal (3, "can't load ROM word at bank %d address %o\n", bank, addr);
			count++;
		}
    }
	
#if 0
	fprintf (stderr, "read %d words from '%s'\n", count, fn);
#endif
	return (true);
}
