/*
$Id: proc_classic.h 686 2005-05-26 09:06:45Z eric $
Copyright 1995, 2003, 2004, 2005 Eric L. Smith <eric@brouhaha.com>

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

#define CLASSIC_LEFT_SCAN (WSIZE - 1)
#define CLASSIC_RIGHT_SCAN 0

#define CLASSIC_DISPLAY_DIGITS 14

#define SSIZE 12
#define EXT_FLAG_SIZE 12

#define MAX_GROUP 2
#define MAX_ROM 8
#define ROM_SIZE 256

#if 1
  // This was the easiest way to use the existing code:
  #define MAX_BANK 1
  #define MAX_PAGE 1
  #define N_PAGE_SIZE (MAX_GROUP * MAX_ROM * ROM_SIZE)
#else
  // This would somewhat more accurately represent the hardware
  // behavior, but the current code won't deal with it correctly:
  #define MAX_BANK (MAX_GROUP * MAX_ROM)
  #define MAX_PAGE 1
  #define N_PAGE_SIZE (ROM_SIZE)
#endif

//struct cpu_t;

typedef struct classic_cpu_t
{
    reg_t a;
    reg_t b;
    reg_t c;
    reg_t d;
    reg_t e;
    reg_t f;
    reg_t m;
    
    digit_t p;
    
    bool carry;
    
    uint8_t pc;
    uint8_t rom;
    uint8_t group;
    
    uint8_t del_rom;
    uint8_t del_grp;
    
    uint8_t ret_pc;
    
    uint16_t s;
    uint16_t ext_flag;  // external flags, e.g., slide switches, magnetic card inserted
    
    // keyboard
    bool key_flag;      /* true if a key is down */
    int key_buf;        /* most recently pressed key */
    
    // display
    bool display_enable;
    
    // ROM:
    rom_word_t  *ucode;  // name "rom" was already taken
    
    // RAM
    uint16_t    max_ram;
    uint16_t    ram_addr;  /* selected RAM address */
    reg_t       *ram;
} cpu_t;

cpu_t *classic_new_processor (int ram_size);

bool classic_read_rom (cpu_t *cpu_reg,
					   uint8_t    bank,
					   addr_t     addr,
					   rom_word_t *val);

bool classic_write_rom (cpu_t *cpu_reg,
						uint8_t    bank,
						addr_t     addr,
						rom_word_t *val);

bool classic_write_rom (cpu_t *cpu_reg,
						uint8_t    bank,
						addr_t     addr,
						rom_word_t *val);

bool classic_execute_instruction (cpu_t *cpu_reg);
void classic_press_key (cpu_t *cpu_reg, int keycode);
void classic_release_key (cpu_t *cpu_reg);
void classic_set_ext_flag (cpu_t *cpu_reg, int flag, bool state);
bool classic_get_ext_flag (cpu_t *cpu_reg, int flag);
void classic_reset (cpu_t *cpu_reg);
void classic_clear_memory (cpu_t *cpu_reg);
bool classic_read_object_file (cpu_t *cpu_reg, const char *fn);
