#!/usr/local/bin/perl

use strict;
use warnings;

use IO::Async::Loop::Epoll;

use IO::Async::Stream;
use IO::Socket::INET;
use IO::Async::Timer::Periodic;

my $loop = IO::Async::Loop::Epoll->new();

my $sock = IO::Socket::INET->new(
	PeerHost => "localhost",
	PeerPort => "12345",
	Type     => SOCK_STREAM,
) or die "Cannot construct socket - $@";

my $srv = IO::Async::Stream->new(
	handle => $sock,
	on_read => sub {
		my ( $self, $buffref ) = @_;
		while( $$buffref =~ s/^(.*)\r?\n// ) {
			#print "RECEIV: $1\n";
		}
	},
	on_closed => sub {
		my ( $self ) = @_;
		#print "server closed\n";
		$loop->loop_stop;
	},
);
my $count = 0;
my $timer = IO::Async::Timer::Periodic->new(
    interval => 1,

    on_tick => sub {
		#print "$count\n";
		$sock->write("count($$): $count\n");
		if($count++ > 1000){
			$loop->loop_stop;
		}
    },
 );

$timer->start;

$loop->add($srv);
$loop->add( $timer );

$loop->loop_forever();
