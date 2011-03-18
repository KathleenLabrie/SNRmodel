#!/usr/local/bin/perl -w

$PI = 3.1415926535;

$x0=52;
$y0=48;
$scale=8;
$lsolar = 3.9e26;

$x=shift @ARGV;		#pix
$y=shift @ARGV;		#pix
$feiilum=shift @ARGV;	#W/m2
$diam=shift @ARGV;	#arcsec

$gcd = sqrt( ($y-$y0)*($y-$y0) + ($x-$x0)*($x-$x0) ) * $scale;
$gcl = atan2(($y-$y0),($x-$x0))*180./$PI;
if ( $gcl < 0. ) { $gcl += 360.; }
$diam *= $scale;
$sb = $feiilum*$lsolar/($PI*$diam*$diam/4.);

printf "%.2f  %.2f    %.2f  %.4g\n",$gcd,$gcl,$diam,$sb;

exit(0);
