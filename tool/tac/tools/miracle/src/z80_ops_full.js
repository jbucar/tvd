function sign_extend(v) {
  return v < 128 ? v : v-256;
}
function z80_defaults(ops) {
    for (var i = 0; i < 256; ++i) {
        if (!ops[i]) ops[i] = ops[256];
    }
}
var z80_base_ops = function z80_setup_base_ops() {
    var ops = [];
    ops[0x00] = function op_0x00(tempaddr) {
    };
    ops[0x01] = function op_0x01(tempaddr) {
      tstates+=6;
      z80.c=readbyte(z80.pc++);
      z80.pc &= 0xffff;
      z80.b=readbyte(z80.pc++);
      z80.pc &= 0xffff;
    };
    ops[0x02] = function op_0x02(tempaddr) {
      tstates+=3;
      writebyte((z80.c | (z80.b << 8)),z80.a);
    };
    ops[0x03] = function op_0x03(tempaddr) {
      tstates += 2;
      var wordtemp = ((z80.c | (z80.b << 8)) + 1) & 0xffff;
      z80.b = wordtemp >> 8;
      z80.c = wordtemp & 0xff;
    };
    ops[0x04] = function op_0x04(tempaddr) {
      { (z80.b) = ((z80.b) + 1) & 0xff; z80.f = ( z80.f & 0x01 ) | ( (z80.b)==0x80 ? 0x04 : 0 ) | ( (z80.b)&0x0f ? 0 : 0x10 ) | sz53_table[(z80.b)];};
    };
    ops[0x05] = function op_0x05(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | ( (z80.b)&0x0f ? 0 : 0x10 ) | 0x02; (z80.b) = ((z80.b) - 1) & 0xff; z80.f |= ( (z80.b)==0x7f ? 0x04 : 0 ) | sz53_table[z80.b];};
    };
    ops[0x06] = function op_0x06(tempaddr) {
      tstates+=3;
      z80.b=readbyte(z80.pc++); z80.pc &= 0xffff;
    };
    ops[0x07] = function op_0x07(tempaddr) {
      z80.a = ( (z80.a & 0x7f) << 1 ) | ( z80.a >> 7 );
      z80.f = ( z80.f & ( 0x04 | 0x40 | 0x80 ) ) |
    ( z80.a & ( 0x01 | 0x08 | 0x20 ) );
    };
    ops[0x08] = function op_0x08(tempaddr) {
      {
          var olda = z80.a; var oldf = z80.f;
          z80.a = z80.a_; z80.f = z80.f_;
          z80.a_ = olda; z80.f_ = oldf;
      }
    };
    ops[0x09] = function op_0x09(tempaddr) {
      { var add16temp = ((z80.l | (z80.h << 8))) + ((z80.c | (z80.b << 8))); var lookup = ( ( ((z80.l | (z80.h << 8))) & 0x0800 ) >> 11 ) | ( ( ((z80.c | (z80.b << 8))) & 0x0800 ) >> 10 ) | ( ( add16temp & 0x0800 ) >> 9 ); tstates += 7; (z80.h) = (add16temp >> 8) & 0xff; (z80.l) = add16temp & 0xff; z80.f = ( z80.f & ( 0x04 | 0x40 | 0x80 ) ) | ( add16temp & 0x10000 ? 0x01 : 0 )| ( ( add16temp >> 8 ) & ( 0x08 | 0x20 ) ) | halfcarry_add_table[lookup];};
    };
    ops[0x0a] = function op_0x0a(tempaddr) {
      tstates+=3;
      z80.a=readbyte((z80.c | (z80.b << 8)));
    };
    ops[0x0b] = function op_0x0b(tempaddr) {
      tstates += 2;
      var wordtemp = ((z80.c | (z80.b << 8)) - 1) & 0xffff;
      z80.b = wordtemp >> 8;
      z80.c = wordtemp & 0xff;
    };
    ops[0x0c] = function op_0x0c(tempaddr) {
      { (z80.c) = ((z80.c) + 1) & 0xff; z80.f = ( z80.f & 0x01 ) | ( (z80.c)==0x80 ? 0x04 : 0 ) | ( (z80.c)&0x0f ? 0 : 0x10 ) | sz53_table[(z80.c)];};
    };
    ops[0x0d] = function op_0x0d(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | ( (z80.c)&0x0f ? 0 : 0x10 ) | 0x02; (z80.c) = ((z80.c) - 1) & 0xff; z80.f |= ( (z80.c)==0x7f ? 0x04 : 0 ) | sz53_table[z80.c];};
    };
    ops[0x0e] = function op_0x0e(tempaddr) {
      tstates+=3;
      z80.c=readbyte(z80.pc++); z80.pc &= 0xffff;
    };
    ops[0x0f] = function op_0x0f(tempaddr) {
      z80.f = ( z80.f & ( 0x04 | 0x40 | 0x80 ) ) | ( z80.a & 0x01 );
      z80.a = ( z80.a >> 1) | ( (z80.a & 0x01) << 7 );
      z80.f |= ( z80.a & ( 0x08 | 0x20 ) );
    };
    ops[0x10] = function op_0x10(tempaddr) {
      tstates+=4;
      z80.b = (z80.b-1) & 0xff;
      if(z80.b) { { tstates+=5; z80.pc += sign_extend(readbyte( z80.pc )); z80.pc &= 0xffff; }; }
      z80.pc++;
      z80.pc &= 0xffff;
    };
    ops[0x11] = function op_0x11(tempaddr) {
      tstates+=6;
      z80.e=readbyte(z80.pc++);
      z80.pc &= 0xffff;
      z80.d=readbyte(z80.pc++);
      z80.pc &= 0xffff;
    };
    ops[0x12] = function op_0x12(tempaddr) {
      tstates+=3;
      writebyte((z80.e | (z80.d << 8)),z80.a);
    };
    ops[0x13] = function op_0x13(tempaddr) {
      tstates += 2;
      var wordtemp = ((z80.e | (z80.d << 8)) + 1) & 0xffff;
      z80.d = wordtemp >> 8;
      z80.e = wordtemp & 0xff;
    };
    ops[0x14] = function op_0x14(tempaddr) {
      { (z80.d) = ((z80.d) + 1) & 0xff; z80.f = ( z80.f & 0x01 ) | ( (z80.d)==0x80 ? 0x04 : 0 ) | ( (z80.d)&0x0f ? 0 : 0x10 ) | sz53_table[(z80.d)];};
    };
    ops[0x15] = function op_0x15(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | ( (z80.d)&0x0f ? 0 : 0x10 ) | 0x02; (z80.d) = ((z80.d) - 1) & 0xff; z80.f |= ( (z80.d)==0x7f ? 0x04 : 0 ) | sz53_table[z80.d];};
    };
    ops[0x16] = function op_0x16(tempaddr) {
      tstates+=3;
      z80.d=readbyte(z80.pc++); z80.pc &= 0xffff;
    };
    ops[0x17] = function op_0x17(tempaddr) {
      {
    var bytetemp = z80.a;
    z80.a = ( (z80.a & 0x7f) << 1 ) | ( z80.f & 0x01 );
    z80.f = ( z80.f & ( 0x04 | 0x40 | 0x80 ) ) |
      ( z80.a & ( 0x08 | 0x20 ) ) | ( bytetemp >> 7 );
      }
    };
    ops[0x18] = function op_0x18(tempaddr) {
      tstates+=3;
      { tstates+=5; z80.pc += sign_extend(readbyte( z80.pc )); z80.pc &= 0xffff; };
      z80.pc++; z80.pc &= 0xffff;
    };
    ops[0x19] = function op_0x19(tempaddr) {
      { var add16temp = ((z80.l | (z80.h << 8))) + ((z80.e | (z80.d << 8))); var lookup = ( ( ((z80.l | (z80.h << 8))) & 0x0800 ) >> 11 ) | ( ( ((z80.e | (z80.d << 8))) & 0x0800 ) >> 10 ) | ( ( add16temp & 0x0800 ) >> 9 ); tstates += 7; (z80.h) = (add16temp >> 8) & 0xff; (z80.l) = add16temp & 0xff; z80.f = ( z80.f & ( 0x04 | 0x40 | 0x80 ) ) | ( add16temp & 0x10000 ? 0x01 : 0 )| ( ( add16temp >> 8 ) & ( 0x08 | 0x20 ) ) | halfcarry_add_table[lookup];};
    };
    ops[0x1a] = function op_0x1a(tempaddr) {
      tstates+=3;
      z80.a=readbyte((z80.e | (z80.d << 8)));
    };
    ops[0x1b] = function op_0x1b(tempaddr) {
      tstates += 2;
      var wordtemp = ((z80.e | (z80.d << 8)) - 1) & 0xffff;
      z80.d = wordtemp >> 8;
      z80.e = wordtemp & 0xff;
    };
    ops[0x1c] = function op_0x1c(tempaddr) {
      { (z80.e) = ((z80.e) + 1) & 0xff; z80.f = ( z80.f & 0x01 ) | ( (z80.e)==0x80 ? 0x04 : 0 ) | ( (z80.e)&0x0f ? 0 : 0x10 ) | sz53_table[(z80.e)];};
    };
    ops[0x1d] = function op_0x1d(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | ( (z80.e)&0x0f ? 0 : 0x10 ) | 0x02; (z80.e) = ((z80.e) - 1) & 0xff; z80.f |= ( (z80.e)==0x7f ? 0x04 : 0 ) | sz53_table[z80.e];};
    };
    ops[0x1e] = function op_0x1e(tempaddr) {
      tstates+=3;
      z80.e=readbyte(z80.pc++); z80.pc &= 0xffff;
    };
    ops[0x1f] = function op_0x1f(tempaddr) {
      {
    var bytetemp = z80.a;
    z80.a = ( z80.a >> 1 ) | ( (z80.f & 0x01) << 7 );
    z80.f = ( z80.f & ( 0x04 | 0x40 | 0x80 ) ) |
      ( z80.a & ( 0x08 | 0x20 ) ) | ( bytetemp & 0x01 ) ;
      }
    };
    ops[0x20] = function op_0x20(tempaddr) {
      tstates+=3;
      if( ! ( z80.f & 0x40 ) ) { { tstates+=5; z80.pc += sign_extend(readbyte( z80.pc )); z80.pc &= 0xffff; }; }
      z80.pc++; z80.pc &= 0xffff;
    };
    ops[0x21] = function op_0x21(tempaddr) {
      tstates+=6;
      z80.l=readbyte(z80.pc++);
      z80.pc &= 0xffff;
      z80.h=readbyte(z80.pc++);
      z80.pc &= 0xffff;
    };
    ops[0x22] = function op_0x22(tempaddr) {
      { var ldtemp; tstates+=12; ldtemp=readbyte(z80.pc++); z80.pc &= 0xffff; ldtemp|=readbyte(z80.pc++) << 8; z80.pc &= 0xffff; writebyte(ldtemp++,(z80.l)); ldtemp &= 0xffff; writebyte(ldtemp,(z80.h));};
    };
    ops[0x23] = function op_0x23(tempaddr) {
      tstates += 2;
      var wordtemp = ((z80.l | (z80.h << 8)) + 1) & 0xffff;
      z80.h = wordtemp >> 8;
      z80.l = wordtemp & 0xff;
    };
    ops[0x24] = function op_0x24(tempaddr) {
      { (z80.h) = ((z80.h) + 1) & 0xff; z80.f = ( z80.f & 0x01 ) | ( (z80.h)==0x80 ? 0x04 : 0 ) | ( (z80.h)&0x0f ? 0 : 0x10 ) | sz53_table[(z80.h)];};
    };
    ops[0x25] = function op_0x25(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | ( (z80.h)&0x0f ? 0 : 0x10 ) | 0x02; (z80.h) = ((z80.h) - 1) & 0xff; z80.f |= ( (z80.h)==0x7f ? 0x04 : 0 ) | sz53_table[z80.h];};
    };
    ops[0x26] = function op_0x26(tempaddr) {
      tstates+=3;
      z80.h=readbyte(z80.pc++); z80.pc &= 0xffff;
    };
    ops[0x27] = function op_0x27(tempaddr) {
      {
    var add = 0, carry = ( z80.f & 0x01 );
    if( ( z80.f & 0x10 ) || ( (z80.a & 0x0f)>9 ) ) add=6;
    if( carry || (z80.a > 0x99 ) ) add|=0x60;
    if( z80.a > 0x99 ) carry=0x01;
    if ( z80.f & 0x02 ) {
      { var subtemp = z80.a - (add); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (add) & 0x88 ) >> 2 ) | ( (subtemp & 0x88 ) >> 1 ); z80.a=subtemp & 0xff; z80.f = ( subtemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    } else {
      { var addtemp = z80.a + (add); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (add) & 0x88 ) >> 2 ) | ( ( addtemp & 0x88 ) >> 1 ); z80.a=addtemp & 0xff; z80.f = ( addtemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    }
    z80.f = ( z80.f & ~( 0x01 | 0x04) ) | carry | parity_table[z80.a];
      }
    };
    ops[0x28] = function op_0x28(tempaddr) {
      tstates+=3;
      if( z80.f & 0x40 ) { { tstates+=5; z80.pc += sign_extend(readbyte( z80.pc )); z80.pc &= 0xffff; }; }
      z80.pc++; z80.pc &= 0xffff;
    };
    ops[0x29] = function op_0x29(tempaddr) {
      { var add16temp = ((z80.l | (z80.h << 8))) + ((z80.l | (z80.h << 8))); var lookup = ( ( ((z80.l | (z80.h << 8))) & 0x0800 ) >> 11 ) | ( ( ((z80.l | (z80.h << 8))) & 0x0800 ) >> 10 ) | ( ( add16temp & 0x0800 ) >> 9 ); tstates += 7; (z80.h) = (add16temp >> 8) & 0xff; (z80.l) = add16temp & 0xff; z80.f = ( z80.f & ( 0x04 | 0x40 | 0x80 ) ) | ( add16temp & 0x10000 ? 0x01 : 0 )| ( ( add16temp >> 8 ) & ( 0x08 | 0x20 ) ) | halfcarry_add_table[lookup];};
    };
    ops[0x2a] = function op_0x2a(tempaddr) {
      { var ldtemp; tstates+=12; ldtemp=readbyte(z80.pc++); z80.pc &= 0xffff; ldtemp|=readbyte(z80.pc++) << 8; z80.pc &= 0xffff; (z80.l)=readbyte(ldtemp++); ldtemp &= 0xffff; (z80.h)=readbyte(ldtemp);};
    };
    ops[0x2b] = function op_0x2b(tempaddr) {
      tstates += 2;
      var wordtemp = ((z80.l | (z80.h << 8)) - 1) & 0xffff;
      z80.h = wordtemp >> 8;
      z80.l = wordtemp & 0xff;
    };
    ops[0x2c] = function op_0x2c(tempaddr) {
      { (z80.l) = ((z80.l) + 1) & 0xff; z80.f = ( z80.f & 0x01 ) | ( (z80.l)==0x80 ? 0x04 : 0 ) | ( (z80.l)&0x0f ? 0 : 0x10 ) | sz53_table[(z80.l)];};
    };
    ops[0x2d] = function op_0x2d(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | ( (z80.l)&0x0f ? 0 : 0x10 ) | 0x02; (z80.l) = ((z80.l) - 1) & 0xff; z80.f |= ( (z80.l)==0x7f ? 0x04 : 0 ) | sz53_table[z80.l];};
    };
    ops[0x2e] = function op_0x2e(tempaddr) {
      tstates+=3;
      z80.l=readbyte(z80.pc++); z80.pc &= 0xffff;
    };
    ops[0x2f] = function op_0x2f(tempaddr) {
      z80.a ^= 0xff;
      z80.f = ( z80.f & ( 0x01 | 0x04 | 0x40 | 0x80 ) ) |
    ( z80.a & ( 0x08 | 0x20 ) ) | ( 0x02 | 0x10 );
    };
    ops[0x30] = function op_0x30(tempaddr) {
      tstates+=3;
      if( ! ( z80.f & 0x01 ) ) { { tstates+=5; z80.pc += sign_extend(readbyte( z80.pc )); z80.pc &= 0xffff; }; }
      z80.pc++; z80.pc &= 0xffff;
    };
    ops[0x31] = function op_0x31(tempaddr) {
      tstates+=6;
      var splow = readbyte(z80.pc++);
      z80.pc &= 0xffff;
      var sphigh=readbyte(z80.pc++);
      z80.sp = splow | (sphigh << 8);
      z80.pc &= 0xffff;
    };
    ops[0x32] = function op_0x32(tempaddr) {
      tstates+=3;
      {
    var wordtemp = readbyte( z80.pc++ );
    z80.pc &= 0xffff;
    tstates+=6;
    wordtemp|=readbyte(z80.pc++) << 8;
    z80.pc &= 0xffff;
    writebyte(wordtemp,z80.a);
      }
    };
    ops[0x33] = function op_0x33(tempaddr) {
      tstates += 2;
      z80.sp = (z80.sp + 1) & 0xffff;
    };
    ops[0x34] = function op_0x34(tempaddr) {
      tstates+=7;
      {
    var bytetemp = readbyte( (z80.l | (z80.h << 8)) );
    { (bytetemp) = ((bytetemp) + 1) & 0xff; z80.f = ( z80.f & 0x01 ) | ( (bytetemp)==0x80 ? 0x04 : 0 ) | ( (bytetemp)&0x0f ? 0 : 0x10 ) | sz53_table[(bytetemp)];};
    writebyte((z80.l | (z80.h << 8)),bytetemp);
      }
    };
    ops[0x35] = function op_0x35(tempaddr) {
      tstates+=7;
      {
    var bytetemp = readbyte( (z80.l | (z80.h << 8)) );
    { z80.f = ( z80.f & 0x01 ) | ( (bytetemp)&0x0f ? 0 : 0x10 ) | 0x02; (bytetemp) = ((bytetemp) - 1) & 0xff; z80.f |= ( (bytetemp)==0x7f ? 0x04 : 0 ) | sz53_table[bytetemp];};
    writebyte((z80.l | (z80.h << 8)),bytetemp);
      }
    };
    ops[0x36] = function op_0x36(tempaddr) {
      tstates+=6;
      writebyte((z80.l | (z80.h << 8)),readbyte(z80.pc++));
      z80.pc &= 0xffff;
    };
    ops[0x37] = function op_0x37(tempaddr) {
      z80.f = ( z80.f & ( 0x04 | 0x40 | 0x80 ) ) |
        ( z80.a & ( 0x08 | 0x20 ) ) |
        0x01;
    };
    ops[0x38] = function op_0x38(tempaddr) {
      tstates+=3;
      if( z80.f & 0x01 ) { { tstates+=5; z80.pc += sign_extend(readbyte( z80.pc )); z80.pc &= 0xffff; }; }
      z80.pc++; z80.pc &= 0xffff;
    };
    ops[0x39] = function op_0x39(tempaddr) {
      { var add16temp = ((z80.l | (z80.h << 8))) + (z80.sp); var lookup = ( ( ((z80.l | (z80.h << 8))) & 0x0800 ) >> 11 ) | ( ( (z80.sp) & 0x0800 ) >> 10 ) | ( ( add16temp & 0x0800 ) >> 9 ); tstates += 7; (z80.h) = (add16temp >> 8) & 0xff; (z80.l) = add16temp & 0xff; z80.f = ( z80.f & ( 0x04 | 0x40 | 0x80 ) ) | ( add16temp & 0x10000 ? 0x01 : 0 )| ( ( add16temp >> 8 ) & ( 0x08 | 0x20 ) ) | halfcarry_add_table[lookup];};
    };
    ops[0x3a] = function op_0x3a(tempaddr) {
      {
    var wordtemp;
    tstates+=9;
    wordtemp = readbyte(z80.pc++);
    z80.pc &= 0xffff;
    wordtemp|= ( readbyte(z80.pc++) << 8 );
    z80.pc &= 0xffff;
    z80.a=readbyte(wordtemp);
      }
    };
    ops[0x3b] = function op_0x3b(tempaddr) {
      tstates += 2;
      z80.sp = (z80.sp - 1) & 0xffff;
    };
    ops[0x3c] = function op_0x3c(tempaddr) {
      { (z80.a) = ((z80.a) + 1) & 0xff; z80.f = ( z80.f & 0x01 ) | ( (z80.a)==0x80 ? 0x04 : 0 ) | ( (z80.a)&0x0f ? 0 : 0x10 ) | sz53_table[(z80.a)];};
    };
    ops[0x3d] = function op_0x3d(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | ( (z80.a)&0x0f ? 0 : 0x10 ) | 0x02; (z80.a) = ((z80.a) - 1) & 0xff; z80.f |= ( (z80.a)==0x7f ? 0x04 : 0 ) | sz53_table[z80.a];};
    };
    ops[0x3e] = function op_0x3e(tempaddr) {
      tstates+=3;
      z80.a=readbyte(z80.pc++); z80.pc &= 0xffff;
    };
    ops[0x3f] = function op_0x3f(tempaddr) {
      z80.f = ( z80.f & ( 0x04 | 0x40 | 0x80 ) ) |
    ( ( z80.f & 0x01 ) ? 0x10 : 0x01 ) | ( z80.a & ( 0x08 | 0x20 ) );
    };
    ops[0x40] = function op_0x40(tempaddr) {
    };
    ops[0x41] = function op_0x41(tempaddr) {
      z80.b=z80.c;
    };
    ops[0x42] = function op_0x42(tempaddr) {
      z80.b=z80.d;
    };
    ops[0x43] = function op_0x43(tempaddr) {
      z80.b=z80.e;
    };
    ops[0x44] = function op_0x44(tempaddr) {
      z80.b=z80.h;
    };
    ops[0x45] = function op_0x45(tempaddr) {
      z80.b=z80.l;
    };
    ops[0x46] = function op_0x46(tempaddr) {
      tstates+=3;
      z80.b=readbyte((z80.l | (z80.h << 8)));
    };
    ops[0x47] = function op_0x47(tempaddr) {
      z80.b=z80.a;
    };
    ops[0x48] = function op_0x48(tempaddr) {
      z80.c=z80.b;
    };
    ops[0x49] = function op_0x49(tempaddr) {
    };
    ops[0x4a] = function op_0x4a(tempaddr) {
      z80.c=z80.d;
    };
    ops[0x4b] = function op_0x4b(tempaddr) {
      z80.c=z80.e;
    };
    ops[0x4c] = function op_0x4c(tempaddr) {
      z80.c=z80.h;
    };
    ops[0x4d] = function op_0x4d(tempaddr) {
      z80.c=z80.l;
    };
    ops[0x4e] = function op_0x4e(tempaddr) {
      tstates+=3;
      z80.c=readbyte((z80.l | (z80.h << 8)));
    };
    ops[0x4f] = function op_0x4f(tempaddr) {
      z80.c=z80.a;
    };
    ops[0x50] = function op_0x50(tempaddr) {
      z80.d=z80.b;
    };
    ops[0x51] = function op_0x51(tempaddr) {
      z80.d=z80.c;
    };
    ops[0x52] = function op_0x52(tempaddr) {
    };
    ops[0x53] = function op_0x53(tempaddr) {
      z80.d=z80.e;
    };
    ops[0x54] = function op_0x54(tempaddr) {
      z80.d=z80.h;
    };
    ops[0x55] = function op_0x55(tempaddr) {
      z80.d=z80.l;
    };
    ops[0x56] = function op_0x56(tempaddr) {
      tstates+=3;
      z80.d=readbyte((z80.l | (z80.h << 8)));
    };
    ops[0x57] = function op_0x57(tempaddr) {
      z80.d=z80.a;
    };
    ops[0x58] = function op_0x58(tempaddr) {
      z80.e=z80.b;
    };
    ops[0x59] = function op_0x59(tempaddr) {
      z80.e=z80.c;
    };
    ops[0x5a] = function op_0x5a(tempaddr) {
      z80.e=z80.d;
    };
    ops[0x5b] = function op_0x5b(tempaddr) {
    };
    ops[0x5c] = function op_0x5c(tempaddr) {
      z80.e=z80.h;
    };
    ops[0x5d] = function op_0x5d(tempaddr) {
      z80.e=z80.l;
    };
    ops[0x5e] = function op_0x5e(tempaddr) {
      tstates+=3;
      z80.e=readbyte((z80.l | (z80.h << 8)));
    };
    ops[0x5f] = function op_0x5f(tempaddr) {
      z80.e=z80.a;
    };
    ops[0x60] = function op_0x60(tempaddr) {
      z80.h=z80.b;
    };
    ops[0x61] = function op_0x61(tempaddr) {
      z80.h=z80.c;
    };
    ops[0x62] = function op_0x62(tempaddr) {
      z80.h=z80.d;
    };
    ops[0x63] = function op_0x63(tempaddr) {
      z80.h=z80.e;
    };
    ops[0x64] = function op_0x64(tempaddr) {
    };
    ops[0x65] = function op_0x65(tempaddr) {
      z80.h=z80.l;
    };
    ops[0x66] = function op_0x66(tempaddr) {
      tstates+=3;
      z80.h=readbyte((z80.l | (z80.h << 8)));
    };
    ops[0x67] = function op_0x67(tempaddr) {
      z80.h=z80.a;
    };
    ops[0x68] = function op_0x68(tempaddr) {
      z80.l=z80.b;
    };
    ops[0x69] = function op_0x69(tempaddr) {
      z80.l=z80.c;
    };
    ops[0x6a] = function op_0x6a(tempaddr) {
      z80.l=z80.d;
    };
    ops[0x6b] = function op_0x6b(tempaddr) {
      z80.l=z80.e;
    };
    ops[0x6c] = function op_0x6c(tempaddr) {
      z80.l=z80.h;
    };
    ops[0x6d] = function op_0x6d(tempaddr) {
    };
    ops[0x6e] = function op_0x6e(tempaddr) {
      tstates+=3;
      z80.l=readbyte((z80.l | (z80.h << 8)));
    };
    ops[0x6f] = function op_0x6f(tempaddr) {
      z80.l=z80.a;
    };
    ops[0x70] = function op_0x70(tempaddr) {
      tstates+=3;
      writebyte((z80.l | (z80.h << 8)),z80.b);
    };
    ops[0x71] = function op_0x71(tempaddr) {
      tstates+=3;
      writebyte((z80.l | (z80.h << 8)),z80.c);
    };
    ops[0x72] = function op_0x72(tempaddr) {
      tstates+=3;
      writebyte((z80.l | (z80.h << 8)),z80.d);
    };
    ops[0x73] = function op_0x73(tempaddr) {
      tstates+=3;
      writebyte((z80.l | (z80.h << 8)),z80.e);
    };
    ops[0x74] = function op_0x74(tempaddr) {
      tstates+=3;
      writebyte((z80.l | (z80.h << 8)),z80.h);
    };
    ops[0x75] = function op_0x75(tempaddr) {
      tstates+=3;
      writebyte((z80.l | (z80.h << 8)),z80.l);
    };
    ops[0x76] = function op_0x76(tempaddr) {
      z80.halted=1;
      z80.pc--;z80.pc &= 0xffff;
    };
    ops[0x77] = function op_0x77(tempaddr) {
      tstates+=3;
      writebyte((z80.l | (z80.h << 8)),z80.a);
    };
    ops[0x78] = function op_0x78(tempaddr) {
      z80.a=z80.b;
    };
    ops[0x79] = function op_0x79(tempaddr) {
      z80.a=z80.c;
    };
    ops[0x7a] = function op_0x7a(tempaddr) {
      z80.a=z80.d;
    };
    ops[0x7b] = function op_0x7b(tempaddr) {
      z80.a=z80.e;
    };
    ops[0x7c] = function op_0x7c(tempaddr) {
      z80.a=z80.h;
    };
    ops[0x7d] = function op_0x7d(tempaddr) {
      z80.a=z80.l;
    };
    ops[0x7e] = function op_0x7e(tempaddr) {
      tstates+=3;
      z80.a=readbyte((z80.l | (z80.h << 8)));
    };
    ops[0x7f] = function op_0x7f(tempaddr) {
    };
    ops[0x80] = function op_0x80(tempaddr) {
      { var addtemp = z80.a + (z80.b); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.b) & 0x88 ) >> 2 ) | ( ( addtemp & 0x88 ) >> 1 ); z80.a=addtemp & 0xff; z80.f = ( addtemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x81] = function op_0x81(tempaddr) {
      { var addtemp = z80.a + (z80.c); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.c) & 0x88 ) >> 2 ) | ( ( addtemp & 0x88 ) >> 1 ); z80.a=addtemp & 0xff; z80.f = ( addtemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x82] = function op_0x82(tempaddr) {
      { var addtemp = z80.a + (z80.d); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.d) & 0x88 ) >> 2 ) | ( ( addtemp & 0x88 ) >> 1 ); z80.a=addtemp & 0xff; z80.f = ( addtemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x83] = function op_0x83(tempaddr) {
      { var addtemp = z80.a + (z80.e); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.e) & 0x88 ) >> 2 ) | ( ( addtemp & 0x88 ) >> 1 ); z80.a=addtemp & 0xff; z80.f = ( addtemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x84] = function op_0x84(tempaddr) {
      { var addtemp = z80.a + (z80.h); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.h) & 0x88 ) >> 2 ) | ( ( addtemp & 0x88 ) >> 1 ); z80.a=addtemp & 0xff; z80.f = ( addtemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x85] = function op_0x85(tempaddr) {
      { var addtemp = z80.a + (z80.l); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.l) & 0x88 ) >> 2 ) | ( ( addtemp & 0x88 ) >> 1 ); z80.a=addtemp & 0xff; z80.f = ( addtemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x86] = function op_0x86(tempaddr) {
      tstates+=3;
      {
    var bytetemp = readbyte( (z80.l | (z80.h << 8)) );
    { var addtemp = z80.a + (bytetemp); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (bytetemp) & 0x88 ) >> 2 ) | ( ( addtemp & 0x88 ) >> 1 ); z80.a=addtemp & 0xff; z80.f = ( addtemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
      }
    };
    ops[0x87] = function op_0x87(tempaddr) {
      { var addtemp = z80.a + (z80.a); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.a) & 0x88 ) >> 2 ) | ( ( addtemp & 0x88 ) >> 1 ); z80.a=addtemp & 0xff; z80.f = ( addtemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x88] = function op_0x88(tempaddr) {
      { var adctemp = z80.a + (z80.b) + ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.b) & 0x88 ) >> 2 ) | ( ( adctemp & 0x88 ) >> 1 ); z80.a=adctemp & 0xff; z80.f = ( adctemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x89] = function op_0x89(tempaddr) {
      { var adctemp = z80.a + (z80.c) + ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.c) & 0x88 ) >> 2 ) | ( ( adctemp & 0x88 ) >> 1 ); z80.a=adctemp & 0xff; z80.f = ( adctemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x8a] = function op_0x8a(tempaddr) {
      { var adctemp = z80.a + (z80.d) + ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.d) & 0x88 ) >> 2 ) | ( ( adctemp & 0x88 ) >> 1 ); z80.a=adctemp & 0xff; z80.f = ( adctemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x8b] = function op_0x8b(tempaddr) {
      { var adctemp = z80.a + (z80.e) + ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.e) & 0x88 ) >> 2 ) | ( ( adctemp & 0x88 ) >> 1 ); z80.a=adctemp & 0xff; z80.f = ( adctemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x8c] = function op_0x8c(tempaddr) {
      { var adctemp = z80.a + (z80.h) + ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.h) & 0x88 ) >> 2 ) | ( ( adctemp & 0x88 ) >> 1 ); z80.a=adctemp & 0xff; z80.f = ( adctemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x8d] = function op_0x8d(tempaddr) {
      { var adctemp = z80.a + (z80.l) + ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.l) & 0x88 ) >> 2 ) | ( ( adctemp & 0x88 ) >> 1 ); z80.a=adctemp & 0xff; z80.f = ( adctemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x8e] = function op_0x8e(tempaddr) {
      tstates+=3;
      {
    var bytetemp = readbyte( (z80.l | (z80.h << 8)) );
    { var adctemp = z80.a + (bytetemp) + ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (bytetemp) & 0x88 ) >> 2 ) | ( ( adctemp & 0x88 ) >> 1 ); z80.a=adctemp & 0xff; z80.f = ( adctemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
      }
    };
    ops[0x8f] = function op_0x8f(tempaddr) {
      { var adctemp = z80.a + (z80.a) + ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.a) & 0x88 ) >> 2 ) | ( ( adctemp & 0x88 ) >> 1 ); z80.a=adctemp & 0xff; z80.f = ( adctemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x90] = function op_0x90(tempaddr) {
      { var subtemp = z80.a - (z80.b); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.b) & 0x88 ) >> 2 ) | ( (subtemp & 0x88 ) >> 1 ); z80.a=subtemp & 0xff; z80.f = ( subtemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x91] = function op_0x91(tempaddr) {
      { var subtemp = z80.a - (z80.c); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.c) & 0x88 ) >> 2 ) | ( (subtemp & 0x88 ) >> 1 ); z80.a=subtemp & 0xff; z80.f = ( subtemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x92] = function op_0x92(tempaddr) {
      { var subtemp = z80.a - (z80.d); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.d) & 0x88 ) >> 2 ) | ( (subtemp & 0x88 ) >> 1 ); z80.a=subtemp & 0xff; z80.f = ( subtemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x93] = function op_0x93(tempaddr) {
      { var subtemp = z80.a - (z80.e); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.e) & 0x88 ) >> 2 ) | ( (subtemp & 0x88 ) >> 1 ); z80.a=subtemp & 0xff; z80.f = ( subtemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x94] = function op_0x94(tempaddr) {
      { var subtemp = z80.a - (z80.h); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.h) & 0x88 ) >> 2 ) | ( (subtemp & 0x88 ) >> 1 ); z80.a=subtemp & 0xff; z80.f = ( subtemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x95] = function op_0x95(tempaddr) {
      { var subtemp = z80.a - (z80.l); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.l) & 0x88 ) >> 2 ) | ( (subtemp & 0x88 ) >> 1 ); z80.a=subtemp & 0xff; z80.f = ( subtemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x96] = function op_0x96(tempaddr) {
      tstates+=3;
      {
    var bytetemp = readbyte( (z80.l | (z80.h << 8)) );
    { var subtemp = z80.a - (bytetemp); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (bytetemp) & 0x88 ) >> 2 ) | ( (subtemp & 0x88 ) >> 1 ); z80.a=subtemp & 0xff; z80.f = ( subtemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
      }
    };
    ops[0x97] = function op_0x97(tempaddr) {
      { var subtemp = z80.a - (z80.a); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.a) & 0x88 ) >> 2 ) | ( (subtemp & 0x88 ) >> 1 ); z80.a=subtemp & 0xff; z80.f = ( subtemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x98] = function op_0x98(tempaddr) {
      { var sbctemp = z80.a - (z80.b) - ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.b) & 0x88 ) >> 2 ) | ( ( sbctemp & 0x88 ) >> 1 ); z80.a=sbctemp & 0xff; z80.f = ( sbctemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x99] = function op_0x99(tempaddr) {
      { var sbctemp = z80.a - (z80.c) - ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.c) & 0x88 ) >> 2 ) | ( ( sbctemp & 0x88 ) >> 1 ); z80.a=sbctemp & 0xff; z80.f = ( sbctemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x9a] = function op_0x9a(tempaddr) {
      { var sbctemp = z80.a - (z80.d) - ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.d) & 0x88 ) >> 2 ) | ( ( sbctemp & 0x88 ) >> 1 ); z80.a=sbctemp & 0xff; z80.f = ( sbctemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x9b] = function op_0x9b(tempaddr) {
      { var sbctemp = z80.a - (z80.e) - ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.e) & 0x88 ) >> 2 ) | ( ( sbctemp & 0x88 ) >> 1 ); z80.a=sbctemp & 0xff; z80.f = ( sbctemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x9c] = function op_0x9c(tempaddr) {
      { var sbctemp = z80.a - (z80.h) - ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.h) & 0x88 ) >> 2 ) | ( ( sbctemp & 0x88 ) >> 1 ); z80.a=sbctemp & 0xff; z80.f = ( sbctemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x9d] = function op_0x9d(tempaddr) {
      { var sbctemp = z80.a - (z80.l) - ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.l) & 0x88 ) >> 2 ) | ( ( sbctemp & 0x88 ) >> 1 ); z80.a=sbctemp & 0xff; z80.f = ( sbctemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x9e] = function op_0x9e(tempaddr) {
      tstates+=3;
      {
    var bytetemp = readbyte( (z80.l | (z80.h << 8)) );
    { var sbctemp = z80.a - (bytetemp) - ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (bytetemp) & 0x88 ) >> 2 ) | ( ( sbctemp & 0x88 ) >> 1 ); z80.a=sbctemp & 0xff; z80.f = ( sbctemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
      }
    };
    ops[0x9f] = function op_0x9f(tempaddr) {
      { var sbctemp = z80.a - (z80.a) - ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.a) & 0x88 ) >> 2 ) | ( ( sbctemp & 0x88 ) >> 1 ); z80.a=sbctemp & 0xff; z80.f = ( sbctemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0xa0] = function op_0xa0(tempaddr) {
      { z80.a &= (z80.b); z80.f = 0x10 | sz53p_table[z80.a];};
    };
    ops[0xa1] = function op_0xa1(tempaddr) {
      { z80.a &= (z80.c); z80.f = 0x10 | sz53p_table[z80.a];};
    };
    ops[0xa2] = function op_0xa2(tempaddr) {
      { z80.a &= (z80.d); z80.f = 0x10 | sz53p_table[z80.a];};
    };
    ops[0xa3] = function op_0xa3(tempaddr) {
      { z80.a &= (z80.e); z80.f = 0x10 | sz53p_table[z80.a];};
    };
    ops[0xa4] = function op_0xa4(tempaddr) {
      { z80.a &= (z80.h); z80.f = 0x10 | sz53p_table[z80.a];};
    };
    ops[0xa5] = function op_0xa5(tempaddr) {
      { z80.a &= (z80.l); z80.f = 0x10 | sz53p_table[z80.a];};
    };
    ops[0xa6] = function op_0xa6(tempaddr) {
      tstates+=3;
      {
    var bytetemp = readbyte( (z80.l | (z80.h << 8)) );
    { z80.a &= (bytetemp); z80.f = 0x10 | sz53p_table[z80.a];};
      }
    };
    ops[0xa7] = function op_0xa7(tempaddr) {
      { z80.a &= (z80.a); z80.f = 0x10 | sz53p_table[z80.a];};
    };
    ops[0xa8] = function op_0xa8(tempaddr) {
      { z80.a ^= (z80.b); z80.f = sz53p_table[z80.a];};
    };
    ops[0xa9] = function op_0xa9(tempaddr) {
      { z80.a ^= (z80.c); z80.f = sz53p_table[z80.a];};
    };
    ops[0xaa] = function op_0xaa(tempaddr) {
      { z80.a ^= (z80.d); z80.f = sz53p_table[z80.a];};
    };
    ops[0xab] = function op_0xab(tempaddr) {
      { z80.a ^= (z80.e); z80.f = sz53p_table[z80.a];};
    };
    ops[0xac] = function op_0xac(tempaddr) {
      { z80.a ^= (z80.h); z80.f = sz53p_table[z80.a];};
    };
    ops[0xad] = function op_0xad(tempaddr) {
      { z80.a ^= (z80.l); z80.f = sz53p_table[z80.a];};
    };
    ops[0xae] = function op_0xae(tempaddr) {
      tstates+=3;
      {
    var bytetemp = readbyte( (z80.l | (z80.h << 8)) );
    { z80.a ^= (bytetemp); z80.f = sz53p_table[z80.a];};
      }
    };
    ops[0xaf] = function op_0xaf(tempaddr) {
      { z80.a ^= (z80.a); z80.f = sz53p_table[z80.a];};
    };
    ops[0xb0] = function op_0xb0(tempaddr) {
      { z80.a |= (z80.b); z80.f = sz53p_table[z80.a];};
    };
    ops[0xb1] = function op_0xb1(tempaddr) {
      { z80.a |= (z80.c); z80.f = sz53p_table[z80.a];};
    };
    ops[0xb2] = function op_0xb2(tempaddr) {
      { z80.a |= (z80.d); z80.f = sz53p_table[z80.a];};
    };
    ops[0xb3] = function op_0xb3(tempaddr) {
      { z80.a |= (z80.e); z80.f = sz53p_table[z80.a];};
    };
    ops[0xb4] = function op_0xb4(tempaddr) {
      { z80.a |= (z80.h); z80.f = sz53p_table[z80.a];};
    };
    ops[0xb5] = function op_0xb5(tempaddr) {
      { z80.a |= (z80.l); z80.f = sz53p_table[z80.a];};
    };
    ops[0xb6] = function op_0xb6(tempaddr) {
      tstates+=3;
      {
    var bytetemp = readbyte( (z80.l | (z80.h << 8)) );
    { z80.a |= (bytetemp); z80.f = sz53p_table[z80.a];};
      }
    };
    ops[0xb7] = function op_0xb7(tempaddr) {
      { z80.a |= (z80.a); z80.f = sz53p_table[z80.a];};
    };
    ops[0xb8] = function op_0xb8(tempaddr) {
      { var cptemp = z80.a - z80.b; var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.b) & 0x88 ) >> 2 ) | ( ( cptemp & 0x88 ) >> 1 ); z80.f = ( cptemp & 0x100 ? 0x01 : ( cptemp ? 0 : 0x40 ) ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | ( z80.b & ( 0x08 | 0x20 ) ) | ( cptemp & 0x80 );};
    };
    ops[0xb9] = function op_0xb9(tempaddr) {
      { var cptemp = z80.a - z80.c; var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.c) & 0x88 ) >> 2 ) | ( ( cptemp & 0x88 ) >> 1 ); z80.f = ( cptemp & 0x100 ? 0x01 : ( cptemp ? 0 : 0x40 ) ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | ( z80.c & ( 0x08 | 0x20 ) ) | ( cptemp & 0x80 );};
    };
    ops[0xba] = function op_0xba(tempaddr) {
      { var cptemp = z80.a - z80.d; var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.d) & 0x88 ) >> 2 ) | ( ( cptemp & 0x88 ) >> 1 ); z80.f = ( cptemp & 0x100 ? 0x01 : ( cptemp ? 0 : 0x40 ) ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | ( z80.d & ( 0x08 | 0x20 ) ) | ( cptemp & 0x80 );};
    };
    ops[0xbb] = function op_0xbb(tempaddr) {
      { var cptemp = z80.a - z80.e; var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.e) & 0x88 ) >> 2 ) | ( ( cptemp & 0x88 ) >> 1 ); z80.f = ( cptemp & 0x100 ? 0x01 : ( cptemp ? 0 : 0x40 ) ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | ( z80.e & ( 0x08 | 0x20 ) ) | ( cptemp & 0x80 );};
    };
    ops[0xbc] = function op_0xbc(tempaddr) {
      { var cptemp = z80.a - z80.h; var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.h) & 0x88 ) >> 2 ) | ( ( cptemp & 0x88 ) >> 1 ); z80.f = ( cptemp & 0x100 ? 0x01 : ( cptemp ? 0 : 0x40 ) ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | ( z80.h & ( 0x08 | 0x20 ) ) | ( cptemp & 0x80 );};
    };
    ops[0xbd] = function op_0xbd(tempaddr) {
      { var cptemp = z80.a - z80.l; var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.l) & 0x88 ) >> 2 ) | ( ( cptemp & 0x88 ) >> 1 ); z80.f = ( cptemp & 0x100 ? 0x01 : ( cptemp ? 0 : 0x40 ) ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | ( z80.l & ( 0x08 | 0x20 ) ) | ( cptemp & 0x80 );};
    };
    ops[0xbe] = function op_0xbe(tempaddr) {
      tstates+=3;
      {
    var bytetemp = readbyte( (z80.l | (z80.h << 8)) );
    { var cptemp = z80.a - bytetemp; var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (bytetemp) & 0x88 ) >> 2 ) | ( ( cptemp & 0x88 ) >> 1 ); z80.f = ( cptemp & 0x100 ? 0x01 : ( cptemp ? 0 : 0x40 ) ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | ( bytetemp & ( 0x08 | 0x20 ) ) | ( cptemp & 0x80 );};
      }
    };
    ops[0xbf] = function op_0xbf(tempaddr) {
      { var cptemp = z80.a - z80.a; var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.a) & 0x88 ) >> 2 ) | ( ( cptemp & 0x88 ) >> 1 ); z80.f = ( cptemp & 0x100 ? 0x01 : ( cptemp ? 0 : 0x40 ) ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | ( z80.a & ( 0x08 | 0x20 ) ) | ( cptemp & 0x80 );};
    };
    ops[0xc0] = function op_0xc0(tempaddr) {
      tstates++;
      if( ! ( z80.f & 0x40 ) ) { { { tstates+=6; var lowbyte =readbyte(z80.sp++); z80.sp &= 0xffff; var highbyte=readbyte(z80.sp++); z80.sp &= 0xffff; (z80.pc) = lowbyte | (highbyte << 8);};}; }
    };
    ops[0xc1] = function op_0xc1(tempaddr) {
      { tstates+=6; (z80.c)=readbyte(z80.sp++); z80.sp &= 0xffff; (z80.b)=readbyte(z80.sp++); z80.sp &= 0xffff;};
    };
    ops[0xc2] = function op_0xc2(tempaddr) {
      tstates+=6;
      if( ! ( z80.f & 0x40 ) ) { { var jptemp=z80.pc; var pcl =readbyte(jptemp++); jptemp &= 0xffff; var pch =readbyte(jptemp); z80.pc = pcl | (pch << 8);}; }
      else z80.pc+=2;
    };
    ops[0xc3] = function op_0xc3(tempaddr) {
      tstates+=6;
      { var jptemp=z80.pc; var pcl =readbyte(jptemp++); jptemp &= 0xffff; var pch =readbyte(jptemp); z80.pc = pcl | (pch << 8);};
    };
    ops[0xc4] = function op_0xc4(tempaddr) {
      tstates+=6;
      if( ! ( z80.f & 0x40 ) ) { { var calltempl, calltemph; calltempl=readbyte(z80.pc++); z80.pc &= 0xffff; tstates++; calltemph=readbyte(z80.pc++); z80.pc &= 0xffff; { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) >> 8); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) & 0xff);}; var pcl=calltempl; var pch=calltemph; z80.pc = pcl | (pch << 8);}; }
      else z80.pc+=2;
    };
    ops[0xc5] = function op_0xc5(tempaddr) {
      tstates++;
      { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.b)); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.c));};
    };
    ops[0xc6] = function op_0xc6(tempaddr) {
      tstates+=3;
      {
    var bytetemp = readbyte( z80.pc++ );
    { var addtemp = z80.a + (bytetemp); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (bytetemp) & 0x88 ) >> 2 ) | ( ( addtemp & 0x88 ) >> 1 ); z80.a=addtemp & 0xff; z80.f = ( addtemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
      }
    };
    ops[0xc7] = function op_0xc7(tempaddr) {
      tstates++;
      { { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) >> 8); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) & 0xff);}; z80.pc=(0x00);};
    };
    ops[0xc8] = function op_0xc8(tempaddr) {
      tstates++;
      if( z80.f & 0x40 ) { { { tstates+=6; var lowbyte =readbyte(z80.sp++); z80.sp &= 0xffff; var highbyte=readbyte(z80.sp++); z80.sp &= 0xffff; (z80.pc) = lowbyte | (highbyte << 8);};}; }
    };
    ops[0xc9] = function op_0xc9(tempaddr) {
      { { tstates+=6; var lowbyte =readbyte(z80.sp++); z80.sp &= 0xffff; var highbyte=readbyte(z80.sp++); z80.sp &= 0xffff; (z80.pc) = lowbyte | (highbyte << 8);};};
    };
    ops[0xca] = function op_0xca(tempaddr) {
      tstates+=6;
      if( z80.f & 0x40 ) { { var jptemp=z80.pc; var pcl =readbyte(jptemp++); jptemp &= 0xffff; var pch =readbyte(jptemp); z80.pc = pcl | (pch << 8);}; }
      else z80.pc+=2;
    };
    ops[0xcb] = function op_0xcb(tempaddr) {
      {
    var opcode2;
    tstates+=4;
    opcode2 = readbyte( z80.pc++ );
    z80.pc &= 0xffff;
    z80.r = (z80.r+1) & 0x7f;
    z80_cbxx(opcode2);
      }
    };
    ops[0xcc] = function op_0xcc(tempaddr) {
      tstates+=6;
      if( z80.f & 0x40 ) { { var calltempl, calltemph; calltempl=readbyte(z80.pc++); z80.pc &= 0xffff; tstates++; calltemph=readbyte(z80.pc++); z80.pc &= 0xffff; { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) >> 8); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) & 0xff);}; var pcl=calltempl; var pch=calltemph; z80.pc = pcl | (pch << 8);}; }
      else z80.pc+=2;
    };
    ops[0xcd] = function op_0xcd(tempaddr) {
      tstates+=6;
      { var calltempl, calltemph; calltempl=readbyte(z80.pc++); z80.pc &= 0xffff; tstates++; calltemph=readbyte(z80.pc++); z80.pc &= 0xffff; { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) >> 8); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) & 0xff);}; var pcl=calltempl; var pch=calltemph; z80.pc = pcl | (pch << 8);};
    };
    ops[0xce] = function op_0xce(tempaddr) {
      tstates+=3;
      {
    var bytetemp = readbyte( z80.pc++ );
    { var adctemp = z80.a + (bytetemp) + ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (bytetemp) & 0x88 ) >> 2 ) | ( ( adctemp & 0x88 ) >> 1 ); z80.a=adctemp & 0xff; z80.f = ( adctemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
      }
    };
    ops[0xcf] = function op_0xcf(tempaddr) {
      tstates++;
      { { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) >> 8); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) & 0xff);}; z80.pc=(0x08);};
    };
    ops[0xd0] = function op_0xd0(tempaddr) {
      tstates++;
      if( ! ( z80.f & 0x01 ) ) { { { tstates+=6; var lowbyte =readbyte(z80.sp++); z80.sp &= 0xffff; var highbyte=readbyte(z80.sp++); z80.sp &= 0xffff; (z80.pc) = lowbyte | (highbyte << 8);};}; }
    };
    ops[0xd1] = function op_0xd1(tempaddr) {
      { tstates+=6; (z80.e)=readbyte(z80.sp++); z80.sp &= 0xffff; (z80.d)=readbyte(z80.sp++); z80.sp &= 0xffff;};
    };
    ops[0xd2] = function op_0xd2(tempaddr) {
      tstates+=6;
      if( ! ( z80.f & 0x01 ) ) { { var jptemp=z80.pc; var pcl =readbyte(jptemp++); jptemp &= 0xffff; var pch =readbyte(jptemp); z80.pc = pcl | (pch << 8);}; }
      else z80.pc+=2;
    };
    ops[0xd3] = function op_0xd3(tempaddr) {
      {
    var outtemp;
    tstates+=4;
    outtemp = readbyte( z80.pc++ ) + ( z80.a << 8 );
    z80.pc &= 0xffff;
    { tstates += (3);; writeport(outtemp,z80.a);};
      }
    };
    ops[0xd4] = function op_0xd4(tempaddr) {
      tstates+=6;
      if( ! ( z80.f & 0x01 ) ) { { var calltempl, calltemph; calltempl=readbyte(z80.pc++); z80.pc &= 0xffff; tstates++; calltemph=readbyte(z80.pc++); z80.pc &= 0xffff; { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) >> 8); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) & 0xff);}; var pcl=calltempl; var pch=calltemph; z80.pc = pcl | (pch << 8);}; }
      else z80.pc+=2;
    };
    ops[0xd5] = function op_0xd5(tempaddr) {
      tstates++;
      { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.d)); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.e));};
    };
    ops[0xd6] = function op_0xd6(tempaddr) {
      tstates+=3;
      {
    var bytetemp = readbyte( z80.pc++ );
    { var subtemp = z80.a - (bytetemp); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (bytetemp) & 0x88 ) >> 2 ) | ( (subtemp & 0x88 ) >> 1 ); z80.a=subtemp & 0xff; z80.f = ( subtemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
      }
    };
    ops[0xd7] = function op_0xd7(tempaddr) {
      tstates++;
      { { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) >> 8); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) & 0xff);}; z80.pc=(0x10);};
    };
    ops[0xd8] = function op_0xd8(tempaddr) {
      tstates++;
      if( z80.f & 0x01 ) { { { tstates+=6; var lowbyte =readbyte(z80.sp++); z80.sp &= 0xffff; var highbyte=readbyte(z80.sp++); z80.sp &= 0xffff; (z80.pc) = lowbyte | (highbyte << 8);};}; }
    };
    ops[0xd9] = function op_0xd9(tempaddr) {
      {
    var bytetemp;
    bytetemp = z80.b; z80.b = z80.b_; z80.b_ = bytetemp;
    bytetemp = z80.c; z80.c = z80.c_; z80.c_ = bytetemp;
    bytetemp = z80.d; z80.d = z80.d_; z80.d_ = bytetemp;
    bytetemp = z80.e; z80.e = z80.e_; z80.e_ = bytetemp;
    bytetemp = z80.h; z80.h = z80.h_; z80.h_ = bytetemp;
    bytetemp = z80.l; z80.l = z80.l_; z80.l_ = bytetemp;
      }
    };
    ops[0xda] = function op_0xda(tempaddr) {
      tstates+=6;
      if( z80.f & 0x01 ) { { var jptemp=z80.pc; var pcl =readbyte(jptemp++); jptemp &= 0xffff; var pch =readbyte(jptemp); z80.pc = pcl | (pch << 8);}; }
      else z80.pc+=2;
    };
    ops[0xdb] = function op_0xdb(tempaddr) {
      {
    var intemp;
    tstates+=4;
    intemp = readbyte( z80.pc++ ) + ( z80.a << 8 );
    z80.pc &= 0xffff;
    tstates += (3);;
        z80.a=readport( intemp );
      }
    };
    ops[0xdc] = function op_0xdc(tempaddr) {
      tstates+=6;
      if( z80.f & 0x01 ) { { var calltempl, calltemph; calltempl=readbyte(z80.pc++); z80.pc &= 0xffff; tstates++; calltemph=readbyte(z80.pc++); z80.pc &= 0xffff; { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) >> 8); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) & 0xff);}; var pcl=calltempl; var pch=calltemph; z80.pc = pcl | (pch << 8);}; }
      else z80.pc+=2;
    };
    ops[0xdd] = function op_0xdd(tempaddr) {
      {
    var opcode2;
    tstates+=4;
    opcode2 = readbyte( z80.pc++ );
    z80.pc &= 0xffff;
    z80.r = (z80.r+1) & 0x7f;
    z80_ddxx(opcode2);
      }
    };
    ops[0xde] = function op_0xde(tempaddr) {
      tstates+=3;
      {
    var bytetemp = readbyte( z80.pc++ );
    { var sbctemp = z80.a - (bytetemp) - ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (bytetemp) & 0x88 ) >> 2 ) | ( ( sbctemp & 0x88 ) >> 1 ); z80.a=sbctemp & 0xff; z80.f = ( sbctemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
      }
    };
    ops[0xdf] = function op_0xdf(tempaddr) {
      tstates++;
      { { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) >> 8); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) & 0xff);}; z80.pc=(0x18);};
    };
    ops[0xe0] = function op_0xe0(tempaddr) {
      tstates++;
      if( ! ( z80.f & 0x04 ) ) { { { tstates+=6; var lowbyte =readbyte(z80.sp++); z80.sp &= 0xffff; var highbyte=readbyte(z80.sp++); z80.sp &= 0xffff; (z80.pc) = lowbyte | (highbyte << 8);};}; }
    };
    ops[0xe1] = function op_0xe1(tempaddr) {
      { tstates+=6; (z80.l)=readbyte(z80.sp++); z80.sp &= 0xffff; (z80.h)=readbyte(z80.sp++); z80.sp &= 0xffff;};
    };
    ops[0xe2] = function op_0xe2(tempaddr) {
      tstates+=6;
      if( ! ( z80.f & 0x04 ) ) { { var jptemp=z80.pc; var pcl =readbyte(jptemp++); jptemp &= 0xffff; var pch =readbyte(jptemp); z80.pc = pcl | (pch << 8);}; }
      else z80.pc+=2;
    };
    ops[0xe3] = function op_0xe3(tempaddr) {
      {
    var bytetempl = readbyte( z80.sp ),
                     bytetemph = readbyte( z80.sp + 1 );
    tstates+=15;
    writebyte(z80.sp+1,z80.h); writebyte(z80.sp,z80.l);
    z80.l=bytetempl; z80.h=bytetemph;
      }
    };
    ops[0xe4] = function op_0xe4(tempaddr) {
      tstates+=6;
      if( ! ( z80.f & 0x04 ) ) { { var calltempl, calltemph; calltempl=readbyte(z80.pc++); z80.pc &= 0xffff; tstates++; calltemph=readbyte(z80.pc++); z80.pc &= 0xffff; { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) >> 8); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) & 0xff);}; var pcl=calltempl; var pch=calltemph; z80.pc = pcl | (pch << 8);}; }
      else z80.pc+=2;
    };
    ops[0xe5] = function op_0xe5(tempaddr) {
      tstates++;
      { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.h)); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.l));};
    };
    ops[0xe6] = function op_0xe6(tempaddr) {
      tstates+=3;
      {
    var bytetemp = readbyte( z80.pc++ );
    { z80.a &= (bytetemp); z80.f = 0x10 | sz53p_table[z80.a];};
      }
    };
    ops[0xe7] = function op_0xe7(tempaddr) {
      tstates++;
      { { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) >> 8); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) & 0xff);}; z80.pc=(0x20);};
    };
    ops[0xe8] = function op_0xe8(tempaddr) {
      tstates++;
      if( z80.f & 0x04 ) { { { tstates+=6; var lowbyte =readbyte(z80.sp++); z80.sp &= 0xffff; var highbyte=readbyte(z80.sp++); z80.sp &= 0xffff; (z80.pc) = lowbyte | (highbyte << 8);};}; }
    };
    ops[0xe9] = function op_0xe9(tempaddr) {
      z80.pc=(z80.l | (z80.h << 8));
    };
    ops[0xea] = function op_0xea(tempaddr) {
      tstates+=6;
      if( z80.f & 0x04 ) { { var jptemp=z80.pc; var pcl =readbyte(jptemp++); jptemp &= 0xffff; var pch =readbyte(jptemp); z80.pc = pcl | (pch << 8);}; }
      else z80.pc+=2;
    };
    ops[0xeb] = function op_0xeb(tempaddr) {
      {
    var bytetemp;
    bytetemp = z80.d; z80.d = z80.h; z80.h = bytetemp;
    bytetemp = z80.e; z80.e = z80.l; z80.l = bytetemp;
      }
    };
    ops[0xec] = function op_0xec(tempaddr) {
      tstates+=6;
      if( z80.f & 0x04 ) { { var calltempl, calltemph; calltempl=readbyte(z80.pc++); z80.pc &= 0xffff; tstates++; calltemph=readbyte(z80.pc++); z80.pc &= 0xffff; { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) >> 8); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) & 0xff);}; var pcl=calltempl; var pch=calltemph; z80.pc = pcl | (pch << 8);}; }
      else z80.pc+=2;
    };
    ops[0xed] = function op_0xed(tempaddr) {
      {
    var opcode2;
    tstates+=4;
    opcode2 = readbyte( z80.pc++ );
    z80.pc &= 0xffff;
    z80.r = (z80.r+1) & 0x7f;
    z80_edxx(opcode2);
      }
    };
    ops[0xee] = function op_0xee(tempaddr) {
      tstates+=3;
      {
    var bytetemp = readbyte( z80.pc++ );
    { z80.a ^= (bytetemp); z80.f = sz53p_table[z80.a];};
      }
    };
    ops[0xef] = function op_0xef(tempaddr) {
      tstates++;
      { { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) >> 8); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) & 0xff);}; z80.pc=(0x28);};
    };
    ops[0xf0] = function op_0xf0(tempaddr) {
      tstates++;
      if( ! ( z80.f & 0x80 ) ) { { { tstates+=6; var lowbyte =readbyte(z80.sp++); z80.sp &= 0xffff; var highbyte=readbyte(z80.sp++); z80.sp &= 0xffff; (z80.pc) = lowbyte | (highbyte << 8);};}; }
    };
    ops[0xf1] = function op_0xf1(tempaddr) {
      { tstates+=6; (z80.f)=readbyte(z80.sp++); z80.sp &= 0xffff; (z80.a)=readbyte(z80.sp++); z80.sp &= 0xffff;};
    };
    ops[0xf2] = function op_0xf2(tempaddr) {
      tstates+=6;
      if( ! ( z80.f & 0x80 ) ) { { var jptemp=z80.pc; var pcl =readbyte(jptemp++); jptemp &= 0xffff; var pch =readbyte(jptemp); z80.pc = pcl | (pch << 8);}; }
      else z80.pc+=2;
    };
    ops[0xf3] = function op_0xf3(tempaddr) {
      z80.iff1=z80.iff2=0;
    };
    ops[0xf4] = function op_0xf4(tempaddr) {
      tstates+=6;
      if( ! ( z80.f & 0x80 ) ) { { var calltempl, calltemph; calltempl=readbyte(z80.pc++); z80.pc &= 0xffff; tstates++; calltemph=readbyte(z80.pc++); z80.pc &= 0xffff; { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) >> 8); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) & 0xff);}; var pcl=calltempl; var pch=calltemph; z80.pc = pcl | (pch << 8);}; }
      else z80.pc+=2;
    };
    ops[0xf5] = function op_0xf5(tempaddr) {
      tstates++;
      { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.a)); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.f));};
    };
    ops[0xf6] = function op_0xf6(tempaddr) {
      tstates+=3;
      {
    var bytetemp = readbyte( z80.pc++ );
    { z80.a |= (bytetemp); z80.f = sz53p_table[z80.a];};
      }
    };
    ops[0xf7] = function op_0xf7(tempaddr) {
      tstates++;
      { { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) >> 8); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) & 0xff);}; z80.pc=(0x30);};
    };
    ops[0xf8] = function op_0xf8(tempaddr) {
      tstates++;
      if( z80.f & 0x80 ) { { { tstates+=6; var lowbyte =readbyte(z80.sp++); z80.sp &= 0xffff; var highbyte=readbyte(z80.sp++); z80.sp &= 0xffff; (z80.pc) = lowbyte | (highbyte << 8);};}; }
    };
    ops[0xf9] = function op_0xf9(tempaddr) {
      tstates += 2;
      z80.sp=(z80.l | (z80.h << 8));
    };
    ops[0xfa] = function op_0xfa(tempaddr) {
      tstates+=6;
      if( z80.f & 0x80 ) { { var jptemp=z80.pc; var pcl =readbyte(jptemp++); jptemp &= 0xffff; var pch =readbyte(jptemp); z80.pc = pcl | (pch << 8);}; }
      else z80.pc+=2;
    };
    ops[0xfb] = function op_0xfb(tempaddr) {
      z80.iff1=z80.iff2=1;
    };
    ops[0xfc] = function op_0xfc(tempaddr) {
      tstates+=6;
      if( z80.f & 0x80 ) { { var calltempl, calltemph; calltempl=readbyte(z80.pc++); z80.pc &= 0xffff; tstates++; calltemph=readbyte(z80.pc++); z80.pc &= 0xffff; { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) >> 8); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) & 0xff);}; var pcl=calltempl; var pch=calltemph; z80.pc = pcl | (pch << 8);}; }
      else z80.pc+=2;
    };
    ops[0xfd] = function op_0xfd(tempaddr) {
      {
    var opcode2;
    tstates+=4;
    opcode2 = readbyte( z80.pc++ );
    z80.pc &= 0xffff;
    z80.r = (z80.r+1) & 0x7f;
    z80_fdxx(opcode2);
      }
    };
    ops[0xfe] = function op_0xfe(tempaddr) {
      tstates+=3;
      {
    var bytetemp = readbyte( z80.pc++ );
    { var cptemp = z80.a - bytetemp; var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (bytetemp) & 0x88 ) >> 2 ) | ( ( cptemp & 0x88 ) >> 1 ); z80.f = ( cptemp & 0x100 ? 0x01 : ( cptemp ? 0 : 0x40 ) ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | ( bytetemp & ( 0x08 | 0x20 ) ) | ( cptemp & 0x80 );};
      }
    };
    ops[0xff] = function op_0xff(tempaddr) {
      tstates++;
      { { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) >> 8); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.pc) & 0xff);}; z80.pc=(0x38);};
    };
    ops[256] = function() {};
    z80_defaults(ops);
    return ops;
}();
var z80_ed_ops = function z80_setup_ed_ops() {
    var ops = [];
    ops[0x40] = function op_0x40(tempaddr) {
      tstates += 1;
      { tstates += (3);; (z80.b)=readport(((z80.c | (z80.b << 8)))); z80.f = ( z80.f & 0x01) | sz53p_table[(z80.b)];};
    };
    ops[0x41] = function op_0x41(tempaddr) {
      tstates += 1;
      { tstates += (3);; writeport((z80.c | (z80.b << 8)),z80.b);};
    };
    ops[0x42] = function op_0x42(tempaddr) {
      tstates += 7;
      { var sub16temp = (z80.l | (z80.h << 8)) - ((z80.c | (z80.b << 8))) - (z80.f & 0x01); var lookup = ( ( (z80.l | (z80.h << 8)) & 0x8800 ) >> 11 ) | ( ( ((z80.c | (z80.b << 8))) & 0x8800 ) >> 10 ) | ( ( sub16temp & 0x8800 ) >> 9 ); z80.h = (sub16temp >> 8) & 0xff; z80.l = sub16temp & 0xff; z80.f = ( sub16temp & 0x10000 ? 0x01 : 0 ) | 0x02 | overflow_sub_table[lookup >> 4] | ( z80.h & ( 0x08 | 0x20 | 0x80 ) ) | halfcarry_sub_table[lookup&0x07] | ( (z80.l | (z80.h << 8)) ? 0 : 0x40) ;};
    };
    ops[0x43] = function op_0x43(tempaddr) {
      { var ldtemp; tstates+=12; ldtemp=readbyte(z80.pc++); z80.pc &= 0xffff; ldtemp|=readbyte(z80.pc++) << 8; z80.pc &= 0xffff; writebyte(ldtemp++,(z80.c)); ldtemp &= 0xffff; writebyte(ldtemp,(z80.b));};
    };
    ops[0x44] =
    ops[0x4c] =
    ops[0x54] =
    ops[0x5c] =
    ops[0x64] =
    ops[0x6c] =
    ops[0x74] =
    ops[0x7c] = function op_0x7c(tempaddr) {
      {
    var bytetemp=z80.a;
    z80.a=0;
    { var subtemp = z80.a - (bytetemp); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (bytetemp) & 0x88 ) >> 2 ) | ( (subtemp & 0x88 ) >> 1 ); z80.a=subtemp & 0xff; z80.f = ( subtemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
      }
    };
    ops[0x45] =
    ops[0x4d] =
    ops[0x55] =
    ops[0x5d] =
    ops[0x65] =
    ops[0x6d] =
    ops[0x75] =
    ops[0x7d] = function op_0x7d(tempaddr) {
      z80.iff1=z80.iff2;
      { { tstates+=6; var lowbyte =readbyte(z80.sp++); z80.sp &= 0xffff; var highbyte=readbyte(z80.sp++); z80.sp &= 0xffff; (z80.pc) = lowbyte | (highbyte << 8);};};
    };
    ops[0x46] =
    ops[0x4e] =
    ops[0x66] =
    ops[0x6e] = function op_0x6e(tempaddr) {
      z80.im=0;
    };
    ops[0x47] = function op_0x47(tempaddr) {
      tstates += 1;
      z80.i=z80.a;
    };
    ops[0x48] = function op_0x48(tempaddr) {
      tstates += 1;
      { tstates += (3);; (z80.c)=readport(((z80.c | (z80.b << 8)))); z80.f = ( z80.f & 0x01) | sz53p_table[(z80.c)];};
    };
    ops[0x49] = function op_0x49(tempaddr) {
      tstates += 1;
      { tstates += (3);; writeport((z80.c | (z80.b << 8)),z80.c);};
    };
    ops[0x4a] = function op_0x4a(tempaddr) {
      tstates += 7;
      { var add16temp= (z80.l | (z80.h << 8)) + ((z80.c | (z80.b << 8))) + ( z80.f & 0x01 ); var lookup = ( ( (z80.l | (z80.h << 8)) & 0x8800 ) >> 11 ) | ( ( ((z80.c | (z80.b << 8))) & 0x8800 ) >> 10 ) | ( ( add16temp & 0x8800 ) >> 9 ); z80.h = (add16temp >> 8) & 0xff; z80.l = add16temp & 0xff; z80.f = ( add16temp & 0x10000 ? 0x01 : 0 )| overflow_add_table[lookup >> 4] | ( z80.h & ( 0x08 | 0x20 | 0x80 ) ) | halfcarry_add_table[lookup&0x07]| ( (z80.l | (z80.h << 8)) ? 0 : 0x40 );};
    };
    ops[0x4b] = function op_0x4b(tempaddr) {
      { var ldtemp; tstates+=12; ldtemp=readbyte(z80.pc++); z80.pc &= 0xffff; ldtemp|=readbyte(z80.pc++) << 8; z80.pc &= 0xffff; (z80.c)=readbyte(ldtemp++); ldtemp &= 0xffff; (z80.b)=readbyte(ldtemp);};
    };
    ops[0x4f] = function op_0x4f(tempaddr) {
      tstates += 1;
      z80.r=z80.r7=z80.a;
    };
    ops[0x50] = function op_0x50(tempaddr) {
      tstates += 1;
      { tstates += (3);; (z80.d)=readport(((z80.c | (z80.b << 8)))); z80.f = ( z80.f & 0x01) | sz53p_table[(z80.d)];};
    };
    ops[0x51] = function op_0x51(tempaddr) {
      tstates += 1;
      { tstates += (3);; writeport((z80.c | (z80.b << 8)),z80.d);};
    };
    ops[0x52] = function op_0x52(tempaddr) {
      tstates += 7;
      { var sub16temp = (z80.l | (z80.h << 8)) - ((z80.e | (z80.d << 8))) - (z80.f & 0x01); var lookup = ( ( (z80.l | (z80.h << 8)) & 0x8800 ) >> 11 ) | ( ( ((z80.e | (z80.d << 8))) & 0x8800 ) >> 10 ) | ( ( sub16temp & 0x8800 ) >> 9 ); z80.h = (sub16temp >> 8) & 0xff; z80.l = sub16temp & 0xff; z80.f = ( sub16temp & 0x10000 ? 0x01 : 0 ) | 0x02 | overflow_sub_table[lookup >> 4] | ( z80.h & ( 0x08 | 0x20 | 0x80 ) ) | halfcarry_sub_table[lookup&0x07] | ( (z80.l | (z80.h << 8)) ? 0 : 0x40) ;};
    };
    ops[0x53] = function op_0x53(tempaddr) {
      { var ldtemp; tstates+=12; ldtemp=readbyte(z80.pc++); z80.pc &= 0xffff; ldtemp|=readbyte(z80.pc++) << 8; z80.pc &= 0xffff; writebyte(ldtemp++,(z80.e)); ldtemp &= 0xffff; writebyte(ldtemp,(z80.d));};
    };
    ops[0x56] =
    ops[0x76] = function op_0x76(tempaddr) {
      z80.im=1;
    };
    ops[0x57] = function op_0x57(tempaddr) {
      tstates += 1;
      z80.a=z80.i;
      z80.f = ( z80.f & 0x01 ) | sz53_table[z80.a] | ( z80.iff2 ? 0x04 : 0 );
    };
    ops[0x58] = function op_0x58(tempaddr) {
      tstates += 1;
      { tstates += (3);; (z80.e)=readport(((z80.c | (z80.b << 8)))); z80.f = ( z80.f & 0x01) | sz53p_table[(z80.e)];};
    };
    ops[0x59] = function op_0x59(tempaddr) {
      tstates += 1;
      { tstates += (3);; writeport((z80.c | (z80.b << 8)),z80.e);};
    };
    ops[0x5a] = function op_0x5a(tempaddr) {
      tstates += 7;
      { var add16temp= (z80.l | (z80.h << 8)) + ((z80.e | (z80.d << 8))) + ( z80.f & 0x01 ); var lookup = ( ( (z80.l | (z80.h << 8)) & 0x8800 ) >> 11 ) | ( ( ((z80.e | (z80.d << 8))) & 0x8800 ) >> 10 ) | ( ( add16temp & 0x8800 ) >> 9 ); z80.h = (add16temp >> 8) & 0xff; z80.l = add16temp & 0xff; z80.f = ( add16temp & 0x10000 ? 0x01 : 0 )| overflow_add_table[lookup >> 4] | ( z80.h & ( 0x08 | 0x20 | 0x80 ) ) | halfcarry_add_table[lookup&0x07]| ( (z80.l | (z80.h << 8)) ? 0 : 0x40 );};
    };
    ops[0x5b] = function op_0x5b(tempaddr) {
      { var ldtemp; tstates+=12; ldtemp=readbyte(z80.pc++); z80.pc &= 0xffff; ldtemp|=readbyte(z80.pc++) << 8; z80.pc &= 0xffff; (z80.e)=readbyte(ldtemp++); ldtemp &= 0xffff; (z80.d)=readbyte(ldtemp);};
    };
    ops[0x5e] =
    ops[0x7e] = function op_0x7e(tempaddr) {
      z80.im=2;
    };
    ops[0x5f] = function op_0x5f(tempaddr) {
      tstates += 1;
      z80.a=(z80.r&0x7f) | (z80.r7&0x80);
      z80.f = ( z80.f & 0x01 ) | sz53_table[z80.a] | ( z80.iff2 ? 0x04 : 0 );
    };
    ops[0x60] = function op_0x60(tempaddr) {
      tstates += 1;
      { tstates += (3);; (z80.h)=readport(((z80.c | (z80.b << 8)))); z80.f = ( z80.f & 0x01) | sz53p_table[(z80.h)];};
    };
    ops[0x61] = function op_0x61(tempaddr) {
      tstates += 1;
      { tstates += (3);; writeport((z80.c | (z80.b << 8)),z80.h);};
    };
    ops[0x62] = function op_0x62(tempaddr) {
      tstates += 7;
      { var sub16temp = (z80.l | (z80.h << 8)) - ((z80.l | (z80.h << 8))) - (z80.f & 0x01); var lookup = ( ( (z80.l | (z80.h << 8)) & 0x8800 ) >> 11 ) | ( ( ((z80.l | (z80.h << 8))) & 0x8800 ) >> 10 ) | ( ( sub16temp & 0x8800 ) >> 9 ); z80.h = (sub16temp >> 8) & 0xff; z80.l = sub16temp & 0xff; z80.f = ( sub16temp & 0x10000 ? 0x01 : 0 ) | 0x02 | overflow_sub_table[lookup >> 4] | ( z80.h & ( 0x08 | 0x20 | 0x80 ) ) | halfcarry_sub_table[lookup&0x07] | ( (z80.l | (z80.h << 8)) ? 0 : 0x40) ;};
    };
    ops[0x63] = function op_0x63(tempaddr) {
      { var ldtemp; tstates+=12; ldtemp=readbyte(z80.pc++); z80.pc &= 0xffff; ldtemp|=readbyte(z80.pc++) << 8; z80.pc &= 0xffff; writebyte(ldtemp++,(z80.l)); ldtemp &= 0xffff; writebyte(ldtemp,(z80.h));};
    };
    ops[0x67] = function op_0x67(tempaddr) {
      {
    var bytetemp = readbyte( (z80.l | (z80.h << 8)) );
    tstates+=10;
    writebyte((z80.l | (z80.h << 8)), ( (z80.a & 0x0f) << 4 ) | ( bytetemp >> 4 ) );
    z80.a = ( z80.a & 0xf0 ) | ( bytetemp & 0x0f );
    z80.f = ( z80.f & 0x01 ) | sz53p_table[z80.a];
      }
    };
    ops[0x68] = function op_0x68(tempaddr) {
      tstates += 1;
      { tstates += (3);; (z80.l)=readport(((z80.c | (z80.b << 8)))); z80.f = ( z80.f & 0x01) | sz53p_table[(z80.l)];};
    };
    ops[0x69] = function op_0x69(tempaddr) {
      tstates += 1;
      { tstates += (3);; writeport((z80.c | (z80.b << 8)),z80.l);};
    };
    ops[0x6a] = function op_0x6a(tempaddr) {
      tstates += 7;
      { var add16temp= (z80.l | (z80.h << 8)) + ((z80.l | (z80.h << 8))) + ( z80.f & 0x01 ); var lookup = ( ( (z80.l | (z80.h << 8)) & 0x8800 ) >> 11 ) | ( ( ((z80.l | (z80.h << 8))) & 0x8800 ) >> 10 ) | ( ( add16temp & 0x8800 ) >> 9 ); z80.h = (add16temp >> 8) & 0xff; z80.l = add16temp & 0xff; z80.f = ( add16temp & 0x10000 ? 0x01 : 0 )| overflow_add_table[lookup >> 4] | ( z80.h & ( 0x08 | 0x20 | 0x80 ) ) | halfcarry_add_table[lookup&0x07]| ( (z80.l | (z80.h << 8)) ? 0 : 0x40 );};
    };
    ops[0x6b] = function op_0x6b(tempaddr) {
      { var ldtemp; tstates+=12; ldtemp=readbyte(z80.pc++); z80.pc &= 0xffff; ldtemp|=readbyte(z80.pc++) << 8; z80.pc &= 0xffff; (z80.l)=readbyte(ldtemp++); ldtemp &= 0xffff; (z80.h)=readbyte(ldtemp);};
    };
    ops[0x6f] = function op_0x6f(tempaddr) {
      {
    var bytetemp = readbyte( (z80.l | (z80.h << 8)) );
    tstates+=10;
    writebyte((z80.l | (z80.h << 8)), ((bytetemp & 0x0f) << 4 ) | ( z80.a & 0x0f ) );
    z80.a = ( z80.a & 0xf0 ) | ( bytetemp >> 4 );
    z80.f = ( z80.f & 0x01 ) | sz53p_table[z80.a];
      }
    };
    ops[0x70] = function op_0x70(tempaddr) {
      tstates += 1;
      {
    var bytetemp;
    { tstates += (3);; (bytetemp)=readport(((z80.c | (z80.b << 8)))); z80.f = ( z80.f & 0x01) | sz53p_table[(bytetemp)];};
      }
    };
    ops[0x71] = function op_0x71(tempaddr) {
      tstates += 1;
      { tstates += (3);; writeport((z80.c | (z80.b << 8)),0);};
    };
    ops[0x72] = function op_0x72(tempaddr) {
      tstates += 7;
      { var sub16temp = (z80.l | (z80.h << 8)) - (z80.sp) - (z80.f & 0x01); var lookup = ( ( (z80.l | (z80.h << 8)) & 0x8800 ) >> 11 ) | ( ( (z80.sp) & 0x8800 ) >> 10 ) | ( ( sub16temp & 0x8800 ) >> 9 ); z80.h = (sub16temp >> 8) & 0xff; z80.l = sub16temp & 0xff; z80.f = ( sub16temp & 0x10000 ? 0x01 : 0 ) | 0x02 | overflow_sub_table[lookup >> 4] | ( z80.h & ( 0x08 | 0x20 | 0x80 ) ) | halfcarry_sub_table[lookup&0x07] | ( (z80.l | (z80.h << 8)) ? 0 : 0x40) ;};
    };
    ops[0x73] = function op_0x73(tempaddr) {
      { var ldtemp; tstates+=12; ldtemp=readbyte(z80.pc++); z80.pc &= 0xffff; ldtemp|=readbyte(z80.pc++) << 8; z80.pc &= 0xffff; writebyte(ldtemp++,((z80.sp & 0xff))); ldtemp &= 0xffff; writebyte(ldtemp,((z80.sp >> 8)));};
    };
    ops[0x78] = function op_0x78(tempaddr) {
      tstates += 1;
      { tstates += (3);; (z80.a)=readport(((z80.c | (z80.b << 8)))); z80.f = ( z80.f & 0x01) | sz53p_table[(z80.a)];};
    };
    ops[0x79] = function op_0x79(tempaddr) {
      tstates += 1;
      { tstates += (3);; writeport((z80.c | (z80.b << 8)),z80.a);};
    };
    ops[0x7a] = function op_0x7a(tempaddr) {
      tstates += 7;
      { var add16temp= (z80.l | (z80.h << 8)) + (z80.sp) + ( z80.f & 0x01 ); var lookup = ( ( (z80.l | (z80.h << 8)) & 0x8800 ) >> 11 ) | ( ( (z80.sp) & 0x8800 ) >> 10 ) | ( ( add16temp & 0x8800 ) >> 9 ); z80.h = (add16temp >> 8) & 0xff; z80.l = add16temp & 0xff; z80.f = ( add16temp & 0x10000 ? 0x01 : 0 )| overflow_add_table[lookup >> 4] | ( z80.h & ( 0x08 | 0x20 | 0x80 ) ) | halfcarry_add_table[lookup&0x07]| ( (z80.l | (z80.h << 8)) ? 0 : 0x40 );};
    };
    ops[0x7b] = function op_0x7b(tempaddr) {
      { var ldtemp; tstates+=12; ldtemp=readbyte(z80.pc++); z80.pc &= 0xffff; ldtemp|=readbyte(z80.pc++) << 8; z80.pc &= 0xffff; var regl = readbyte(ldtemp++); ldtemp &= 0xffff; var regh =readbyte(ldtemp); z80.sp = regl | (regh << 8);};
    };
    ops[0xa0] = function op_0xa0(tempaddr) {
      {
    var bytetemp=readbyte( (z80.l | (z80.h << 8)) );
    tstates+=8;
    var bctemp = ((z80.c | (z80.b << 8)) - 1) & 0xffff; z80.b = bctemp >> 8; z80.c = bctemp & 0xff;
    writebyte((z80.e | (z80.d << 8)),bytetemp);
    var detemp = ((z80.e | (z80.d << 8)) + 1) & 0xffff; z80.d = detemp >> 8; z80.e = detemp & 0xff;
    var hltemp = ((z80.l | (z80.h << 8)) + 1) & 0xffff; z80.h = hltemp >> 8; z80.l = hltemp & 0xff;
    bytetemp = (bytetemp + z80.a) & 0xff;
    z80.f = ( z80.f & ( 0x01 | 0x40 | 0x80 ) ) | ( (z80.c | (z80.b << 8)) ? 0x04 : 0 ) |
      ( bytetemp & 0x08 ) | ( (bytetemp & 0x02) ? 0x20 : 0 );
      }
    };
    ops[0xa1] = function op_0xa1(tempaddr) {
      {
    var value = readbyte( (z80.l | (z80.h << 8)) ), bytetemp = (z80.a - value) & 0xff,
      lookup = ( ( z80.a & 0x08 ) >> 3 ) |
               ( ( (value) & 0x08 ) >> 2 ) |
               ( ( bytetemp & 0x08 ) >> 1 );
    tstates+=8;
    var hltemp = ((z80.l | (z80.h << 8)) + 1) & 0xffff; z80.h = hltemp >> 8; z80.l = hltemp & 0xff;
    var bctemp = ((z80.c | (z80.b << 8)) - 1) & 0xffff; z80.b = bctemp >> 8; z80.c = bctemp & 0xff;
    z80.f = ( z80.f & 0x01 ) | ( (z80.c | (z80.b << 8)) ? ( 0x04 | 0x02 ) : 0x02 ) |
      halfcarry_sub_table[lookup] | ( bytetemp ? 0 : 0x40 ) |
      ( bytetemp & 0x80 );
    if(z80.f & 0x10) bytetemp--;
    z80.f |= ( bytetemp & 0x08 ) | ( (bytetemp&0x02) ? 0x20 : 0 );
      }
    };
    ops[0xa2] = function op_0xa2(tempaddr) {
      {
    var initemp = readport( (z80.c | (z80.b << 8)) );
    tstates += 5; tstates += (3);;
    writebyte((z80.l | (z80.h << 8)),initemp);
    z80.b = (z80.b-1)&0xff;
    var hltemp = ((z80.l | (z80.h << 8)) + 1) & 0xffff; z80.h = hltemp >> 8; z80.l = hltemp & 0xff;
    z80.f = (initemp & 0x80 ? 0x02 : 0 ) | sz53_table[z80.b];
      }
    };
    ops[0xa3] = function op_0xa3(tempaddr) {
      {
    var outitemp=readbyte( (z80.l | (z80.h << 8)) );
    z80.b = (z80.b-1)&0xff;
    tstates+=5; tstates += (3);;
    var hltemp = ((z80.l | (z80.h << 8)) + 1) & 0xffff; z80.h = hltemp >> 8; z80.l = hltemp & 0xff;
    writeport((z80.c | (z80.b << 8)),outitemp);
    z80.f = (outitemp & 0x80 ? 0x02 : 0 ) | sz53_table[z80.b];
      }
    };
    ops[0xa8] = function op_0xa8(tempaddr) {
      {
    var bytetemp=readbyte( (z80.l | (z80.h << 8)) );
    tstates+=8;
    var bctemp = ((z80.c | (z80.b << 8)) - 1) & 0xffff; z80.b = bctemp >> 8; z80.c = bctemp & 0xff;
    writebyte((z80.e | (z80.d << 8)),bytetemp);
    var detemp = ((z80.e | (z80.d << 8)) - 1) & 0xffff; z80.d = detemp >> 8; z80.e = detemp & 0xff;
    var hltemp = ((z80.l | (z80.h << 8)) - 1) & 0xffff; z80.h = hltemp >> 8; z80.l = hltemp & 0xff;
    bytetemp = (bytetemp + z80.a) & 0xff;
    z80.f = ( z80.f & ( 0x01 | 0x40 | 0x80 ) ) | ( (z80.c | (z80.b << 8)) ? 0x04 : 0 ) |
      ( bytetemp & 0x08 ) | ( (bytetemp & 0x02) ? 0x20 : 0 );
      }
    };
    ops[0xa9] = function op_0xa9(tempaddr) {
      {
    var value = readbyte( (z80.l | (z80.h << 8)) ), bytetemp = (z80.a - value) & 0xff,
      lookup = ( ( z80.a & 0x08 ) >> 3 ) |
               ( ( (value) & 0x08 ) >> 2 ) |
               ( ( bytetemp & 0x08 ) >> 1 );
    tstates+=8;
    var hltemp = ((z80.l | (z80.h << 8)) - 1) & 0xffff; z80.h = hltemp >> 8; z80.l = hltemp & 0xff;
    var bctemp = ((z80.c | (z80.b << 8)) - 1) & 0xffff; z80.b = bctemp >> 8; z80.c = bctemp & 0xff;
    z80.f = ( z80.f & 0x01 ) | ( (z80.c | (z80.b << 8)) ? ( 0x04 | 0x02 ) : 0x02 ) |
      halfcarry_sub_table[lookup] | ( bytetemp ? 0 : 0x40 ) |
      ( bytetemp & 0x80 );
    if(z80.f & 0x10) bytetemp--;
    z80.f |= ( bytetemp & 0x08 ) | ( (bytetemp&0x02) ? 0x20 : 0 );
      }
    };
    ops[0xaa] = function op_0xaa(tempaddr) {
      {
    var initemp = readport( (z80.c | (z80.b << 8)) );
    tstates += 5; tstates += (3);;
    writebyte((z80.l | (z80.h << 8)),initemp);
    z80.b = (z80.b-1)&0xff;
    var hltemp = ((z80.l | (z80.h << 8)) - 1) & 0xffff; z80.h = hltemp >> 8; z80.l = hltemp & 0xff;
    z80.f = (initemp & 0x80 ? 0x02 : 0 ) | sz53_table[z80.b];
      }
    };
    ops[0xab] = function op_0xab(tempaddr) {
      {
    var outitemp=readbyte( (z80.l | (z80.h << 8)) );
    z80.b = (z80.b-1)&0xff;
    tstates+=5; tstates += (3);;
    var hltemp = ((z80.l | (z80.h << 8)) - 1) & 0xffff; z80.h = hltemp >> 8; z80.l = hltemp & 0xff;
    writeport((z80.c | (z80.b << 8)),outitemp);
    z80.f = (outitemp & 0x80 ? 0x02 : 0 ) | sz53_table[z80.b];
      }
    };
    ops[0xb0] = function op_0xb0(tempaddr) {
      {
    var bytetemp=readbyte( (z80.l | (z80.h << 8)) );
    tstates+=8;
    writebyte((z80.e | (z80.d << 8)),bytetemp);
    var hltemp = ((z80.l | (z80.h << 8)) + 1) & 0xffff; z80.h = hltemp >> 8; z80.l = hltemp & 0xff;
    var detemp = ((z80.e | (z80.d << 8)) + 1) & 0xffff; z80.d = detemp >> 8; z80.e = detemp & 0xff;
    var bctemp = ((z80.c | (z80.b << 8)) - 1) & 0xffff; z80.b = bctemp >> 8; z80.c = bctemp & 0xff;
    bytetemp = (bytetemp + z80.a) & 0xff;
    z80.f = ( z80.f & ( 0x01 | 0x40 | 0x80 ) ) | ( (z80.c | (z80.b << 8)) ? 0x04 : 0 ) |
      ( bytetemp & 0x08 ) | ( (bytetemp & 0x02) ? 0x20 : 0 );
    if((z80.c | (z80.b << 8))) {
      tstates+=5;
      z80.pc-=2;
    }
      }
    };
    ops[0xb1] = function op_0xb1(tempaddr) {
      {
    var value = readbyte( (z80.l | (z80.h << 8)) ), bytetemp = (z80.a - value) & 0xff,
      lookup = ( ( z80.a & 0x08 ) >> 3 ) |
           ( ( (value) & 0x08 ) >> 2 ) |
           ( ( bytetemp & 0x08 ) >> 1 );
    tstates+=8;
    var hltemp = ((z80.l | (z80.h << 8)) + 1) & 0xffff; z80.h = hltemp >> 8; z80.l = hltemp & 0xff;
    var bctemp = ((z80.c | (z80.b << 8)) - 1) & 0xffff; z80.b = bctemp >> 8; z80.c = bctemp & 0xff;
    z80.f = ( z80.f & 0x01 ) | ( (z80.c | (z80.b << 8)) ? ( 0x04 | 0x02 ) : 0x02 ) |
      halfcarry_sub_table[lookup] | ( bytetemp ? 0 : 0x40 ) |
      ( bytetemp & 0x80 );
    if(z80.f & 0x10) bytetemp--;
    z80.f |= ( bytetemp & 0x08 ) | ( (bytetemp&0x02) ? 0x20 : 0 );
    if( ( z80.f & ( 0x04 | 0x40 ) ) == 0x04 ) {
      tstates+=5;
      z80.pc-=2;
    }
      }
    };
    ops[0xb2] = function op_0xb2(tempaddr) {
      {
    var initemp=readport( (z80.c | (z80.b << 8)) );
    tstates += 5; tstates += (3);;
    writebyte((z80.l | (z80.h << 8)),initemp);
    z80.b = (z80.b-1)&0xff;
    var hltemp = ((z80.l | (z80.h << 8)) + 1) & 0xffff; z80.h = hltemp >> 8; z80.l = hltemp & 0xff;
    z80.f = (initemp & 0x80 ? 0x02 : 0 ) | sz53_table[z80.b];
    if(z80.b) {
      tstates+=5;
      z80.pc-=2;
    }
      }
    };
    ops[0xb3] = function op_0xb3(tempaddr) {
      {
    var outitemp=readbyte( (z80.l | (z80.h << 8)) );
    tstates+=5;
    z80.b = (z80.b-1)&0xff;
    var hltemp = ((z80.l | (z80.h << 8)) + 1) & 0xffff; z80.h = hltemp >> 8; z80.l = hltemp & 0xff;
    writeport((z80.c | (z80.b << 8)),outitemp);
    z80.f = (outitemp & 0x80 ? 0x02 : 0 ) | sz53_table[z80.b];
    if(z80.b) {
      tstates += (1);;
      tstates+=7;
      z80.pc-=2;
    } else {
      tstates += (3);;
    }
      }
    };
    ops[0xb8] = function op_0xb8(tempaddr) {
      {
    var bytetemp=readbyte( (z80.l | (z80.h << 8)) );
    tstates+=8;
    writebyte((z80.e | (z80.d << 8)),bytetemp);
    var hltemp = ((z80.l | (z80.h << 8)) - 1) & 0xffff; z80.h = hltemp >> 8; z80.l = hltemp & 0xff;
    var detemp = ((z80.e | (z80.d << 8)) - 1) & 0xffff; z80.d = detemp >> 8; z80.e = detemp & 0xff;
    var bctemp = ((z80.c | (z80.b << 8)) - 1) & 0xffff; z80.b = bctemp >> 8; z80.c = bctemp & 0xff;
    bytetemp = (bytetemp + z80.a) & 0xff;
    z80.f = ( z80.f & ( 0x01 | 0x40 | 0x80 ) ) | ( (z80.c | (z80.b << 8)) ? 0x04 : 0 ) |
      ( bytetemp & 0x08 ) | ( (bytetemp & 0x02) ? 0x20 : 0 );
    if((z80.c | (z80.b << 8))) {
      tstates+=5;
      z80.pc-=2;
    }
      }
    };
    ops[0xb9] = function op_0xb9(tempaddr) {
      {
    var value = readbyte( (z80.l | (z80.h << 8)) ), bytetemp = (z80.a - value) & 0xff,
      lookup = ( ( z80.a & 0x08 ) >> 3 ) |
           ( ( (value) & 0x08 ) >> 2 ) |
           ( ( bytetemp & 0x08 ) >> 1 );
    tstates+=8;
    var hltemp = ((z80.l | (z80.h << 8)) - 1) & 0xffff; z80.h = hltemp >> 8; z80.l = hltemp & 0xff;
    var bctemp = ((z80.c | (z80.b << 8)) - 1) & 0xffff; z80.b = bctemp >> 8; z80.c = bctemp & 0xff;
    z80.f = ( z80.f & 0x01 ) | ( (z80.c | (z80.b << 8)) ? ( 0x04 | 0x02 ) : 0x02 ) |
      halfcarry_sub_table[lookup] | ( bytetemp ? 0 : 0x40 ) |
      ( bytetemp & 0x80 );
    if(z80.f & 0x10) bytetemp--;
    z80.f |= ( bytetemp & 0x08 ) | ( (bytetemp&0x02) ? 0x20 : 0 );
    if( ( z80.f & ( 0x04 | 0x40 ) ) == 0x04 ) {
      tstates+=5;
      z80.pc-=2;
    }
      }
    };
    ops[0xba] = function op_0xba(tempaddr) {
      {
    var initemp=readport( (z80.c | (z80.b << 8)) );
    tstates += 5; tstates += (3);;
    writebyte((z80.l | (z80.h << 8)),initemp);
    z80.b = (z80.b-1)&0xff;
    var hltemp = ((z80.l | (z80.h << 8)) - 1) & 0xffff; z80.h = hltemp >> 8; z80.l = hltemp & 0xff;
    z80.f = (initemp & 0x80 ? 0x02 : 0 ) | sz53_table[z80.b];
    if(z80.b) {
      tstates+=5;
      z80.pc-=2;
    }
      }
    };
    ops[0xbb] = function op_0xbb(tempaddr) {
      {
    var outitemp=readbyte( (z80.l | (z80.h << 8)) );
    tstates+=5;
    z80.b = (z80.b-1)&0xff;
    var hltemp = ((z80.l | (z80.h << 8)) - 1) & 0xffff; z80.h = hltemp >> 8; z80.l = hltemp & 0xff;
    writeport((z80.c | (z80.b << 8)),outitemp);
    z80.f = (outitemp & 0x80 ? 0x02 : 0 ) | sz53_table[z80.b];
    if(z80.b) {
      tstates += (1);;
      tstates+=7;
      z80.pc-=2;
    } else {
      tstates += (3);;
    }
      }
    };
    ops[256] = function() {};
    z80_defaults(ops);
    return ops;
}();
function z80_edxx(opcode) { z80_ed_ops[opcode](); }
var z80_cb_ops = function z80_setup_cb_ops() {
    var ops = [];
    ops[0x00] = function op_0x00(tempaddr) {
      { (z80.b) = ( ((z80.b) & 0x7f)<<1 ) | ( (z80.b)>>7 ); z80.f = ( (z80.b) & 0x01 ) | sz53p_table[(z80.b)];};
    };
    ops[0x01] = function op_0x01(tempaddr) {
      { (z80.c) = ( ((z80.c) & 0x7f)<<1 ) | ( (z80.c)>>7 ); z80.f = ( (z80.c) & 0x01 ) | sz53p_table[(z80.c)];};
    };
    ops[0x02] = function op_0x02(tempaddr) {
      { (z80.d) = ( ((z80.d) & 0x7f)<<1 ) | ( (z80.d)>>7 ); z80.f = ( (z80.d) & 0x01 ) | sz53p_table[(z80.d)];};
    };
    ops[0x03] = function op_0x03(tempaddr) {
      { (z80.e) = ( ((z80.e) & 0x7f)<<1 ) | ( (z80.e)>>7 ); z80.f = ( (z80.e) & 0x01 ) | sz53p_table[(z80.e)];};
    };
    ops[0x04] = function op_0x04(tempaddr) {
      { (z80.h) = ( ((z80.h) & 0x7f)<<1 ) | ( (z80.h)>>7 ); z80.f = ( (z80.h) & 0x01 ) | sz53p_table[(z80.h)];};
    };
    ops[0x05] = function op_0x05(tempaddr) {
      { (z80.l) = ( ((z80.l) & 0x7f)<<1 ) | ( (z80.l)>>7 ); z80.f = ( (z80.l) & 0x01 ) | sz53p_table[(z80.l)];};
    };
    ops[0x06] = function op_0x06(tempaddr) {
      {
    var bytetemp = readbyte((z80.l | (z80.h << 8)));
    tstates+=7;
    { (bytetemp) = ( ((bytetemp) & 0x7f)<<1 ) | ( (bytetemp)>>7 ); z80.f = ( (bytetemp) & 0x01 ) | sz53p_table[(bytetemp)];};
    writebyte((z80.l | (z80.h << 8)),bytetemp);
      }
    };
    ops[0x07] = function op_0x07(tempaddr) {
      { (z80.a) = ( ((z80.a) & 0x7f)<<1 ) | ( (z80.a)>>7 ); z80.f = ( (z80.a) & 0x01 ) | sz53p_table[(z80.a)];};
    };
    ops[0x08] = function op_0x08(tempaddr) {
      { z80.f = (z80.b) & 0x01; (z80.b) = ( (z80.b)>>1 ) | ( ((z80.b) & 0x01)<<7 ); z80.f |= sz53p_table[(z80.b)];};
    };
    ops[0x09] = function op_0x09(tempaddr) {
      { z80.f = (z80.c) & 0x01; (z80.c) = ( (z80.c)>>1 ) | ( ((z80.c) & 0x01)<<7 ); z80.f |= sz53p_table[(z80.c)];};
    };
    ops[0x0a] = function op_0x0a(tempaddr) {
      { z80.f = (z80.d) & 0x01; (z80.d) = ( (z80.d)>>1 ) | ( ((z80.d) & 0x01)<<7 ); z80.f |= sz53p_table[(z80.d)];};
    };
    ops[0x0b] = function op_0x0b(tempaddr) {
      { z80.f = (z80.e) & 0x01; (z80.e) = ( (z80.e)>>1 ) | ( ((z80.e) & 0x01)<<7 ); z80.f |= sz53p_table[(z80.e)];};
    };
    ops[0x0c] = function op_0x0c(tempaddr) {
      { z80.f = (z80.h) & 0x01; (z80.h) = ( (z80.h)>>1 ) | ( ((z80.h) & 0x01)<<7 ); z80.f |= sz53p_table[(z80.h)];};
    };
    ops[0x0d] = function op_0x0d(tempaddr) {
      { z80.f = (z80.l) & 0x01; (z80.l) = ( (z80.l)>>1 ) | ( ((z80.l) & 0x01)<<7 ); z80.f |= sz53p_table[(z80.l)];};
    };
    ops[0x0e] = function op_0x0e(tempaddr) {
      {
    var bytetemp = readbyte((z80.l | (z80.h << 8)));
    tstates+=7;
    { z80.f = (bytetemp) & 0x01; (bytetemp) = ( (bytetemp)>>1 ) | ( ((bytetemp) & 0x01)<<7 ); z80.f |= sz53p_table[(bytetemp)];};
    writebyte((z80.l | (z80.h << 8)),bytetemp);
      }
    };
    ops[0x0f] = function op_0x0f(tempaddr) {
      { z80.f = (z80.a) & 0x01; (z80.a) = ( (z80.a)>>1 ) | ( ((z80.a) & 0x01)<<7 ); z80.f |= sz53p_table[(z80.a)];};
    };
    ops[0x10] = function op_0x10(tempaddr) {
      { var rltemp = (z80.b); (z80.b) = ( ((z80.b) & 0x7f)<<1 ) | ( z80.f & 0x01 ); z80.f = ( rltemp >> 7 ) | sz53p_table[(z80.b)];};
    };
    ops[0x11] = function op_0x11(tempaddr) {
      { var rltemp = (z80.c); (z80.c) = ( ((z80.c) & 0x7f)<<1 ) | ( z80.f & 0x01 ); z80.f = ( rltemp >> 7 ) | sz53p_table[(z80.c)];};
    };
    ops[0x12] = function op_0x12(tempaddr) {
      { var rltemp = (z80.d); (z80.d) = ( ((z80.d) & 0x7f)<<1 ) | ( z80.f & 0x01 ); z80.f = ( rltemp >> 7 ) | sz53p_table[(z80.d)];};
    };
    ops[0x13] = function op_0x13(tempaddr) {
      { var rltemp = (z80.e); (z80.e) = ( ((z80.e) & 0x7f)<<1 ) | ( z80.f & 0x01 ); z80.f = ( rltemp >> 7 ) | sz53p_table[(z80.e)];};
    };
    ops[0x14] = function op_0x14(tempaddr) {
      { var rltemp = (z80.h); (z80.h) = ( ((z80.h) & 0x7f)<<1 ) | ( z80.f & 0x01 ); z80.f = ( rltemp >> 7 ) | sz53p_table[(z80.h)];};
    };
    ops[0x15] = function op_0x15(tempaddr) {
      { var rltemp = (z80.l); (z80.l) = ( ((z80.l) & 0x7f)<<1 ) | ( z80.f & 0x01 ); z80.f = ( rltemp >> 7 ) | sz53p_table[(z80.l)];};
    };
    ops[0x16] = function op_0x16(tempaddr) {
      {
    var bytetemp = readbyte((z80.l | (z80.h << 8)));
    tstates+=7;
    { var rltemp = (bytetemp); (bytetemp) = ( ((bytetemp) & 0x7f)<<1 ) | ( z80.f & 0x01 ); z80.f = ( rltemp >> 7 ) | sz53p_table[(bytetemp)];};
    writebyte((z80.l | (z80.h << 8)),bytetemp);
      }
    };
    ops[0x17] = function op_0x17(tempaddr) {
      { var rltemp = (z80.a); (z80.a) = ( ((z80.a) & 0x7f)<<1 ) | ( z80.f & 0x01 ); z80.f = ( rltemp >> 7 ) | sz53p_table[(z80.a)];};
    };
    ops[0x18] = function op_0x18(tempaddr) {
      { var rrtemp = (z80.b); (z80.b) = ( (z80.b)>>1 ) | ( (z80.f & 0x01) << 7 ); z80.f = ( rrtemp & 0x01 ) | sz53p_table[(z80.b)];};
    };
    ops[0x19] = function op_0x19(tempaddr) {
      { var rrtemp = (z80.c); (z80.c) = ( (z80.c)>>1 ) | ( (z80.f & 0x01) << 7 ); z80.f = ( rrtemp & 0x01 ) | sz53p_table[(z80.c)];};
    };
    ops[0x1a] = function op_0x1a(tempaddr) {
      { var rrtemp = (z80.d); (z80.d) = ( (z80.d)>>1 ) | ( (z80.f & 0x01) << 7 ); z80.f = ( rrtemp & 0x01 ) | sz53p_table[(z80.d)];};
    };
    ops[0x1b] = function op_0x1b(tempaddr) {
      { var rrtemp = (z80.e); (z80.e) = ( (z80.e)>>1 ) | ( (z80.f & 0x01) << 7 ); z80.f = ( rrtemp & 0x01 ) | sz53p_table[(z80.e)];};
    };
    ops[0x1c] = function op_0x1c(tempaddr) {
      { var rrtemp = (z80.h); (z80.h) = ( (z80.h)>>1 ) | ( (z80.f & 0x01) << 7 ); z80.f = ( rrtemp & 0x01 ) | sz53p_table[(z80.h)];};
    };
    ops[0x1d] = function op_0x1d(tempaddr) {
      { var rrtemp = (z80.l); (z80.l) = ( (z80.l)>>1 ) | ( (z80.f & 0x01) << 7 ); z80.f = ( rrtemp & 0x01 ) | sz53p_table[(z80.l)];};
    };
    ops[0x1e] = function op_0x1e(tempaddr) {
      {
    var bytetemp = readbyte((z80.l | (z80.h << 8)));
    tstates+=7;
    { var rrtemp = (bytetemp); (bytetemp) = ( (bytetemp)>>1 ) | ( (z80.f & 0x01) << 7 ); z80.f = ( rrtemp & 0x01 ) | sz53p_table[(bytetemp)];};
    writebyte((z80.l | (z80.h << 8)),bytetemp);
      }
    };
    ops[0x1f] = function op_0x1f(tempaddr) {
      { var rrtemp = (z80.a); (z80.a) = ( (z80.a)>>1 ) | ( (z80.f & 0x01) << 7 ); z80.f = ( rrtemp & 0x01 ) | sz53p_table[(z80.a)];};
    };
    ops[0x20] = function op_0x20(tempaddr) {
      { z80.f = (z80.b) >> 7; (z80.b) <<= 1; (z80.b) &= 0xff; z80.f |= sz53p_table[(z80.b)];};
    };
    ops[0x21] = function op_0x21(tempaddr) {
      { z80.f = (z80.c) >> 7; (z80.c) <<= 1; (z80.c) &= 0xff; z80.f |= sz53p_table[(z80.c)];};
    };
    ops[0x22] = function op_0x22(tempaddr) {
      { z80.f = (z80.d) >> 7; (z80.d) <<= 1; (z80.d) &= 0xff; z80.f |= sz53p_table[(z80.d)];};
    };
    ops[0x23] = function op_0x23(tempaddr) {
      { z80.f = (z80.e) >> 7; (z80.e) <<= 1; (z80.e) &= 0xff; z80.f |= sz53p_table[(z80.e)];};
    };
    ops[0x24] = function op_0x24(tempaddr) {
      { z80.f = (z80.h) >> 7; (z80.h) <<= 1; (z80.h) &= 0xff; z80.f |= sz53p_table[(z80.h)];};
    };
    ops[0x25] = function op_0x25(tempaddr) {
      { z80.f = (z80.l) >> 7; (z80.l) <<= 1; (z80.l) &= 0xff; z80.f |= sz53p_table[(z80.l)];};
    };
    ops[0x26] = function op_0x26(tempaddr) {
      {
    var bytetemp = readbyte((z80.l | (z80.h << 8)));
    tstates+=7;
    { z80.f = (bytetemp) >> 7; (bytetemp) <<= 1; (bytetemp) &= 0xff; z80.f |= sz53p_table[(bytetemp)];};
    writebyte((z80.l | (z80.h << 8)),bytetemp);
      }
    };
    ops[0x27] = function op_0x27(tempaddr) {
      { z80.f = (z80.a) >> 7; (z80.a) <<= 1; (z80.a) &= 0xff; z80.f |= sz53p_table[(z80.a)];};
    };
    ops[0x28] = function op_0x28(tempaddr) {
      { z80.f = (z80.b) & 0x01; (z80.b) = ( (z80.b) & 0x80 ) | ( (z80.b) >> 1 ); z80.f |= sz53p_table[(z80.b)];};
    };
    ops[0x29] = function op_0x29(tempaddr) {
      { z80.f = (z80.c) & 0x01; (z80.c) = ( (z80.c) & 0x80 ) | ( (z80.c) >> 1 ); z80.f |= sz53p_table[(z80.c)];};
    };
    ops[0x2a] = function op_0x2a(tempaddr) {
      { z80.f = (z80.d) & 0x01; (z80.d) = ( (z80.d) & 0x80 ) | ( (z80.d) >> 1 ); z80.f |= sz53p_table[(z80.d)];};
    };
    ops[0x2b] = function op_0x2b(tempaddr) {
      { z80.f = (z80.e) & 0x01; (z80.e) = ( (z80.e) & 0x80 ) | ( (z80.e) >> 1 ); z80.f |= sz53p_table[(z80.e)];};
    };
    ops[0x2c] = function op_0x2c(tempaddr) {
      { z80.f = (z80.h) & 0x01; (z80.h) = ( (z80.h) & 0x80 ) | ( (z80.h) >> 1 ); z80.f |= sz53p_table[(z80.h)];};
    };
    ops[0x2d] = function op_0x2d(tempaddr) {
      { z80.f = (z80.l) & 0x01; (z80.l) = ( (z80.l) & 0x80 ) | ( (z80.l) >> 1 ); z80.f |= sz53p_table[(z80.l)];};
    };
    ops[0x2e] = function op_0x2e(tempaddr) {
      {
    var bytetemp = readbyte((z80.l | (z80.h << 8)));
    tstates+=7;
    { z80.f = (bytetemp) & 0x01; (bytetemp) = ( (bytetemp) & 0x80 ) | ( (bytetemp) >> 1 ); z80.f |= sz53p_table[(bytetemp)];};
    writebyte((z80.l | (z80.h << 8)),bytetemp);
      }
    };
    ops[0x2f] = function op_0x2f(tempaddr) {
      { z80.f = (z80.a) & 0x01; (z80.a) = ( (z80.a) & 0x80 ) | ( (z80.a) >> 1 ); z80.f |= sz53p_table[(z80.a)];};
    };
    ops[0x30] = function op_0x30(tempaddr) {
      { z80.f = (z80.b) >> 7; (z80.b) = ( (z80.b) << 1 ) | 0x01; (z80.b) &= 0xff; z80.f |= sz53p_table[(z80.b)];};
    };
    ops[0x31] = function op_0x31(tempaddr) {
      { z80.f = (z80.c) >> 7; (z80.c) = ( (z80.c) << 1 ) | 0x01; (z80.c) &= 0xff; z80.f |= sz53p_table[(z80.c)];};
    };
    ops[0x32] = function op_0x32(tempaddr) {
      { z80.f = (z80.d) >> 7; (z80.d) = ( (z80.d) << 1 ) | 0x01; (z80.d) &= 0xff; z80.f |= sz53p_table[(z80.d)];};
    };
    ops[0x33] = function op_0x33(tempaddr) {
      { z80.f = (z80.e) >> 7; (z80.e) = ( (z80.e) << 1 ) | 0x01; (z80.e) &= 0xff; z80.f |= sz53p_table[(z80.e)];};
    };
    ops[0x34] = function op_0x34(tempaddr) {
      { z80.f = (z80.h) >> 7; (z80.h) = ( (z80.h) << 1 ) | 0x01; (z80.h) &= 0xff; z80.f |= sz53p_table[(z80.h)];};
    };
    ops[0x35] = function op_0x35(tempaddr) {
      { z80.f = (z80.l) >> 7; (z80.l) = ( (z80.l) << 1 ) | 0x01; (z80.l) &= 0xff; z80.f |= sz53p_table[(z80.l)];};
    };
    ops[0x36] = function op_0x36(tempaddr) {
      {
    var bytetemp = readbyte((z80.l | (z80.h << 8)));
    tstates+=7;
    { z80.f = (bytetemp) >> 7; (bytetemp) = ( (bytetemp) << 1 ) | 0x01; (bytetemp) &= 0xff; z80.f |= sz53p_table[(bytetemp)];};
    writebyte((z80.l | (z80.h << 8)),bytetemp);
      }
    };
    ops[0x37] = function op_0x37(tempaddr) {
      { z80.f = (z80.a) >> 7; (z80.a) = ( (z80.a) << 1 ) | 0x01; (z80.a) &= 0xff; z80.f |= sz53p_table[(z80.a)];};
    };
    ops[0x38] = function op_0x38(tempaddr) {
      { z80.f = (z80.b) & 0x01; (z80.b) >>= 1; z80.f |= sz53p_table[(z80.b)];};
    };
    ops[0x39] = function op_0x39(tempaddr) {
      { z80.f = (z80.c) & 0x01; (z80.c) >>= 1; z80.f |= sz53p_table[(z80.c)];};
    };
    ops[0x3a] = function op_0x3a(tempaddr) {
      { z80.f = (z80.d) & 0x01; (z80.d) >>= 1; z80.f |= sz53p_table[(z80.d)];};
    };
    ops[0x3b] = function op_0x3b(tempaddr) {
      { z80.f = (z80.e) & 0x01; (z80.e) >>= 1; z80.f |= sz53p_table[(z80.e)];};
    };
    ops[0x3c] = function op_0x3c(tempaddr) {
      { z80.f = (z80.h) & 0x01; (z80.h) >>= 1; z80.f |= sz53p_table[(z80.h)];};
    };
    ops[0x3d] = function op_0x3d(tempaddr) {
      { z80.f = (z80.l) & 0x01; (z80.l) >>= 1; z80.f |= sz53p_table[(z80.l)];};
    };
    ops[0x3e] = function op_0x3e(tempaddr) {
      {
    var bytetemp = readbyte((z80.l | (z80.h << 8)));
    tstates+=7;
    { z80.f = (bytetemp) & 0x01; (bytetemp) >>= 1; z80.f |= sz53p_table[(bytetemp)];};
    writebyte((z80.l | (z80.h << 8)),bytetemp);
      }
    };
    ops[0x3f] = function op_0x3f(tempaddr) {
      { z80.f = (z80.a) & 0x01; (z80.a) >>= 1; z80.f |= sz53p_table[(z80.a)];};
    };
    ops[0x40] = function op_0x40(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.b & ( 0x08 | 0x20 ) ); if( ! ( (z80.b) & ( 0x01 << (0) ) ) ) z80.f |= 0x04 | 0x40; if( (0) == 7 && (z80.b) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x41] = function op_0x41(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.c & ( 0x08 | 0x20 ) ); if( ! ( (z80.c) & ( 0x01 << (0) ) ) ) z80.f |= 0x04 | 0x40; if( (0) == 7 && (z80.c) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x42] = function op_0x42(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.d & ( 0x08 | 0x20 ) ); if( ! ( (z80.d) & ( 0x01 << (0) ) ) ) z80.f |= 0x04 | 0x40; if( (0) == 7 && (z80.d) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x43] = function op_0x43(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.e & ( 0x08 | 0x20 ) ); if( ! ( (z80.e) & ( 0x01 << (0) ) ) ) z80.f |= 0x04 | 0x40; if( (0) == 7 && (z80.e) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x44] = function op_0x44(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.h & ( 0x08 | 0x20 ) ); if( ! ( (z80.h) & ( 0x01 << (0) ) ) ) z80.f |= 0x04 | 0x40; if( (0) == 7 && (z80.h) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x45] = function op_0x45(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.l & ( 0x08 | 0x20 ) ); if( ! ( (z80.l) & ( 0x01 << (0) ) ) ) z80.f |= 0x04 | 0x40; if( (0) == 7 && (z80.l) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x46] = function op_0x46(tempaddr) {
      {
    var bytetemp = readbyte( (z80.l | (z80.h << 8)) );
    tstates+=4;
    { z80.f = ( z80.f & 0x01 ) | 0x10 | ( bytetemp & ( 0x08 | 0x20 ) ); if( ! ( (bytetemp) & ( 0x01 << (0) ) ) ) z80.f |= 0x04 | 0x40; if( (0) == 7 && (bytetemp) & 0x80 ) z80.f |= 0x80; };
      }
    };
    ops[0x47] = function op_0x47(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.a & ( 0x08 | 0x20 ) ); if( ! ( (z80.a) & ( 0x01 << (0) ) ) ) z80.f |= 0x04 | 0x40; if( (0) == 7 && (z80.a) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x48] = function op_0x48(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.b & ( 0x08 | 0x20 ) ); if( ! ( (z80.b) & ( 0x01 << (1) ) ) ) z80.f |= 0x04 | 0x40; if( (1) == 7 && (z80.b) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x49] = function op_0x49(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.c & ( 0x08 | 0x20 ) ); if( ! ( (z80.c) & ( 0x01 << (1) ) ) ) z80.f |= 0x04 | 0x40; if( (1) == 7 && (z80.c) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x4a] = function op_0x4a(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.d & ( 0x08 | 0x20 ) ); if( ! ( (z80.d) & ( 0x01 << (1) ) ) ) z80.f |= 0x04 | 0x40; if( (1) == 7 && (z80.d) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x4b] = function op_0x4b(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.e & ( 0x08 | 0x20 ) ); if( ! ( (z80.e) & ( 0x01 << (1) ) ) ) z80.f |= 0x04 | 0x40; if( (1) == 7 && (z80.e) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x4c] = function op_0x4c(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.h & ( 0x08 | 0x20 ) ); if( ! ( (z80.h) & ( 0x01 << (1) ) ) ) z80.f |= 0x04 | 0x40; if( (1) == 7 && (z80.h) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x4d] = function op_0x4d(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.l & ( 0x08 | 0x20 ) ); if( ! ( (z80.l) & ( 0x01 << (1) ) ) ) z80.f |= 0x04 | 0x40; if( (1) == 7 && (z80.l) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x4e] = function op_0x4e(tempaddr) {
      {
    var bytetemp = readbyte( (z80.l | (z80.h << 8)) );
    tstates+=4;
    { z80.f = ( z80.f & 0x01 ) | 0x10 | ( bytetemp & ( 0x08 | 0x20 ) ); if( ! ( (bytetemp) & ( 0x01 << (1) ) ) ) z80.f |= 0x04 | 0x40; if( (1) == 7 && (bytetemp) & 0x80 ) z80.f |= 0x80; };
      }
    };
    ops[0x4f] = function op_0x4f(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.a & ( 0x08 | 0x20 ) ); if( ! ( (z80.a) & ( 0x01 << (1) ) ) ) z80.f |= 0x04 | 0x40; if( (1) == 7 && (z80.a) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x50] = function op_0x50(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.b & ( 0x08 | 0x20 ) ); if( ! ( (z80.b) & ( 0x01 << (2) ) ) ) z80.f |= 0x04 | 0x40; if( (2) == 7 && (z80.b) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x51] = function op_0x51(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.c & ( 0x08 | 0x20 ) ); if( ! ( (z80.c) & ( 0x01 << (2) ) ) ) z80.f |= 0x04 | 0x40; if( (2) == 7 && (z80.c) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x52] = function op_0x52(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.d & ( 0x08 | 0x20 ) ); if( ! ( (z80.d) & ( 0x01 << (2) ) ) ) z80.f |= 0x04 | 0x40; if( (2) == 7 && (z80.d) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x53] = function op_0x53(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.e & ( 0x08 | 0x20 ) ); if( ! ( (z80.e) & ( 0x01 << (2) ) ) ) z80.f |= 0x04 | 0x40; if( (2) == 7 && (z80.e) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x54] = function op_0x54(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.h & ( 0x08 | 0x20 ) ); if( ! ( (z80.h) & ( 0x01 << (2) ) ) ) z80.f |= 0x04 | 0x40; if( (2) == 7 && (z80.h) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x55] = function op_0x55(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.l & ( 0x08 | 0x20 ) ); if( ! ( (z80.l) & ( 0x01 << (2) ) ) ) z80.f |= 0x04 | 0x40; if( (2) == 7 && (z80.l) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x56] = function op_0x56(tempaddr) {
      {
    var bytetemp = readbyte( (z80.l | (z80.h << 8)) );
    tstates+=4;
    { z80.f = ( z80.f & 0x01 ) | 0x10 | ( bytetemp & ( 0x08 | 0x20 ) ); if( ! ( (bytetemp) & ( 0x01 << (2) ) ) ) z80.f |= 0x04 | 0x40; if( (2) == 7 && (bytetemp) & 0x80 ) z80.f |= 0x80; };
      }
    };
    ops[0x57] = function op_0x57(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.a & ( 0x08 | 0x20 ) ); if( ! ( (z80.a) & ( 0x01 << (2) ) ) ) z80.f |= 0x04 | 0x40; if( (2) == 7 && (z80.a) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x58] = function op_0x58(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.b & ( 0x08 | 0x20 ) ); if( ! ( (z80.b) & ( 0x01 << (3) ) ) ) z80.f |= 0x04 | 0x40; if( (3) == 7 && (z80.b) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x59] = function op_0x59(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.c & ( 0x08 | 0x20 ) ); if( ! ( (z80.c) & ( 0x01 << (3) ) ) ) z80.f |= 0x04 | 0x40; if( (3) == 7 && (z80.c) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x5a] = function op_0x5a(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.d & ( 0x08 | 0x20 ) ); if( ! ( (z80.d) & ( 0x01 << (3) ) ) ) z80.f |= 0x04 | 0x40; if( (3) == 7 && (z80.d) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x5b] = function op_0x5b(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.e & ( 0x08 | 0x20 ) ); if( ! ( (z80.e) & ( 0x01 << (3) ) ) ) z80.f |= 0x04 | 0x40; if( (3) == 7 && (z80.e) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x5c] = function op_0x5c(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.h & ( 0x08 | 0x20 ) ); if( ! ( (z80.h) & ( 0x01 << (3) ) ) ) z80.f |= 0x04 | 0x40; if( (3) == 7 && (z80.h) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x5d] = function op_0x5d(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.l & ( 0x08 | 0x20 ) ); if( ! ( (z80.l) & ( 0x01 << (3) ) ) ) z80.f |= 0x04 | 0x40; if( (3) == 7 && (z80.l) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x5e] = function op_0x5e(tempaddr) {
      {
    var bytetemp = readbyte( (z80.l | (z80.h << 8)) );
    tstates+=4;
    { z80.f = ( z80.f & 0x01 ) | 0x10 | ( bytetemp & ( 0x08 | 0x20 ) ); if( ! ( (bytetemp) & ( 0x01 << (3) ) ) ) z80.f |= 0x04 | 0x40; if( (3) == 7 && (bytetemp) & 0x80 ) z80.f |= 0x80; };
      }
    };
    ops[0x5f] = function op_0x5f(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.a & ( 0x08 | 0x20 ) ); if( ! ( (z80.a) & ( 0x01 << (3) ) ) ) z80.f |= 0x04 | 0x40; if( (3) == 7 && (z80.a) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x60] = function op_0x60(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.b & ( 0x08 | 0x20 ) ); if( ! ( (z80.b) & ( 0x01 << (4) ) ) ) z80.f |= 0x04 | 0x40; if( (4) == 7 && (z80.b) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x61] = function op_0x61(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.c & ( 0x08 | 0x20 ) ); if( ! ( (z80.c) & ( 0x01 << (4) ) ) ) z80.f |= 0x04 | 0x40; if( (4) == 7 && (z80.c) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x62] = function op_0x62(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.d & ( 0x08 | 0x20 ) ); if( ! ( (z80.d) & ( 0x01 << (4) ) ) ) z80.f |= 0x04 | 0x40; if( (4) == 7 && (z80.d) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x63] = function op_0x63(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.e & ( 0x08 | 0x20 ) ); if( ! ( (z80.e) & ( 0x01 << (4) ) ) ) z80.f |= 0x04 | 0x40; if( (4) == 7 && (z80.e) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x64] = function op_0x64(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.h & ( 0x08 | 0x20 ) ); if( ! ( (z80.h) & ( 0x01 << (4) ) ) ) z80.f |= 0x04 | 0x40; if( (4) == 7 && (z80.h) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x65] = function op_0x65(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.l & ( 0x08 | 0x20 ) ); if( ! ( (z80.l) & ( 0x01 << (4) ) ) ) z80.f |= 0x04 | 0x40; if( (4) == 7 && (z80.l) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x66] = function op_0x66(tempaddr) {
      {
    var bytetemp = readbyte( (z80.l | (z80.h << 8)) );
    tstates+=4;
    { z80.f = ( z80.f & 0x01 ) | 0x10 | ( bytetemp & ( 0x08 | 0x20 ) ); if( ! ( (bytetemp) & ( 0x01 << (4) ) ) ) z80.f |= 0x04 | 0x40; if( (4) == 7 && (bytetemp) & 0x80 ) z80.f |= 0x80; };
      }
    };
    ops[0x67] = function op_0x67(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.a & ( 0x08 | 0x20 ) ); if( ! ( (z80.a) & ( 0x01 << (4) ) ) ) z80.f |= 0x04 | 0x40; if( (4) == 7 && (z80.a) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x68] = function op_0x68(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.b & ( 0x08 | 0x20 ) ); if( ! ( (z80.b) & ( 0x01 << (5) ) ) ) z80.f |= 0x04 | 0x40; if( (5) == 7 && (z80.b) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x69] = function op_0x69(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.c & ( 0x08 | 0x20 ) ); if( ! ( (z80.c) & ( 0x01 << (5) ) ) ) z80.f |= 0x04 | 0x40; if( (5) == 7 && (z80.c) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x6a] = function op_0x6a(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.d & ( 0x08 | 0x20 ) ); if( ! ( (z80.d) & ( 0x01 << (5) ) ) ) z80.f |= 0x04 | 0x40; if( (5) == 7 && (z80.d) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x6b] = function op_0x6b(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.e & ( 0x08 | 0x20 ) ); if( ! ( (z80.e) & ( 0x01 << (5) ) ) ) z80.f |= 0x04 | 0x40; if( (5) == 7 && (z80.e) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x6c] = function op_0x6c(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.h & ( 0x08 | 0x20 ) ); if( ! ( (z80.h) & ( 0x01 << (5) ) ) ) z80.f |= 0x04 | 0x40; if( (5) == 7 && (z80.h) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x6d] = function op_0x6d(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.l & ( 0x08 | 0x20 ) ); if( ! ( (z80.l) & ( 0x01 << (5) ) ) ) z80.f |= 0x04 | 0x40; if( (5) == 7 && (z80.l) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x6e] = function op_0x6e(tempaddr) {
      {
    var bytetemp = readbyte( (z80.l | (z80.h << 8)) );
    tstates+=4;
    { z80.f = ( z80.f & 0x01 ) | 0x10 | ( bytetemp & ( 0x08 | 0x20 ) ); if( ! ( (bytetemp) & ( 0x01 << (5) ) ) ) z80.f |= 0x04 | 0x40; if( (5) == 7 && (bytetemp) & 0x80 ) z80.f |= 0x80; };
      }
    };
    ops[0x6f] = function op_0x6f(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.a & ( 0x08 | 0x20 ) ); if( ! ( (z80.a) & ( 0x01 << (5) ) ) ) z80.f |= 0x04 | 0x40; if( (5) == 7 && (z80.a) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x70] = function op_0x70(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.b & ( 0x08 | 0x20 ) ); if( ! ( (z80.b) & ( 0x01 << (6) ) ) ) z80.f |= 0x04 | 0x40; if( (6) == 7 && (z80.b) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x71] = function op_0x71(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.c & ( 0x08 | 0x20 ) ); if( ! ( (z80.c) & ( 0x01 << (6) ) ) ) z80.f |= 0x04 | 0x40; if( (6) == 7 && (z80.c) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x72] = function op_0x72(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.d & ( 0x08 | 0x20 ) ); if( ! ( (z80.d) & ( 0x01 << (6) ) ) ) z80.f |= 0x04 | 0x40; if( (6) == 7 && (z80.d) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x73] = function op_0x73(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.e & ( 0x08 | 0x20 ) ); if( ! ( (z80.e) & ( 0x01 << (6) ) ) ) z80.f |= 0x04 | 0x40; if( (6) == 7 && (z80.e) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x74] = function op_0x74(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.h & ( 0x08 | 0x20 ) ); if( ! ( (z80.h) & ( 0x01 << (6) ) ) ) z80.f |= 0x04 | 0x40; if( (6) == 7 && (z80.h) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x75] = function op_0x75(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.l & ( 0x08 | 0x20 ) ); if( ! ( (z80.l) & ( 0x01 << (6) ) ) ) z80.f |= 0x04 | 0x40; if( (6) == 7 && (z80.l) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x76] = function op_0x76(tempaddr) {
      {
    var bytetemp = readbyte( (z80.l | (z80.h << 8)) );
    tstates+=4;
    { z80.f = ( z80.f & 0x01 ) | 0x10 | ( bytetemp & ( 0x08 | 0x20 ) ); if( ! ( (bytetemp) & ( 0x01 << (6) ) ) ) z80.f |= 0x04 | 0x40; if( (6) == 7 && (bytetemp) & 0x80 ) z80.f |= 0x80; };
      }
    };
    ops[0x77] = function op_0x77(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.a & ( 0x08 | 0x20 ) ); if( ! ( (z80.a) & ( 0x01 << (6) ) ) ) z80.f |= 0x04 | 0x40; if( (6) == 7 && (z80.a) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x78] = function op_0x78(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.b & ( 0x08 | 0x20 ) ); if( ! ( (z80.b) & ( 0x01 << (7) ) ) ) z80.f |= 0x04 | 0x40; if( (7) == 7 && (z80.b) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x79] = function op_0x79(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.c & ( 0x08 | 0x20 ) ); if( ! ( (z80.c) & ( 0x01 << (7) ) ) ) z80.f |= 0x04 | 0x40; if( (7) == 7 && (z80.c) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x7a] = function op_0x7a(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.d & ( 0x08 | 0x20 ) ); if( ! ( (z80.d) & ( 0x01 << (7) ) ) ) z80.f |= 0x04 | 0x40; if( (7) == 7 && (z80.d) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x7b] = function op_0x7b(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.e & ( 0x08 | 0x20 ) ); if( ! ( (z80.e) & ( 0x01 << (7) ) ) ) z80.f |= 0x04 | 0x40; if( (7) == 7 && (z80.e) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x7c] = function op_0x7c(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.h & ( 0x08 | 0x20 ) ); if( ! ( (z80.h) & ( 0x01 << (7) ) ) ) z80.f |= 0x04 | 0x40; if( (7) == 7 && (z80.h) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x7d] = function op_0x7d(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.l & ( 0x08 | 0x20 ) ); if( ! ( (z80.l) & ( 0x01 << (7) ) ) ) z80.f |= 0x04 | 0x40; if( (7) == 7 && (z80.l) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x7e] = function op_0x7e(tempaddr) {
      {
    var bytetemp = readbyte( (z80.l | (z80.h << 8)) );
    tstates+=4;
    { z80.f = ( z80.f & 0x01 ) | 0x10 | ( bytetemp & ( 0x08 | 0x20 ) ); if( ! ( (bytetemp) & ( 0x01 << (7) ) ) ) z80.f |= 0x04 | 0x40; if( (7) == 7 && (bytetemp) & 0x80 ) z80.f |= 0x80; };
      }
    };
    ops[0x7f] = function op_0x7f(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | 0x10 | ( z80.a & ( 0x08 | 0x20 ) ); if( ! ( (z80.a) & ( 0x01 << (7) ) ) ) z80.f |= 0x04 | 0x40; if( (7) == 7 && (z80.a) & 0x80 ) z80.f |= 0x80; };
    };
    ops[0x80] = function op_0x80(tempaddr) {
      z80.b &= 0xfe;
    };
    ops[0x81] = function op_0x81(tempaddr) {
      z80.c &= 0xfe;
    };
    ops[0x82] = function op_0x82(tempaddr) {
      z80.d &= 0xfe;
    };
    ops[0x83] = function op_0x83(tempaddr) {
      z80.e &= 0xfe;
    };
    ops[0x84] = function op_0x84(tempaddr) {
      z80.h &= 0xfe;
    };
    ops[0x85] = function op_0x85(tempaddr) {
      z80.l &= 0xfe;
    };
    ops[0x86] = function op_0x86(tempaddr) {
      tstates+=7;
      writebyte((z80.l | (z80.h << 8)), readbyte((z80.l | (z80.h << 8))) & 0xfe);
    };
    ops[0x87] = function op_0x87(tempaddr) {
      z80.a &= 0xfe;
    };
    ops[0x88] = function op_0x88(tempaddr) {
      z80.b &= 0xfd;
    };
    ops[0x89] = function op_0x89(tempaddr) {
      z80.c &= 0xfd;
    };
    ops[0x8a] = function op_0x8a(tempaddr) {
      z80.d &= 0xfd;
    };
    ops[0x8b] = function op_0x8b(tempaddr) {
      z80.e &= 0xfd;
    };
    ops[0x8c] = function op_0x8c(tempaddr) {
      z80.h &= 0xfd;
    };
    ops[0x8d] = function op_0x8d(tempaddr) {
      z80.l &= 0xfd;
    };
    ops[0x8e] = function op_0x8e(tempaddr) {
      tstates+=7;
      writebyte((z80.l | (z80.h << 8)), readbyte((z80.l | (z80.h << 8))) & 0xfd);
    };
    ops[0x8f] = function op_0x8f(tempaddr) {
      z80.a &= 0xfd;
    };
    ops[0x90] = function op_0x90(tempaddr) {
      z80.b &= 0xfb;
    };
    ops[0x91] = function op_0x91(tempaddr) {
      z80.c &= 0xfb;
    };
    ops[0x92] = function op_0x92(tempaddr) {
      z80.d &= 0xfb;
    };
    ops[0x93] = function op_0x93(tempaddr) {
      z80.e &= 0xfb;
    };
    ops[0x94] = function op_0x94(tempaddr) {
      z80.h &= 0xfb;
    };
    ops[0x95] = function op_0x95(tempaddr) {
      z80.l &= 0xfb;
    };
    ops[0x96] = function op_0x96(tempaddr) {
      tstates+=7;
      writebyte((z80.l | (z80.h << 8)), readbyte((z80.l | (z80.h << 8))) & 0xfb);
    };
    ops[0x97] = function op_0x97(tempaddr) {
      z80.a &= 0xfb;
    };
    ops[0x98] = function op_0x98(tempaddr) {
      z80.b &= 0xf7;
    };
    ops[0x99] = function op_0x99(tempaddr) {
      z80.c &= 0xf7;
    };
    ops[0x9a] = function op_0x9a(tempaddr) {
      z80.d &= 0xf7;
    };
    ops[0x9b] = function op_0x9b(tempaddr) {
      z80.e &= 0xf7;
    };
    ops[0x9c] = function op_0x9c(tempaddr) {
      z80.h &= 0xf7;
    };
    ops[0x9d] = function op_0x9d(tempaddr) {
      z80.l &= 0xf7;
    };
    ops[0x9e] = function op_0x9e(tempaddr) {
      tstates+=7;
      writebyte((z80.l | (z80.h << 8)), readbyte((z80.l | (z80.h << 8))) & 0xf7);
    };
    ops[0x9f] = function op_0x9f(tempaddr) {
      z80.a &= 0xf7;
    };
    ops[0xa0] = function op_0xa0(tempaddr) {
      z80.b &= 0xef;
    };
    ops[0xa1] = function op_0xa1(tempaddr) {
      z80.c &= 0xef;
    };
    ops[0xa2] = function op_0xa2(tempaddr) {
      z80.d &= 0xef;
    };
    ops[0xa3] = function op_0xa3(tempaddr) {
      z80.e &= 0xef;
    };
    ops[0xa4] = function op_0xa4(tempaddr) {
      z80.h &= 0xef;
    };
    ops[0xa5] = function op_0xa5(tempaddr) {
      z80.l &= 0xef;
    };
    ops[0xa6] = function op_0xa6(tempaddr) {
      tstates+=7;
      writebyte((z80.l | (z80.h << 8)), readbyte((z80.l | (z80.h << 8))) & 0xef);
    };
    ops[0xa7] = function op_0xa7(tempaddr) {
      z80.a &= 0xef;
    };
    ops[0xa8] = function op_0xa8(tempaddr) {
      z80.b &= 0xdf;
    };
    ops[0xa9] = function op_0xa9(tempaddr) {
      z80.c &= 0xdf;
    };
    ops[0xaa] = function op_0xaa(tempaddr) {
      z80.d &= 0xdf;
    };
    ops[0xab] = function op_0xab(tempaddr) {
      z80.e &= 0xdf;
    };
    ops[0xac] = function op_0xac(tempaddr) {
      z80.h &= 0xdf;
    };
    ops[0xad] = function op_0xad(tempaddr) {
      z80.l &= 0xdf;
    };
    ops[0xae] = function op_0xae(tempaddr) {
      tstates+=7;
      writebyte((z80.l | (z80.h << 8)), readbyte((z80.l | (z80.h << 8))) & 0xdf);
    };
    ops[0xaf] = function op_0xaf(tempaddr) {
      z80.a &= 0xdf;
    };
    ops[0xb0] = function op_0xb0(tempaddr) {
      z80.b &= 0xbf;
    };
    ops[0xb1] = function op_0xb1(tempaddr) {
      z80.c &= 0xbf;
    };
    ops[0xb2] = function op_0xb2(tempaddr) {
      z80.d &= 0xbf;
    };
    ops[0xb3] = function op_0xb3(tempaddr) {
      z80.e &= 0xbf;
    };
    ops[0xb4] = function op_0xb4(tempaddr) {
      z80.h &= 0xbf;
    };
    ops[0xb5] = function op_0xb5(tempaddr) {
      z80.l &= 0xbf;
    };
    ops[0xb6] = function op_0xb6(tempaddr) {
      tstates+=7;
      writebyte((z80.l | (z80.h << 8)), readbyte((z80.l | (z80.h << 8))) & 0xbf);
    };
    ops[0xb7] = function op_0xb7(tempaddr) {
      z80.a &= 0xbf;
    };
    ops[0xb8] = function op_0xb8(tempaddr) {
      z80.b &= 0x7f;
    };
    ops[0xb9] = function op_0xb9(tempaddr) {
      z80.c &= 0x7f;
    };
    ops[0xba] = function op_0xba(tempaddr) {
      z80.d &= 0x7f;
    };
    ops[0xbb] = function op_0xbb(tempaddr) {
      z80.e &= 0x7f;
    };
    ops[0xbc] = function op_0xbc(tempaddr) {
      z80.h &= 0x7f;
    };
    ops[0xbd] = function op_0xbd(tempaddr) {
      z80.l &= 0x7f;
    };
    ops[0xbe] = function op_0xbe(tempaddr) {
      tstates+=7;
      writebyte((z80.l | (z80.h << 8)), readbyte((z80.l | (z80.h << 8))) & 0x7f);
    };
    ops[0xbf] = function op_0xbf(tempaddr) {
      z80.a &= 0x7f;
    };
    ops[0xc0] = function op_0xc0(tempaddr) {
      z80.b |= 0x01;
    };
    ops[0xc1] = function op_0xc1(tempaddr) {
      z80.c |= 0x01;
    };
    ops[0xc2] = function op_0xc2(tempaddr) {
      z80.d |= 0x01;
    };
    ops[0xc3] = function op_0xc3(tempaddr) {
      z80.e |= 0x01;
    };
    ops[0xc4] = function op_0xc4(tempaddr) {
      z80.h |= 0x01;
    };
    ops[0xc5] = function op_0xc5(tempaddr) {
      z80.l |= 0x01;
    };
    ops[0xc6] = function op_0xc6(tempaddr) {
      tstates+=7;
      writebyte((z80.l | (z80.h << 8)), readbyte((z80.l | (z80.h << 8))) | 0x01);
    };
    ops[0xc7] = function op_0xc7(tempaddr) {
      z80.a |= 0x01;
    };
    ops[0xc8] = function op_0xc8(tempaddr) {
      z80.b |= 0x02;
    };
    ops[0xc9] = function op_0xc9(tempaddr) {
      z80.c |= 0x02;
    };
    ops[0xca] = function op_0xca(tempaddr) {
      z80.d |= 0x02;
    };
    ops[0xcb] = function op_0xcb(tempaddr) {
      z80.e |= 0x02;
    };
    ops[0xcc] = function op_0xcc(tempaddr) {
      z80.h |= 0x02;
    };
    ops[0xcd] = function op_0xcd(tempaddr) {
      z80.l |= 0x02;
    };
    ops[0xce] = function op_0xce(tempaddr) {
      tstates+=7;
      writebyte((z80.l | (z80.h << 8)), readbyte((z80.l | (z80.h << 8))) | 0x02);
    };
    ops[0xcf] = function op_0xcf(tempaddr) {
      z80.a |= 0x02;
    };
    ops[0xd0] = function op_0xd0(tempaddr) {
      z80.b |= 0x04;
    };
    ops[0xd1] = function op_0xd1(tempaddr) {
      z80.c |= 0x04;
    };
    ops[0xd2] = function op_0xd2(tempaddr) {
      z80.d |= 0x04;
    };
    ops[0xd3] = function op_0xd3(tempaddr) {
      z80.e |= 0x04;
    };
    ops[0xd4] = function op_0xd4(tempaddr) {
      z80.h |= 0x04;
    };
    ops[0xd5] = function op_0xd5(tempaddr) {
      z80.l |= 0x04;
    };
    ops[0xd6] = function op_0xd6(tempaddr) {
      tstates+=7;
      writebyte((z80.l | (z80.h << 8)), readbyte((z80.l | (z80.h << 8))) | 0x04);
    };
    ops[0xd7] = function op_0xd7(tempaddr) {
      z80.a |= 0x04;
    };
    ops[0xd8] = function op_0xd8(tempaddr) {
      z80.b |= 0x08;
    };
    ops[0xd9] = function op_0xd9(tempaddr) {
      z80.c |= 0x08;
    };
    ops[0xda] = function op_0xda(tempaddr) {
      z80.d |= 0x08;
    };
    ops[0xdb] = function op_0xdb(tempaddr) {
      z80.e |= 0x08;
    };
    ops[0xdc] = function op_0xdc(tempaddr) {
      z80.h |= 0x08;
    };
    ops[0xdd] = function op_0xdd(tempaddr) {
      z80.l |= 0x08;
    };
    ops[0xde] = function op_0xde(tempaddr) {
      tstates+=7;
      writebyte((z80.l | (z80.h << 8)), readbyte((z80.l | (z80.h << 8))) | 0x08);
    };
    ops[0xdf] = function op_0xdf(tempaddr) {
      z80.a |= 0x08;
    };
    ops[0xe0] = function op_0xe0(tempaddr) {
      z80.b |= 0x10;
    };
    ops[0xe1] = function op_0xe1(tempaddr) {
      z80.c |= 0x10;
    };
    ops[0xe2] = function op_0xe2(tempaddr) {
      z80.d |= 0x10;
    };
    ops[0xe3] = function op_0xe3(tempaddr) {
      z80.e |= 0x10;
    };
    ops[0xe4] = function op_0xe4(tempaddr) {
      z80.h |= 0x10;
    };
    ops[0xe5] = function op_0xe5(tempaddr) {
      z80.l |= 0x10;
    };
    ops[0xe6] = function op_0xe6(tempaddr) {
      tstates+=7;
      writebyte((z80.l | (z80.h << 8)), readbyte((z80.l | (z80.h << 8))) | 0x10);
    };
    ops[0xe7] = function op_0xe7(tempaddr) {
      z80.a |= 0x10;
    };
    ops[0xe8] = function op_0xe8(tempaddr) {
      z80.b |= 0x20;
    };
    ops[0xe9] = function op_0xe9(tempaddr) {
      z80.c |= 0x20;
    };
    ops[0xea] = function op_0xea(tempaddr) {
      z80.d |= 0x20;
    };
    ops[0xeb] = function op_0xeb(tempaddr) {
      z80.e |= 0x20;
    };
    ops[0xec] = function op_0xec(tempaddr) {
      z80.h |= 0x20;
    };
    ops[0xed] = function op_0xed(tempaddr) {
      z80.l |= 0x20;
    };
    ops[0xee] = function op_0xee(tempaddr) {
      tstates+=7;
      writebyte((z80.l | (z80.h << 8)), readbyte((z80.l | (z80.h << 8))) | 0x20);
    };
    ops[0xef] = function op_0xef(tempaddr) {
      z80.a |= 0x20;
    };
    ops[0xf0] = function op_0xf0(tempaddr) {
      z80.b |= 0x40;
    };
    ops[0xf1] = function op_0xf1(tempaddr) {
      z80.c |= 0x40;
    };
    ops[0xf2] = function op_0xf2(tempaddr) {
      z80.d |= 0x40;
    };
    ops[0xf3] = function op_0xf3(tempaddr) {
      z80.e |= 0x40;
    };
    ops[0xf4] = function op_0xf4(tempaddr) {
      z80.h |= 0x40;
    };
    ops[0xf5] = function op_0xf5(tempaddr) {
      z80.l |= 0x40;
    };
    ops[0xf6] = function op_0xf6(tempaddr) {
      tstates+=7;
      writebyte((z80.l | (z80.h << 8)), readbyte((z80.l | (z80.h << 8))) | 0x40);
    };
    ops[0xf7] = function op_0xf7(tempaddr) {
      z80.a |= 0x40;
    };
    ops[0xf8] = function op_0xf8(tempaddr) {
      z80.b |= 0x80;
    };
    ops[0xf9] = function op_0xf9(tempaddr) {
      z80.c |= 0x80;
    };
    ops[0xfa] = function op_0xfa(tempaddr) {
      z80.d |= 0x80;
    };
    ops[0xfb] = function op_0xfb(tempaddr) {
      z80.e |= 0x80;
    };
    ops[0xfc] = function op_0xfc(tempaddr) {
      z80.h |= 0x80;
    };
    ops[0xfd] = function op_0xfd(tempaddr) {
      z80.l |= 0x80;
    };
    ops[0xfe] = function op_0xfe(tempaddr) {
      tstates+=7;
      writebyte((z80.l | (z80.h << 8)), readbyte((z80.l | (z80.h << 8))) | 0x80);
    };
    ops[0xff] = function op_0xff(tempaddr) {
      z80.a |= 0x80;
    };
    ops[256] = function() {};
    z80_defaults(ops);
    return ops;
}();
function z80_cbxx(opcode) { z80_cb_ops[opcode](); }
var z80_dd_ops = function z80_setup_dd_ops() {
    var ops = [];
    ops[0x09] = function op_0x09(tempaddr) {
      { var add16temp = ((z80.ixl | (z80.ixh << 8))) + ((z80.c | (z80.b << 8))); var lookup = ( ( ((z80.ixl | (z80.ixh << 8))) & 0x0800 ) >> 11 ) | ( ( ((z80.c | (z80.b << 8))) & 0x0800 ) >> 10 ) | ( ( add16temp & 0x0800 ) >> 9 ); tstates += 7; (z80.ixh) = (add16temp >> 8) & 0xff; (z80.ixl) = add16temp & 0xff; z80.f = ( z80.f & ( 0x04 | 0x40 | 0x80 ) ) | ( add16temp & 0x10000 ? 0x01 : 0 )| ( ( add16temp >> 8 ) & ( 0x08 | 0x20 ) ) | halfcarry_add_table[lookup];};
    };
    ops[0x19] = function op_0x19(tempaddr) {
      { var add16temp = ((z80.ixl | (z80.ixh << 8))) + ((z80.e | (z80.d << 8))); var lookup = ( ( ((z80.ixl | (z80.ixh << 8))) & 0x0800 ) >> 11 ) | ( ( ((z80.e | (z80.d << 8))) & 0x0800 ) >> 10 ) | ( ( add16temp & 0x0800 ) >> 9 ); tstates += 7; (z80.ixh) = (add16temp >> 8) & 0xff; (z80.ixl) = add16temp & 0xff; z80.f = ( z80.f & ( 0x04 | 0x40 | 0x80 ) ) | ( add16temp & 0x10000 ? 0x01 : 0 )| ( ( add16temp >> 8 ) & ( 0x08 | 0x20 ) ) | halfcarry_add_table[lookup];};
    };
    ops[0x21] = function op_0x21(tempaddr) {
      tstates+=6;
      z80.ixl=readbyte(z80.pc++);
      z80.pc &= 0xffff;
      z80.ixh=readbyte(z80.pc++);
      z80.pc &= 0xffff;
    };
    ops[0x22] = function op_0x22(tempaddr) {
      { var ldtemp; tstates+=12; ldtemp=readbyte(z80.pc++); z80.pc &= 0xffff; ldtemp|=readbyte(z80.pc++) << 8; z80.pc &= 0xffff; writebyte(ldtemp++,(z80.ixl)); ldtemp &= 0xffff; writebyte(ldtemp,(z80.ixh));};
    };
    ops[0x23] = function op_0x23(tempaddr) {
      tstates += 2;
      var wordtemp = ((z80.ixl | (z80.ixh << 8)) + 1) & 0xffff;
      z80.ixh = wordtemp >> 8;
      z80.ixl = wordtemp & 0xff;
    };
    ops[0x24] = function op_0x24(tempaddr) {
      { (z80.ixh) = ((z80.ixh) + 1) & 0xff; z80.f = ( z80.f & 0x01 ) | ( (z80.ixh)==0x80 ? 0x04 : 0 ) | ( (z80.ixh)&0x0f ? 0 : 0x10 ) | sz53_table[(z80.ixh)];};
    };
    ops[0x25] = function op_0x25(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | ( (z80.ixh)&0x0f ? 0 : 0x10 ) | 0x02; (z80.ixh) = ((z80.ixh) - 1) & 0xff; z80.f |= ( (z80.ixh)==0x7f ? 0x04 : 0 ) | sz53_table[z80.ixh];};
    };
    ops[0x26] = function op_0x26(tempaddr) {
      tstates+=3;
      z80.ixh=readbyte(z80.pc++); z80.pc &= 0xffff;
    };
    ops[0x29] = function op_0x29(tempaddr) {
      { var add16temp = ((z80.ixl | (z80.ixh << 8))) + ((z80.ixl | (z80.ixh << 8))); var lookup = ( ( ((z80.ixl | (z80.ixh << 8))) & 0x0800 ) >> 11 ) | ( ( ((z80.ixl | (z80.ixh << 8))) & 0x0800 ) >> 10 ) | ( ( add16temp & 0x0800 ) >> 9 ); tstates += 7; (z80.ixh) = (add16temp >> 8) & 0xff; (z80.ixl) = add16temp & 0xff; z80.f = ( z80.f & ( 0x04 | 0x40 | 0x80 ) ) | ( add16temp & 0x10000 ? 0x01 : 0 )| ( ( add16temp >> 8 ) & ( 0x08 | 0x20 ) ) | halfcarry_add_table[lookup];};
    };
    ops[0x2a] = function op_0x2a(tempaddr) {
      { var ldtemp; tstates+=12; ldtemp=readbyte(z80.pc++); z80.pc &= 0xffff; ldtemp|=readbyte(z80.pc++) << 8; z80.pc &= 0xffff; (z80.ixl)=readbyte(ldtemp++); ldtemp &= 0xffff; (z80.ixh)=readbyte(ldtemp);};
    };
    ops[0x2b] = function op_0x2b(tempaddr) {
      tstates += 2;
      var wordtemp = ((z80.ixl | (z80.ixh << 8)) - 1) & 0xffff;
      z80.ixh = wordtemp >> 8;
      z80.ixl = wordtemp & 0xff;
    };
    ops[0x2c] = function op_0x2c(tempaddr) {
      { (z80.ixl) = ((z80.ixl) + 1) & 0xff; z80.f = ( z80.f & 0x01 ) | ( (z80.ixl)==0x80 ? 0x04 : 0 ) | ( (z80.ixl)&0x0f ? 0 : 0x10 ) | sz53_table[(z80.ixl)];};
    };
    ops[0x2d] = function op_0x2d(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | ( (z80.ixl)&0x0f ? 0 : 0x10 ) | 0x02; (z80.ixl) = ((z80.ixl) - 1) & 0xff; z80.f |= ( (z80.ixl)==0x7f ? 0x04 : 0 ) | sz53_table[z80.ixl];};
    };
    ops[0x2e] = function op_0x2e(tempaddr) {
      tstates+=3;
      z80.ixl=readbyte(z80.pc++); z80.pc &= 0xffff;
    };
    ops[0x34] = function op_0x34(tempaddr) {
      tstates += 15;
      {
    var wordtemp =
        ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff;
    z80.pc &= 0xffff;
    var bytetemp = readbyte( wordtemp );
    { (bytetemp) = ((bytetemp) + 1) & 0xff; z80.f = ( z80.f & 0x01 ) | ( (bytetemp)==0x80 ? 0x04 : 0 ) | ( (bytetemp)&0x0f ? 0 : 0x10 ) | sz53_table[(bytetemp)];};
    writebyte(wordtemp,bytetemp);
      }
    };
    ops[0x35] = function op_0x35(tempaddr) {
      tstates += 15;
      {
    var wordtemp =
        ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff;
    z80.pc &= 0xffff;
    var bytetemp = readbyte( wordtemp );
    { z80.f = ( z80.f & 0x01 ) | ( (bytetemp)&0x0f ? 0 : 0x10 ) | 0x02; (bytetemp) = ((bytetemp) - 1) & 0xff; z80.f |= ( (bytetemp)==0x7f ? 0x04 : 0 ) | sz53_table[bytetemp];};
    writebyte(wordtemp,bytetemp);
      }
    };
    ops[0x36] = function op_0x36(tempaddr) {
      tstates += 11;
      {
    var wordtemp =
        ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff;
    z80.pc &= 0xffff;
    writebyte(wordtemp,readbyte(z80.pc++));
    z80.pc &= 0xffff;
      }
    };
    ops[0x39] = function op_0x39(tempaddr) {
      { var add16temp = ((z80.ixl | (z80.ixh << 8))) + (z80.sp); var lookup = ( ( ((z80.ixl | (z80.ixh << 8))) & 0x0800 ) >> 11 ) | ( ( (z80.sp) & 0x0800 ) >> 10 ) | ( ( add16temp & 0x0800 ) >> 9 ); tstates += 7; (z80.ixh) = (add16temp >> 8) & 0xff; (z80.ixl) = add16temp & 0xff; z80.f = ( z80.f & ( 0x04 | 0x40 | 0x80 ) ) | ( add16temp & 0x10000 ? 0x01 : 0 )| ( ( add16temp >> 8 ) & ( 0x08 | 0x20 ) ) | halfcarry_add_table[lookup];};
    };
    ops[0x44] = function op_0x44(tempaddr) {
      z80.b=z80.ixh;
    };
    ops[0x45] = function op_0x45(tempaddr) {
      z80.b=z80.ixl;
    };
    ops[0x46] = function op_0x46(tempaddr) {
      tstates += 11;
      z80.b = readbyte( ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
      z80.pc &= 0xffff;
    };
    ops[0x4c] = function op_0x4c(tempaddr) {
      z80.c=z80.ixh;
    };
    ops[0x4d] = function op_0x4d(tempaddr) {
      z80.c=z80.ixl;
    };
    ops[0x4e] = function op_0x4e(tempaddr) {
      tstates += 11;
      z80.c = readbyte( ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
      z80.pc &= 0xffff;
    };
    ops[0x54] = function op_0x54(tempaddr) {
      z80.d=z80.ixh;
    };
    ops[0x55] = function op_0x55(tempaddr) {
      z80.d=z80.ixl;
    };
    ops[0x56] = function op_0x56(tempaddr) {
      tstates += 11;
      z80.d = readbyte( ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
      z80.pc &= 0xffff;
    };
    ops[0x5c] = function op_0x5c(tempaddr) {
      z80.e=z80.ixh;
    };
    ops[0x5d] = function op_0x5d(tempaddr) {
      z80.e=z80.ixl;
    };
    ops[0x5e] = function op_0x5e(tempaddr) {
      tstates += 11;
      z80.e = readbyte( ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
      z80.pc &= 0xffff;
    };
    ops[0x60] = function op_0x60(tempaddr) {
      z80.ixh=z80.b;
    };
    ops[0x61] = function op_0x61(tempaddr) {
      z80.ixh=z80.c;
    };
    ops[0x62] = function op_0x62(tempaddr) {
      z80.ixh=z80.d;
    };
    ops[0x63] = function op_0x63(tempaddr) {
      z80.ixh=z80.e;
    };
    ops[0x64] = function op_0x64(tempaddr) {
    };
    ops[0x65] = function op_0x65(tempaddr) {
      z80.ixh=z80.ixl;
    };
    ops[0x66] = function op_0x66(tempaddr) {
      tstates += 11;
      z80.h = readbyte( ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
      z80.pc &= 0xffff;
    };
    ops[0x67] = function op_0x67(tempaddr) {
      z80.ixh=z80.a;
    };
    ops[0x68] = function op_0x68(tempaddr) {
      z80.ixl=z80.b;
    };
    ops[0x69] = function op_0x69(tempaddr) {
      z80.ixl=z80.c;
    };
    ops[0x6a] = function op_0x6a(tempaddr) {
      z80.ixl=z80.d;
    };
    ops[0x6b] = function op_0x6b(tempaddr) {
      z80.ixl=z80.e;
    };
    ops[0x6c] = function op_0x6c(tempaddr) {
      z80.ixl=z80.ixh;
    };
    ops[0x6d] = function op_0x6d(tempaddr) {
    };
    ops[0x6e] = function op_0x6e(tempaddr) {
      tstates += 11;
      z80.l = readbyte( ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
      z80.pc &= 0xffff;
    };
    ops[0x6f] = function op_0x6f(tempaddr) {
      z80.ixl=z80.a;
    };
    ops[0x70] = function op_0x70(tempaddr) {
      tstates += 11;
      writebyte( ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff, z80.b );
      z80.pc &= 0xffff;
    };
    ops[0x71] = function op_0x71(tempaddr) {
      tstates += 11;
      writebyte( ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff, z80.c );
      z80.pc &= 0xffff;
    };
    ops[0x72] = function op_0x72(tempaddr) {
      tstates += 11;
      writebyte( ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff, z80.d );
      z80.pc &= 0xffff;
    };
    ops[0x73] = function op_0x73(tempaddr) {
      tstates += 11;
      writebyte( ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff, z80.e );
      z80.pc &= 0xffff;
    };
    ops[0x74] = function op_0x74(tempaddr) {
      tstates += 11;
      writebyte( ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff, z80.h );
      z80.pc &= 0xffff;
    };
    ops[0x75] = function op_0x75(tempaddr) {
      tstates += 11;
      writebyte( ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff, z80.l );
      z80.pc &= 0xffff;
    };
    ops[0x77] = function op_0x77(tempaddr) {
      tstates += 11;
      writebyte( ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff, z80.a );
      z80.pc &= 0xffff;
    };
    ops[0x7c] = function op_0x7c(tempaddr) {
      z80.a=z80.ixh;
    };
    ops[0x7d] = function op_0x7d(tempaddr) {
      z80.a=z80.ixl;
    };
    ops[0x7e] = function op_0x7e(tempaddr) {
      tstates += 11;
      z80.a = readbyte( ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
      z80.pc &= 0xffff;
    };
    ops[0x84] = function op_0x84(tempaddr) {
      { var addtemp = z80.a + (z80.ixh); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.ixh) & 0x88 ) >> 2 ) | ( ( addtemp & 0x88 ) >> 1 ); z80.a=addtemp & 0xff; z80.f = ( addtemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x85] = function op_0x85(tempaddr) {
      { var addtemp = z80.a + (z80.ixl); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.ixl) & 0x88 ) >> 2 ) | ( ( addtemp & 0x88 ) >> 1 ); z80.a=addtemp & 0xff; z80.f = ( addtemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x86] = function op_0x86(tempaddr) {
      tstates += 11;
      {
    var bytetemp =
        readbyte( ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
        z80.pc &= 0xffff;
    { var addtemp = z80.a + (bytetemp); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (bytetemp) & 0x88 ) >> 2 ) | ( ( addtemp & 0x88 ) >> 1 ); z80.a=addtemp & 0xff; z80.f = ( addtemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
      }
    };
    ops[0x8c] = function op_0x8c(tempaddr) {
      { var adctemp = z80.a + (z80.ixh) + ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.ixh) & 0x88 ) >> 2 ) | ( ( adctemp & 0x88 ) >> 1 ); z80.a=adctemp & 0xff; z80.f = ( adctemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x8d] = function op_0x8d(tempaddr) {
      { var adctemp = z80.a + (z80.ixl) + ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.ixl) & 0x88 ) >> 2 ) | ( ( adctemp & 0x88 ) >> 1 ); z80.a=adctemp & 0xff; z80.f = ( adctemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x8e] = function op_0x8e(tempaddr) {
      tstates += 11;
      {
    var bytetemp =
        readbyte( ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
        z80.pc &= 0xffff;
    { var adctemp = z80.a + (bytetemp) + ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (bytetemp) & 0x88 ) >> 2 ) | ( ( adctemp & 0x88 ) >> 1 ); z80.a=adctemp & 0xff; z80.f = ( adctemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
      }
    };
    ops[0x94] = function op_0x94(tempaddr) {
      { var subtemp = z80.a - (z80.ixh); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.ixh) & 0x88 ) >> 2 ) | ( (subtemp & 0x88 ) >> 1 ); z80.a=subtemp & 0xff; z80.f = ( subtemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x95] = function op_0x95(tempaddr) {
      { var subtemp = z80.a - (z80.ixl); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.ixl) & 0x88 ) >> 2 ) | ( (subtemp & 0x88 ) >> 1 ); z80.a=subtemp & 0xff; z80.f = ( subtemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x96] = function op_0x96(tempaddr) {
      tstates += 11;
      {
    var bytetemp =
        readbyte( ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
        z80.pc &= 0xffff;
    { var subtemp = z80.a - (bytetemp); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (bytetemp) & 0x88 ) >> 2 ) | ( (subtemp & 0x88 ) >> 1 ); z80.a=subtemp & 0xff; z80.f = ( subtemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
      }
    };
    ops[0x9c] = function op_0x9c(tempaddr) {
      { var sbctemp = z80.a - (z80.ixh) - ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.ixh) & 0x88 ) >> 2 ) | ( ( sbctemp & 0x88 ) >> 1 ); z80.a=sbctemp & 0xff; z80.f = ( sbctemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x9d] = function op_0x9d(tempaddr) {
      { var sbctemp = z80.a - (z80.ixl) - ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.ixl) & 0x88 ) >> 2 ) | ( ( sbctemp & 0x88 ) >> 1 ); z80.a=sbctemp & 0xff; z80.f = ( sbctemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x9e] = function op_0x9e(tempaddr) {
      tstates += 11;
      {
    var bytetemp =
        readbyte( ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
        z80.pc &= 0xffff;
    { var sbctemp = z80.a - (bytetemp) - ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (bytetemp) & 0x88 ) >> 2 ) | ( ( sbctemp & 0x88 ) >> 1 ); z80.a=sbctemp & 0xff; z80.f = ( sbctemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
      }
    };
    ops[0xa4] = function op_0xa4(tempaddr) {
      { z80.a &= (z80.ixh); z80.f = 0x10 | sz53p_table[z80.a];};
    };
    ops[0xa5] = function op_0xa5(tempaddr) {
      { z80.a &= (z80.ixl); z80.f = 0x10 | sz53p_table[z80.a];};
    };
    ops[0xa6] = function op_0xa6(tempaddr) {
      tstates += 11;
      {
    var bytetemp =
        readbyte( ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
        z80.pc &= 0xffff;
    { z80.a &= (bytetemp); z80.f = 0x10 | sz53p_table[z80.a];};
      }
    };
    ops[0xac] = function op_0xac(tempaddr) {
      { z80.a ^= (z80.ixh); z80.f = sz53p_table[z80.a];};
    };
    ops[0xad] = function op_0xad(tempaddr) {
      { z80.a ^= (z80.ixl); z80.f = sz53p_table[z80.a];};
    };
    ops[0xae] = function op_0xae(tempaddr) {
      tstates += 11;
      {
    var bytetemp =
        readbyte( ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
        z80.pc &= 0xffff;
    { z80.a ^= (bytetemp); z80.f = sz53p_table[z80.a];};
      }
    };
    ops[0xb4] = function op_0xb4(tempaddr) {
      { z80.a |= (z80.ixh); z80.f = sz53p_table[z80.a];};
    };
    ops[0xb5] = function op_0xb5(tempaddr) {
      { z80.a |= (z80.ixl); z80.f = sz53p_table[z80.a];};
    };
    ops[0xb6] = function op_0xb6(tempaddr) {
      tstates += 11;
      {
    var bytetemp =
        readbyte( ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
        z80.pc &= 0xffff;
    { z80.a |= (bytetemp); z80.f = sz53p_table[z80.a];};
      }
    };
    ops[0xbc] = function op_0xbc(tempaddr) {
      { var cptemp = z80.a - z80.ixh; var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.ixh) & 0x88 ) >> 2 ) | ( ( cptemp & 0x88 ) >> 1 ); z80.f = ( cptemp & 0x100 ? 0x01 : ( cptemp ? 0 : 0x40 ) ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | ( z80.ixh & ( 0x08 | 0x20 ) ) | ( cptemp & 0x80 );};
    };
    ops[0xbd] = function op_0xbd(tempaddr) {
      { var cptemp = z80.a - z80.ixl; var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.ixl) & 0x88 ) >> 2 ) | ( ( cptemp & 0x88 ) >> 1 ); z80.f = ( cptemp & 0x100 ? 0x01 : ( cptemp ? 0 : 0x40 ) ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | ( z80.ixl & ( 0x08 | 0x20 ) ) | ( cptemp & 0x80 );};
    };
    ops[0xbe] = function op_0xbe(tempaddr) {
      tstates += 11;
      {
    var bytetemp =
        readbyte( ((z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
        z80.pc &= 0xffff;
    { var cptemp = z80.a - bytetemp; var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (bytetemp) & 0x88 ) >> 2 ) | ( ( cptemp & 0x88 ) >> 1 ); z80.f = ( cptemp & 0x100 ? 0x01 : ( cptemp ? 0 : 0x40 ) ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | ( bytetemp & ( 0x08 | 0x20 ) ) | ( cptemp & 0x80 );};
      }
    };
    ops[0xcb] = function op_0xcb(tempaddr) {
      {
    var opcode3;
    tstates+=7;
    tempaddr =
        (z80.ixl | (z80.ixh << 8)) + sign_extend(readbyte( z80.pc++ ));
    z80.pc &= 0xffff;
    opcode3 = readbyte( z80.pc++ );
    z80.pc &= 0xffff;
    z80_ddfdcbxx(opcode3,tempaddr);
      }
    };
    ops[0xe1] = function op_0xe1(tempaddr) {
      { tstates+=6; (z80.ixl)=readbyte(z80.sp++); z80.sp &= 0xffff; (z80.ixh)=readbyte(z80.sp++); z80.sp &= 0xffff;};
    };
    ops[0xe3] = function op_0xe3(tempaddr) {
      {
    var bytetempl = readbyte( z80.sp ),
                     bytetemph = readbyte( z80.sp + 1 );
    tstates+=15;
    writebyte(z80.sp+1,z80.ixh); writebyte(z80.sp,z80.ixl);
    z80.ixl=bytetempl; z80.ixh=bytetemph;
      }
    };
    ops[0xe5] = function op_0xe5(tempaddr) {
      tstates++;
      { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.ixh)); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.ixl));};
    };
    ops[0xe9] = function op_0xe9(tempaddr) {
      z80.pc=(z80.ixl | (z80.ixh << 8));
    };
    ops[0xf9] = function op_0xf9(tempaddr) {
      tstates += 2;
      z80.sp=(z80.ixl | (z80.ixh << 8));
    };
    ops[256] = function z80_ddfd_default() {
      z80.pc--;
      z80.pc &= 0xffff;
      z80.r--;
      z80.r &= 0x7f;
    }
    z80_defaults(ops);
    return ops;
}();
function z80_ddxx(opcode) { z80_dd_ops[opcode](); }
var z80_fd_ops = function z80_setup_fd_ops() {
    var ops = [];
    ops[0x09] = function op_0x09(tempaddr) {
      { var add16temp = ((z80.iyl | (z80.iyh << 8))) + ((z80.c | (z80.b << 8))); var lookup = ( ( ((z80.iyl | (z80.iyh << 8))) & 0x0800 ) >> 11 ) | ( ( ((z80.c | (z80.b << 8))) & 0x0800 ) >> 10 ) | ( ( add16temp & 0x0800 ) >> 9 ); tstates += 7; (z80.iyh) = (add16temp >> 8) & 0xff; (z80.iyl) = add16temp & 0xff; z80.f = ( z80.f & ( 0x04 | 0x40 | 0x80 ) ) | ( add16temp & 0x10000 ? 0x01 : 0 )| ( ( add16temp >> 8 ) & ( 0x08 | 0x20 ) ) | halfcarry_add_table[lookup];};
    };
    ops[0x19] = function op_0x19(tempaddr) {
      { var add16temp = ((z80.iyl | (z80.iyh << 8))) + ((z80.e | (z80.d << 8))); var lookup = ( ( ((z80.iyl | (z80.iyh << 8))) & 0x0800 ) >> 11 ) | ( ( ((z80.e | (z80.d << 8))) & 0x0800 ) >> 10 ) | ( ( add16temp & 0x0800 ) >> 9 ); tstates += 7; (z80.iyh) = (add16temp >> 8) & 0xff; (z80.iyl) = add16temp & 0xff; z80.f = ( z80.f & ( 0x04 | 0x40 | 0x80 ) ) | ( add16temp & 0x10000 ? 0x01 : 0 )| ( ( add16temp >> 8 ) & ( 0x08 | 0x20 ) ) | halfcarry_add_table[lookup];};
    };
    ops[0x21] = function op_0x21(tempaddr) {
      tstates+=6;
      z80.iyl=readbyte(z80.pc++);
      z80.pc &= 0xffff;
      z80.iyh=readbyte(z80.pc++);
      z80.pc &= 0xffff;
    };
    ops[0x22] = function op_0x22(tempaddr) {
      { var ldtemp; tstates+=12; ldtemp=readbyte(z80.pc++); z80.pc &= 0xffff; ldtemp|=readbyte(z80.pc++) << 8; z80.pc &= 0xffff; writebyte(ldtemp++,(z80.iyl)); ldtemp &= 0xffff; writebyte(ldtemp,(z80.iyh));};
    };
    ops[0x23] = function op_0x23(tempaddr) {
      tstates += 2;
      var wordtemp = ((z80.iyl | (z80.iyh << 8)) + 1) & 0xffff;
      z80.iyh = wordtemp >> 8;
      z80.iyl = wordtemp & 0xff;
    };
    ops[0x24] = function op_0x24(tempaddr) {
      { (z80.iyh) = ((z80.iyh) + 1) & 0xff; z80.f = ( z80.f & 0x01 ) | ( (z80.iyh)==0x80 ? 0x04 : 0 ) | ( (z80.iyh)&0x0f ? 0 : 0x10 ) | sz53_table[(z80.iyh)];};
    };
    ops[0x25] = function op_0x25(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | ( (z80.iyh)&0x0f ? 0 : 0x10 ) | 0x02; (z80.iyh) = ((z80.iyh) - 1) & 0xff; z80.f |= ( (z80.iyh)==0x7f ? 0x04 : 0 ) | sz53_table[z80.iyh];};
    };
    ops[0x26] = function op_0x26(tempaddr) {
      tstates+=3;
      z80.iyh=readbyte(z80.pc++); z80.pc &= 0xffff;
    };
    ops[0x29] = function op_0x29(tempaddr) {
      { var add16temp = ((z80.iyl | (z80.iyh << 8))) + ((z80.iyl | (z80.iyh << 8))); var lookup = ( ( ((z80.iyl | (z80.iyh << 8))) & 0x0800 ) >> 11 ) | ( ( ((z80.iyl | (z80.iyh << 8))) & 0x0800 ) >> 10 ) | ( ( add16temp & 0x0800 ) >> 9 ); tstates += 7; (z80.iyh) = (add16temp >> 8) & 0xff; (z80.iyl) = add16temp & 0xff; z80.f = ( z80.f & ( 0x04 | 0x40 | 0x80 ) ) | ( add16temp & 0x10000 ? 0x01 : 0 )| ( ( add16temp >> 8 ) & ( 0x08 | 0x20 ) ) | halfcarry_add_table[lookup];};
    };
    ops[0x2a] = function op_0x2a(tempaddr) {
      { var ldtemp; tstates+=12; ldtemp=readbyte(z80.pc++); z80.pc &= 0xffff; ldtemp|=readbyte(z80.pc++) << 8; z80.pc &= 0xffff; (z80.iyl)=readbyte(ldtemp++); ldtemp &= 0xffff; (z80.iyh)=readbyte(ldtemp);};
    };
    ops[0x2b] = function op_0x2b(tempaddr) {
      tstates += 2;
      var wordtemp = ((z80.iyl | (z80.iyh << 8)) - 1) & 0xffff;
      z80.iyh = wordtemp >> 8;
      z80.iyl = wordtemp & 0xff;
    };
    ops[0x2c] = function op_0x2c(tempaddr) {
      { (z80.iyl) = ((z80.iyl) + 1) & 0xff; z80.f = ( z80.f & 0x01 ) | ( (z80.iyl)==0x80 ? 0x04 : 0 ) | ( (z80.iyl)&0x0f ? 0 : 0x10 ) | sz53_table[(z80.iyl)];};
    };
    ops[0x2d] = function op_0x2d(tempaddr) {
      { z80.f = ( z80.f & 0x01 ) | ( (z80.iyl)&0x0f ? 0 : 0x10 ) | 0x02; (z80.iyl) = ((z80.iyl) - 1) & 0xff; z80.f |= ( (z80.iyl)==0x7f ? 0x04 : 0 ) | sz53_table[z80.iyl];};
    };
    ops[0x2e] = function op_0x2e(tempaddr) {
      tstates+=3;
      z80.iyl=readbyte(z80.pc++); z80.pc &= 0xffff;
    };
    ops[0x34] = function op_0x34(tempaddr) {
      tstates += 15;
      {
    var wordtemp =
        ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff;
    z80.pc &= 0xffff;
    var bytetemp = readbyte( wordtemp );
    { (bytetemp) = ((bytetemp) + 1) & 0xff; z80.f = ( z80.f & 0x01 ) | ( (bytetemp)==0x80 ? 0x04 : 0 ) | ( (bytetemp)&0x0f ? 0 : 0x10 ) | sz53_table[(bytetemp)];};
    writebyte(wordtemp,bytetemp);
      }
    };
    ops[0x35] = function op_0x35(tempaddr) {
      tstates += 15;
      {
    var wordtemp =
        ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff;
    z80.pc &= 0xffff;
    var bytetemp = readbyte( wordtemp );
    { z80.f = ( z80.f & 0x01 ) | ( (bytetemp)&0x0f ? 0 : 0x10 ) | 0x02; (bytetemp) = ((bytetemp) - 1) & 0xff; z80.f |= ( (bytetemp)==0x7f ? 0x04 : 0 ) | sz53_table[bytetemp];};
    writebyte(wordtemp,bytetemp);
      }
    };
    ops[0x36] = function op_0x36(tempaddr) {
      tstates += 11;
      {
    var wordtemp =
        ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff;
    z80.pc &= 0xffff;
    writebyte(wordtemp,readbyte(z80.pc++));
    z80.pc &= 0xffff;
      }
    };
    ops[0x39] = function op_0x39(tempaddr) {
      { var add16temp = ((z80.iyl | (z80.iyh << 8))) + (z80.sp); var lookup = ( ( ((z80.iyl | (z80.iyh << 8))) & 0x0800 ) >> 11 ) | ( ( (z80.sp) & 0x0800 ) >> 10 ) | ( ( add16temp & 0x0800 ) >> 9 ); tstates += 7; (z80.iyh) = (add16temp >> 8) & 0xff; (z80.iyl) = add16temp & 0xff; z80.f = ( z80.f & ( 0x04 | 0x40 | 0x80 ) ) | ( add16temp & 0x10000 ? 0x01 : 0 )| ( ( add16temp >> 8 ) & ( 0x08 | 0x20 ) ) | halfcarry_add_table[lookup];};
    };
    ops[0x44] = function op_0x44(tempaddr) {
      z80.b=z80.iyh;
    };
    ops[0x45] = function op_0x45(tempaddr) {
      z80.b=z80.iyl;
    };
    ops[0x46] = function op_0x46(tempaddr) {
      tstates += 11;
      z80.b = readbyte( ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
      z80.pc &= 0xffff;
    };
    ops[0x4c] = function op_0x4c(tempaddr) {
      z80.c=z80.iyh;
    };
    ops[0x4d] = function op_0x4d(tempaddr) {
      z80.c=z80.iyl;
    };
    ops[0x4e] = function op_0x4e(tempaddr) {
      tstates += 11;
      z80.c = readbyte( ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
      z80.pc &= 0xffff;
    };
    ops[0x54] = function op_0x54(tempaddr) {
      z80.d=z80.iyh;
    };
    ops[0x55] = function op_0x55(tempaddr) {
      z80.d=z80.iyl;
    };
    ops[0x56] = function op_0x56(tempaddr) {
      tstates += 11;
      z80.d = readbyte( ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
      z80.pc &= 0xffff;
    };
    ops[0x5c] = function op_0x5c(tempaddr) {
      z80.e=z80.iyh;
    };
    ops[0x5d] = function op_0x5d(tempaddr) {
      z80.e=z80.iyl;
    };
    ops[0x5e] = function op_0x5e(tempaddr) {
      tstates += 11;
      z80.e = readbyte( ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
      z80.pc &= 0xffff;
    };
    ops[0x60] = function op_0x60(tempaddr) {
      z80.iyh=z80.b;
    };
    ops[0x61] = function op_0x61(tempaddr) {
      z80.iyh=z80.c;
    };
    ops[0x62] = function op_0x62(tempaddr) {
      z80.iyh=z80.d;
    };
    ops[0x63] = function op_0x63(tempaddr) {
      z80.iyh=z80.e;
    };
    ops[0x64] = function op_0x64(tempaddr) {
    };
    ops[0x65] = function op_0x65(tempaddr) {
      z80.iyh=z80.iyl;
    };
    ops[0x66] = function op_0x66(tempaddr) {
      tstates += 11;
      z80.h = readbyte( ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
      z80.pc &= 0xffff;
    };
    ops[0x67] = function op_0x67(tempaddr) {
      z80.iyh=z80.a;
    };
    ops[0x68] = function op_0x68(tempaddr) {
      z80.iyl=z80.b;
    };
    ops[0x69] = function op_0x69(tempaddr) {
      z80.iyl=z80.c;
    };
    ops[0x6a] = function op_0x6a(tempaddr) {
      z80.iyl=z80.d;
    };
    ops[0x6b] = function op_0x6b(tempaddr) {
      z80.iyl=z80.e;
    };
    ops[0x6c] = function op_0x6c(tempaddr) {
      z80.iyl=z80.iyh;
    };
    ops[0x6d] = function op_0x6d(tempaddr) {
    };
    ops[0x6e] = function op_0x6e(tempaddr) {
      tstates += 11;
      z80.l = readbyte( ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
      z80.pc &= 0xffff;
    };
    ops[0x6f] = function op_0x6f(tempaddr) {
      z80.iyl=z80.a;
    };
    ops[0x70] = function op_0x70(tempaddr) {
      tstates += 11;
      writebyte( ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff, z80.b );
      z80.pc &= 0xffff;
    };
    ops[0x71] = function op_0x71(tempaddr) {
      tstates += 11;
      writebyte( ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff, z80.c );
      z80.pc &= 0xffff;
    };
    ops[0x72] = function op_0x72(tempaddr) {
      tstates += 11;
      writebyte( ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff, z80.d );
      z80.pc &= 0xffff;
    };
    ops[0x73] = function op_0x73(tempaddr) {
      tstates += 11;
      writebyte( ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff, z80.e );
      z80.pc &= 0xffff;
    };
    ops[0x74] = function op_0x74(tempaddr) {
      tstates += 11;
      writebyte( ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff, z80.h );
      z80.pc &= 0xffff;
    };
    ops[0x75] = function op_0x75(tempaddr) {
      tstates += 11;
      writebyte( ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff, z80.l );
      z80.pc &= 0xffff;
    };
    ops[0x77] = function op_0x77(tempaddr) {
      tstates += 11;
      writebyte( ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff, z80.a );
      z80.pc &= 0xffff;
    };
    ops[0x7c] = function op_0x7c(tempaddr) {
      z80.a=z80.iyh;
    };
    ops[0x7d] = function op_0x7d(tempaddr) {
      z80.a=z80.iyl;
    };
    ops[0x7e] = function op_0x7e(tempaddr) {
      tstates += 11;
      z80.a = readbyte( ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
      z80.pc &= 0xffff;
    };
    ops[0x84] = function op_0x84(tempaddr) {
      { var addtemp = z80.a + (z80.iyh); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.iyh) & 0x88 ) >> 2 ) | ( ( addtemp & 0x88 ) >> 1 ); z80.a=addtemp & 0xff; z80.f = ( addtemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x85] = function op_0x85(tempaddr) {
      { var addtemp = z80.a + (z80.iyl); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.iyl) & 0x88 ) >> 2 ) | ( ( addtemp & 0x88 ) >> 1 ); z80.a=addtemp & 0xff; z80.f = ( addtemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x86] = function op_0x86(tempaddr) {
      tstates += 11;
      {
    var bytetemp =
        readbyte( ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
        z80.pc &= 0xffff;
    { var addtemp = z80.a + (bytetemp); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (bytetemp) & 0x88 ) >> 2 ) | ( ( addtemp & 0x88 ) >> 1 ); z80.a=addtemp & 0xff; z80.f = ( addtemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
      }
    };
    ops[0x8c] = function op_0x8c(tempaddr) {
      { var adctemp = z80.a + (z80.iyh) + ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.iyh) & 0x88 ) >> 2 ) | ( ( adctemp & 0x88 ) >> 1 ); z80.a=adctemp & 0xff; z80.f = ( adctemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x8d] = function op_0x8d(tempaddr) {
      { var adctemp = z80.a + (z80.iyl) + ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.iyl) & 0x88 ) >> 2 ) | ( ( adctemp & 0x88 ) >> 1 ); z80.a=adctemp & 0xff; z80.f = ( adctemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x8e] = function op_0x8e(tempaddr) {
      tstates += 11;
      {
    var bytetemp =
        readbyte( ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
        z80.pc &= 0xffff;
    { var adctemp = z80.a + (bytetemp) + ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (bytetemp) & 0x88 ) >> 2 ) | ( ( adctemp & 0x88 ) >> 1 ); z80.a=adctemp & 0xff; z80.f = ( adctemp & 0x100 ? 0x01 : 0 ) | halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] | sz53_table[z80.a];};
      }
    };
    ops[0x94] = function op_0x94(tempaddr) {
      { var subtemp = z80.a - (z80.iyh); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.iyh) & 0x88 ) >> 2 ) | ( (subtemp & 0x88 ) >> 1 ); z80.a=subtemp & 0xff; z80.f = ( subtemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x95] = function op_0x95(tempaddr) {
      { var subtemp = z80.a - (z80.iyl); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.iyl) & 0x88 ) >> 2 ) | ( (subtemp & 0x88 ) >> 1 ); z80.a=subtemp & 0xff; z80.f = ( subtemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x96] = function op_0x96(tempaddr) {
      tstates += 11;
      {
    var bytetemp =
        readbyte( ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
        z80.pc &= 0xffff;
    { var subtemp = z80.a - (bytetemp); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (bytetemp) & 0x88 ) >> 2 ) | ( (subtemp & 0x88 ) >> 1 ); z80.a=subtemp & 0xff; z80.f = ( subtemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
      }
    };
    ops[0x9c] = function op_0x9c(tempaddr) {
      { var sbctemp = z80.a - (z80.iyh) - ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.iyh) & 0x88 ) >> 2 ) | ( ( sbctemp & 0x88 ) >> 1 ); z80.a=sbctemp & 0xff; z80.f = ( sbctemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x9d] = function op_0x9d(tempaddr) {
      { var sbctemp = z80.a - (z80.iyl) - ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.iyl) & 0x88 ) >> 2 ) | ( ( sbctemp & 0x88 ) >> 1 ); z80.a=sbctemp & 0xff; z80.f = ( sbctemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
    };
    ops[0x9e] = function op_0x9e(tempaddr) {
      tstates += 11;
      {
    var bytetemp =
        readbyte( ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
        z80.pc &= 0xffff;
    { var sbctemp = z80.a - (bytetemp) - ( z80.f & 0x01 ); var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (bytetemp) & 0x88 ) >> 2 ) | ( ( sbctemp & 0x88 ) >> 1 ); z80.a=sbctemp & 0xff; z80.f = ( sbctemp & 0x100 ? 0x01 : 0 ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | sz53_table[z80.a];};
      }
    };
    ops[0xa4] = function op_0xa4(tempaddr) {
      { z80.a &= (z80.iyh); z80.f = 0x10 | sz53p_table[z80.a];};
    };
    ops[0xa5] = function op_0xa5(tempaddr) {
      { z80.a &= (z80.iyl); z80.f = 0x10 | sz53p_table[z80.a];};
    };
    ops[0xa6] = function op_0xa6(tempaddr) {
      tstates += 11;
      {
    var bytetemp =
        readbyte( ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
        z80.pc &= 0xffff;
    { z80.a &= (bytetemp); z80.f = 0x10 | sz53p_table[z80.a];};
      }
    };
    ops[0xac] = function op_0xac(tempaddr) {
      { z80.a ^= (z80.iyh); z80.f = sz53p_table[z80.a];};
    };
    ops[0xad] = function op_0xad(tempaddr) {
      { z80.a ^= (z80.iyl); z80.f = sz53p_table[z80.a];};
    };
    ops[0xae] = function op_0xae(tempaddr) {
      tstates += 11;
      {
    var bytetemp =
        readbyte( ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
        z80.pc &= 0xffff;
    { z80.a ^= (bytetemp); z80.f = sz53p_table[z80.a];};
      }
    };
    ops[0xb4] = function op_0xb4(tempaddr) {
      { z80.a |= (z80.iyh); z80.f = sz53p_table[z80.a];};
    };
    ops[0xb5] = function op_0xb5(tempaddr) {
      { z80.a |= (z80.iyl); z80.f = sz53p_table[z80.a];};
    };
    ops[0xb6] = function op_0xb6(tempaddr) {
      tstates += 11;
      {
    var bytetemp =
        readbyte( ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
        z80.pc &= 0xffff;
    { z80.a |= (bytetemp); z80.f = sz53p_table[z80.a];};
      }
    };
    ops[0xbc] = function op_0xbc(tempaddr) {
      { var cptemp = z80.a - z80.iyh; var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.iyh) & 0x88 ) >> 2 ) | ( ( cptemp & 0x88 ) >> 1 ); z80.f = ( cptemp & 0x100 ? 0x01 : ( cptemp ? 0 : 0x40 ) ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | ( z80.iyh & ( 0x08 | 0x20 ) ) | ( cptemp & 0x80 );};
    };
    ops[0xbd] = function op_0xbd(tempaddr) {
      { var cptemp = z80.a - z80.iyl; var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (z80.iyl) & 0x88 ) >> 2 ) | ( ( cptemp & 0x88 ) >> 1 ); z80.f = ( cptemp & 0x100 ? 0x01 : ( cptemp ? 0 : 0x40 ) ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | ( z80.iyl & ( 0x08 | 0x20 ) ) | ( cptemp & 0x80 );};
    };
    ops[0xbe] = function op_0xbe(tempaddr) {
      tstates += 11;
      {
    var bytetemp =
        readbyte( ((z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ))) & 0xffff );
        z80.pc &= 0xffff;
    { var cptemp = z80.a - bytetemp; var lookup = ( ( z80.a & 0x88 ) >> 3 ) | ( ( (bytetemp) & 0x88 ) >> 2 ) | ( ( cptemp & 0x88 ) >> 1 ); z80.f = ( cptemp & 0x100 ? 0x01 : ( cptemp ? 0 : 0x40 ) ) | 0x02 | halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] | ( bytetemp & ( 0x08 | 0x20 ) ) | ( cptemp & 0x80 );};
      }
    };
    ops[0xcb] = function op_0xcb(tempaddr) {
      {
    var opcode3;
    tstates+=7;
    tempaddr =
        (z80.iyl | (z80.iyh << 8)) + sign_extend(readbyte( z80.pc++ ));
    z80.pc &= 0xffff;
    opcode3 = readbyte( z80.pc++ );
    z80.pc &= 0xffff;
    z80_ddfdcbxx(opcode3,tempaddr);
      }
    };
    ops[0xe1] = function op_0xe1(tempaddr) {
      { tstates+=6; (z80.iyl)=readbyte(z80.sp++); z80.sp &= 0xffff; (z80.iyh)=readbyte(z80.sp++); z80.sp &= 0xffff;};
    };
    ops[0xe3] = function op_0xe3(tempaddr) {
      {
    var bytetempl = readbyte( z80.sp ),
                     bytetemph = readbyte( z80.sp + 1 );
    tstates+=15;
    writebyte(z80.sp+1,z80.iyh); writebyte(z80.sp,z80.iyl);
    z80.iyl=bytetempl; z80.iyh=bytetemph;
      }
    };
    ops[0xe5] = function op_0xe5(tempaddr) {
      tstates++;
      { tstates+=6; z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.iyh)); z80.sp--; z80.sp &= 0xffff; writebyte(z80.sp,(z80.iyl));};
    };
    ops[0xe9] = function op_0xe9(tempaddr) {
      z80.pc=(z80.iyl | (z80.iyh << 8));
    };
    ops[0xf9] = function op_0xf9(tempaddr) {
      tstates += 2;
      z80.sp=(z80.iyl | (z80.iyh << 8));
    };
    ops[256] = function z80_ddfd_default() {
      z80.pc--;
      z80.pc &= 0xffff;
      z80.r--;
      z80.r &= 0x7f;
    }
    z80_defaults(ops);
    return ops;
}();
function z80_fdxx(opcode) { z80_fd_ops[opcode](); }
var z80_ddfdcb_ops = function z80_setup_ddfdcb_ops() {
    var ops = [];
    ops[0x00] = function op_0x00(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr);
      { (z80.b) = ( ((z80.b) & 0x7f)<<1 ) | ( (z80.b)>>7 ); z80.f = ( (z80.b) & 0x01 ) | sz53p_table[(z80.b)];};
      writebyte(tempaddr, z80.b);
      };
    ops[0x01] = function op_0x01(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr);
      { (z80.c) = ( ((z80.c) & 0x7f)<<1 ) | ( (z80.c)>>7 ); z80.f = ( (z80.c) & 0x01 ) | sz53p_table[(z80.c)];};
      writebyte(tempaddr, z80.c);
      };
    ops[0x02] = function op_0x02(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr);
      { (z80.d) = ( ((z80.d) & 0x7f)<<1 ) | ( (z80.d)>>7 ); z80.f = ( (z80.d) & 0x01 ) | sz53p_table[(z80.d)];};
      writebyte(tempaddr, z80.d);
      };
    ops[0x03] = function op_0x03(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr);
      { (z80.e) = ( ((z80.e) & 0x7f)<<1 ) | ( (z80.e)>>7 ); z80.f = ( (z80.e) & 0x01 ) | sz53p_table[(z80.e)];};
      writebyte(tempaddr, z80.e);
      };
    ops[0x04] = function op_0x04(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr);
      { (z80.h) = ( ((z80.h) & 0x7f)<<1 ) | ( (z80.h)>>7 ); z80.f = ( (z80.h) & 0x01 ) | sz53p_table[(z80.h)];};
      writebyte(tempaddr, z80.h);
      };
    ops[0x05] = function op_0x05(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr);
      { (z80.l) = ( ((z80.l) & 0x7f)<<1 ) | ( (z80.l)>>7 ); z80.f = ( (z80.l) & 0x01 ) | sz53p_table[(z80.l)];};
      writebyte(tempaddr, z80.l);
      };
    ops[0x06] = function op_0x06(tempaddr) {
      tstates += 8;
      {
    var bytetemp = readbyte(tempaddr);
    { (bytetemp) = ( ((bytetemp) & 0x7f)<<1 ) | ( (bytetemp)>>7 ); z80.f = ( (bytetemp) & 0x01 ) | sz53p_table[(bytetemp)];};
    writebyte(tempaddr,bytetemp);
      }
    };
    ops[0x07] = function op_0x07(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr);
      { (z80.a) = ( ((z80.a) & 0x7f)<<1 ) | ( (z80.a)>>7 ); z80.f = ( (z80.a) & 0x01 ) | sz53p_table[(z80.a)];};
      writebyte(tempaddr, z80.a);
      };
    ops[0x08] = function op_0x08(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr);
      { z80.f = (z80.b) & 0x01; (z80.b) = ( (z80.b)>>1 ) | ( ((z80.b) & 0x01)<<7 ); z80.f |= sz53p_table[(z80.b)];};
      writebyte(tempaddr, z80.b);
      };
    ops[0x09] = function op_0x09(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr);
      { z80.f = (z80.c) & 0x01; (z80.c) = ( (z80.c)>>1 ) | ( ((z80.c) & 0x01)<<7 ); z80.f |= sz53p_table[(z80.c)];};
      writebyte(tempaddr, z80.c);
      };
    ops[0x0a] = function op_0x0a(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr);
      { z80.f = (z80.d) & 0x01; (z80.d) = ( (z80.d)>>1 ) | ( ((z80.d) & 0x01)<<7 ); z80.f |= sz53p_table[(z80.d)];};
      writebyte(tempaddr, z80.d);
      };
    ops[0x0b] = function op_0x0b(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr);
      { z80.f = (z80.e) & 0x01; (z80.e) = ( (z80.e)>>1 ) | ( ((z80.e) & 0x01)<<7 ); z80.f |= sz53p_table[(z80.e)];};
      writebyte(tempaddr, z80.e);
      };
    ops[0x0c] = function op_0x0c(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr);
      { z80.f = (z80.h) & 0x01; (z80.h) = ( (z80.h)>>1 ) | ( ((z80.h) & 0x01)<<7 ); z80.f |= sz53p_table[(z80.h)];};
      writebyte(tempaddr, z80.h);
      };
    ops[0x0d] = function op_0x0d(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr);
      { z80.f = (z80.l) & 0x01; (z80.l) = ( (z80.l)>>1 ) | ( ((z80.l) & 0x01)<<7 ); z80.f |= sz53p_table[(z80.l)];};
      writebyte(tempaddr, z80.l);
      };
    ops[0x0e] = function op_0x0e(tempaddr) {
      tstates += 8;
      {
    var bytetemp = readbyte(tempaddr);
    { z80.f = (bytetemp) & 0x01; (bytetemp) = ( (bytetemp)>>1 ) | ( ((bytetemp) & 0x01)<<7 ); z80.f |= sz53p_table[(bytetemp)];};
    writebyte(tempaddr,bytetemp);
      }
    };
    ops[0x0f] = function op_0x0f(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr);
      { z80.f = (z80.a) & 0x01; (z80.a) = ( (z80.a)>>1 ) | ( ((z80.a) & 0x01)<<7 ); z80.f |= sz53p_table[(z80.a)];};
      writebyte(tempaddr, z80.a);
      };
    ops[0x10] = function op_0x10(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr);
      { var rltemp = (z80.b); (z80.b) = ( ((z80.b) & 0x7f)<<1 ) | ( z80.f & 0x01 ); z80.f = ( rltemp >> 7 ) | sz53p_table[(z80.b)];};
      writebyte(tempaddr, z80.b);
      };
    ops[0x11] = function op_0x11(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr);
      { var rltemp = (z80.c); (z80.c) = ( ((z80.c) & 0x7f)<<1 ) | ( z80.f & 0x01 ); z80.f = ( rltemp >> 7 ) | sz53p_table[(z80.c)];};
      writebyte(tempaddr, z80.c);
      };
    ops[0x12] = function op_0x12(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr);
      { var rltemp = (z80.d); (z80.d) = ( ((z80.d) & 0x7f)<<1 ) | ( z80.f & 0x01 ); z80.f = ( rltemp >> 7 ) | sz53p_table[(z80.d)];};
      writebyte(tempaddr, z80.d);
      };
    ops[0x13] = function op_0x13(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr);
      { var rltemp = (z80.e); (z80.e) = ( ((z80.e) & 0x7f)<<1 ) | ( z80.f & 0x01 ); z80.f = ( rltemp >> 7 ) | sz53p_table[(z80.e)];};
      writebyte(tempaddr, z80.e);
      };
    ops[0x14] = function op_0x14(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr);
      { var rltemp = (z80.h); (z80.h) = ( ((z80.h) & 0x7f)<<1 ) | ( z80.f & 0x01 ); z80.f = ( rltemp >> 7 ) | sz53p_table[(z80.h)];};
      writebyte(tempaddr, z80.h);
      };
    ops[0x15] = function op_0x15(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr);
      { var rltemp = (z80.l); (z80.l) = ( ((z80.l) & 0x7f)<<1 ) | ( z80.f & 0x01 ); z80.f = ( rltemp >> 7 ) | sz53p_table[(z80.l)];};
      writebyte(tempaddr, z80.l);
      };
    ops[0x16] = function op_0x16(tempaddr) {
      tstates += 8;
      {
    var bytetemp = readbyte(tempaddr);
    { var rltemp = (bytetemp); (bytetemp) = ( ((bytetemp) & 0x7f)<<1 ) | ( z80.f & 0x01 ); z80.f = ( rltemp >> 7 ) | sz53p_table[(bytetemp)];};
    writebyte(tempaddr,bytetemp);
      }
    };
    ops[0x17] = function op_0x17(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr);
      { var rltemp = (z80.a); (z80.a) = ( ((z80.a) & 0x7f)<<1 ) | ( z80.f & 0x01 ); z80.f = ( rltemp >> 7 ) | sz53p_table[(z80.a)];};
      writebyte(tempaddr, z80.a);
      };
    ops[0x18] = function op_0x18(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr);
      { var rrtemp = (z80.b); (z80.b) = ( (z80.b)>>1 ) | ( (z80.f & 0x01) << 7 ); z80.f = ( rrtemp & 0x01 ) | sz53p_table[(z80.b)];};
      writebyte(tempaddr, z80.b);
      };
    ops[0x19] = function op_0x19(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr);
      { var rrtemp = (z80.c); (z80.c) = ( (z80.c)>>1 ) | ( (z80.f & 0x01) << 7 ); z80.f = ( rrtemp & 0x01 ) | sz53p_table[(z80.c)];};
      writebyte(tempaddr, z80.c);
      };
    ops[0x1a] = function op_0x1a(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr);
      { var rrtemp = (z80.d); (z80.d) = ( (z80.d)>>1 ) | ( (z80.f & 0x01) << 7 ); z80.f = ( rrtemp & 0x01 ) | sz53p_table[(z80.d)];};
      writebyte(tempaddr, z80.d);
      };
    ops[0x1b] = function op_0x1b(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr);
      { var rrtemp = (z80.e); (z80.e) = ( (z80.e)>>1 ) | ( (z80.f & 0x01) << 7 ); z80.f = ( rrtemp & 0x01 ) | sz53p_table[(z80.e)];};
      writebyte(tempaddr, z80.e);
      };
    ops[0x1c] = function op_0x1c(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr);
      { var rrtemp = (z80.h); (z80.h) = ( (z80.h)>>1 ) | ( (z80.f & 0x01) << 7 ); z80.f = ( rrtemp & 0x01 ) | sz53p_table[(z80.h)];};
      writebyte(tempaddr, z80.h);
      };
    ops[0x1d] = function op_0x1d(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr);
      { var rrtemp = (z80.l); (z80.l) = ( (z80.l)>>1 ) | ( (z80.f & 0x01) << 7 ); z80.f = ( rrtemp & 0x01 ) | sz53p_table[(z80.l)];};
      writebyte(tempaddr, z80.l);
      };
    ops[0x1e] = function op_0x1e(tempaddr) {
      tstates += 8;
      {
    var bytetemp = readbyte(tempaddr);
    { var rrtemp = (bytetemp); (bytetemp) = ( (bytetemp)>>1 ) | ( (z80.f & 0x01) << 7 ); z80.f = ( rrtemp & 0x01 ) | sz53p_table[(bytetemp)];};
    writebyte(tempaddr,bytetemp);
      }
    };
    ops[0x1f] = function op_0x1f(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr);
      { var rrtemp = (z80.a); (z80.a) = ( (z80.a)>>1 ) | ( (z80.f & 0x01) << 7 ); z80.f = ( rrtemp & 0x01 ) | sz53p_table[(z80.a)];};
      writebyte(tempaddr, z80.a);
      };
    ops[0x20] = function op_0x20(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr);
      { z80.f = (z80.b) >> 7; (z80.b) <<= 1; (z80.b) &= 0xff; z80.f |= sz53p_table[(z80.b)];};
      writebyte(tempaddr, z80.b);
      };
    ops[0x21] = function op_0x21(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr);
      { z80.f = (z80.c) >> 7; (z80.c) <<= 1; (z80.c) &= 0xff; z80.f |= sz53p_table[(z80.c)];};
      writebyte(tempaddr, z80.c);
      };
    ops[0x22] = function op_0x22(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr);
      { z80.f = (z80.d) >> 7; (z80.d) <<= 1; (z80.d) &= 0xff; z80.f |= sz53p_table[(z80.d)];};
      writebyte(tempaddr, z80.d);
      };
    ops[0x23] = function op_0x23(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr);
      { z80.f = (z80.e) >> 7; (z80.e) <<= 1; (z80.e) &= 0xff; z80.f |= sz53p_table[(z80.e)];};
      writebyte(tempaddr, z80.e);
      };
    ops[0x24] = function op_0x24(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr);
      { z80.f = (z80.h) >> 7; (z80.h) <<= 1; (z80.h) &= 0xff; z80.f |= sz53p_table[(z80.h)];};
      writebyte(tempaddr, z80.h);
      };
    ops[0x25] = function op_0x25(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr);
      { z80.f = (z80.l) >> 7; (z80.l) <<= 1; (z80.l) &= 0xff; z80.f |= sz53p_table[(z80.l)];};
      writebyte(tempaddr, z80.l);
      };
    ops[0x26] = function op_0x26(tempaddr) {
      tstates += 8;
      {
    var bytetemp = readbyte(tempaddr);
    { z80.f = (bytetemp) >> 7; (bytetemp) <<= 1; (bytetemp) &= 0xff; z80.f |= sz53p_table[(bytetemp)];};
    writebyte(tempaddr,bytetemp);
      }
    };
    ops[0x27] = function op_0x27(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr);
      { z80.f = (z80.a) >> 7; (z80.a) <<= 1; (z80.a) &= 0xff; z80.f |= sz53p_table[(z80.a)];};
      writebyte(tempaddr, z80.a);
      };
    ops[0x28] = function op_0x28(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr);
      { z80.f = (z80.b) & 0x01; (z80.b) = ( (z80.b) & 0x80 ) | ( (z80.b) >> 1 ); z80.f |= sz53p_table[(z80.b)];};
      writebyte(tempaddr, z80.b);
      };
    ops[0x29] = function op_0x29(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr);
      { z80.f = (z80.c) & 0x01; (z80.c) = ( (z80.c) & 0x80 ) | ( (z80.c) >> 1 ); z80.f |= sz53p_table[(z80.c)];};
      writebyte(tempaddr, z80.c);
      };
    ops[0x2a] = function op_0x2a(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr);
      { z80.f = (z80.d) & 0x01; (z80.d) = ( (z80.d) & 0x80 ) | ( (z80.d) >> 1 ); z80.f |= sz53p_table[(z80.d)];};
      writebyte(tempaddr, z80.d);
      };
    ops[0x2b] = function op_0x2b(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr);
      { z80.f = (z80.e) & 0x01; (z80.e) = ( (z80.e) & 0x80 ) | ( (z80.e) >> 1 ); z80.f |= sz53p_table[(z80.e)];};
      writebyte(tempaddr, z80.e);
      };
    ops[0x2c] = function op_0x2c(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr);
      { z80.f = (z80.h) & 0x01; (z80.h) = ( (z80.h) & 0x80 ) | ( (z80.h) >> 1 ); z80.f |= sz53p_table[(z80.h)];};
      writebyte(tempaddr, z80.h);
      };
    ops[0x2d] = function op_0x2d(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr);
      { z80.f = (z80.l) & 0x01; (z80.l) = ( (z80.l) & 0x80 ) | ( (z80.l) >> 1 ); z80.f |= sz53p_table[(z80.l)];};
      writebyte(tempaddr, z80.l);
      };
    ops[0x2e] = function op_0x2e(tempaddr) {
      tstates += 8;
      {
    var bytetemp = readbyte(tempaddr);
    { z80.f = (bytetemp) & 0x01; (bytetemp) = ( (bytetemp) & 0x80 ) | ( (bytetemp) >> 1 ); z80.f |= sz53p_table[(bytetemp)];};
    writebyte(tempaddr,bytetemp);
      }
    };
    ops[0x2f] = function op_0x2f(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr);
      { z80.f = (z80.a) & 0x01; (z80.a) = ( (z80.a) & 0x80 ) | ( (z80.a) >> 1 ); z80.f |= sz53p_table[(z80.a)];};
      writebyte(tempaddr, z80.a);
      };
    ops[0x30] = function op_0x30(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr);
      { z80.f = (z80.b) >> 7; (z80.b) = ( (z80.b) << 1 ) | 0x01; (z80.b) &= 0xff; z80.f |= sz53p_table[(z80.b)];};
      writebyte(tempaddr, z80.b);
      };
    ops[0x31] = function op_0x31(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr);
      { z80.f = (z80.c) >> 7; (z80.c) = ( (z80.c) << 1 ) | 0x01; (z80.c) &= 0xff; z80.f |= sz53p_table[(z80.c)];};
      writebyte(tempaddr, z80.c);
      };
    ops[0x32] = function op_0x32(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr);
      { z80.f = (z80.d) >> 7; (z80.d) = ( (z80.d) << 1 ) | 0x01; (z80.d) &= 0xff; z80.f |= sz53p_table[(z80.d)];};
      writebyte(tempaddr, z80.d);
      };
    ops[0x33] = function op_0x33(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr);
      { z80.f = (z80.e) >> 7; (z80.e) = ( (z80.e) << 1 ) | 0x01; (z80.e) &= 0xff; z80.f |= sz53p_table[(z80.e)];};
      writebyte(tempaddr, z80.e);
      };
    ops[0x34] = function op_0x34(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr);
      { z80.f = (z80.h) >> 7; (z80.h) = ( (z80.h) << 1 ) | 0x01; (z80.h) &= 0xff; z80.f |= sz53p_table[(z80.h)];};
      writebyte(tempaddr, z80.h);
      };
    ops[0x35] = function op_0x35(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr);
      { z80.f = (z80.l) >> 7; (z80.l) = ( (z80.l) << 1 ) | 0x01; (z80.l) &= 0xff; z80.f |= sz53p_table[(z80.l)];};
      writebyte(tempaddr, z80.l);
      };
    ops[0x36] = function op_0x36(tempaddr) {
      tstates += 8;
      {
    var bytetemp = readbyte(tempaddr);
    { z80.f = (bytetemp) >> 7; (bytetemp) = ( (bytetemp) << 1 ) | 0x01; (bytetemp) &= 0xff; z80.f |= sz53p_table[(bytetemp)];};
    writebyte(tempaddr,bytetemp);
      }
    };
    ops[0x37] = function op_0x37(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr);
      { z80.f = (z80.a) >> 7; (z80.a) = ( (z80.a) << 1 ) | 0x01; (z80.a) &= 0xff; z80.f |= sz53p_table[(z80.a)];};
      writebyte(tempaddr, z80.a);
      };
    ops[0x38] = function op_0x38(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr);
      { z80.f = (z80.b) & 0x01; (z80.b) >>= 1; z80.f |= sz53p_table[(z80.b)];};
      writebyte(tempaddr, z80.b);
      };
    ops[0x39] = function op_0x39(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr);
      { z80.f = (z80.c) & 0x01; (z80.c) >>= 1; z80.f |= sz53p_table[(z80.c)];};
      writebyte(tempaddr, z80.c);
      };
    ops[0x3a] = function op_0x3a(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr);
      { z80.f = (z80.d) & 0x01; (z80.d) >>= 1; z80.f |= sz53p_table[(z80.d)];};
      writebyte(tempaddr, z80.d);
      };
    ops[0x3b] = function op_0x3b(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr);
      { z80.f = (z80.e) & 0x01; (z80.e) >>= 1; z80.f |= sz53p_table[(z80.e)];};
      writebyte(tempaddr, z80.e);
      };
    ops[0x3c] = function op_0x3c(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr);
      { z80.f = (z80.h) & 0x01; (z80.h) >>= 1; z80.f |= sz53p_table[(z80.h)];};
      writebyte(tempaddr, z80.h);
      };
    ops[0x3d] = function op_0x3d(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr);
      { z80.f = (z80.l) & 0x01; (z80.l) >>= 1; z80.f |= sz53p_table[(z80.l)];};
      writebyte(tempaddr, z80.l);
      };
    ops[0x3e] = function op_0x3e(tempaddr) {
      tstates += 8;
      {
    var bytetemp = readbyte(tempaddr);
    { z80.f = (bytetemp) & 0x01; (bytetemp) >>= 1; z80.f |= sz53p_table[(bytetemp)];};
    writebyte(tempaddr,bytetemp);
      }
    };
    ops[0x3f] = function op_0x3f(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr);
      { z80.f = (z80.a) & 0x01; (z80.a) >>= 1; z80.f |= sz53p_table[(z80.a)];};
      writebyte(tempaddr, z80.a);
      };
    ops[0x40] =
    ops[0x41] =
    ops[0x42] =
    ops[0x43] =
    ops[0x44] =
    ops[0x45] =
    ops[0x46] =
    ops[0x47] = function op_0x47(tempaddr) {
      tstates += 5;
      {
    var bytetemp = readbyte( tempaddr );
    { z80.f = ( z80.f & 0x01 ) | 0x10 | ( ( tempaddr >> 8 ) & ( 0x08 | 0x20 ) ); if( ! ( (bytetemp) & ( 0x01 << (0) ) ) ) z80.f |= 0x04 | 0x40; if( (0) == 7 && (bytetemp) & 0x80 ) z80.f |= 0x80; };
      }
    };
    ops[0x48] =
    ops[0x49] =
    ops[0x4a] =
    ops[0x4b] =
    ops[0x4c] =
    ops[0x4d] =
    ops[0x4e] =
    ops[0x4f] = function op_0x4f(tempaddr) {
      tstates += 5;
      {
    var bytetemp = readbyte( tempaddr );
    { z80.f = ( z80.f & 0x01 ) | 0x10 | ( ( tempaddr >> 8 ) & ( 0x08 | 0x20 ) ); if( ! ( (bytetemp) & ( 0x01 << (1) ) ) ) z80.f |= 0x04 | 0x40; if( (1) == 7 && (bytetemp) & 0x80 ) z80.f |= 0x80; };
      }
    };
    ops[0x50] =
    ops[0x51] =
    ops[0x52] =
    ops[0x53] =
    ops[0x54] =
    ops[0x55] =
    ops[0x56] =
    ops[0x57] = function op_0x57(tempaddr) {
      tstates += 5;
      {
    var bytetemp = readbyte( tempaddr );
    { z80.f = ( z80.f & 0x01 ) | 0x10 | ( ( tempaddr >> 8 ) & ( 0x08 | 0x20 ) ); if( ! ( (bytetemp) & ( 0x01 << (2) ) ) ) z80.f |= 0x04 | 0x40; if( (2) == 7 && (bytetemp) & 0x80 ) z80.f |= 0x80; };
      }
    };
    ops[0x58] =
    ops[0x59] =
    ops[0x5a] =
    ops[0x5b] =
    ops[0x5c] =
    ops[0x5d] =
    ops[0x5e] =
    ops[0x5f] = function op_0x5f(tempaddr) {
      tstates += 5;
      {
    var bytetemp = readbyte( tempaddr );
    { z80.f = ( z80.f & 0x01 ) | 0x10 | ( ( tempaddr >> 8 ) & ( 0x08 | 0x20 ) ); if( ! ( (bytetemp) & ( 0x01 << (3) ) ) ) z80.f |= 0x04 | 0x40; if( (3) == 7 && (bytetemp) & 0x80 ) z80.f |= 0x80; };
      }
    };
    ops[0x60] =
    ops[0x61] =
    ops[0x62] =
    ops[0x63] =
    ops[0x64] =
    ops[0x65] =
    ops[0x66] =
    ops[0x67] = function op_0x67(tempaddr) {
      tstates += 5;
      {
    var bytetemp = readbyte( tempaddr );
    { z80.f = ( z80.f & 0x01 ) | 0x10 | ( ( tempaddr >> 8 ) & ( 0x08 | 0x20 ) ); if( ! ( (bytetemp) & ( 0x01 << (4) ) ) ) z80.f |= 0x04 | 0x40; if( (4) == 7 && (bytetemp) & 0x80 ) z80.f |= 0x80; };
      }
    };
    ops[0x68] =
    ops[0x69] =
    ops[0x6a] =
    ops[0x6b] =
    ops[0x6c] =
    ops[0x6d] =
    ops[0x6e] =
    ops[0x6f] = function op_0x6f(tempaddr) {
      tstates += 5;
      {
    var bytetemp = readbyte( tempaddr );
    { z80.f = ( z80.f & 0x01 ) | 0x10 | ( ( tempaddr >> 8 ) & ( 0x08 | 0x20 ) ); if( ! ( (bytetemp) & ( 0x01 << (5) ) ) ) z80.f |= 0x04 | 0x40; if( (5) == 7 && (bytetemp) & 0x80 ) z80.f |= 0x80; };
      }
    };
    ops[0x70] =
    ops[0x71] =
    ops[0x72] =
    ops[0x73] =
    ops[0x74] =
    ops[0x75] =
    ops[0x76] =
    ops[0x77] = function op_0x77(tempaddr) {
      tstates += 5;
      {
    var bytetemp = readbyte( tempaddr );
    { z80.f = ( z80.f & 0x01 ) | 0x10 | ( ( tempaddr >> 8 ) & ( 0x08 | 0x20 ) ); if( ! ( (bytetemp) & ( 0x01 << (6) ) ) ) z80.f |= 0x04 | 0x40; if( (6) == 7 && (bytetemp) & 0x80 ) z80.f |= 0x80; };
      }
    };
    ops[0x78] =
    ops[0x79] =
    ops[0x7a] =
    ops[0x7b] =
    ops[0x7c] =
    ops[0x7d] =
    ops[0x7e] =
    ops[0x7f] = function op_0x7f(tempaddr) {
      tstates += 5;
      {
    var bytetemp = readbyte( tempaddr );
    { z80.f = ( z80.f & 0x01 ) | 0x10 | ( ( tempaddr >> 8 ) & ( 0x08 | 0x20 ) ); if( ! ( (bytetemp) & ( 0x01 << (7) ) ) ) z80.f |= 0x04 | 0x40; if( (7) == 7 && (bytetemp) & 0x80 ) z80.f |= 0x80; };
      }
    };
    ops[0x80] = function op_0x80(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr) & 0xfe;
      writebyte(tempaddr, z80.b);
      };
    ops[0x81] = function op_0x81(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr) & 0xfe;
      writebyte(tempaddr, z80.c);
      };
    ops[0x82] = function op_0x82(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr) & 0xfe;
      writebyte(tempaddr, z80.d);
      };
    ops[0x83] = function op_0x83(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr) & 0xfe;
      writebyte(tempaddr, z80.e);
      };
    ops[0x84] = function op_0x84(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr) & 0xfe;
      writebyte(tempaddr, z80.h);
      };
    ops[0x85] = function op_0x85(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr) & 0xfe;
      writebyte(tempaddr, z80.l);
      };
    ops[0x86] = function op_0x86(tempaddr) {
      tstates += 8;
      writebyte(tempaddr, readbyte(tempaddr) & 0xfe);
    };
    ops[0x87] = function op_0x87(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr) & 0xfe;
      writebyte(tempaddr, z80.a);
      };
    ops[0x88] = function op_0x88(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr) & 0xfd;
      writebyte(tempaddr, z80.b);
      };
    ops[0x89] = function op_0x89(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr) & 0xfd;
      writebyte(tempaddr, z80.c);
      };
    ops[0x8a] = function op_0x8a(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr) & 0xfd;
      writebyte(tempaddr, z80.d);
      };
    ops[0x8b] = function op_0x8b(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr) & 0xfd;
      writebyte(tempaddr, z80.e);
      };
    ops[0x8c] = function op_0x8c(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr) & 0xfd;
      writebyte(tempaddr, z80.h);
      };
    ops[0x8d] = function op_0x8d(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr) & 0xfd;
      writebyte(tempaddr, z80.l);
      };
    ops[0x8e] = function op_0x8e(tempaddr) {
      tstates += 8;
      writebyte(tempaddr, readbyte(tempaddr) & 0xfd);
    };
    ops[0x8f] = function op_0x8f(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr) & 0xfd;
      writebyte(tempaddr, z80.a);
      };
    ops[0x90] = function op_0x90(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr) & 0xfb;
      writebyte(tempaddr, z80.b);
      };
    ops[0x91] = function op_0x91(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr) & 0xfb;
      writebyte(tempaddr, z80.c);
      };
    ops[0x92] = function op_0x92(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr) & 0xfb;
      writebyte(tempaddr, z80.d);
      };
    ops[0x93] = function op_0x93(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr) & 0xfb;
      writebyte(tempaddr, z80.e);
      };
    ops[0x94] = function op_0x94(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr) & 0xfb;
      writebyte(tempaddr, z80.h);
      };
    ops[0x95] = function op_0x95(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr) & 0xfb;
      writebyte(tempaddr, z80.l);
      };
    ops[0x96] = function op_0x96(tempaddr) {
      tstates += 8;
      writebyte(tempaddr, readbyte(tempaddr) & 0xfb);
    };
    ops[0x97] = function op_0x97(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr) & 0xfb;
      writebyte(tempaddr, z80.a);
      };
    ops[0x98] = function op_0x98(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr) & 0xf7;
      writebyte(tempaddr, z80.b);
      };
    ops[0x99] = function op_0x99(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr) & 0xf7;
      writebyte(tempaddr, z80.c);
      };
    ops[0x9a] = function op_0x9a(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr) & 0xf7;
      writebyte(tempaddr, z80.d);
      };
    ops[0x9b] = function op_0x9b(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr) & 0xf7;
      writebyte(tempaddr, z80.e);
      };
    ops[0x9c] = function op_0x9c(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr) & 0xf7;
      writebyte(tempaddr, z80.h);
      };
    ops[0x9d] = function op_0x9d(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr) & 0xf7;
      writebyte(tempaddr, z80.l);
      };
    ops[0x9e] = function op_0x9e(tempaddr) {
      tstates += 8;
      writebyte(tempaddr, readbyte(tempaddr) & 0xf7);
    };
    ops[0x9f] = function op_0x9f(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr) & 0xf7;
      writebyte(tempaddr, z80.a);
      };
    ops[0xa0] = function op_0xa0(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr) & 0xef;
      writebyte(tempaddr, z80.b);
      };
    ops[0xa1] = function op_0xa1(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr) & 0xef;
      writebyte(tempaddr, z80.c);
      };
    ops[0xa2] = function op_0xa2(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr) & 0xef;
      writebyte(tempaddr, z80.d);
      };
    ops[0xa3] = function op_0xa3(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr) & 0xef;
      writebyte(tempaddr, z80.e);
      };
    ops[0xa4] = function op_0xa4(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr) & 0xef;
      writebyte(tempaddr, z80.h);
      };
    ops[0xa5] = function op_0xa5(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr) & 0xef;
      writebyte(tempaddr, z80.l);
      };
    ops[0xa6] = function op_0xa6(tempaddr) {
      tstates += 8;
      writebyte(tempaddr, readbyte(tempaddr) & 0xef);
    };
    ops[0xa7] = function op_0xa7(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr) & 0xef;
      writebyte(tempaddr, z80.a);
      };
    ops[0xa8] = function op_0xa8(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr) & 0xdf;
      writebyte(tempaddr, z80.b);
      };
    ops[0xa9] = function op_0xa9(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr) & 0xdf;
      writebyte(tempaddr, z80.c);
      };
    ops[0xaa] = function op_0xaa(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr) & 0xdf;
      writebyte(tempaddr, z80.d);
      };
    ops[0xab] = function op_0xab(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr) & 0xdf;
      writebyte(tempaddr, z80.e);
      };
    ops[0xac] = function op_0xac(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr) & 0xdf;
      writebyte(tempaddr, z80.h);
      };
    ops[0xad] = function op_0xad(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr) & 0xdf;
      writebyte(tempaddr, z80.l);
      };
    ops[0xae] = function op_0xae(tempaddr) {
      tstates += 8;
      writebyte(tempaddr, readbyte(tempaddr) & 0xdf);
    };
    ops[0xaf] = function op_0xaf(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr) & 0xdf;
      writebyte(tempaddr, z80.a);
      };
    ops[0xb0] = function op_0xb0(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr) & 0xbf;
      writebyte(tempaddr, z80.b);
      };
    ops[0xb1] = function op_0xb1(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr) & 0xbf;
      writebyte(tempaddr, z80.c);
      };
    ops[0xb2] = function op_0xb2(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr) & 0xbf;
      writebyte(tempaddr, z80.d);
      };
    ops[0xb3] = function op_0xb3(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr) & 0xbf;
      writebyte(tempaddr, z80.e);
      };
    ops[0xb4] = function op_0xb4(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr) & 0xbf;
      writebyte(tempaddr, z80.h);
      };
    ops[0xb5] = function op_0xb5(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr) & 0xbf;
      writebyte(tempaddr, z80.l);
      };
    ops[0xb6] = function op_0xb6(tempaddr) {
      tstates += 8;
      writebyte(tempaddr, readbyte(tempaddr) & 0xbf);
    };
    ops[0xb7] = function op_0xb7(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr) & 0xbf;
      writebyte(tempaddr, z80.a);
      };
    ops[0xb8] = function op_0xb8(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr) & 0x7f;
      writebyte(tempaddr, z80.b);
      };
    ops[0xb9] = function op_0xb9(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr) & 0x7f;
      writebyte(tempaddr, z80.c);
      };
    ops[0xba] = function op_0xba(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr) & 0x7f;
      writebyte(tempaddr, z80.d);
      };
    ops[0xbb] = function op_0xbb(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr) & 0x7f;
      writebyte(tempaddr, z80.e);
      };
    ops[0xbc] = function op_0xbc(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr) & 0x7f;
      writebyte(tempaddr, z80.h);
      };
    ops[0xbd] = function op_0xbd(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr) & 0x7f;
      writebyte(tempaddr, z80.l);
      };
    ops[0xbe] = function op_0xbe(tempaddr) {
      tstates += 8;
      writebyte(tempaddr, readbyte(tempaddr) & 0x7f);
    };
    ops[0xbf] = function op_0xbf(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr) & 0x7f;
      writebyte(tempaddr, z80.a);
      };
    ops[0xc0] = function op_0xc0(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr) | 0x01;
      writebyte(tempaddr, z80.b);
      };
    ops[0xc1] = function op_0xc1(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr) | 0x01;
      writebyte(tempaddr, z80.c);
      };
    ops[0xc2] = function op_0xc2(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr) | 0x01;
      writebyte(tempaddr, z80.d);
      };
    ops[0xc3] = function op_0xc3(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr) | 0x01;
      writebyte(tempaddr, z80.e);
      };
    ops[0xc4] = function op_0xc4(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr) | 0x01;
      writebyte(tempaddr, z80.h);
      };
    ops[0xc5] = function op_0xc5(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr) | 0x01;
      writebyte(tempaddr, z80.l);
      };
    ops[0xc6] = function op_0xc6(tempaddr) {
      tstates += 8;
      writebyte(tempaddr, readbyte(tempaddr) | 0x01);
    };
    ops[0xc7] = function op_0xc7(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr) | 0x01;
      writebyte(tempaddr, z80.a);
      };
    ops[0xc8] = function op_0xc8(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr) | 0x02;
      writebyte(tempaddr, z80.b);
      };
    ops[0xc9] = function op_0xc9(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr) | 0x02;
      writebyte(tempaddr, z80.c);
      };
    ops[0xca] = function op_0xca(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr) | 0x02;
      writebyte(tempaddr, z80.d);
      };
    ops[0xcb] = function op_0xcb(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr) | 0x02;
      writebyte(tempaddr, z80.e);
      };
    ops[0xcc] = function op_0xcc(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr) | 0x02;
      writebyte(tempaddr, z80.h);
      };
    ops[0xcd] = function op_0xcd(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr) | 0x02;
      writebyte(tempaddr, z80.l);
      };
    ops[0xce] = function op_0xce(tempaddr) {
      tstates += 8;
      writebyte(tempaddr, readbyte(tempaddr) | 0x02);
    };
    ops[0xcf] = function op_0xcf(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr) | 0x02;
      writebyte(tempaddr, z80.a);
      };
    ops[0xd0] = function op_0xd0(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr) | 0x04;
      writebyte(tempaddr, z80.b);
      };
    ops[0xd1] = function op_0xd1(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr) | 0x04;
      writebyte(tempaddr, z80.c);
      };
    ops[0xd2] = function op_0xd2(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr) | 0x04;
      writebyte(tempaddr, z80.d);
      };
    ops[0xd3] = function op_0xd3(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr) | 0x04;
      writebyte(tempaddr, z80.e);
      };
    ops[0xd4] = function op_0xd4(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr) | 0x04;
      writebyte(tempaddr, z80.h);
      };
    ops[0xd5] = function op_0xd5(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr) | 0x04;
      writebyte(tempaddr, z80.l);
      };
    ops[0xd6] = function op_0xd6(tempaddr) {
      tstates += 8;
      writebyte(tempaddr, readbyte(tempaddr) | 0x04);
    };
    ops[0xd7] = function op_0xd7(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr) | 0x04;
      writebyte(tempaddr, z80.a);
      };
    ops[0xd8] = function op_0xd8(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr) | 0x08;
      writebyte(tempaddr, z80.b);
      };
    ops[0xd9] = function op_0xd9(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr) | 0x08;
      writebyte(tempaddr, z80.c);
      };
    ops[0xda] = function op_0xda(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr) | 0x08;
      writebyte(tempaddr, z80.d);
      };
    ops[0xdb] = function op_0xdb(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr) | 0x08;
      writebyte(tempaddr, z80.e);
      };
    ops[0xdc] = function op_0xdc(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr) | 0x08;
      writebyte(tempaddr, z80.h);
      };
    ops[0xdd] = function op_0xdd(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr) | 0x08;
      writebyte(tempaddr, z80.l);
      };
    ops[0xde] = function op_0xde(tempaddr) {
      tstates += 8;
      writebyte(tempaddr, readbyte(tempaddr) | 0x08);
    };
    ops[0xdf] = function op_0xdf(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr) | 0x08;
      writebyte(tempaddr, z80.a);
      };
    ops[0xe0] = function op_0xe0(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr) | 0x10;
      writebyte(tempaddr, z80.b);
      };
    ops[0xe1] = function op_0xe1(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr) | 0x10;
      writebyte(tempaddr, z80.c);
      };
    ops[0xe2] = function op_0xe2(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr) | 0x10;
      writebyte(tempaddr, z80.d);
      };
    ops[0xe3] = function op_0xe3(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr) | 0x10;
      writebyte(tempaddr, z80.e);
      };
    ops[0xe4] = function op_0xe4(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr) | 0x10;
      writebyte(tempaddr, z80.h);
      };
    ops[0xe5] = function op_0xe5(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr) | 0x10;
      writebyte(tempaddr, z80.l);
      };
    ops[0xe6] = function op_0xe6(tempaddr) {
      tstates += 8;
      writebyte(tempaddr, readbyte(tempaddr) | 0x10);
    };
    ops[0xe7] = function op_0xe7(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr) | 0x10;
      writebyte(tempaddr, z80.a);
      };
    ops[0xe8] = function op_0xe8(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr) | 0x20;
      writebyte(tempaddr, z80.b);
      };
    ops[0xe9] = function op_0xe9(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr) | 0x20;
      writebyte(tempaddr, z80.c);
      };
    ops[0xea] = function op_0xea(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr) | 0x20;
      writebyte(tempaddr, z80.d);
      };
    ops[0xeb] = function op_0xeb(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr) | 0x20;
      writebyte(tempaddr, z80.e);
      };
    ops[0xec] = function op_0xec(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr) | 0x20;
      writebyte(tempaddr, z80.h);
      };
    ops[0xed] = function op_0xed(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr) | 0x20;
      writebyte(tempaddr, z80.l);
      };
    ops[0xee] = function op_0xee(tempaddr) {
      tstates += 8;
      writebyte(tempaddr, readbyte(tempaddr) | 0x20);
    };
    ops[0xef] = function op_0xef(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr) | 0x20;
      writebyte(tempaddr, z80.a);
      };
    ops[0xf0] = function op_0xf0(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr) | 0x40;
      writebyte(tempaddr, z80.b);
      };
    ops[0xf1] = function op_0xf1(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr) | 0x40;
      writebyte(tempaddr, z80.c);
      };
    ops[0xf2] = function op_0xf2(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr) | 0x40;
      writebyte(tempaddr, z80.d);
      };
    ops[0xf3] = function op_0xf3(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr) | 0x40;
      writebyte(tempaddr, z80.e);
      };
    ops[0xf4] = function op_0xf4(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr) | 0x40;
      writebyte(tempaddr, z80.h);
      };
    ops[0xf5] = function op_0xf5(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr) | 0x40;
      writebyte(tempaddr, z80.l);
      };
    ops[0xf6] = function op_0xf6(tempaddr) {
      tstates += 8;
      writebyte(tempaddr, readbyte(tempaddr) | 0x40);
    };
    ops[0xf7] = function op_0xf7(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr) | 0x40;
      writebyte(tempaddr, z80.a);
      };
    ops[0xf8] = function op_0xf8(tempaddr) {
      tstates += 8;
      z80.b=readbyte(tempaddr) | 0x80;
      writebyte(tempaddr, z80.b);
      };
    ops[0xf9] = function op_0xf9(tempaddr) {
      tstates += 8;
      z80.c=readbyte(tempaddr) | 0x80;
      writebyte(tempaddr, z80.c);
      };
    ops[0xfa] = function op_0xfa(tempaddr) {
      tstates += 8;
      z80.d=readbyte(tempaddr) | 0x80;
      writebyte(tempaddr, z80.d);
      };
    ops[0xfb] = function op_0xfb(tempaddr) {
      tstates += 8;
      z80.e=readbyte(tempaddr) | 0x80;
      writebyte(tempaddr, z80.e);
      };
    ops[0xfc] = function op_0xfc(tempaddr) {
      tstates += 8;
      z80.h=readbyte(tempaddr) | 0x80;
      writebyte(tempaddr, z80.h);
      };
    ops[0xfd] = function op_0xfd(tempaddr) {
      tstates += 8;
      z80.l=readbyte(tempaddr) | 0x80;
      writebyte(tempaddr, z80.l);
      };
    ops[0xfe] = function op_0xfe(tempaddr) {
      tstates += 8;
      writebyte(tempaddr, readbyte(tempaddr) | 0x80);
    };
    ops[0xff] = function op_0xff(tempaddr) {
      tstates += 8;
      z80.a=readbyte(tempaddr) | 0x80;
      writebyte(tempaddr, z80.a);
      };
    ops[256] = function() {};
    z80_defaults(ops);
    return ops;
}();
function z80_ddfdcbxx(opcode,tempaddr) { z80_ddfdcb_ops[opcode](tempaddr); }
function z80_do_opcodes(cycleCallback) {
  while (tstates < event_next_event) {
    if (z80.irq_pending && z80.iff1) {
        if (z80.irq_suppress) {
            z80.irq_suppress = false;
        } else {
            z80.irq_suppress = true;
            z80_interrupt();
        }
    }
       var oldTstates = tstates;
    tstates+=4; z80.r = (z80.r+1) & 0x7f;
    var opcode = readbyte(z80.pc);
    z80_instruction_hook(z80.pc, opcode);
    z80.pc = (z80.pc + 1) & 0xffff;
    z80_base_ops[opcode]();
    cycleCallback(tstates - oldTstates);
  }
}
