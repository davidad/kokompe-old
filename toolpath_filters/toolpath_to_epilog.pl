#!/usr/bin/perl

$numargs = @ARGV;
if($numargs<3) {die "Usage: {invoke} rate power speed < toolpath > foo.epi";}

$rate=shift;
$power=shift;
$speed=shift;

print "%-12345X@PJL JOB NAME=boundaries\n";
print "E@PJL ENTER LANGUAGE=PCL\n";
print "&y1A&l0U&l0Z&u600D*p0X";
print "*p0Y*t600R*r0F&y50P&z50S";
print "*r6600T*r5100S*r1A*rC%1BIN;";
print "XR$rate;YP$power;ZS$speed;";

while(<>) {
    s/\{\[/PU/g;
    s/\],\s?\[/;PD/g;
    s/\]\}/;/g;
    s/,\s?/,/g;
    chomp;
    print;
}

print "%0B%1BPUE%-12345X@PJL EOJ ";
