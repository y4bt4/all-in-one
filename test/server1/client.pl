#!/usr/local/bin/perl

use strict;
use warnings;

use IO::Async::Loop::Epoll;

use IO::Async::Stream;
use IO::Async::Signal;
use IO::Socket::INET;

my $loop = IO::Async::Loop::Epoll->new();

my $sock = IO::Socket::INET->new(
	#Domain    => AF_INET,
	PeerHost => "localhost",
	PeerPort => "12345",
	Type     => SOCK_STREAM,
) or die "Cannot construct socket - $@";

# my $familyname = ( $sock->sockdomain == AF_INET6 ) ? "IPv6" :
#                  ( $sock->sockdomain == AF_INET  ) ? "IPv4" :
#                                                      "unknown";
#													  
#print "FAMILY $familyname\n";

my $srv = IO::Async::Stream->new(
	handle => $sock,
	on_read => sub {
		my ( $self, $buffref ) = @_;
		while( $$buffref =~ s/^(.*)\r?\n// ) {
			print scalar localtime(),": $1\n";
		}
	},
	on_closed => sub {
         my ( $self ) = @_;
		 print "server closed\n";
		 $loop->loop_stop;
      },
);

$loop->add($srv);


$loop->add( IO::Async::Stream->new(
	read_handle => \*STDIN,
	on_read => sub {
		my ( $self, $buffref ) = @_;
		while( $$buffref =~ s/^(.*)\r?\n// ) {
			print "SEND: $1\n";
			$srv->write("($$)$1\n");
		}
	},
) );

$loop->add( IO::Async::Signal->new(
	name => 'INT',
	on_receipt => sub {
		print "SIGINT, will now quit\n";
		$loop->loop_stop;
	},
) );

$loop->loop_forever();
