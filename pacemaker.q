//This file was generated from (Commercial) UPPAAL 4.0.14 (rev. 5615), May 2014

/*
URI check. Main part of query is ANDed sequence at the end: we want to see if it's always true that: we haven't just started (avoids failing at first ventricular event), are in interVpace (just received Vpace), we haven't exceeded TURI, and we are not in keyboard mode (keyboard mode lets us pace at any time).
ORed variables negate ANDed sequence except for clk >= TURI, keeping query valid when any one of other expressions isn't true at a given time.
*/
A[] (!uri.interVpace || uri.just_started || keyboard.manual) || (!uri.just_started && uri.interVpace && clk >= TURI && !keyboard.manual)

/*

*/
avi.AVI --> (avi.Idle && avi.avi_t <= TAVI)

/*

*/
lri.LRI --> lri.ASed || (lri.interApace && lri.lri_t <= TLRI - TAVI)

/*

*/
A[] lri.lri_t <= TLRI

/*

*/
A[] not deadlock
