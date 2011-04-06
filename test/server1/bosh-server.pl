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
my %client;
my %waiting;
my @messages;
my $shift = 0;
my $last_len = 0;
my $later = 0;
my $waiter = 0;

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

my $sm = 0;
if(0){
	$loop->later( \&later );
	$loop->loop_forever;
}else{
	my $or = gettimeofday;;
	for(;;){
		$sm = 0;
		my $r0 = gettimeofday;
		my $c = $loop->loop_once(1);
		my $r1 = gettimeofday;
		my $lp = $r1-$r0;
		
		my $lpsm = $sm;
		$sm = 0;

		my $old = sprintf(
				"shift=%d messages=%d waiter=%d last_len=%d,later_work=%d\n",
				$shift,scalar(@messages),$waiter,$last_len,$later);


		$r1 = gettimeofday;
		later("direct");
		my $r2 = gettimeofday;
		my $lt = $r2-$r1;
		my $ltsm = $sm;
		
		if($lt > 0.5){
			my $new = sprintf(
				"shift=%d messages=%d waiter=%d last_len=%d,later_work=%d\n",
				$shift,scalar(@messages),$waiter,$last_len,$later);
			print $lt,"\n";
			print $old;
			print $new;
		}
		if($lp > 2){
			print "loop $c $lp\n";
		}
		if($r0 - $or > 2){
			print "r=$c loop=$lp later=$lt\n";
		}
		#if($c && $c > 0 && ($ltsm > 0 || $lpsm > 0)){
		#	print "c=$c loop_sm=$lpsm later_sm=$ltsm lt=$lt lp=$lp\n";
		#}
		$or = $r0;
	}
}
sub later {
	my $direct = shift;
	my @w = keys %waiting;
	my @delete;
	$waiter = @w;
	my $old_later = $later;
	my $old_last = $last_len;
	$later = 0;
	$last_len = 0;
	for my $w(@w){
		my $o = $waiting{$w};
		unless((my $i = $client{$o->{login}} - $shift) == @messages){
			my $msg = "";
			my $c = @messages - $i;
			if($last_len < $c){
				$last_len = $c;
			}
			for(; $i < @messages; $i++){
				#$msg .= $messages[$i];
				$sm++;
			}
			my $result = "from later $c $o->{check} $o->{ask} ".sprintf("%02d:%02d:%02d", (localtime)[2,1,0])."\n";
			$msg = $result;
			#if($o->{login} eq "first"){
			#	print $result;
			#}
			$o->{stream}->write($msg);
			$client{$o->{login}} = $shift + @messages;
			push @delete,$w;
			$o->{stream}->close;
			$later++;
		}
	}
	delete @waiting{@delete};
	if(@messages > 20000){
		splice @messages,0,2000;
		$shift += 2000;
	}
	unless($later){
		$later = $old_later;
	}
	unless($last_len){
		$last_len = $old_last;
	}
	$loop->later( \&later ) unless $direct;
}


package ChatListener;
use base qw(IO::Async::Listener);
use Data::Dumper;

sub on_stream
{
	my $self = shift;
	my ( $stream ) = @_;
	#print "stream create\n";
	
	$stream->configure(
		#write_all => 1,
		#autoflush  => 1,
		on_read => sub {
			my ( $self, $buffref, $eof ) = @_;
	  
			my $login;
			my $check;
			while( $$buffref =~ s/^(.*\n)// ) {
				my $line = $1;
				if($line=~/^(\w+): (.*)/){
					$login = $1;
					my $rest = $2;
					if($rest =~/^login/){
						push @messages,"$login logged\n";
						$client{$login} = @messages + $shift;
						$self->write("welcome\n");
						my @n = $loop->notifiers;
						printf "%02d:%02d:%02d %s logged total=%d\n",(localtime)[2,1,0],$login,scalar(@n);
					}elsif(defined $client{$login}){
						if($rest =~/^stat\b/){
							$self->write(sprintf(
								"shift=%d messages=%d waiter=%d last_len=%d,later_work=%d\n",
								$shift,scalar(@messages),$waiter,$last_len,$later));
						}elsif($rest =~/^check\b/){
							if($rest =~ /\/ (\d+:\d+:\d+)/){
								$check  = $1;
							}else{
								$check  = 1;
							}
							if($login eq "first"){
								printf "$rest ASK %02d:%02d:%02d\n", (localtime)[2,1,0];
							}
						}else{
							#print "message: $line";
							push @messages,sprintf "%02d:%02d:%02d %s", (localtime)[2,1,0],$line;
							$self->write("accepted\n");
						}
					}else{
						$self->write("denied\n");
						last;
					}
				}
			}
			if($check){
				if($client{$login} - $shift == @messages){
					$waiting{$self} = {
								stream => $self,
								login => $login,
								check => $check,
								ask =>  sprintf("%02d:%02d:%02d", (localtime)[2,1,0]),
								};
					return 0;
				}
				if($client{$login} - $shift < 0 ){
					$client{$login} = $shift;
				}
				my $msg = "";
				my $c = @messages - ($client{$login} - $shift);
				for(my $i = $client{$login} - $shift; $i < @messages; $i++){
					#$msg .= $messages[$i];
					$sm++;
				}
				my $result = sprintf("from loop %d %s %02d:%02d:%02d\n",$c,$check,(localtime)[2,1,0]);
				$msg = $result;
				#if($login eq "first"){
				#	print $result;
				#}
				$self->write($msg);
				$client{$login} = @messages + $shift;
			}
			$self->close;
			return 0;
		},
		on_closed => sub {
			my ($self) = @_;
			delete $waiting{$self};
		},
		on_write_error => sub {
			my ($self,$errno) = @_;
			delete $waiting{$self};
		},
	);
	
	$loop->add( $stream );
}
