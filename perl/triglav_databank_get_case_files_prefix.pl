#!/usr/bin/perl

use strict;
use warnings;
use XML::Parser;

#
# This script gets the following arguments:
#   <databank filename> (mandatory)
#
# It prints out the case files prefix.
#

my $databank_filename = shift;

die "$0 <databank filename>" unless defined $databank_filename;


sub handle_start
{
    my ($expat, $element, %attr) = @_;    
    
    if ($element =~ /^\w+:case_files/)
    {
        print $attr{"prefix"}, "\n";
    }
}

sub handle_end
{
    my ($expat, $element) = @_;    
}

sub handle_char
{
    my ($expat, $string) = @_;
}

my $p = XML::Parser->new(Handlers => { Start => \&handle_start, End => \&handle_end, Char => \&handle_char });

$p->parsefile($databank_filename);



 
 
 
