#!/usr/pkg/gnu/bin/perl

# mkfunctmap.pl makes a graph of C functions calls within a set of source
# files.  The graph is in the input language for the DOT filter from AT&T
# Bell Labs.
# (c) copyright 1995 by James Hoagland (hoagland@cs.ucdavis.edu)

# version 1.0.1

#caveats: function call need to be one line
# function header needs to be at the start of a line (no spaces but type okay)
# macros ignored so function calls within them are ignored
# "}" closing a function body needs to be at the start of a line

$DOT = 'dot';
$UCBMAIL = '/usr/ucb/mail';
$SIZE = "10,7.5";
$ORIENT ='landscape';
$PAGE = "8.5,11";
$RATIO = 'normal';
$CONC= 'true';
$MAILTO = "drawdag@toucan.research.att.com";
$LANG = 'ps';
$DOTARGS='';

foreach (@ARGV) {
    ($graphopt{"$1"}=$2, next) if /^-G(\w+)=(.*)/; # process any -Gfoo=bar switches
    ($nodeopt{"$1"}=$2, next) if /^-N(\w+)=(.*)/; # process any -Nfoo=bar switches
    ($edgeopt{"$1"}=$2, next) if /^-E(\w+)=(.*)/; # process any -Efoo=bar switches
    ($MAILTO=$1, next) if /^-mailto=(.*)/; 
    ($LANG=$1, next) if /^-lang[^=]*=(.*)/; 
    &usage, next if (/^-help/);
    eval '$debug=$1||1', next if (/^-d(\d*)$/);
    eval '$'.$1.'=1', next if /^-(debug|mail|dot)$/;
    ($ORIENT='portrait', $SIZE="7.5,10", next) if (/^-port/);
    ($ORIENT='landscape', $SIZE="10,7.5", next) if (/^-land/);
    ($CONC='false', next) if (/^-noco/);
    ($CONC='true', next) if (/^-conc/);
    ($RATIO='fill', next) if (/^-fill$/);
    ($RATIO='normal', next) if (/^-nofi/);
    push(@FILES,$_);
}

# set up output
die "use either -mail or -dot, not both\n" if (defined($dot) && defined($mail));
open(STDOUT,"| $UCBMAIL -s 'dot $DOTARGS -T$LANG' $MAILTO") if defined($mail);
open(STDOUT,"| $DOT $DOTARGS -T$LANG") if defined($dot);


print <<">>";
/* produced by Mkfunctmap.pl version 1.0 */
digraph G {
    concentrate=$CONC;
    center=true;
    fontsize=16;
    orientation=$ORIENT;
    size="$SIZE";
>>

    print "    ratio=$RATIO;\n" if $RATIO ne 'normal';

    foreach (keys %graphopt) {
	print "    $_=$graphopt{$_};		/* specified by -G option */\n";
    }
    foreach (keys %nodeopt) {
	print "    node [$_=$nodeopt{$_}];  	/* specified by -N option */\n";
    }
    foreach (keys %edgeopt) {
	print "    edge [$_=$edgeopt{$_}];  	/* specified by -E option */\n";
    }

foreach $sfile (@FILES) {
    open (SFILE,$sfile);	 
    $infun="";			
    $funsin{$sfile}= "";
    print "    subgraph \"cluster$sfile\" {\n        style=dotted; label=\"$sfile\";\n";
    while (<SFILE>) {
	$_ .= <SFILE> while (s/\\\s*\n$//); # combine any continued lines 
	#print "< $_";
	s/^\s*#.*//; #eat "#" stuff
	s#/\*[^/]+/##g; #eat one-line comments
	s/\\\"//g;		# eat quotes
	s/\"[^\"]+\"/\"\"/g;
	if ($infun eq "") {  # not presently in a function
	    if (s#^([\w][\w\s\*]+)\([^)]\)*#\xff#) {
		$preparen= $1;	
		$preparen =~ s/\s*$//;
		$preparen =~ /(\w+)$/;
		$infun= $1;
		print "        \"$infun\";\n";
		#print "<-$infun->\n";
		$sfileof{$infun}= $sfile;
		$funsin{$sfile}.= "$infun;";
		$calls{$infun}= "";
	    }
	} else {
	    if (s/^\}/*was a fun end here*/) {
		$infun= "";
	    } else {
		while (s/(\w+)(\s*)\(([^\)]*\))/*fun call was here*$2$3/) {
		    $called= $1;
		    if (($called ne "while") && ($called ne "for") && ($called ne "if") && ($called ne "switch") && ($called ne "return")) {
			#print "->$called<-\n";
			$calls{$infun}.= "$called;" if ($calls{$infun} !~ /$called;/);
		    }
		}
	    }
	}
	#print "> $_";
    }
    print "    }\n\n";
}

foreach $sfile (keys %funsin) {    
    foreach $funct (split(/;/,$funsin{$sfile})) {
	#print "/*$calls{$funct}*/\n";
	foreach $called (split(/;/,$calls{$funct})) {
	    #print "/*$funct -?> $called*/\n";
	    if (defined($sfileof{$called})) {
		print "    \"$funct\" -> \"$called\";\n";
	    }
	}
    }
}

    print "}\n";		 
    
sub usage {
    print <<">>";
usage: mkfunctmap.pl { source_file } [-portrait|-landscape] [-concentrate|-noconcentrate] [-fill|-nofill] [{-Ggraph_attr=value}] [{-Nnode_attr=value}] [{-Eedge_attr=value}] [-mail|-dot] [-mailto=email_addr] [-lang=outputlang] [-help]
Most options can be abbreviated to 4 letters
For an explaination of options, see URL:
"http://seclab.cs.ucdavis.edu/mkfunctmap.html"
>>
    exit 1;
}

1;
