#!/usr/local/bin/perl -w
use strict;

use IO::Epoll;
use IO::Socket::INET;

my $port = shift || 12345;
my $lsn = new IO::Socket::INET(
			Listen => 100,
			LocalHost => "127.0.0.1",
			LocalPort => $port,
			ReuseAddr => 1,
		);
my $epfd = epoll_create(10);

if(epoll_ctl($epfd, EPOLL_CTL_ADD, $lsn->fileno, EPOLLIN) < 0){
	die "epoll_ctl: $!\n";
}
my %fn2fh;

while(my $events = epoll_wait($epfd, 10, -1)){
	foreach my $ev (@$events) {
		if($ev->[0] == $lsn->fileno) {
			# Create a new socket
			my $new = $lsn->accept;
			epoll_ctl($epfd, EPOLL_CTL_ADD, $new->fileno, EPOLLIN);
			$fn2fh{$new->fileno} = $new;
		}elsif(my $fh = $fn2fh{$ev->[0]}){
			my $buf;
			my $rs = $fh->recv($buf,1024);
			if(!$buf || $buf=~/\n/){
				$fh->send($buf.sprintf("accept %02d:%02d:%02d\n",(localtime)[2,1,0]));
				epoll_ctl($epfd, EPOLL_CTL_DEL, $ev->[0], EPOLLIN);
				delete $fn2fh{$ev->[0]};
				$fh->close;
			}elsif(!$rs){
				die "recv error $!";
			}
		}
	}
}

