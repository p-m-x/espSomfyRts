# espSomfyRts
Somfy Rts and Simu Hz for esp8266

Ported from https://github.com/Nickduino/Somfy_Remote/blob/master/Somfy_Remote.ino

# How to use it
Just make POST request to the device endpoint `/remote` with JSON body 
```
curl -XPOST http://192.168.1.10/remote -d '{"remoteId":1,"cmd":"UP"}'
```
This version of Somfy RTS gate suppport 10 (from #1 to #10) remote devices but i can be simply changed in code. Supported commands are: UP, DOWN, STOP, PROG and any other will cause 400 Bad request response

---
> If you want to learn more about the Somfy RTS protocol, check out [Pushtack](https://pushstack.wordpress.com/somfy-rts-protocol/).

How the hardware works: Connect a 433.42 Mhz RF transmitter to Arduino Pin 5 (or change the pin in the sketch). I couldn't find a 433.42 MHz transmitter so I hacked a remote to send my signals. I then ordered 433.42 MHz crystals to change the regular 433.92 MHz ones I have on my transmitters: that's the cheapest way to do it. Other option would be to use a tunable transmitter (but that hardly looks like the easy way and I'm not a ham radio, so...).

How the software works: What you really want to keep here are the BuildFrame() and SendCommand() procedures. Input the remote address and the rolling code value and you have a remote. With the sketch, you can send the command through serial line but that would be easily modified to button press or whatever (I plan on running it on an internet-connected ESP8266 to shut the blinds at sunset every day).

The rolling code value is stored in the EEPROM, so that you don't loose count of your rolling code after a reset.
