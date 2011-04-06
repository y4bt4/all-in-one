package Average;
use strict;
use warnings;

use Time::HiRes qw(gettimeofday);

sub new {
    my $class = shift;
	my %args = @_;
    my $self = bless \%args, $class;
	$self->{len} ||= 10;
	$self->{count} = 0;
	$self->{time} = 0;
	$self->{ary} = [{count => 0, time => scalar gettimeofday(),diff => 0}];
    return $self;
}
sub count {
	my $self = shift;
	my $add = shift || 1 ;

	my $now = gettimeofday();
	my $prev = $self->{ary}[-1]{time};
	my $diff = $now - $prev;

	my $tmp = {count => $add, time => $now, diff => $diff};

	push @{$self->{ary}},$tmp;
	$self->{count} += $add;
	$self->{time} += $diff;
	
	if(@{$self->{ary}} > $self->{len}){
		$tmp = shift @{$self->{ary}};
		$self->{count} -= $tmp->{count};
		$self->{time} -= $tmp->{diff};
	}
}
sub check {
	my $self = shift;
	my $to = shift || 1;
	my $now = gettimeofday();
	if(($self->{last}||=$now) + $to > $now){
		return 0;
	}
	$self->{last} = $now;
}

sub speed {
	my $self = shift;
	return $self->{count}/($self->{time}||1);
}
sub rate {
	my $self = shift;
	return @{$self->{ary}}/($self->{time}||1);
}
sub avg {
	my $self = shift;
	return $self->{count}/@{$self->{ary}};
}
sub list_count {
	my $self = shift;
	return map {$_->{count}} @{$self->{ary}};
}
sub list_diff {
	my $self = shift;
	return map {$_->{diff}} @{$self->{ary}};
}
1;
