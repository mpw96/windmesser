# Windmesser

I need to retract the awning above our terrasse if the wind is too strong.
For that I first need to find out, how strong the wind is.
Then I need to make the wind speed available to my home automation.

Therefore, I need an anemometer from which I can read the windspeed with an ESP8266.


# Hardware

I use this [anemometer](https://funduinoshop.com/elektronische-module/sensoren/bewegung-distanz/windstaerke-messgeraet-anemometer-fuer-arduino).

# Formula

The ADC on the board understands a voltage between 0 and 1 V.
It has a 10-bit resolution, which means there are values between 0 and 1023.
On the board there also a voltage divider to make the A0 pin handle voltage up to 3.2V.
That means that the conversion result must be multiplied with 3.2 to get the actual voltage.

The anemometer creates 1V per 25m/s resp. 1V per 90km/h airspeed.
It can create a max. voltage of 4V, which means 360km/h.
Therefore, I'll leave out the resistor to bring down 4V to 3.2V.
If we have an airspeed of more than 288km/h, the damaged ESP is not the most serious problem I guess.

`airspeed_kmh = voltage_V / 1024.0 * 3.2 * 3.6`
