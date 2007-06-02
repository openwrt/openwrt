#!/usr/bin/perl

print "struct reg_des {
  char *name;
  unsigned int addr;
} reg_des[] = {\n";
while(<>) {
  if (/define+ (\w+) .*IO_TYPECAST+.*DWORD.*\s+(\w+)/) {
    print "  \"$1\", $2,\n";
  }
}
  print "  NULL, 0
};
";
