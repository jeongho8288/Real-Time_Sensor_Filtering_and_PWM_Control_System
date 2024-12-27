# Project conducted in Robotics Laboratory 3
The project was conducted using the DE0 board, and programming was carried out in VHDL using Altera Quartus.

The robot was equipped with 8 IR sensors, but in the actual implementation, 
only 6 IR sensor data were utilized to compensate for sensor errors and achieve the final implementation.

The robot achieved a record of 21.57 seconds, securing first place in the class.

<img src="https://github.com/user-attachments/assets/1e478383-bd30-44f2-bc8d-743a14ff60df" alt="Description" style="width: 60%;">
<hr style="border-top: 3px solid #bbb;">



## System Architecture & Sensors Used
<div style="display: flex; justify-content: space-around; align-items: center;">
  <img src="https://github.com/user-attachments/assets/6f0f1eaf-5175-4fff-8120-060f5205c805" alt="Description1" style="width: 40%; height: 250px; margin-right: 2%;">
  <img src="https://github.com/user-attachments/assets/0c07986e-1012-416b-9ced-7bc3740ee500" alt="Description" alt="Description2" style="width: 40%; height: 250px;">
</div>
<hr style="border-top: 3px solid #bbb;">



## Robot&Map Images

- For a single horizontal line, the robot is programmed to stop.
- For two thin horizontal lines, the robot implements a traffic light signal.
- For two thick horizontal lines, the robot activates an acceleration zone.
<div style="display: flex; justify-content: space-around; align-items: center;">
  <img src="https://github.com/user-attachments/assets/af79d44b-dcf5-4a49-9904-48fe3d5580b0" alt="Description1" style="width: 30%; height: 200px; margin-right: 2%;">
  <img src="https://github.com/user-attachments/assets/55896f91-41db-4f25-b69a-6841159d8ef9" alt="Description2" style="width: 30%; height: 200px;">
</div>

<div style="display: flex; justify-content: space-around;">
  <img src="https://github.com/user-attachments/assets/4680cdc7-4af4-4007-b154-bf3fa56a96f1" alt="Description1" style="width: 20%; height: 180px; margin-right: 2%;">
  <img src="https://github.com/user-attachments/assets/91b677ab-0271-45ea-9167-e0a290b641f6" alt="Description2" style="width: 20%; height: 180px; margin-right: 2%;">
  <img src="https://github.com/user-attachments/assets/61cd6b32-37fb-4d91-9aac-43c0cf3c059e" alt="Description3" style="width: 20%; height: 180px;">
</div>
<hr style="border-top: 3px solid #bbb;">

## circuit diagram
sensor
<div style="display: flex; justify-content: space-around; align-items: center;">
  <img src="https://github.com/user-attachments/assets/ee20c8ae-67f8-4024-8af2-1539c9a86ff0" alt="Description1" style="width: 30%; height: 200px; margin-right: 2%;">
  <img src="https://github.com/user-attachments/assets/d49f79ce-e576-40b6-acaf-c33a93a22274" alt="Description2" style="width: 30%; height: 200px;">
</div>
motor
<div style="display: flex; justify-content: space-around; align-items: center;">
  <img src="https://github.com/user-attachments/assets/67cdebfb-39a8-4de3-bccf-d878ee270114" alt="Description1" style="width: 30%; height: 200px; margin-right: 2%;">
  <img src="https://github.com/user-attachments/assets/f49f7ea7-b9e6-47e8-86f6-542e6a35ee61" alt="Description2" style="width: 30%; height: 200px;">
</div>
<hr style="border-top: 3px solid #bbb;">


## Problems Encountered During the Project

The robot was equipped with a total of 8 IR sensors, but each sensor experienced significant noise, causing issues in accurately recognizing a horizontal line exactly once when passing over it.
Upon debugging the robot's line detection counts through the DE0 board, it was observed that due to noise, the robot counted tens of values when passing over a single horizontal line.
Initially, an attempt was made to reduce noise by manually adjusting the resistance values of the IR sensors, but the problem persisted.
To address this issue, a method was implemented where sensor values were sampled only during specific time intervals in the control cycle. This approach allowed the robot to correctly recognize horizontal lines.





