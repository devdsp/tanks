#!/usr/bin/perl

foreach $file (@ARGV) { 
    $tanks = {};
    open FH, "<$file";
    foreach (<FH>) {
        chomp;
        @tank{"id","path","cause","killer","errorpos","error"} = split;
        $tanks->{$tank{"id"}} = {%tank};
        $paths->{$tank{"path"}}++}; 
        foreach (values %$tanks) {
            next if $_->{"killer"} eq "(nil)";
            next unless $_->{"cause"} eq "shot";
            $data->{$tanks->{$_->{"killer"}}->{"path"}}->{$_->{"path"}}++
        } 
    };
    @paths = keys %$paths;
    print 
        "var names = [",
        (join 
            ",", 
            map {
                open NF, $_."/name";
                $name = <NF>;
                chomp $name;"\"$name\""
            } 
            @paths
        ),
        "];\n";

    print "var colors = [",
        (join 
            ",", 
            map {
                open CF, $_."/color";
                $color = <CF>;
                $color ||= "#c0c0c0";
                chomp $color;
                "\"$color\""
            } @paths
        ),
        "];\n";

    print 
        "var matrix = [\n  [", 
        (join 
            ("],\n  [", 
            map {
                join(
                    ",", 
                    map{$_||=0} @{$data->{$_}}{@paths} 
                )
            } 
            @paths)
        ),"]\n];\n";
