#!/usr/local/bin/perl

use strict;
use warnings;

use IO::Async::Loop::Epoll;
use IO::Async::Stream;
#use IO::Socket::IP;
use IO::Async::Function;

our $function = IO::Async::Function->new(
	code => sub {
		my ( $number ) = @_;
		if($number=~/(\d+)/){
			sleep $1 if $1 > 0;
			return "sleeping $1\n";
		}
		return "was simple calling with $number\n";
	},
);
unless($function){
	die "cannot function: $!";
}

my $PORT = 12345;

my $loop = IO::Async::Loop::Epoll->new;

my $listener = ChatListener->new();

$loop->add( $listener );
$loop->add( $function );

$listener->listen(
   family   => "inet",
   service  => $PORT,
   socktype => 'stream',
   ip       => "localhost",

   on_resolve_error => sub { die "Cannot resolve - $_[0]\n"; },
   on_listen_error  => sub { die "Cannot listen\n"; },
);

$loop->loop_forever;

package ChatListener;
use base qw( IO::Async::Listener );
use Data::Dumper;

my @clients;

sub on_stream
{
	my $self = shift;
	my ( $stream ) = @_;
	
	# $socket is just an IO::Socket reference
	my $socket = $stream->read_handle;
	my $peeraddr = 1;
	
	# Inform the others
	$_->write( "$peeraddr joins\n" ) for @clients;
	
	$stream->configure(
		on_read => sub {
			my ( $self, $buffref, $eof ) = @_;
	  
			while( $$buffref =~ s/^(.*\n)// ) {
				my $line = $1;
				if($line=~/call (\w+)/){
					$main::function->call(
						args => [ $1 ],
						on_return => sub {
							my $res = shift;
							print $res;
							$self->write("CALL: $res");
						},
						on_error => sub {
						   print STDERR "Cannot determine if it's prime - $_[0]\n";
						},
					);
				#}elsif($line=~/sleep (\d+)/){
				#	print "sleep $1\n";
				#	sleep $1;
				#	print "after sleep $1\n";
				}
				# eat a line from the stream input
	   
				# Reflect it to all but the stream who wrote it
				#$_ == $self or
				$_->write( "$peeraddr: $line" ) for @clients;
				#print "RECEIV: $peeraddr: $line";
				#my @n = $loop->notifiers;
				#print "current:", scalar(@n),"\n";
			}
			return 0;
		},
	
		on_closed => sub {
			my ( $self ) = @_;
			@clients = grep { $_ != $self } @clients;
	  
			# Inform the others
			$_->write( "$peeraddr leaves\n" ) for @clients;
		},
	);
	
	$loop->add( $stream );
	push @clients, $stream;

	print "!!!!new joined\n";
	my @n = $loop->notifiers;
	print "current:", scalar(@n),"\n";
}
