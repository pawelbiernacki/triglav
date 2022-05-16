#!/usr/bin/perl

use strict;
use warnings;
use XML::Parser;

#
# This script gets the following arguments:
#   <databank filename> (mandatory)

# It prints out the databank (an XML file) with the validation items
# assigned to various processors. The amount of processors is read
# from the original databank.

my $databank_filename = shift;

die "$0 <databank filename>" unless defined $databank_filename;

my $amount_of_processors = 1;
my $namespace = "";
my @validation_item=();

my $validation_range_active=0;
my $validation_item_active=0;

sub handle_start
{
    my ($expat, $element, %attr) = @_;    
    
    if ($element =~ /^\w+:machine/)
    {
        $amount_of_processors = $attr{"amount_of_processors"};
        print "<$element ", join(" ", map { "$_=\"$attr{$_}\"" } sort keys %attr), ">";
    }
    elsif ($element =~ /^(\w+):validation_range$/)
    {
        $validation_range_active = 1;      
        $namespace = $1;
                
        print "<$element ", join(" ", map { "$_=\"$attr{$_}\"" } sort keys %attr), ">";
        
    }
    elsif ($element =~ /\w+:validation_item$/)
    {
        $validation_item_active = 1;        
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
        $validation_range_active = 0;
        print map { my $processor=$_%$amount_of_processors; "<$namespace:validation_item processor=\"$processor\">$validation_item[$_]</$namespace:validation_item>\n" } 0..$#validation_item;
        print "</$element>";
    }
    elsif ($element =~ /\w+:validation_item$/)
    {
        $validation_item_active = 0;
    }
    else
    {
        print "</$element>";
    }
}

sub handle_char
{
    my ($expat, $string) = @_;
    
    if ($validation_item_active)
    {
        push @validation_item, $string;
    }
    else
    {
        print "$string";
    }
}

my $p = XML::Parser->new(Handlers => { Start => \&handle_start, End => \&handle_end, Char => \&handle_char });

$p->parsefile($databank_filename);


