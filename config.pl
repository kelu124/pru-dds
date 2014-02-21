#!/usr/bin/perl

$tablebits = 13;
# How many bits should be used to address the wavetable?
# The number of samples in the wavetable will be two raised
# to this power, maximum 13.  13=>8192, 12=>4096, etc

$ddsbits = 8;
# how many bits should the DDS output?
# Choose 1 to 8 (perhaps more later)
# (so far only tested with 8)

$cpucycles = 7;
# How many CPU cycles in the DDS main loop? (see dds.p)

$clockrate = 200000000;
# CPU cycles per second of the PRU.
# This shouldn't change unless youre not on a beaglebone black

$prunum = 0;
# Which PRU are we to run the DDS code on? I still need to write
# code to make PRU1 an option, so theres really one choice ...

##############################################################

$tablelen = 1<<$tablebits;  # how many 8-bit samples in our table?
$tableprec = 32-$tablebits; # how many bits left over in skiplen for the fractional part?
$tablemult = 1<<$tableprec; # what I divide skiplen integer by to determine how many samples to jump

open($configh, ">config.h");

print $configh <<ECONFIGH;
// generated by config.pl
// modify the top of that file, not this one

#ifndef _CONFIG_H

#define TABLEBITS ($tablebits)
#define CPUCYCLES ($cpucycles)
#define CLOCKRATE ($clockrate)
#define PRUNUM ($prunum)
#define TABLELEN  ($tablelen) 
#define TABLEPREC ($tableprec)
#define TABLEMULT ($tablemult)

#endif
ECONFIGH
close ($configh);

#this is probably bad form ...

@resources = ("P9.25","P9.27","P9.28","P9.29","P9.30","P9.31","P9.41","P9.42");
@pins = ("0x1ac","0x1a4","0x19c","0x194","0x198","0x190","0x1a8","0x1a0");

open($dts, ">BB-BONE-PRUDDS-1-00A0.dts");
print $dts <<EDTS1;
// generated by config.pl
// modify the top of that file, not this one
/dts-v1/;
/plugin/;

/ {
	compatible = "ti,beaglebone", "ti,beaglebone-black";

	/* identification */
	part-number = "BB-BONE-PRUDDS-1";
	version = "00A0";

	/* state the resources this cape uses */
	exclusive-use =
		/* the pin header uses */
EDTS1
for ($i=0; $i<$ddsbits; $i++) {
  print $dts "		\"$resources[$i]\",\n";
  }
print $dts <<EDTS2;
		/* the hardware IP uses */
		"pru0";

	fragment\@0 {
		target = <&am33xx_pinmux>;
		__overlay__ {

			pru_gpio_pins: pinmux_pru_gpio_pins {
				pinctrl-single,pins = <
EDTS2
for ($i=0; $i<$ddsbits; $i++) {
  print $dts "\t\t$pins[$i] 0x0f\n";
  }
print $dts <<EDTS3;
				>;
			};

			pru_pru_pins: pinmux_pru_pru_pins {
				pinctrl-single,pins = <
EDTS3
for ($i=0; $i<$ddsbits; $i++) {
  print $dts "\t\t$pins[$i] 0x25\n";
  }
print $dts <<EDTS4;
				>;
			};
		};	
	};

	fragment\@2 {
		target = <&pruss>;
		__overlay__ {
			status = "okay";

			pinctrl-names = "default";
			pinctrl-0 = <&pru_pru_pins>;
		};
	};
};
EDTS4
close($dts);