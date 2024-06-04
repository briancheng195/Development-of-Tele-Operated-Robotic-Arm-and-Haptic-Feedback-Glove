# Development-of-Tele-Operated-Robotic-Arm-and-Haptic-Feedback-Glove
- Designed and developed a pneumatic actuation system for haptic feedback glove to emulate the touch senses of tele-operated robotic hand.
- There are two components in this project: fingers sensors of the robotic hand and the pneumatic system of the haptic feedback glove.
  The objective of this project was for the user to experience the force exerted on the fingers of robotic hand when he/she wears the haptic feedback glove. 

# Proof of Concept
<img width="500" alt="image" src="https://github.com/briancheng195/Development-of-Tele-Operated-Robotic-Arm-and-Haptic-Feedback-Glove/assets/122734373/9b8b0724-5012-4e11-abf8-5ac9aa3a13b4">

# Description of Robotic Finger Sensors (LPS25HB) Program
- The robotic finger sensors directly measures the pressure exerted on each finger of the robotic hand
- The sensors are connected to a mutliplexer as it acts as a channel to store each pressure input of each finger sensor and is received within the microcontroller
- The pressure readings of each finger sensor are wirelessly transmitted to the haptic feedback glove through I2C communication (using the nRF24L01 module)

# Electronics Design for Robotic Finger Sensors (Transmitter)
<img width="500" alt="image" src="https://github.com/briancheng195/Development-of-Tele-Operated-Robotic-Arm-and-Haptic-Feedback-Glove/assets/122734373/4c1c6a4f-3edd-4b3c-8019-b480dde4fbce">

# Description of Pneumatic System of Haptic Feedback Glove Program
- The haptic feedback glove acts as a receiver as it receives the pressure readings from the finger sensors through I2C communication (using the nRF24L01 module)
- The pressure readings from each finger sensor (ranging from 0kPa to 12kPa) are mapped linearly to PWM values (0 to 255) for the motor control (L293D)
- The PWM values from the L293D would control the air pump DC motors in operating the pneumatic system of the glove
- The pneumatic system compromises of air pump DC motors and air valves (FA0520E) and there is one of each for every finger of the glove. Each air pump motor is connected to the air valve, and the air valve will be attached to an air sac on each finger of the glove
- The valve acts as a gate to either inflate or deflate the air sac when the DC motor is actuated by the PWM value, or it releases air from the air sac when the motor is not actuated
<img width="200" alt="image" src="https://github.com/briancheng195/Development-of-Tele-Operated-Robotic-Arm-and-Haptic-Feedback-Glove/assets/122734373/64b89397-c4f6-4312-ab56-d3903554e777">
<img width="500" alt="image" src="https://github.com/briancheng195/Development-of-Tele-Operated-Robotic-Arm-and-Haptic-Feedback-Glove/assets/122734373/ce00e40d-c870-433e-b6f1-8a0569d9b1f9">

# Electronics Design for Pneumatic System of Haptic Feedback Glove
<img width="500" alt="image" src="https://github.com/briancheng195/Development-of-Tele-Operated-Robotic-Arm-and-Haptic-Feedback-Glove/assets/122734373/2d19b5af-f786-4ca2-b44c-6a95ca2d3d78">

# Mechanical and Electrical Designs for Robotic Hand with Finger Sensors
<img width="500" alt="image" src="https://github.com/briancheng195/Development-of-Tele-Operated-Robotic-Arm-and-Haptic-Feedback-Glove/assets/122734373/286886a4-6135-44a5-bce3-c8892cd3a341">
<img width="500" alt="image" src="https://github.com/briancheng195/Development-of-Tele-Operated-Robotic-Arm-and-Haptic-Feedback-Glove/assets/122734373/cd91ba12-77e5-4bfe-bdeb-40a3d6b59b8e">

# Mechanical and Electrical Designs for Pneumatic System for Haptic Feedback Glove
<img width="500" alt="image" src="https://github.com/briancheng195/Development-of-Tele-Operated-Robotic-Arm-and-Haptic-Feedback-Glove/assets/122734373/f4aaba6c-112f-44ba-8c25-c3a4be2087a1">
<img width="500" alt="image" src="https://github.com/briancheng195/Development-of-Tele-Operated-Robotic-Arm-and-Haptic-Feedback-Glove/assets/122734373/73ad5775-955e-4885-9826-d82b8d3dcb2b">

# Video Demonstration
https://www.youtube.com/watch?v=OH2SIxybgDM&feature=youtu.be

# Summary
- User is able to feel the force exerted on the finger sensors of the robotic hand when wearing the haptic feedback glove
- When greater pressure is exerted on the finger sensors, the air sacs in the glove inflate largely which emulates greater touch sensations for the user
- When less pressure is exerted on the finger sensors, the air sacs in the glove inflate less which emulates less touch sensations for the user
- When no pressure is exerted on the finger sensors, the air sacs in the glove deflate and the user would not feel any touch sensations

