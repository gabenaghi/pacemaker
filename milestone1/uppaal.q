//This file was generated from (Commercial) UPPAAL 4.0.14 (rev. 5615), May 2014

/*
URI check. Main part of query is ANDed sequence at the end: we want to see if it's always true that: we haven't just started (avoids failing at first ventricular event), are in interVpace (just received Vpace), we haven't exceeded TURI, and we are not in keyboard mode (keyboard mode lets us pace at any time).
ORed variables negate ANDed sequence except for clk >= TURI, keeping query valid when any one of other expressions isn't true at a given time.
*/
A[] (clk >= TURI) || (clk < TURI && (!uri.interVpace || !uri.just_started || !PM_keyboard.manual))

/*

*/
A[] lri.lri_t <= TLRI or (PM_keyboard.manual) or lri.ASed

/*

*/
avi.AVI --> (avi.Idle && avi.avi_t <= TAVI)

/*

*/
avi.AVI --> (signals_received.r_Vpace and avi.avi_t >= TAVI) or not signals_received.r_Vpace

/*

*/
A<> (clk >= TURI and clk <= TLRI imply not (speaker.playingLow or speaker.playingHigh))

/*

*/
(clk < TURI && (signals_received.r_Vpace || signals_received.r_Vget) && !uri.just_started && !PM_keyboard.manual) --> speaker.playingHigh

/*

*/
(led_apace.on and signals_received.r_Apace) --> led_apace.off

/*

*/
(!ledOn[LED_APACE] and signals_received.r_Apace) --> led_apace.turnOn

/*

*/
(ledOn[LED_APACE] and signals_received.r_Apace) --> led_apace.turnOff

/*

*/
(!ledOn[LED_VPACE] and signals_received.r_Vpace) --> led_vpace.turnOn

/*

*/
(ledOn[LED_VPACE] and signals_received.r_Vpace) --> led_vpace.turnOff

/*

*/
(!ledOn[LED_AGET] and signals_received.r_Aget) --> led_aget.turnOn

/*

*/
(ledOn[LED_AGET] and signals_received.r_Aget) --> led_aget.turnOff

/*

*/
(ledOn[LED_VGET] and signals_received.r_Vget) --> led_vget.turnOff

/*

*/
A<> (obs_interval == 1000 imply obs_interval == 100)

/*

*/
A<> (obs_interval == 100 imply obs_interval == 1000)

/*

*/
(pvarp.PVARP and (signals_received.r_Aget or signals_received.r_Apace)) --> (pvarp.t <= TPVARP and not signals_received.r_Vpace) or pvarp.t > TPVARP

/*

*/
(vrp.VRP and signals_received.r_Vget) --> (vrp.t <= TVRP and not vrp.inter) or vrp.t > TVRP

/*

*/
A<> (H_led_HVpace.off and H_signals_received.r_Vsignal) imply H_led_HVpace.on

/*

*/
A<> (H_led_HVpace.on and H_signals_received.r_Vsignal) imply H_led_HVpace.off

/*

*/
A<> (H_led_HApace.on and H_signals_received.r_Asignal) imply H_led_HApace.off

/*

*/
A<> (H_led_HApace.off and H_signals_received.r_Asignal) imply H_led_HApace.on

/*

*/
A<> (H_led_Vpace.on and signals_received.r_Vpace) imply H_led_Vpace.off

/*

*/
A<> (H_led_Vpace.off and signals_received.r_Vpace) imply H_led_Vpace.on

/*

*/
A<> (H_led_Apace.on and signals_received.r_Apace) imply H_led_Apace.off

/*

*/
(H_led_HVpace.off and H_signals_received.r_Vsignal) --> H_led_HVpace.on

/*

*/
A<> (H_led_Apace.off and signals_received.r_Apace) imply H_led_Apace.on

/*

*/
(generator.waiting and H_signals_received.r_Vsignal) --> generator.gotV

/*

*/
(generator.waiting and H_signals_received.r_Apace) --> generator.gotA

/*

*/
(generator.waiting and H_signals_received.r_Vpace) --> generator.gotV

/*

*/
(responder.Manual_V)  --> generator.gotV

/*

*/
(responder.Manual_V)  --> generator.gotV

/*

*/
A[] not deadlock
