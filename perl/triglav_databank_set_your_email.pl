#!/usr/bin/perl

use strict;
use warnings;
use XML::Parser;

#
# This script gets the following arguments:
#   <databank filename> (mandatory)
#   <email> (mandatory)
#
# It sets the author's email.
#

my $databank_filename = shift;
my $email = shift;

die "$0 <databank filename> <email>" unless defined $email;


sub handle_start
{
    my ($expat, $element, %attr) = @_;    
    
    if ($element =~ /^\w+:author$/)
    {
        $attr{"email"} = $email;
                        
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



 
 
 
 
 
