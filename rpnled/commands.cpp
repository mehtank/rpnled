#include "commands.h"

#define POP(n) if (stack_ptr < n) break
#define PUSH(n) if (stack_ptr > STACK_SIZE - n) break

// Create a stack for program execution
int16_t stack[STACK_SIZE];
uint8_t stack_ptr = 0;

CRGB runCmd(int16_t *cmd, uint8_t cmdlen, uint32_t time, uint16_t index, uint8_t rnd) {
  static int16_t reg[5] = {0,0,0,0,0};
  stack_ptr = 0;

  int16_t loopstart = -1;
  uint8_t loopindex = 0;

  for (uint8_t i = 0; i < cmdlen; i++) {
    switch (cmd[i]){ // START SWITCH
      case C_RGB: POP(3); return CRGB(stack[stack_ptr-3], stack[stack_ptr-2], stack[stack_ptr-1]); break;
      case C_HSV: POP(3); return CHSV(stack[stack_ptr-3], stack[stack_ptr-2], stack[stack_ptr-1]); break;
      case C_HUE: POP(1); return CHSV(stack[stack_ptr-1], 255, 255); break;
      case C_VALUE: POP(1); return CHSV(0, 0, stack[stack_ptr-1]); break;

      case C_TIME: PUSH(1); stack[stack_ptr++] = time; break;
      case C_TIMESHIFT: POP(1); stack[stack_ptr-1] = (time >> stack[stack_ptr-1]); break;
      case C_INDEX: PUSH(1); stack[stack_ptr++] = index; break;
      case C_RANDOM8: PUSH(1); stack[stack_ptr++] = random(256); break;
      case C_RANDC: PUSH(1); stack[stack_ptr++] = rnd; break;

      case C_RSHIFT: POP(2); stack[stack_ptr-2] = stack[stack_ptr-2] >> stack[stack_ptr-1]; stack_ptr--; break;
      case C_LSHIFT: POP(2); stack[stack_ptr-2] = stack[stack_ptr-2] << stack[stack_ptr-1]; stack_ptr--; break;
      case C_BITAND: POP(2); stack[stack_ptr-2] = stack[stack_ptr-2] & stack[stack_ptr-1]; stack_ptr--; break;
      case C_BITOR: POP(2); stack[stack_ptr-2] = stack[stack_ptr-2] | stack[stack_ptr-1]; stack_ptr--; break;
      case C_BITXOR: POP(2); stack[stack_ptr-2] = stack[stack_ptr-2] ^ stack[stack_ptr-1]; stack_ptr--; break;
      case C_BITNOT: POP(1); stack[stack_ptr-1] = ~(stack[stack_ptr-1]); break;
      case C_AND: POP(2); stack[stack_ptr-2] = stack[stack_ptr-2] && stack[stack_ptr-1]; stack_ptr--; break;
      case C_OR: POP(2); stack[stack_ptr-2] = stack[stack_ptr-2] || stack[stack_ptr-1]; stack_ptr--; break;
      case C_NOT: POP(1); stack[stack_ptr-1] = !(stack[stack_ptr-1]); break;

      case C_PLUS: POP(2); stack[stack_ptr-2] = stack[stack_ptr-2] + stack[stack_ptr-1]; stack_ptr--; break;
      case C_MINUS: POP(2); stack[stack_ptr-2] = stack[stack_ptr-2] - stack[stack_ptr-1]; stack_ptr--; break;
      case C_TIMES: POP(2); stack[stack_ptr-2] = stack[stack_ptr-2] * stack[stack_ptr-1]; stack_ptr--; break;
      case C_DIVIDE: POP(2); stack[stack_ptr-2] = stack[stack_ptr-2] / stack[stack_ptr-1]; stack_ptr--; break;
      case C_MOD: POP(2); stack[stack_ptr-2] = stack[stack_ptr-2] % stack[stack_ptr-1]; stack_ptr--; break;
      case C_INC: POP(1); stack[stack_ptr-1]++; break;
      case C_DEC: POP(1); stack[stack_ptr-1]--; break;
      case C_DIFF: POP(2); stack[stack_ptr-2] = abs(stack[stack_ptr-2] - stack[stack_ptr-1]); stack_ptr--; break;

      case C_EQ: POP(2); stack[stack_ptr-2] = stack[stack_ptr-2] == stack[stack_ptr-1]; stack_ptr--; break;
      case C_NE: POP(2); stack[stack_ptr-2] = stack[stack_ptr-2] != stack[stack_ptr-1]; stack_ptr--; break;
      case C_GT: POP(2); stack[stack_ptr-2] = stack[stack_ptr-2] > stack[stack_ptr-1]; stack_ptr--; break;
      case C_GE: POP(2); stack[stack_ptr-2] = stack[stack_ptr-2] >= stack[stack_ptr-1]; stack_ptr--; break;
      case C_LT: POP(2); stack[stack_ptr-2] = stack[stack_ptr-2] < stack[stack_ptr-1]; stack_ptr--; break;
      case C_LE: POP(2); stack[stack_ptr-2] = stack[stack_ptr-2] <= stack[stack_ptr-1]; stack_ptr--; break;

      case C_IFTE: POP(3); 
        stack[stack_ptr-3] = stack[stack_ptr-3] ? stack[stack_ptr-2] : stack[stack_ptr-1]; 
        stack_ptr -= 2; 
        break;

      case C_ABS: POP(1); stack[stack_ptr-1] = abs(stack[stack_ptr-1]); break;
      case C_MIN: POP(2); stack[stack_ptr-2] = MIN(stack[stack_ptr-2], stack[stack_ptr-1]); stack_ptr--; break;
      case C_MAX: POP(2); stack[stack_ptr-2] = MAX(stack[stack_ptr-2], stack[stack_ptr-1]); stack_ptr--; break;

      case C_REGA: PUSH(1); stack[stack_ptr++] = reg[0]; break;
      case C_REGB: PUSH(1); stack[stack_ptr++] = reg[1]; break;
      case C_REGC: PUSH(1); stack[stack_ptr++] = reg[2]; break;
      case C_REGD: PUSH(1); stack[stack_ptr++] = reg[3]; break;
      case C_REGZ: PUSH(1); stack[stack_ptr++] = reg[REG_IDX]; break;
      case C_REGN: POP(1);
        if (stack[stack_ptr-1] >= 0 && stack[stack_ptr-1] <= REG_IDX)
          stack[stack_ptr-1] = reg[stack[stack_ptr-1]];
        else
          stack[stack_ptr-1] = 0;
        break;
      case C_REGCYC: PUSH(1); stack[stack_ptr++] = reg[reg[REG_IDX]]; break;

      case C_STA: POP(1); reg[0] = stack[--stack_ptr]; break;
      case C_STB: POP(1); reg[1] = stack[--stack_ptr]; break;
      case C_STC: POP(1); reg[2] = stack[--stack_ptr]; break;
      case C_STD: POP(1); reg[3] = stack[--stack_ptr]; break;
      case C_STZ: POP(1); reg[REG_IDX] = stack[--stack_ptr]; break;      
      case C_STN: POP(2);
        if (stack[stack_ptr-1] >= 0 && stack[stack_ptr-1] <= REG_IDX)
          reg[stack[stack_ptr-1]] = stack[stack_ptr-2];
        stack_ptr -= 2;
        break;
      case C_STCYC: POP(1);
        reg[reg[REG_IDX]] = stack[--stack_ptr];
        reg[REG_IDX] = (reg[REG_IDX] + 1) & REG_MAX;
        break;

      case C_STAIF: POP(2); if (stack[--stack_ptr]) reg[0] = stack[stack_ptr-1]; stack_ptr--; break;
      case C_STBIF: POP(2); if (stack[--stack_ptr]) reg[1] = stack[stack_ptr-1]; stack_ptr--; break;
      case C_STCIF: POP(2); if (stack[--stack_ptr]) reg[2] = stack[stack_ptr-1]; stack_ptr--; break;
      case C_STDIF: POP(2); if (stack[--stack_ptr]) reg[3] = stack[stack_ptr-1]; stack_ptr--; break;
      case C_STZIF: POP(2); if (stack[--stack_ptr]) reg[REG_IDX] = stack[stack_ptr-1]; stack_ptr--; break;      
      case C_STNIF: POP(3);
        if (stack[--stack_ptr])
          if (stack[stack_ptr-1] >= 0 && stack[stack_ptr-1] <= REG_IDX)
            reg[stack[stack_ptr-1]] = stack[stack_ptr-2];
        stack_ptr -= 2;
        break;
      case C_STCYCIF: POP(2);
        if (stack[--stack_ptr]) {
          reg[reg[REG_IDX]] = stack[stack_ptr-1];
          reg[REG_IDX] = (reg[REG_IDX] + 1) & REG_MAX;
        };
        stack_ptr--;
        break;

      case C_LOOPSTART:
        if (loopstart >= 0)
          break;
        loopstart = i;
        loopindex = 0;
        break;

      case C_LOOPREG:
        if (loopstart < 0)
          break;
        stack[stack_ptr++] = reg[loopindex];
        break;

      case C_LOOPMAX:
        stack[stack_ptr-2] = MAX(stack[stack_ptr-2], stack[stack_ptr-1]); stack_ptr--;
        if (loopindex++ < REG_MAX)
          i = loopstart;
        else
          loopstart = -1;
        break;

      case C_TDIFF: POP(1); stack[stack_ptr-1] = abs(time - stack[stack_ptr-1]); break;
      case C_IDIFF: POP(1); stack[stack_ptr-1] = abs(index - stack[stack_ptr-1]); break;

      case C_PRINT: POP(1); Serial.print(stack[stack_ptr-1]); break;

      default: PUSH(1); stack[stack_ptr++] = cmd[i];
    }
    // END SWITCH
  }
  return CRGB(0,0,0);
}
