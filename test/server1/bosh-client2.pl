#!/usr/local/bin/perl

use strict;
use warnings;

use IO::Async::Loop::Epoll;
use IO::Async::Stream;
use IO::Socket::INET;
use IO::Async::Timer::Periodic;

my $login = shift;
my $interval = shift || 1;
my $dump = shift;

my $loop = IO::Async::Loop::Epoll->new();

sub send_msg {
	my $msg = shift;
	my $s = IO::Async::Stream->new(
		handle => (IO::Socket::INET->new(
				PeerHost => "localhost",
				PeerPort => "12345",
				Type     => SOCK_STREAM,
			) || die "Cannot construct socket - $@"),
		on_read => sub {
			my ( $self, $buffref ) = @_;
			while( $$buffref =~ s/^(.*)\r?\n// ) {
				print "$1\n" if $dump;
			}
		},
		on_closed => sub {
			my ( $self ) = @_;
			if($msg eq "check"){
				send_msg($msg);
			}elsif($msg eq "login"){
				send_msg("check");
			}
		},
	);
	$loop->add($s);
	$s->write("$login: $msg\n");
	#print "\t\t\t\tSEND ","$login: $msg\n" if $dump;
}

my $count = 0;
my $timer = IO::Async::Timer::Periodic->new(
    interval => $interval,

    on_tick => sub {
		send_msg("my count = $count ".scalar(localtime));
		if($count++ > 1000){
			$loop->loop_stop;
		}
    },
);
send_msg("login");
#send_msg("check");

$loop->add( $timer );

$timer->start;

$loop->loop_forever();
