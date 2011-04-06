#!/usr/local/bin/perl -w
use strict;

use IO::Select;
use IO::Socket::INET;

my $port = shift || 12345;
my $lsn = new IO::Socket::INET(
			Listen => 100,
			LocalHost => "127.0.0.1",
			LocalPort => $port,
			ReuseAddr => 1,
		);
my $sel = new IO::Select( $lsn );

while(my @ready = $sel->can_read) {
	foreach my $fh (@ready) {
		if($fh == $lsn) {
			# Create a new socket
			my $new = $lsn->accept;
			$sel->add($new);
		}else {
			my $buf;
			my $rs = $fh->recv($buf,1024);
			if(!$buf || $buf=~/\n/){
				$fh->send($buf.sprintf("accept %02d:%02d:%02d\n",(localtime)[2,1,0]));
				$sel->remove($fh);
				$fh->close;
			}elsif(!$rs){
				die "recv error $!";
			}
		}
	}
}

