// C++ code
//


/*
Haus Layout:

              Draußen

##################################
#          #                     #
#    S4    #        W1           #
#          #                     #
#          #                     #
#          #                     #
############        ##############
#          #        #            #
#    B2    #        #    S3      #
#          #        #            #
############        ##############
#          #             Diele   #
#    B1    #                     #
#          #######################
#          #           #         #
#          #     S2    #   S1    #
#          #           #         #
#          #           #         #
#          #           #         #
##################################


Relais Layout:

#########################################
#  1 #  2 #  3 #  4 #  5 #  6 #  7 #  8 #
#########################################
#########################################
# 16 # 15 # 14 # 13 # 12 # 11 # 10 #  9 #
#########################################

#########################################
# 17 # 18 # 19 # 20 # 21 # 22 # 23 # 24 #
#########################################
#########################################
# 32 # 31 # 30 # 29 # 28 # 27 # 26 # 25 #
#########################################


Known Issues:
- Relais 11 ist kaputt. Das wird durch output pin 30 angesteuert.
- Relais 31 ist kaputt. Das wird durch output pin 40 angesteuert.
*/


// ----- START LAMP DEFINITIONS
struct Lamp {  // define the Lamp object:
  int pin_in;  // the input pin of the lamp
  int pin_out;  // the output pin of the lamp
  int led_state;  // the current state of the lamp (HIGH/LOW). Initially LOW.
  String name;  // the name of the lamp (for debug)
};

// define and store all lamps in the lamp array
struct Lamp lamps[] = {
  {63, 22, HIGH, "Licht S2 Re16"},  // Lamp of room S1
  {64, 25, HIGH, "Licht S1 Re7"},  // ...
  {65, 30, HIGH, "Licht S4 Re12"},
  {66, 33, HIGH, "Licht S3 Re3"},
  {67, 36, HIGH, "Licht B1 Re9"},
  //{68, 3, HIGH, "Licht Dusche B2"},  // nicht angeschlossen
  {69, 2, HIGH, "Licht Flur Re25"},
  {14, 48, HIGH, "Licht B2 Re27"},
  {15, 42, HIGH, "Licht W1 Re30"},
  {16, 47, HIGH, "Licht Draußen Re20"}
  //{17, 4, HIGH, "Licht Draußen 1"},  // nicht angeschlossen
  //{18, 5, HIGH, "Licht Draußen 2"}  // nicht angeschlossen
};

// Get the total amount of lamps:
const int NR_LAMPS = sizeof(lamps)/sizeof(lamps[0]);
// ----- END LAMP DEFINITIONS

// START ROLLO DEFINITIONS 
const String ROLLO_STATES[4] = {"stay-down", "motion-down", "stay-up", "motion-up"};
struct Rollo {
  int pin_in;  // the input pin of the Rollo
  int pin_out1;  // the first ouptut pin
  int pin_out2;  // the second output pin
  int state_pointer;  // pointer to the current state of the rollo
  String name;  // The name of the rollo (for debug)
};

struct Rollo rollos[] = {
  {54, 23, 24, 0, "Rollo S2 Re15 (runter) Re8 (stop?)"},  // ...
  {55, 26, 27, 0, "Rollo S1 Re14 (runter) Re6 (hoch)"},
  {56, 28, 29, 0, "Rollo S3 Re5 (runter) Re13 (hoch)"},
  {57, 31, 44, 0, "Rollo W1 Re4 (runter) Re29 (hoch)"},
  {58, 49, 35, 0, "Rollo B1 Re2 (runter) Re19 (hoch)"},
  {59, 37, 50, 0, "Rollo B2 Re26 (runter) Re1 (hoch)"},
  {60, 46, 41, 0, "Rollo S4 Re23 (runter) Re28 (hoch)"}
  //{61, 43, 44, 0, "Rollo W1, 1"},  // nicht angeschlossen
  //{62, 45, 46, 0, "Rollo W1, 2"}  // nicht angeschlossen

};

// Get the total mapunt of rollos:
const int NR_ROLLOS = sizeof(rollos)/sizeof(rollos[0]);

// END ROLLO DEFINITIONS 


void setup(){
  /*
  Setup step:
  - We define the input/output pins.
  
  */
  Serial.begin(9600);
  
  Serial.print("NR_LAMPS=");
  Serial.println(NR_LAMPS);
  Serial.print("NR_ROLLOS=");
  Serial.println(NR_ROLLOS);
  
  
  for (int i = 0; i < NR_LAMPS; i++) {
    pinMode(lamps[i].pin_in, INPUT);
    pinMode(lamps[i].pin_out, OUTPUT);

    digitalWrite(lamps[i].pin_out, HIGH);
  }
  
  for (int i = 0; i < NR_ROLLOS; i++) {
    pinMode(rollos[i].pin_in, INPUT);
    pinMode(rollos[i].pin_out1, OUTPUT);
    pinMode(rollos[i].pin_out2, OUTPUT);

    digitalWrite(rollos[i].pin_out1, HIGH);
    digitalWrite(rollos[i].pin_out2, HIGH);
  }
}


void print_light_event(struct Lamp lamp){
  Serial.print(lamp.name);
  Serial.print(" hat seinen Status verändert.");
  Serial.println();
  
  Serial.print("IN: ");
  Serial.print(lamp.pin_in);
  Serial.print("; OUT: ");
  Serial.print(lamp.pin_out);
  Serial.print(" New state: ");
  Serial.print(lamp.led_state);
  Serial.println();
}


void print_rollo_event(struct Rollo rollo){
  Serial.print(rollo.name);
  Serial.print(" hat seinen Status verändert.");
  Serial.println();
  
  Serial.print("IN: ");
  Serial.print(rollo.pin_in);
  Serial.print("; OUT: ");
  Serial.print(rollo.pin_out1);
  Serial.print(", ");
  Serial.print(rollo.pin_out2);
  Serial.print(" New state: ");
  Serial.print(ROLLO_STATES[rollo.state_pointer]);
  Serial.println();
}


int switch_led_state(struct Lamp lamp){
  if (lamp.led_state == LOW){
    digitalWrite(lamp.pin_out, HIGH);
    return HIGH;
  } else {
    digitalWrite(lamp.pin_out, LOW);
    return LOW;
  }
}


int switch_rollo_state(struct Rollo rollo){
  if (ROLLO_STATES[rollo.state_pointer] == "stay-down"){
    digitalWrite(rollo.pin_out1, HIGH);
    digitalWrite(rollo.pin_out2, LOW);
    int next_state_pointer = (rollo.state_pointer+1) %4;
    return next_state_pointer;
  }
  if (ROLLO_STATES[rollo.state_pointer] == "motion-down"){
    digitalWrite(rollo.pin_out1, HIGH);
    digitalWrite(rollo.pin_out2, HIGH);
    int next_state_pointer = (rollo.state_pointer+1) %4;
    return next_state_pointer;
  }
  if (ROLLO_STATES[rollo.state_pointer] == "stay-up"){
    digitalWrite(rollo.pin_out1, LOW);
    digitalWrite(rollo.pin_out2, HIGH);
    int next_state_pointer = (rollo.state_pointer+1) %4;
    return next_state_pointer;
  }
  if (ROLLO_STATES[rollo.state_pointer] == "motion-up"){
    digitalWrite(rollo.pin_out1, HIGH);
    digitalWrite(rollo.pin_out2, HIGH);
    int next_state_pointer = (rollo.state_pointer+1) %4;
    return next_state_pointer;
  }
  return 0;
}


void loop(){
  for (int i = 0; i < NR_LAMPS; i++) {
    int button_state = digitalRead(lamps[i].pin_in);
    if (button_state == HIGH){
      int new_state = switch_led_state(lamps[i]);
      lamps[i].led_state = new_state;
      print_light_event(lamps[i]);
      delay(500);
    }
  }
  
  for (int i = 0; i < NR_ROLLOS; i++) {
    int button_state = digitalRead(rollos[i].pin_in);
    if (button_state == HIGH){
      int new_state = switch_rollo_state(rollos[i]);
      rollos[i].state_pointer = new_state;
      print_rollo_event(rollos[i]);
      delay(500);
    }
  }
  delay(100);
}
