A laboratory exercise for the Public mobile network course.
A simple OMNeT++ simulation of a LTE mobile network following the process described by 3GPP: https://www.rfc-editor.org/rfc/rfc6459#section-6.3

The purpose of this was to learn OMNeT++ basics and see the basics of who communicates with who in order to establish an internet connection with a 4G capable device. The only realistic thing aimed for was the order of the messages sent and received, the rest has not, I assume and am pretty certain, been implemented even close to how it works in reality.

Functionalities of nodes such as assigning an IP adress, authenticating users, etc. are symbolically represented by a separate function call which in fact does nothing special, logs the message and returns, pretty much.

However, by making it so, it is now easier to potentially upgrade some specific functionalites of the nodes if I needed to do so in the future (but I probably won't).