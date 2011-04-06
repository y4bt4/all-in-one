#!/usr/local/bin/perl -w
use strict;
use warnings;

use IO::Async::Loop::Epoll;
use IO::Async::Stream;
use IO::Socket::INET;
use IO::Async::Timer::Periodic;
use IO::Async::Timer::Countdown;

use Getopt::Long;

my $interval;
my $dump;
my $login;
my $inter;
my $log;
my $per;
my $count = 0;

{
	local $SIG{__WARN__} = sub {die join("\n",@_)};
	GetOptions (
			"interval|i:i"	=> \$interval,    # numeric
			"dump|d"   		=> \$dump,      # string
			"interactive|r"	=> \$inter,      # string
			"login|l:s"		=> \$login,
			"file|f=s"		=> \$log,
			"per|p"			=> \$per,
	) or die "$!";
}
unless($login){
	die "need login";
}
unless($login){
	die "need login";
}
$| =1;
if($log){
	open F,">",$log;
	close F;
}

my $loop = IO::Async::Loop::Epoll->new();

sub send_msg {
	my $msg = shift;
	my $start = time();
	my $ts0 = sprintf "%02d:%02d:%02d", (localtime)[2,1,0];
	my $socket = IO::Socket::INET->new(
				PeerHost => "127.0.0.1",
				PeerPort => 12345,
				Type     => SOCK_STREAM,
			) || die "Cannot construct socket - $@";
	my $end = time();
	my $s = IO::Async::Stream->new(
		handle => $socket,
		#write_all => 1,
		#autoflush  => 1,
		on_read => sub {
			my ( $self, $buffref ) = @_;
			while( $$buffref =~ s/^(.*)\r?\n// ) {
				my $str = "$1\n";
				print $str if $dump;
				if($log){
					open F,">>",$log;
					print F $str;
					close F;
				}
			}
		},
		on_closed => sub {
			my ( $self ) = @_;
			if($msg =~/^check/){
				if($dump){
					my $ts = sprintf "%02d:%02d:%02d", (localtime)[2,1,0];
					print $ts, " ","*" x 20,"\n";
				}
				my $ts = sprintf "%02d:%02d:%02d", (localtime)[2,1,0];
				send_msg("check $ts");
			}elsif($msg eq "login"){
				send_msg("check");
			}
		},
	);
	$loop->add($s);
	my $ts = sprintf "%02d:%02d:%02d", (localtime)[2,1,0];
	$s->write("$login: $msg / $ts\n");
	my $lc;
	if($end - $start > 30){
		$lc = ("*" x 40)."Super Very Long connect ".($end - $start)." ".("*" x 25)."\n";
	}
	elsif($end - $start > 10){
		$lc = ("*" x 30)."Very Long connect ".($end - $start)." ".("*" x 25)."\n";
	}
	elsif($end - $start > 2){
		$lc = ("*" x 20)."Long connect ".($end - $start)." ".("*" x 25)."\n";
	}
	if($lc && $dump){
		print $lc;
		if($log){
			open F,">>",$log;
			print F $ts0," ",$ts," ",$lc;
			close F;
		}
	}
}
sub make_cd {
	my $timer = IO::Async::Timer::Countdown->new(
		delay => $interval,
	
		on_expire => sub {
			my $ts = sprintf "%02d:%02d:%02d", (localtime)[2,1,0];
			send_msg("my count = $count ".$ts);
			if($count++ > 1000){
				$loop->loop_stop;
			}else{
				make_cd();
			}
		},
	);
	$loop->add( $timer );
	$timer->start;
}
sub make_tm {
	my $timer = IO::Async::Timer::Periodic->new(
		interval => $interval,
	
		on_tick => sub {
			my $ts = sprintf "%02d:%02d:%02d", (localtime)[2,1,0];
			send_msg("my count = $count ".$ts);
			if($count++ > 1000){
				$loop->loop_stop;
			}
		},
	);
	$loop->add( $timer );
	$timer->start;
}

if($inter){
	$loop->add( IO::Async::Stream->new(
		read_handle => \*STDIN,
		on_read => sub {
			my ( $self, $buffref ) = @_;
			while( $$buffref =~ s/^(.*)\r?\n// ) {
				my $line = $1;
				my $ts = sprintf "%02d:%02d:%02d", (localtime)[2,1,0];
				send_msg($line." ".$ts);
			}
		},
	) );
	require IO::Async::Signal;
	$loop->add( IO::Async::Signal->new(
		name => 'INT',
		on_receipt => sub {
			print "SIGINT, will now quit\n";
			$loop->loop_stop;
		},
	) );
	$dump = 1;
}

if($interval){
	if($per){
		mke_tm();
	}else{
		make_cd();
	}
}
send_msg("login");


$loop->loop_forever();
