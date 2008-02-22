#!/usr/bin/perl -w

$vs = shift;
$vz = shift;
$zd = shift;
$zu = shift;

print "PA;PA;VS$vs;!VZ$vz;!MC1;!PZ$zd,$zu;";

while(<>) {
  m/\[([-.\d]+), ([-\.\d]+)\]/;
  $x = int($1*1000);
  $y = int($2*1000);
  print "PU$x,$y;";
  while($' =~ /\[([-.\d]+), ([-\.\d]+)\]/) {
    $x = int($1*1000);
    $y = int($2*1000);
    print "PD$x,$y;";
  }
}
print "PU1000,1000;";
foreach (1 .. 800) {
  print "!MC0;"
}
print "\n";
