#!/usr/local/bin/perl
use strict;
use warnings;

use IO::Async::Loop::Epoll;
use IO::Async::Stream;
#use IO::Async::Function;
use Time::HiRes qw(gettimeofday);

my $PORT = 12345;

my $loop = IO::Async::Loop::Epoll->new;
my $listener = ChatListener->new();

$loop->add( $listener );

$listener->listen(
   queuesize  => 100,
   family   => "inet",
   service  => $PORT,
   socktype => 'stream',
   ip       => "localhost",

   on_resolve_error => sub { die "Cannot resolve - $_[0]\n"; },
   on_listen_error  => sub { die "Cannot listen\n"; },
);

$loop->loop_forever;

package ChatListener;
use base qw(IO::Async::Listener);
use Data::Dumper;

sub on_stream
{
	my $self = shift;
	my ( $stream ) = @_;
	
	$stream->configure(
		on_read => sub {
			my ( $self, $buffref, $eof ) = @_;
			if( $$buffref =~ /\n/ ) {
				$self->write($$buffref.sprintf("accept %02d:%02d:%02d\n",(localtime)[2,1,0]));
				$self->close;
			}
			return 0;
		},
	);
	
	$loop->add( $stream );
}
