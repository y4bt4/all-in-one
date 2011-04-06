#!/usr/local/bin/perl -w
use strict;

use Time::HiRes qw(gettimeofday);

my %h;

my $str="";
my $r0=0.0;
my $r1=0.0;

my $count = shift || 10000;

$r0 = gettimeofday;
for(my $i = 0; $i < $count; $i++){
	$str = sprintf("%010d",$i);
	$h{$str} = $str;
}
$r1 = gettimeofday;
printf "%s %f %f\n",scalar(%h),$r1-$r0,$count/($r1-$r0);

$r0 = gettimeofday;
for(my $i = 0; $i < $count; $i++){
	$str = sprintf("%010d",$i);
	$h{$str} = $str;
}
$r1 = gettimeofday;
printf "%s %f %f\n",scalar(%h),$r1-$r0,$count/($r1-$r0);
