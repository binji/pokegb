#include <SDL2/SDL.h>
#include <cstdint>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define OP4_NX8(_) case _: case _ + 8: case _ + 16: case _ + 24:

#define OP4_NX16_REL(_)                                                        \
  case _: case _ + 16: case _ + 32: case _ + 48:                               \
    opcrel = (opcode - _)/16;

#define OP5_FLAG(_, always)                                                    \
  OP4_NX8(_)                                                                   \
  case always:                                                                 \
    carry = (opcode == always) ||                                              \
            (F & F_mask[(opcode - _) / 8]) == F_equals[(opcode - _) / 8];

#define OP8_REL(_)                                                             \
  case _ ... _ + 7:                                                            \
    tmp8 = reg8_access(0, 0, opcrel = opcode);

#define OP8_NX8_REL(_)                                                         \
  OP4_NX8(_) OP4_NX8(_ + 32)                                                   \
    tmp8 = reg8_access(0, 0, opcrel = (opcode - _) / 8);

#define OP64_REL(_)                                                            \
  case _ ... _ + 55: OP8_REL(_ + 56)                                           \
    opcrel = (opcode - _) / 8;

#define OP9_IMM_PTR(_)                                                         \
  OP8_REL(_) case _ + 70:                                                      \
    operand = opcode == _ + 70 ? read8_pc() : tmp8;

uint8_t opcode, opcrel, tmp8, operand, carry, neg, *rom0, *rom1, io[512], video_ram[8192],
    work_ram[16384], *extram, *extrambank,
    reg8[] = {19, 0, 216, 0, 77, 1, 176, 1, 254, 255}, &F = reg8[6],
    &A = reg8[7], *reg8_group[] = {reg8 + 1, reg8,     reg8 + 3, reg8 + 2,
                                   reg8 + 5, reg8 + 4, &F,       &A},
    &IF = io[271], &LCDC = io[320], &LY = io[324], IME, halt, *ptr8;

uint8_t const *key_state;

uint16_t PC = 256, temp16, *reg16 = (uint16_t *)reg8, &HL = reg16[2],
         &SP = reg16[4], &DIV = (uint16_t &)io[259], ppu_dot = 32, *ptr16,
         *reg16_group1[] = {reg16, reg16 + 1, &HL, &SP},
         *reg16_group2[] = {reg16, reg16 + 1, &HL, &HL}, prev_cycles, cycles;

int tmp, tmp2,
    HL_add[] = {0, 0, 1, -1}, F_mask[] = {128, 128, 16, 16},
    F_equals[] = {0, 128, 0, 16}, frame_buffer[23040],
    palette[] = {-1,        -23197,    -65536, -16777216, -1,     -8092417,
                 -12961132, -16777216, -1,     -23197,    -65536, -16777216};

void tick() { cycles += 4; }

uint8_t mem_access(uint16_t addr, uint8_t val, int write) {
  tick();
  switch (addr >> 13) {
    case 1:
      if (write)
        rom1 = rom0 + ((val ? val & 63 : 1) << 14);

    case 0:
      return rom0[addr];

    case 2:
      if (write && val <= 3)
        extrambank = extram + (val << 13);

    case 3:
      return rom1[addr & 16383];

    case 4:
      addr &= 8191;
      if (write)
        video_ram[addr] = val;
      return video_ram[addr];

    case 5:
      addr &= 8191;
      if (write)
        extrambank[addr] = val;
      return extrambank[addr];

    case 7:
      if (addr >= 65024) {
        if (write) {
          if (addr == 65350)
            for (int y = 160; --y >= 0;)
              io[y] = mem_access(val << 8 | y, 0, 0);
          io[addr & 511] = val;
        }

        if (addr == 65280) {
          if (!(io[256] & 16))
            return ~(16 + key_state[SDL_SCANCODE_DOWN] * 8 +
                     key_state[SDL_SCANCODE_UP] * 4 +
                     key_state[SDL_SCANCODE_LEFT] * 2 +
                     key_state[SDL_SCANCODE_RIGHT]);
          if (!(io[256] & 32))
            return ~(32 + key_state[SDL_SCANCODE_RETURN] * 8 +
                     key_state[SDL_SCANCODE_TAB] * 4 +
                     key_state[SDL_SCANCODE_Z] * 2 +
                     key_state[SDL_SCANCODE_X]);
          return 255;
        }
        return io[addr & 511];
      }

    case 6:
      addr &= 16383;
      if (write)
        work_ram[addr] = val;
      return work_ram[addr];
  }
}

uint8_t read8(uint16_t addr = HL) { return mem_access(addr, 0, 0); }

void write8(uint8_t val, uint16_t addr = HL) { mem_access(addr, val, 1); }

void set_flags(uint8_t mask, int Z, int N, int H, int C) {
  F = F & mask | !Z << 7 | N << 6 | H << 5 | C << 4;
}

uint8_t read8_pc() { return read8(PC++); }

uint16_t read16(uint16_t &addr = PC) {
  tmp8 = read8(addr++);
  return read8(addr++) << 8 | tmp8;
}

void push(uint16_t val) {
  write8(val >> 8, --SP);
  write8(val, --SP);
  tick();
}

uint8_t reg8_access(uint8_t val, int write = 1, uint8_t o = opcrel) {
  return (o &= 7) == 6 ? mem_access(HL, val, write)
         : write       ? *reg8_group[o] = val
                       : *reg8_group[o];
}

int main() {
  rom1 = (rom0 = (uint8_t *)mmap(0, 1048576, PROT_READ, MAP_SHARED,
                                 open("rom.gb", O_RDONLY), 0)) +
         32768;
  tmp = open("rom.sav", O_CREAT|O_RDWR, 0666);
  ftruncate(tmp, 32768);
  extrambank = extram =
      (uint8_t *)mmap(0, 32768, PROT_READ | PROT_WRITE, MAP_SHARED, tmp, 0);
  LCDC = 145;
  DIV = 44032;
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Renderer *renderer = SDL_CreateRenderer(
      SDL_CreateWindow("pokegb", 0, 0, 800, 720, SDL_WINDOW_SHOWN), -1,
      SDL_RENDERER_PRESENTVSYNC);
  SDL_Texture *texture = SDL_CreateTexture(
      renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, 160, 144);
  key_state = SDL_GetKeyboardState(0);

  while (1) {
    prev_cycles = cycles;
    if (IME & IF & io[511]) {
      IF = halt = IME = 0;
      cycles += 8;
      carry = 1;
      temp16 = 64;
      goto CALL;
    } else if (halt)
      tick();
    else
      switch (opcode = read8_pc()) {
      case 0: // NOP
        break;

      OP4_NX16_REL(1) // LD r16, u16
        *reg16_group1[opcrel] = read16();
        break;

      OP4_NX16_REL(2) // LD (r16), A
        write8(A, *reg16_group2[opcrel]);
        HL += HL_add[opcrel];
        break;

      OP4_NX16_REL(3) // INC r16
        (*reg16_group1[opcrel])++;
        tick();
        break;

      OP8_NX8_REL(4) // INC r8 / INC (HL)
        reg8_access(++tmp8);
        set_flags(16, tmp8, 0, !(tmp8 & 15), 0);
        break;

      OP8_NX8_REL(5) // DEC r8 / DEC (HL)
        reg8_access(--tmp8);
        set_flags(16, tmp8, 1, tmp8 % 16 == 15, 0);
        break;

      OP8_NX8_REL(6) // LD r8, u8 / LD (HL), u8
        reg8_access(read8_pc());
        break;

      case 7: // RLCA
        A += A + (carry = A >> 7);
        goto CARRY_FLAG;

      OP4_NX16_REL(9) // ADD HL, r16
        ptr16 = reg16_group1[opcrel];
        set_flags(128, 1, 0, HL % 4096 + *ptr16 % 4096 > 4095, HL + *ptr16 > 65535);
        HL += *ptr16;
        tick();
        break;

      OP4_NX16_REL(10) // LD A, (r16)
        A = read8(*reg16_group2[opcrel]);
        HL += HL_add[opcrel];
        break;

      OP4_NX16_REL(11) // DEC r16
        (*reg16_group1[opcrel])--;
        tick();
        break;

      case 15: // RRCA
        A = (carry = A & 1) * 128 + A / 2;
        goto CARRY_FLAG;

      case 23: // RLA
        carry = A >> 7;
        A += A + F / 16 % 2;
      CARRY_FLAG:
        set_flags(0, 1, 0, 0, carry);
        break;

      OP5_FLAG(32, 24) // JR i8 / JR <condition>, i8
        tmp8 = read8_pc();
        if (carry)
          PC += (int8_t)tmp8, tick();
        break;

      case 39: // DAA
        carry = tmp8 = 0;
        if (F & 32 || !(F & 64) && A % 15 > 9)
          tmp8 = 6;
        if (F & 16 || !(F & 64) && A > 153)
          tmp8 |= 96, carry = 1;
        set_flags(65, A += F & 64 ? -tmp8 : tmp8, 0, 0, carry);
        break;

      case 47: // CPL
        A = ~A;
        set_flags(129, 1, 1, 1, 0);
        break;

      case 55: case 63: // SCF / CCF
        set_flags(128, 1, 0, 0, opcode == 55 ? 1 : !(F & 16));
        break;

      OP64_REL(64) // LD r8, r8 / LD r8, (HL) / LD (HL), r8 / HALT
        opcode == 118 ? halt = 1 : reg8_access(tmp8, 1);
        break;

      OP9_IMM_PTR(128) // ADD A, r8 / ADD A, (HL) / ADD A, u8
        neg = carry = 0;
        goto ALU;

      OP9_IMM_PTR(136) // ADC A, r8 / ADC A, (HL) / ADC A, u8
        neg = 0;
        carry = F / 16 % 2;
        goto ALU;

      OP9_IMM_PTR(144) // SUB A, r8 / SUB A, (HL) / SUB A, u8
        carry = 1;
        goto SUBTRACT;

      OP9_IMM_PTR(152) // SBC A, r8 / SBC A, (HL) / SBC A, u8
        carry = !(F / 16 % 2);
      SUBTRACT:
        neg = 1;
        operand = ~operand;
      ALU:
        set_flags(0, tmp8 = A + operand + carry, neg,
                  (A % 16 + operand % 16 + carry > 15) ^ neg,
                  (A + operand + carry > 255) ^ neg);
        A = tmp8;
        break;

      OP9_IMM_PTR(160) // AND A, r8 / AND A, (HL) / AND A, u8
        set_flags(0, A &= operand, 0, 1, 0);
        break;

      OP9_IMM_PTR(168) // XOR A, r8 / XOR A, (HL) / XOR A, u8
        set_flags(0, A ^= operand, 0, 0, 0);
        break;

      OP9_IMM_PTR(176) // OR A, r8 / OR A, (HL) / OR A, u8
        set_flags(0, A |= operand, 0, 0, 0);
        break;

      OP9_IMM_PTR(184) // CP A, r8 / CP A, (HL) / CP A, u8
        set_flags(0, A != operand, 1, A % 16 < operand % 16, A < operand);
        break;

      case 217: // RETI
        carry = IME = 1;
        goto RET;

      OP5_FLAG(192, 201) // RET / RET <condition>
      RET:
        tick();
        if (carry)
          PC = read16(SP);
        break;

      OP4_NX16_REL(193) // POP r16
        reg16[opcrel] = read16(SP);
        break;

      OP5_FLAG(194, 195) // JP u16 / JP <condition>, u16
        temp16 = read16();
        if (carry)
          PC = temp16, tick();
        break;

      OP5_FLAG(196, 205) // CALL u16 / CALL <condition>, u16
        temp16 = read16();
      CALL:
        if (carry)
          push(PC), PC = temp16;
        break;

      OP4_NX16_REL(197) // PUSH r16
        push(reg16[opcrel]);
        break;

      case 203:
        switch (opcode = read8_pc()) {
          OP8_REL(0) // RLC r8 / RLC (HL)
            reg8_access(tmp8 += tmp8 + (carry = tmp8 / 128 % 2));
            goto CARRY_ZERO_FLAGS_U;

          OP8_REL(8) // RRC r8 / RRC (HL)
            reg8_access(tmp8 = (carry = tmp8 & 1) * 128 + tmp8 / 2);
            goto CARRY_ZERO_FLAGS_U;

          OP8_REL(16) // RL r8 / RL (HL)
            carry = tmp8 >> 7;
            reg8_access(tmp8 += tmp8 + F / 16 % 2);
            goto CARRY_ZERO_FLAGS_U;

          OP8_REL(24) // RR r8 / RR (HL)
            carry = tmp8 & 1;
            reg8_access(tmp8 = tmp8 / 2 + (F * 8 & 128));
            goto CARRY_ZERO_FLAGS_U;

          OP8_REL(32) // SLA r8 / SLA (HL)
            carry = tmp8 / 128 % 2;
            reg8_access(tmp8 *= 2);
            goto CARRY_ZERO_FLAGS_U;

          OP8_REL(40) // SRA r8 / SRA (HL)
            carry = tmp8 & 1;
            reg8_access(tmp8 = (int8_t)tmp8 >> 1);
            goto CARRY_ZERO_FLAGS_U;

          OP8_REL(48) // SWAP r8 / SWAP (HL)
            carry = 0;
            reg8_access(tmp8 = tmp8 * 16 + tmp8 / 16);
          CARRY_ZERO_FLAGS_U:
            set_flags(0, tmp8, 0, 0, carry);
            break;

          OP8_REL(56) // SRL r8 / SRL (HL)
            set_flags(0, reg8_access(tmp8 / 2), 0, 0, tmp8 & 1);
            break;

          OP64_REL(64) // BIT bit, r8 / BIT bit, (HL)
            set_flags(16, tmp8 & 1 << opcrel, 0, 1, 0);
            break;

          OP64_REL(128) // RES bit, r8 / RES bit, (HL)
            reg8_access(tmp8 & ~(1 << opcrel),1,opcode);
            break;

          OP64_REL(192) // SET bit, r8 / SET bit, (HL)
            reg8_access(tmp8 | 1 << opcrel,1,opcode);
            break;
        }
        break;

      case 224: case 226: // LD (FF00 + u8), A / LD (FF00 + C), A
        write8(A, 65280 + (opcode == 224 ? read8_pc() : *reg8));
        break;

      case 233: // JP HL
        PC = HL;
        break;

      case 234: // LD (u16), A
        write8(A, read16());
        break;

      case 240: case 250: // LD A, (FF00 + u8) / LD A, (u16)
        A = read8(opcode == 240 ? 65280 | read8_pc() : read16());
        break;

      case 243: case 251: // DI / EI
        IME = opcode != 243;
        break;

      case 248: // LD HL, SP + i8
        tmp8 = read8_pc();
        set_flags(0, 1, 0, (uint8_t)SP + tmp8 > 255, SP % 16 + tmp8 % 16 > 15);
        HL = SP + (int8_t)tmp8;
        tick();
        break;

      case 249: // LD SP, HL
        SP = HL;
        tick();
        break;
      }

    for (DIV += cycles - prev_cycles; prev_cycles++ != cycles;)
      if (LCDC & 128) {
        if (++ppu_dot == 1 && LY == 144)
          IF |= 1;

        if (ppu_dot == 456) {
          if (LY < 144)
            for (tmp = 160; --tmp >= 0;) {
              uint8_t is_window =
                          LCDC & 32 && LY >= io[330] && tmp >= io[331] - 7,
                      x_offset = is_window ? tmp - io[331] + 7 : tmp + io[323],
                      y_offset = is_window ? LY - io[330] : LY + io[322];
              uint16_t tile = video_ram[((LCDC & (is_window ? 64 : 8) ? 7 : 6)
                                         << 10) +
                                        y_offset / 8 * 32 + x_offset / 8],
                       palette_index = 0;

              x_offset = (x_offset ^ 7) & 7;

              uint8_t
                  *tile_data =
                      &video_ram[(LCDC & 16 ? tile : 256 + (int8_t)tile) * 16 +
                                 y_offset % 8 * 2],
                  color = (tile_data[1] >> x_offset) % 2 * 2 +
                          (*tile_data >> x_offset) % 2;

              if (LCDC & 2)
                for (uint8_t *sprite = io; sprite < io + 160; sprite += 4) {
                  uint8_t sprite_x = tmp - sprite[1] + 8,
                          sprite_y = LY - *sprite + 16;
                  if (sprite_x < 8 && sprite_y < 8) {
                    sprite_x ^= sprite[3] & 32 ? 0 : 7;

                    tile_data =
                        &video_ram[sprite[2] * 16 +
                                   (sprite_y ^ (sprite[3] & 64 ? 7 : 0)) * 2];
                    uint8_t sprite_color = (tile_data[1] >> sprite_x) % 2 * 2 +
                                           (*tile_data >> sprite_x) % 2;

                    if (!((sprite[3] & 128) && color) && sprite_color) {
                      color = sprite_color;
                      palette_index += 1 + !!(sprite[3] & 8);
                      break;
                    }
                  }
                }

              frame_buffer[LY * 160 + tmp] =
                  palette[(io[327 + palette_index] >> (2 * color)) % 4 +
                          palette_index * 4];
            }

          if (LY == 144) {
            void *pixels;
            SDL_LockTexture(texture, 0, &pixels, &tmp);
            for (tmp2 = 144; --tmp2 >= 0;)
              memcpy((uint8_t *)pixels + tmp2 * tmp, frame_buffer + tmp2 * 160,
                     640);
            SDL_UnlockTexture(texture);
            SDL_RenderCopy(renderer, texture, 0, 0);
            SDL_RenderPresent(renderer);
            SDL_Event event;
            while (SDL_PollEvent(&event))
              if (event.type == SDL_QUIT)
                return 0;
          }

          LY = (LY + 1) % 154;
          ppu_dot = 0;
        }
      } else
        LY = ppu_dot = 0;
  }
}
