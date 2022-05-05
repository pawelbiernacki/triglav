#!/usr/bin/perl

use strict;
use warnings;
use XML::Parser;

#
# This script gets the following arguments:
#   <databank filename> (mandatory)
#   <case files path> (mandatory)
#
# It sets the case files path.
#

my $databank_filename = shift;
my $case_files_path = shift;

die "$0 <databank filename> <case files path>" unless defined $case_files_path;


sub handle_start
{
    my ($expat, $element, %attr) = @_;    
    
    if ($element =~ /^\w+:case_files$/)
    {
        $attr{"path"} = $case_files_path;
                        
        print "<$element ", join(" ", map { "$_=\"$attr{$_}\"" } sort keys %attr), ">";
    }
    else
    {
        print "<$element ", join(" ", map { "$_=\"$attr{$_}\"" } sort keys %attr), ">";
    }
}

sub handle_end
{
    my ($expat, $element) = @_;    

    print "</$element>";    
}

sub handle_char
{
    my ($expat, $string) = @_;
    
    print "$string";
}

my $p = XML::Parser->new(Handlers => { Start => \&handle_start, End => \&handle_end, Char => \&handle_char });

$p->parsefile($databank_filename);



 
 
 
 
