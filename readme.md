## PlantProtector - PG5500

### Background
In our subject, Embedded Systems (PG5500) at Kristiania University College,
we got tasked with creating a "dings" that would either be of entertainment
or use. The requirement for the "dings" was that it needed to be connected
to the internet using a Particle Photon. One of my biggest issues in my 
apartment is that my plants keeps dying on me, either because I forget to
water them, because they get too much sun light, too warm, or that somebody
knocks it over, etc. I've decided to solve this issue by making a 
PlantProtector which reports information about the plant on a dashboard and locally through a LED

### Wiring
The project consists of many moving parts and one of the things i realised 
very early was that each sensor exposes a new variable, giving a new point of
attack for any middleman of the IoT-device. To keep this nice and tidy, I
reworked the variables and exposed it all in a JSON object instead. This
JSON object is loaded into a buffer and is updated IF 1) The sensor data
changes or IF 2) It has passed 5 since the last update. Originally I just 
wanted to keep track of whenever data changes, but this isn't fast enough,
and neither reports "updated" data for the light sensor and the temperature.


// Nevne tid og nevne json 

#### Parts used
* Particle Photon, of course
* LM35 to keep track of temperature.
* Photoresistor to track light.
* Flame sensor to detect fire around the plant
* MPU6050s accelerometer and gyroscope to keep track of movement (nobody's 
knocking my plant over!)
* Funduino water sensor to track water measurement in a cup (wish I had a soil
measurement instead)
* 220k resistors
* Lots of wires
* An RGB led to report about the latest event locally.

Side note, I would have preferred to use the DHT11 to also measure humidity
but couldn't get the sensor to correctly report data on the Photon, however 
it was working on the Arduino Uno. 

### Online dashboard
In the folder react-dashboard, you'll find an online dashboard to show the 
potential of using these data.

### Video

### Self-grading
