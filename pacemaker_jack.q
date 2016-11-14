//This file was generated from (Commercial) UPPAAL 4.0.14 (rev. 5615), May 2014

/*

*/
E<> (clk >= TURI and clk <= TLRI and not (speaker.playingLow or speaker.playingHigh))

/*

*/
E<> (clk < TURI and speaker.playingHigh)

/*

*/
E<> (clk > TLRI and speaker.playingLow)

/*

*/
(led_apace.on and signals_received.r_Apace) --> led_apace.off

/*

*/
(led_apace.off and signals_received.r_Apace) --> led_apace.on

/*

*/
(led_vpace.on and signals_received.r_Vpace) --> led_vpace.off

/*

*/
(led_vpace.off and signals_received.r_Vpace) --> led_vpace.on

/*

*/
(led_vget.on and signals_received.r_Vget) --> led_vget.off

/*

*/
(led_vget.off and signals_received.r_Vget) --> led_vget.on

/*

*/
(led_aget.on and signals_received.r_Aget) --> led_aget.off

/*

*/
(led_aget.off and signals_received.r_Aget) --> led_aget.on

/*

*/
A<> obs_interval == 100

/*

*/
E<> (pvarp.PVARP and (signals_received.r_Aget or signals_received.r_Apace) and not signals_received.r_Vpace)

/*

*/
A<> (vrp.VRP and not signals_received.r_Vpace)

/*

*/
A[] not deadlock
