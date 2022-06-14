#!/usr/bin/perl

use strict;
use warnings;
use XML::Parser;

#
# This script gets the following arguments:
#   <databank filename> (mandatory)
#   <prefix> (mandatory)
#   <depth> (mandatory)
#
# It prints out the databank (an XML file) with the precalculated files names.
#

my $databank_filename = shift;
my $prefix = shift;
my $depth = shift;

die "$0 <databank filename> <prefix> <depth>" unless defined $depth;


my $case_files_active = 0;
my @precalculated_files = ({prefix => $prefix, depth => $depth, done=>"false"});

sub handle_start
{
    my ($expat, $element, %attr) = @_;    
    
    
    if ($element =~ /^(\w+):case_files$/)
    {
        $case_files_active = 1;        
                
        print "<$element ", join(" ", map { "$_=\"$attr{$_}\"" } sort keys %attr), ">";
                        
    }
    elsif ($element =~ /\w+:precalculated_files$/)
    {
        push @precalculated_files, { prefix => $attr{prefix}, depth => $attr{depth}, done => $attr{done} };
    }
    else
    {
        print "<$element ", join(" ", map { "$_=\"$attr{$_}\"" } sort keys %attr), ">";
    }
}

sub handle_end
{
    my ($expat, $element) = @_;
    
    if ($element =~ /^(\w+):case_files$/)
    {
        $case_files_active = 0;
        
        print "<$1:precalculated_files ", join(" ", map { "prefix=\"$$_{prefix}\" depth=\"$$_{depth}\" done=\"$$_{done}\"" } 
            sort { $$a{depth} <=> $$b{depth} } @precalculated_files), "/>\n";
            
        print "</$element>";
    }
    elsif ($element =~ /\w+:precalculated_files$/)
    {        
    }
    else
    {
        print "</$element>";
    }
}

sub handle_char
{
    my ($expat, $string) = @_;

    print "$string" unless $case_files_active;
}

my $p = XML::Parser->new(Handlers => { Start => \&handle_start, End => \&handle_end, Char => \&handle_char });

$p->parsefile($databank_filename);



