#!/usr/bin/perl

use strict;
use warnings;
use XML::Parser;

#
# This script gets the following arguments:
#   <databank filename> (mandatory)
#   <estimate list filename> (mandatory)
#   <first index> (optional)
#   <last index> (optional)
#
# It prints out the databank (an XML file) with the validation range replaced.
#

my $databank_filename = shift;
my $estimate_list_filename = shift;

die "$0 <databank filename> <estimate_list_filename> [<first_index> <last_index>]" unless defined $estimate_list_filename;

my $first_index = shift;
my $last_index = shift;
my $validation_range_active = 0;
my $validation_item_active = 0;

sub get_estimate_list
{
    my $estimate_list_filename = shift;
    local $/ = undef;
    local *FILE;
    
    open FILE, "<$estimate_list_filename" or die $!;
    my $content = <FILE>;
    close FILE or die $!;
    
    return $content;
}

my @estimate_list = split /\n/, get_estimate_list($estimate_list_filename);

if (!defined($first_index)) { $first_index = 0; }
if (!defined($last_index)) { $last_index = $#estimate_list; }

my @validation_range = @estimate_list[$first_index..$last_index];


sub handle_start
{
    my ($expat, $element, %attr) = @_;    
    
    
    if ($element =~ /^(\w+):validation_range$/)
    {
        $validation_range_active = 1;        
                
        print "<$element ", join(" ", map { "$_=\"$attr{$_}\"" } sort keys %attr), ">";
        
        print map { "<$1:validation_item>$_</$1:validation_item>\n" } @validation_range;        
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

    print "$string" unless $validation_item_active;

}

my $p = XML::Parser->new(Handlers => { Start => \&handle_start, End => \&handle_end, Char => \&handle_char });

$p->parsefile($databank_filename);



