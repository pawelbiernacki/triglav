#!/usr/bin/perl

use strict;
use warnings;
use XML::Parser;

#
# This script gets the following arguments:
#   <databank filename> (mandatory)
#   <amount of processors> (mandatory)
#
# It prints out the databank (an XML file) with the amount of processors replaced.
#

my $databank_filename = shift;
my $amount_of_processors = shift;

die "$0 <databank filename> <amount of processors>" unless defined $amount_of_processors;

my $machine_active = 0;

sub handle_start
{
    my ($expat, $element, %attr) = @_;    
    
    
    if ($element =~ /^\w+:machine$/)
    {
        $machine_active = 1;      
        
        $attr{"amount_of_processors"} = $amount_of_processors;
                        
        print "<$element ", join(" ", map { "$_=\"$attr{$_}\"" } sort keys %attr), ">";
                
    }
    elsif ($element =~ /^(\w+):case_files$/)
    {
        my $namespace = $1;
        
        my $path = $attr{path};
        $path =~ s/\/$//;    
        
        print "<$element ", join(" ", map { "$_=\"$attr{$_}\"" } sort keys %attr), ">";    
                
        for my $number (1..$attr{amount})
        {
            my $name = $path."/".$attr{prefix}.($number-1).".".$attr{extension};
            my $n = ($number-1)%$amount_of_processors;
            print "<$namespace:case_file name=\"$name\" processor=\"$n\"/>\n";
        }
    }
    else
    {
        print "<$element ", join(" ", map { "$_=\"$attr{$_}\"" } sort keys %attr), ">";
    }
}

sub handle_end
{
    my ($expat, $element) = @_;
    
    if ($element =~ /^\w+:validation_range$/)
    {
        $machine_active = 0;
        print "</$element>";
    }
    else
    {
        print "</$element>";
    }
}

sub handle_char
{
    my ($expat, $string) = @_;

    print "$string";
}

my $p = XML::Parser->new(Handlers => { Start => \&handle_start, End => \&handle_end, Char => \&handle_char });

$p->parsefile($databank_filename);



 
