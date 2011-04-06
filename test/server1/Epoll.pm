package Epoll;
use strict;

use IO::Epoll;
use IO::Socket::INET;
use Average;


sub new {
	my $class = shift;
	my %arg = @_;
	my $self = bless \%arg, $class;
	$self->{host} ||= "localhost";
	unless($self->{port}){
		die "need port";
	}
	$self->{lsn} = IO::Socket::INET->new(
			Listen => 100,
			LocalHost => $self->{host},
			LocalPort => $self->{port},
			ReuseAddr => 1,
		);
	unless($self->{lsn}){
		die "Cannot open $self->{host}:$self->{port}: $!";
	}
	$self->{fn2fh} = {};
	$self->{epfd} = epoll_create(10);
	if(!$self->{epfd} || $self->{epfd} == -1){
		die "Cannot create epoll: $!";
	}
	$self->add($self->{lsn});
	$self->{qs} ||= 10;
	$self->{wait} ||= -1;
	$self->{onread} ||= sub{0};
	$self->{later} ||= sub{};
	$self->{onconnect} ||= sub{1};
	$self->{avg} = Average->new(len => 100);
	return $self;
}
sub add {
	my $self = shift;
	my $fh = shift;
	if(epoll_ctl($self->{epfd}, EPOLL_CTL_ADD, $fh->fileno, EPOLLIN) < 0){
		die "epoll_ctl: $!\n";
	}
	$self->{fn2fh}{$fh->fileno} = $fh;
}
sub remove {
	my $self = shift;
	my $fn = shift;
	epoll_ctl($self->{epfd}, EPOLL_CTL_DEL, $fn, EPOLLIN);
	delete $self->{fn2fh}{$fn};
}
sub loop {
	my $self = shift;
	while(my $events = epoll_wait($self->{epfd}, $self->{qs},$self->{wait})){
		$self->{avg}->count(scalar @$events);
		foreach my $ev (@$events) {
			if($ev->[0] == $self->{lsn}->fileno) {
				if(my $new = $self->{lsn}->accept){
					if($self->{onconnect}->($self,$new)){
						$self->add($new);
					}else{
						$new->close;
					}
				}
			}elsif(my $fh = $self->{fn2fh}{$ev->[0]}){
				my $rs = $self->{onread}->($self,$fh);
				unless($rs){
					$self->remove($ev->[0]);
					$fh->close;
				}elsif($rs == -1){
					$self->remove($ev->[0]);
				}
			}
		}
		$self->{later}->($self);
	}
}
1;

