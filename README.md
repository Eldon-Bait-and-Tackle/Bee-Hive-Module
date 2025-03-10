# Bee-Hive-Module

 ### Video

 [Video](https://www.example.com)

### Module Schematics 

Here is the schematic for the project. There are a few notes. For our final sensors, we are considering using a humidity and temperature sensor separately as we have come into ownership of a large number of DHT11 sensors and it would be a large cost saving for us, This however wouldn't require a change in the wiring diagram and you could keep the input pins the same. Another thing is that we are still deciding on the actual Arduino module. I have found another module that has a built-in battery charger and voltage regulator, which would simplify the solar charging mechanism, but has a larger associated cost. In the future, I will update the diagram or include it in a separate release.
![The schematic for the project](schematic.png)
![Wiring diagram](wire_diagram.png)

### Ongoing Development

As I am currently not home and can't solve some of these problems from my apartment, I have a few specific goals that I am going to try to solve in the future.

- Quick attachment and detatchment or some coupling for the module to be placed on a hive. We have decided on a place for it, being on the uppermost hive box, attached to the outside with a small nonintrusive hole for the wires for the sensors. Once we get the chance, I will have photos with examples.
- We have a working model for the inside enclosure for the sensors, although they haven't been fully tested yet, so we will need to complete that. As mentioned above, I will have photos when I get back, and I can do a test deployment. I will also try to get some of the CAD files for the 3d printer.
- My next project will be the web server, so I will have more included details listed there for the project goals.
- Right now, we are having an issue with the 8266 chips that we had on hand, and we are thinking about changing them to the esp32 chips. We are having a problem were we cant easily connect to wifi networks without a specific setting, making it really hard to test when I don't have control over my network.

### Future Ideas

- Using the ESP32 chip and making a zigbee network instead of a wifi network. This would help with the battery life, and for hives could be daisy-chained to allow for more dynamic setups. This would also fix our current issue of having hives that don't have wifi access but would be close enough to another hive that might. The main problem is that we have a 100-acre property that would be the main focus for this addition.




### Useful Websites & Resources
