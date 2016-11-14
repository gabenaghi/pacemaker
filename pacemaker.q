//This file was generated from (Commercial) UPPAAL 4.0.14 (rev. 5615), May 2014

/*
URI check. Main part of query is ANDed sequence at the end: we want to see if it's always true that: we haven't just started (avoids failing at first ventricular event), are in interVpace (just received Vpace), we haven't exceeded TURI, and we are not in keyboard mode (keyboard mode lets us pace at any time).
ORed variables negate ANDed sequence except for clk >= TURI, keeping query valid when any one of other expressions isn't true at a given time.
*/
A[] (!uri.interVpace || uri.just_started || keyboard.manual) || (!uri.just_started && uri.interVpace && clk >= TURI && !keyboard.manual)

/*

*/
A[] lri.lri_t <= TLRI

/*

*/
lri.LRI --> lri.ASed || (lri.interApace && lri.lri_t <= TLRI - TAVI)

/*

*/
avi.AVI --> (avi.Idle && avi.avi_t <= TAVI)

/*

*/
A<> (clk >= TURI and clk <= TLRI imply not (speaker.playingLow or speaker.playingHigh))

/*

*/
E<> (clk < TURI and speaker.playingHigh)

/*

*/
E<> (clk > TLRI and speaker.playingLow)

/*

*/
A<> (led_apace.on and signals_received.r_Apace) imply led_apace.off

/*

*/
A<> (led_apace.off and signals_received.r_Apace) imply led_apace.on

/*

*/
A<> (led_vpace.on and signals_received.r_Vpace) imply led_vpace.off

/*

*/
A<> (led_vpace.off and signals_received.r_Vpace) imply led_vpace.on

/*

*/
A<> (led_vget.on and signals_received.r_Vget) imply led_vget.off

/*

*/
A<> (led_vget.off and signals_received.r_Vget) imply led_vget.on

/*

*/
A<> (led_aget.on and signals_received.r_Aget) imply led_aget.off

/*

*/
A<> (led_aget.off and signals_received.r_Aget) imply led_aget.on

/*

*/
A<> (obs_interval == 1000 imply obs_interval == 100)

/*

*/
A<> (obs_interval == 100 imply obs_interval == 1000)

/*

*/
E<> (pvarp.PVARP and (signals_received.r_Aget or signals_received.r_Apace) and not signals_received.r_Vpace)

/*

*/
A<> (vrp.VRP and not signals_received.r_Vpace)

/*

*/
A[] not deadlock
