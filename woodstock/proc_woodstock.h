/*
$Id: proc_woodstock.h 686 2005-05-26 09:06:45Z eric $
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
// any changes since 0.77 copyright 2005-2012 Maciej Bartosiak
//

//#define WSIZE 14
//#define EXPSIZE 3  // two exponent and one exponent sign digit

#define WOODSTOCK_LEFT_SCAN (WSIZE - 1)
#define WOODSTOCK_RIGHT_SCAN 2

#define SPICE_LEFT_SCAN (WSIZE - 2)

#define WOODSTOCK_DISPLAY_DIGITS 12
#define SPICE_DISPLAY_DIGITS 11

//typedef digit_t reg_t [WSIZE];


#define SSIZE 16
#define STACK_SIZE 2

#define EXT_FLAG_SIZE 16

#define N_PAGE_SIZE 1024
#define MAX_PAGE 4
#define MAX_BANK 2


typedef uint16_t rom_addr_t;


typedef enum
  {
    norm,
    branch,
    selftest
  } inst_state_t;


typedef struct woodstock_cpu_t
{
    reg_t a;
    reg_t b;
    reg_t c;
    reg_t y;
    reg_t z;
    reg_t t;
    reg_t m1;
    reg_t m2;
    
    digit_t f;
    
    digit_t p;
    
    bool decimal;
    
    bool carry, prev_carry;
    
    uint16_t s;
    uint16_t ext_flag;
    
    uint16_t pc;
    
    bool del_rom_flag;
    uint8_t del_rom;
    
    inst_state_t inst_state;
    
    int sp;  /* stack pointer */
    uint16_t stack [STACK_SIZE];
    
    //int prev_pc;  /* used to store complete five-digit octal address of instruction */
    
    int crc;
    
    // keyboard
    
    bool key_flag;      /* true if a key is down */
    int key_buf;        /* most recently pressed key */
    
    // display
    
    bool display_enable;
    //bool display_14_digit;  // true after RESET TWF instruction
    
    void (* op_fcn [1024])(struct woodstock_cpu_t *act_reg, int opcode);
        
    // ROM:
    uint8_t bank_exists [MAX_PAGE];
    bool bank;                       // only a single global bank bit
    rom_word_t *rom;
    
    // RAM:
    uint16_t    max_ram;
    uint16_t    ram_addr;  /* selected RAM address */
    reg_t       *ram;
    
} cpu_t;

cpu_t *spice_new_processor (int ram_size);
cpu_t *woodstock_new_processor (int ram_size);

void woodstock_set_ext_flag (cpu_t *act_reg, int flag, bool state);
bool woodstock_get_ext_flag (cpu_t *act_reg, int flag);

bool woodstock_execute_instruction (cpu_t *act_reg);
bool spice_execute_instruction (cpu_t *act_reg);

bool woodstock_read_object_file (cpu_t *act_reg, const char *fn);
void woodstock_set_object(cpu_t *act_reg, uint8_t *bank_exists, rom_word_t *rom);

void woodstock_press_key (cpu_t *act_reg, int keycode);
void woodstock_release_key (cpu_t *act_reg);

void woodstock_reset (cpu_t *act_reg);
void spice_reset (cpu_t *act_reg);
void woodstock_clear_memory (cpu_t *act_reg);

char* reg2str (char *str, reg_t reg);
void str2reg(reg_t reg, const char *str);