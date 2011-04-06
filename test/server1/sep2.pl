#!/usr/local/bin/perl -w
use strict;

use Epoll;

my $loop = Epoll->new(
	host => "127.0.0.1",
	port => 12345,
	qs => 20,
	#onread => \&pers,
	onread => \&bosh,
	later => sub {
		my $epoll = shift;
		if($epoll->{avg}->check(5)){
			#printf "%d %7.2f %8.2f %.2f [%s]\n", $epoll->{avg}->{count}, $epoll->{avg}->speed, $epoll->{avg}->rate, $epoll->{avg}->avg, join(",",$epoll->{avg}->list_count);
			printf "%d %7.2f %8.2f %.2f\n", $epoll->{avg}->{count}, $epoll->{avg}->speed, $epoll->{avg}->rate, $epoll->{avg}->avg;
		}
	},
);
$loop->loop;

sub bosh{
	my $epoll = shift;
	my $fh = shift;
	my $buf;
	unless(defined $fh->recv($buf,1024)){
		return -1;
	}elsif(!$buf || $buf=~/\n/){
		$fh->send($buf.sprintf("accept %02d:%02d:%02d\n",(localtime)[2,1,0]));
		return 0;
	}
	return 1;
}
sub pers{
	my $epoll = shift;
	my $fh = shift;
	my $buf;
	unless(defined $fh->recv($buf,1024)){
		return -1;
	}elsif(!$buf){
		return -1;
	}elsif($buf=~/\n/){
		$buf .= sprintf("accept %02d:%02d:%02d\n\n",(localtime)[2,1,0]);
		#print "GET: $buf and send answer ",length($buf),"\n";
		$fh->send($buf);
		return 1;
	#}else{
	#	print "GET: $buf\n";
	}
	return 1;
}
