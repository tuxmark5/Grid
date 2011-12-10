#! /usr/bin/ruby

def gen(dst, src, len)
  adv   = [16 - src, 16 - dst, len + 1].min
  mask  = ((1 << adv) - 1) << src;
  shift = (dst & 15) - (src & 15);
  op    = shift < 0 ? ">>" : "<<";
  puts "    case 0x%04x: *dst |= (*src & 0x%04x) %s %2i; return %2i;" %
    [dst << 8 | src << 4 | len << 0, mask, op, shift.abs, adv];
end;

puts("U2 bitcopy(U2* dst, const U2* src, U2 spec)");
puts("{");
puts("  switch (spec)");
puts("  {");

(0..15).each do |dst|
  (0..15).each do |src|
    (0..15).each do |len|
      gen(dst, src, len);
    end;
  end;
end;

puts("  }");
puts("}");


=begin
def outCpp(vec)
  out = "BitCopy g_copyTable[] =\n{\n";
  idx = 0;
  while idx < vec.length
    out += "  ";
    (0..7).each do |i|
      out += vec[idx] + ", ";
      idx += 1;
    end;
    out += "\n";
  end;
  out += "};\n";
end;

def gen(src, len)
  len   = [64 - src, len].min
  mask  = ((1 << len) - 1) << src;
  "0x%016x, %2i" % [mask, len];
end;

vec = [];

(1..64).each do |len|
  (0..63).each do |src|
    vec << gen(src, len);
  end;
end;

puts outCpp(vec);
=end
