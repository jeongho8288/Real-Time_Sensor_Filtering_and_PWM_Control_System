# Project conducted in Robotics Laboratory 3

A smart home was developed to enable convenient and practical living independently by using Atmega128.

It includes six functionalities, such as automatically opening doors, detecting room temperature to operate a fan,
and adjusting the brightness and color of an LED lamp based on the surrounding environment.

Each operation processes sensor input by filtering and quantifying the values, 
enabling precise PWM actions corresponding to the respective sensor readings.

The actions implemented for each sensor are as follows:
<img src="https://github.com/user-attachments/assets/f3569603-0330-40da-bb07-b9af23585e0e" alt="Description1" style="width: 60%; height: 350px;">
<hr style="border-top: 3px solid #bbb;">

## System Architecture & Sensors Used

<div style="display: flex; justify-content: space-around; align-items: center;">
  <img src="https://github.com/user-attachments/assets/6f0f1eaf-5175-4fff-8120-060f5205c805" alt="Description1" style="width: 40%; height: 250px; margin-right: 2%;">
  <img src="https://github.com/user-attachments/assets/0c07986e-1012-416b-9ced-7bc3740ee500" alt="Description" alt="Description2" style="width: 40%; height: 250px;">
</div>
<hr style="border-top: 3px solid #bbb;">

## Hardware Images

<div style="display: flex; justify-content: space-around; align-items: center;">
  <img src="https://github.com/user-attachments/assets/c7909233-b382-4147-bc1a-8802bd04d2ca" alt="Description1" style="width: 30%; height: 200px; margin-right: 2%;">
  <img src="https://github.com/user-attachments/assets/c0591b26-37f2-4f97-b80a-e8b3e7575715" alt="Description2" style="width: 30%; height: 200px;">
</div>

<img src="https://github.com/user-attachments/assets/e9cb752c-fcc2-489c-a148-d3ab53af9464" alt="Description1" style="width: 62%; height: 250px;">
<hr style="border-top: 3px solid #bbb;">

## circuit diagram

<div style="display: flex; justify-content: space-around;">
  <img src="https://github.com/user-attachments/assets/4d8ab495-3d44-472a-8ca2-7db1289ccecf" alt="Description1" style="width: 30%; height: 180px; margin-right: 2%;">
  <img src="https://github.com/user-attachments/assets/73936e29-3da8-41cb-9e17-55d9124fc7e4" alt="Description2" style="width: 30%; height: 180px; margin-right: 2%;">
</div>
<img src="https://github.com/user-attachments/assets/f8a9f11e-201d-4eac-870c-fdd188be039d" alt="Description2" style="width: 30%; height: 180px;">
<hr style="border-top: 3px solid #bbb;">

## Sensor Value Filtering

Used Sensors : potentiometor, Photoresistor, Thermistor, PSD sensor, Sound sensor, Rain Detection Sensor




## Problems Encountered During the Project

The robot was equipped with a total of 8 IR sensors, but each sensor experienced significant noise, causing issues in accurately recognizing a horizontal line exactly once when passing over it.
Upon debugging the robot's line detection counts through the DE0 board, it was observed that due to noise, the robot counted tens of values when passing over a single horizontal line.
Initially, an attempt was made to reduce noise by manually adjusting the resistance values of the IR sensors, but the problem persisted.
To address this issue, a method was implemented where sensor values were sampled only during specific time intervals in the control cycle. This approach allowed the robot to correctly recognize horizontal lines.





