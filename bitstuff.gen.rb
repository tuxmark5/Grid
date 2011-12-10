#! /usr/bin/ruby

def find5(i, zero=false)
  offset = 0;
  count  = zero ? -1000 : 0;
  (0..11).each do |s|
    if i & 1 == 0
      offset = s;
      count  = 0;
    else
      count += 1;
    end;
    i >>= 1;
    return offset if count == 5;
  end;
  return -1;
end;

def toBin(i)
  return i.to_s(2);
  out = "";
  16.times do
    out += (i & 1) == 0 ? "0" : "1";
    i >>= 1;
  end;
  return out;
end;

def insBit(x, i, b)
  start   = ((1 << i) - 1) & x;
  end_    = ((0xFFFF << i) & x) << 1;
  middle  = b << i;
  start | middle | end_;
end;

def delBit(x, i)
  start   = ((1 << i) - 1) & x;
  end_    = ((0xFFFF << (i + 1)) & x) >> 1;
  start | end_;
end;

def outCpp(vec)
  str = "unsigned short vec[] =\n{\n";
  i   = 0;
  while i < vec.length
    str += "  ";
    16.times do
      str += "0x%04x, " % vec[i];
      i += 1;
    end;
    str += "\n";
  end;
  str += "};\n";
  str;
end;

vec0 = [];
vec1 = [];

(0..0x1FFF).each do |i|
  five = find5(i);
  if five == -1
    shift = 8;
    out   = i & 0x1FFF;
  else
    shift = five + 6;
    out   = insBit(i, five, 0);
  end;
  vec0 << [out | ((shift - 6) << 13)];
end;

(0..0x1FFF).each do |i|
  five = find5(i, true);
  if five == -1
    shift = 8;
    out   = i & 0x1FFF;
  else
    shift = five + 6;
    out   = delBit(i, five);
  end;
  vec1 << [out | ((shift - 6) << 13)];
end;

puts outCpp(vec0);
puts "\n";
puts outCpp(vec1);

