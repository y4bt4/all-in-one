#!usr/local/bin/perl
use strict;
use warnings;

use Time::HiRes qw(usleep);
use Average;

my $c = new Average(len=>100);
my $s = 1000;
$c->count(1);

for(my $i = 0; $i < 100000; $i++){
	usleep $s;
	my $v = 20 + int rand 10;
	$c->count($v);
	#printf "v=%d c=%d t=%.3f s=%.2f r=%.2f a=%.2f\n",$v,$c->{count},$c->{time},$c->speed,$c->rate,$c->avg if $c->check(2);
	printf "v=%d c=%d t=%.3f s=%.2f r=%.2f a=%.2f\n",$v,$c->{count},$c->{time},$c->speed,$c->rate,$c->avg if $c->check(2);
}
