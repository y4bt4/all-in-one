#!/usr/local/bin/perl -w
use strict;

use EV;

use IO::Socket::INET;

my $port = shift || 12345;
my $lsn = new IO::Socket::INET(
	    Listen => 100,
	    LocalHost => "127.0.0.1",
	    LocalPort => $port,
	    ReuseAddr => 1,
	);
my %list;
my $w = EV::io $lsn, EV::READ, sub {
    my ($watcher, $revents) = @_;
    my $new = $lsn->accept;
    unless($new){
	print "accept failed: $!\n";
	return 0;
    }
    my $nw = EV::io $new, EV::READ, sub {
	my ($w1, $r1) = @_;
	my $buf;
	$new->recv($buf,1024);
	if($buf){
	    $buf .= sprintf("accept %02d:%02d:%02d\n\n",(localtime)[2,1,0]);
	    $new->send($buf);
	}
	$new->close;
	delete $list{int $w1};
    };
    $list{int $nw} = $nw;
};
EV::loop;
