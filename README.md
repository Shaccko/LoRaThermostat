# LoRaThermostat
A 24V HVAC designed Thermostat built using the STM32 F103C8T6

By Ghanim

This is my first big capstone project, and it was done over the course of my final 
year in computer science. Unfortunately no git commits since I wasn't wise enough
back then to have initialized this project with git, so it's as raw as it gets.

This was made out of necessity, not as a desire to learn (which changed over its duration)
because my thermostat at home was 8 steps away from my room, and I hated having to get up
and change it manually due to people at my home constantly changing it. I thought it would
be nice to have a thermostat with a remote, and coincidentally at the time I was just getting
into my very first introduction to embedded design thats not arduino, with the STM32. I was also
incredibly curious as to how people exactly make the transition from MCU prototype to actual
PCB design.

# Device/Components used
# STM32 F103C8T6
For the MCU, initially I went way overkill and started with the F746ZGT6U family,
which provided 1MB flash and 512kb sram, incredibly overkill with what 
the project intended to be. As I moved to PCB design, this was switched to the F103
family for reduced chip cost, and redo'ing pretty much everything.

# LoRa
This was also my first introduction to LoRa, it was perfect for what my design was
intended for, a transmitter (remote) transmitting at most up to 10 bytes intended
for long range uses. I used a random LoRa library online I found at the time, since
bits were too scary to work with, although going back I definitely wish I'd have
read the LoRa datasheet atleast since it would have shown me different optimization modes
LoRa offers, like sleep mode, receiving a single packet, optimizing current draw etc.

# Si7021/JHD204a
To actually read and display temperature, I used the Si7021 Honeywell module, and the JHD204a
20x4 liquid crystal, controlling both with their own library I found online. 

# Main application design
I wish I did initialize this project with git when I started, because it really would be nice to look
at how disorganized my code first seemed compared to how it is now (relatively, atleast).
Receiver and Transmitter are separated under their own projects. 
Transmitter and receiver both run a state machine and constantly check for LoRa commands from either side.
Transmitter sends a metadata guarded buffer via LoRa to be decoded by receiver. The thermostat goes to sleep
if no commands are received within a duration, the MCU goes to sleep 
by employing STM32's STOP mode, turns off LCD backlight, and wakes up via external interrupts/every 50 seconds. 
STOP mode was configured so as to save as much power as possible during moments of no operations, even though LoRa being in receive continuous mode 
draws moderately more power, the MCU balances its own current draw by going to sleep. 

Temperature is moderated using a 'deadzone' mechanic so as to prevent rapid cool/heat furnace mode which could cause issues. If temperature
is set to cool or heat, but below/above its setpoint, it waits till it crosses setpoint plus or minus its deadzone, then
only it turns on either heat or ac. For the small duration it wakes up, it sends current temperature data back to transmitter,
making sure both modules are in sync. This design philosophy was inspired by observing my current Honeywell thermostat.


# PCB/Schematic Design
This portion took pretty much 60% of the total project time. It was my first introduction to 
pcb design and I wanted to be absolutely 100% sure that I wouldn't mess anything up (due to 
high pcb design cost, atleast from JLCPCB) , and took my time learning the basics of 
electronic design.

# STM32 Schematic
Following Phils Lab intro to KiCad with STM32 guide, I was able to make initial
schematic designs that fit the bare essentials of what a PCB with an embedded MCU would need: crystal,
capacitors, boot modes, and power supply. 

# Transmitter (Remote)
# Transmitter MCU
<img width="1516" height="828" alt="image" src="https://github.com/user-attachments/assets/b35222f3-7274-4174-bc42-e7b8430c0a08" />
A 100n capacitor is installed per VDD pin, as per STM32's MCU design guide. An 8MHz crystal is used, matching what my MCU
prototype was using, and a boot mode switch, which pulls BOOT0 either high or low depending on programming/flash mode.
To program it, we use ST-LINK, 4 male pins are exposed to STM32's SWCLK and SWDIO for programming.
Receiver's MCU looks exact same, only difference is different pinout locations. 

# Power Supply
<img width="975" height="697" alt="image" src="https://github.com/user-attachments/assets/84eea8ac-fa77-4f16-a7aa-59a0535abf91" />
Simple USB-C 5.0V to 3.3V power supply. Uses a cheap LDO regulator, the AMS1117-3.3v to convert 5.0v to 3.3v.

# Components
<img width="1610" height="418" alt="image" src="https://github.com/user-attachments/assets/081fc93b-a7b5-4ef8-9a0a-51ffbe76b409" />
Other than buttons, both LoRa and the JHD204a LCD were substituted with female header pins for budget reasons; I already
had their respective breakout modules at home, so it was a matter of just simply attaching them to their respective headers.

# Receiver (Thermostat)
# Receiver MCU
Same as transmitter.

# Power supply
<img width="1016" height="811" alt="image" src="https://github.com/user-attachments/assets/7f33cb93-1973-49ff-a3dc-28fa80314786" />
Our thermostat wants to be attached to a wall practically, so I went with a 2x Double A battery as input. This in turn is supplied to a 
boost convertor, texas instruments TPS6104 that ramps up voltage to 5.0v, and back to a stable 3.3v using the same LDO regulator to supply 
a stable 3.3v to our circuit. Looking back at it, a separate power supply strategy using batteries should have been used instead,
converting to 5.0v then back to 3.3v is definitely not ideal. Regardless, when I was designing this, I made sure to ensure that I would
achieve atleast 90% efficiency. TPS6104 is designed to be quiescient, meaning minimal battery current usage during stages of low circuit
overall draw. 

# Triac -> SSR for HVAC Routing
<img width="1574" height="779" alt="image" src="https://github.com/user-attachments/assets/d1c781d0-8aaa-48b2-89df-d7b17cc8269e" />
This circuit is responsible for safely sending MCU high/low signals to our HVAC24 system. If heat/cool signals high, voltage reaches
our opto-triac, which in turn signals our triac to open gate and let flow 24V from R line from our furnace. Every half-cycle it checks
if signal is high, and as soon as MCU signals low, on its next half-cycle, the triac will shut its gate down and not let current flow.

Previously, I had a much simpler relay design in mind, where I would use a relay for each temperature pin to signal HVAC to turn heat/cool,
but this was discarded as the current drawage was way too high, and battery would drain very quick, only lasting ~2-3hrs. Our OPTO-Triac consumes
MCU's pin signal current only, consuming at most ~10mA. An accompanying 100k resistor snub is supplied as well, just in-case for unecessary noise
pulling in and accidently opening up our triac gate. 

# Components
<img width="1760" height="743" alt="image" src="https://github.com/user-attachments/assets/1679277a-e327-4407-b484-f3211a6f15f9" />
Same as remote, except for the addition of our Si7021 Temp Sensor. Has two 10k resistors supplied as per its recommended datasheet design.

# PCB Design
I won't be talking much about component placement, I will highlight only core PCB design concepts I followed.
The entire circuit has a back GND copper layer, ideal for low impedance and easy to route GND for components.

# Transmitter (Remote)
<img width="1268" height="870" alt="image" src="https://github.com/user-attachments/assets/180f4b02-9cad-491a-9519-ca83b527e3c7" />

<img width="1339" height="863" alt="image" src="https://github.com/user-attachments/assets/32da50a1-9781-464f-96f6-47f46751cd67" />

Remote battery supply. Get's a 5.0v rating from a USB-C port, supplied to LCD for 5.0v, converted to 3.3v via LDO regulator.

# Receiver (Thermostat)
<img width="962" height="862" alt="image" src="https://github.com/user-attachments/assets/eb7b7e9f-abcb-42c5-8500-a72aa1941394" />


<img width="827" height="716" alt="image" src="https://github.com/user-attachments/assets/93e392b6-fe6c-40e3-9568-f4546f12976f" />

Decoupling caps need to be placed as close as they can be to the circuit it is connected to so as to absorb noise and prevent
high inductance from long loops.

<img width="1021" height="858" alt="image" src="https://github.com/user-attachments/assets/db837116-e938-4fea-9ffa-592ac03b4d73" />

SPI Signals were traced so that they wouldn't come too close to one another, especially SPI_SCK. High MHz signals can cause noise and
crosstalk and they must be separate from other signals.

<img width="767" height="644" alt="image" src="https://github.com/user-attachments/assets/0d7e44b2-e5b7-434e-830b-bef7a762ce48" />

On LoRa's GND connections, a GND copper pour was put in for imepdance control, with plenty of GND via stitching.

<img width="1069" height="862" alt="image" src="https://github.com/user-attachments/assets/0303c601-c7e4-4061-ba4c-23e8bb49ca89" />

Battery supply layout. Nothing special, simply followed TPS6104's example layout.

<img width="955" height="837" alt="image" src="https://github.com/user-attachments/assets/166c0c36-a64c-4dd2-994e-13be67540b6e" />

This terminal takes the 24VAC's cool/heat/R wires. 

# Final Result
Surprisingly, my first PCB order I made was working 100%, both receiver and transmitter. I tested it by actually replacing my in-house
thermostat with this one for a good hour. Temperature was raised, turned off, lowered, heat, cooling, everything was working sound.
I wasn't confident enough however to leave it on for longer time or an entire day, I still trust a commerically designed thermostat
over my amateur one.

# Thoughts
This was a great project to get introduced to pcb design, basic electronics, and high level MCU design. 
I jumped straight to making an LED blink to creating this monster of a project, the learning curve was enormous,
but much was learned, and I am grateful I stuck through it. 


