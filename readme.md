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

#### Fritzing
I include a schematic in the form of a fritizng-document, showing off how the
actual wiring works. Please note that I couldn't find the fire sensor, so I'm
using a dark LED in order to illustrate it. The rest of it is pretty accurate.

### About some of the sensors 
To keep track of the overall situation locally, we change the color of an
RGB LED connected to the breadboard. The colors depend on the event that is
happening. Orange for fire, red for temperature, green for movement, blue
for water, yellow for light.

#### LM35
To be honest, I don't really care about how much light the plant has gotten,
during the day. I could have measured this out and figured out the exact 
amount through the amount of measures we take during the day, but I was rather
interested in knowing when the highest peak was and what the amount of light
was at that time. Since the weather in Norway doesn't change *that* often,
it's okay to say that this gives a sufficient.

Side note, I would have preferred to use the DHT11 to also measure humidity
but couldn't get the sensor to correctly report data on the Photon, however 
it was working on the Arduino Uno. 

#### Funduino Water Sensor
I would have loved to see this being changed out with some sort of soil
measure senors instead. Right now it's not very efficient to put a cup outside
to measure the amount of rain but it gives the idea. I decided that it should
report the amount of rain in MM, and that's what it does.

#### MPU6050
Specifically, I'm proud of managing the use the MPU6050, this was a bit of a 
challenge as the Photon only produces 3.3V power and we need ~5 to drive it.
Luckily for us, the VIN port is to the rescue and it gives us the amount of
power needed to drive the sensor to it's fullest capacity. I'm, however, a
bit unsure about the actual data's that is being used to "trigger" what is
considered a "movement". We wanna make sure that the plant isn't considered
moved just because we poke around it with a watering can. AKA The values might 
not be considered high enough, but that's alright.

#### Flame sensor
I know this is an unnecessary one, but it was a fun thing to do. The "dings"
keeps track of fire and then also functions as a small fire alarm. 
The dashboard becomes orange if there is a fire in the apartment and it would
be a full on crisis for my plants if this was the case. 

#### A reset
To keep track of correct measurements, I used the built-in Time-object of 
Particle to reset values, should the time be 00:00 in the evening. This way
I make sure that the data is based on the current day and not just a random
time at some point.

### Services
#### Online dashboard
In the folder react-dashboard, you'll find an online dashboard to show the 
potential of using these data. This app is very simple and here we already 
see the cleverness of delivering JSON instead of 9 different variables.
If this had been the case, we would have had to call the API and rely on 
X amount of network requests to finish, in order to keep the dashboard updated.
You can run the dashboard by running  `npm install` followed by `npm run dev`, 
but you need node installed. Keep in mind that the Particle probably won't be online
at the time, so my video shows me using it.

![Online dashboard](https://i.imgur.com/9W9ViVY.png)

#### IFTTT 
I took the liberty to also utilize IFTTT for this task. Since we already
have a fire sensor, we'll send ourselves our an email with the request of 
calling the fire department because... Well.. there is a fire?

However, IFTTT isn't very good at handling JSON, so I needed to expose the 
fire-variable as well.

![IFTTT](https://i.imgur.com/bc2wAIm.png)

### Video
To complete it all, I decided to create a video of the project, located here:
https://www.youtube.com/watch?v=R1UsC-3g-nQ
I know I'm talking in the video and that makes me "exposed", but that's okay.
It was important to explain everything of the project, show off the LED locally
and also show the dashboard being updated with colours as the events were 
happening.

### Self-grading
Overall, I think the project has been a great success. I've managed to 
connect several sensors, both digital and analog and in total there are 5
relevant sensors for the projects + the connection with IFTTT + the online 
dashboard with updated data from the sensors. The project gives flexibility 
because of the JSON values and it functions as a backend should: unaware of 
the frontend or anyone calling it. With this in mind, I think I deserve to
lay a place between an A/B.
