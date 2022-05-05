#!/usr/bin/perl

use strict;
use warnings;
use XML::Parser;

#
# This script gets a databank filename as an argument.
# It prints out the validation range stored in this file.
#


my $databank_filename = shift;

die "$0 <databank filename>" unless defined $databank_filename;

my $validation_range_active = 0;
my $validation_item_active = 0;

sub handle_start
{
    my ($expat, $element, %attr) = @_;    
    
    
    if ($element =~ /^\w+:validation_range$/)
    {
        $validation_range_active = 1;        
    }    
    elsif ($element =~ /^\w+:validation_item$/)
    {
        $validation_item_active = 1;
    }
}

sub handle_end
{
    my ($expat, $element) = @_;
    
    if ($element =~ /^\w+:validation_range$/)
    {
        $validation_range_active = 0;
    }
    elsif ($element =~ /^\w+:validation_item$/)
    {
        $validation_item_active = 0;
    }
}

sub handle_char
{
    my ($expat, $string) = @_;
    
    if ($validation_range_active && $validation_item_active)
    {
        print "$string\n";
    }
}

my $p = XML::Parser->new(Handlers => { Start => \&handle_start, End => \&handle_end, Char => \&handle_char });

$p->parsefile($databank_filename);

 
