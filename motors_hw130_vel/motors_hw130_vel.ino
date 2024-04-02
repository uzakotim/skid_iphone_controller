#include <AFMotor.h>

AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);

// Global variables
float alpha = 1.0;      // Smoothing factor (0 < alpha < 1)
int filtered_left = 0;   // Initialize filtered value
int filtered_right = 0;  // Initialize filtered value
int left_speed = 0;
int right_speed = 0;

void setup()
{
  // Set initial speed of the motor & stop
  motor1.setSpeed(0);
  motor2.setSpeed(0);
  motor3.setSpeed(0);
  motor4.setSpeed(0);

  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);

  // Start serial communication
  Serial.begin(9600);
}

void loop()
{
  if (Serial.available() > 0)
  {                                                       // Check if data is available to read
    String receivedString = Serial.readStringUntil('\n'); // Read the string until newline character
    
    sscanf(receivedString.c_str(), "%d %d", &left_speed, &right_speed); // Parse the integers from the string
    // Update filtered value using EMA
    filtered_left = static_cast<int>(round(alpha * left_speed + (1 - alpha) * filtered_left));
    filtered_right = static_cast<int>(round(alpha * right_speed + (1 - alpha) * filtered_right));
    executeCommand(filtered_left, filtered_right);
    delay(0.1); // delay in milliseconds: 100 microseconds
  }
}
void executeCommand(int left_speed, int right_speed)
{
  // motor 4 forward = forward right
  // motor 3 forward = forward right
  // motor 2 forward = forward left
  // motor 1 forward = forward left

  if (left_speed == right_speed)
  {
    // if speeds are equal

    if (left_speed > 0)
    {
      motor1.setSpeed(left_speed);
      motor2.setSpeed(left_speed);
      motor3.setSpeed(right_speed);
      motor4.setSpeed(right_speed);
      moveMotorLeftForward();
      moveMotorRightForward();
    }
    else if (left_speed < 0)
    {
      motor1.setSpeed(-left_speed);
      motor2.setSpeed(-left_speed);
      motor3.setSpeed(-right_speed);
      motor4.setSpeed(-right_speed);
      moveMotorLeftBackward();
      moveMotorRightBackward();
    }
    else
      stopMotors();
  }
  else
  {
    // if speeds are not equal
    if (left_speed == 0)
    {
      // if left is zero
      stopMotorLeft();
      if (right_speed > 0)
      {
        motor3.setSpeed(right_speed);
        motor4.setSpeed(right_speed);
        moveMotorRightForward();
      }
      else
      {
        motor3.setSpeed(-right_speed);
        motor4.setSpeed(-right_speed);
        moveMotorRightBackward();
      }
    }
    else if (right_speed == 0)
    {
      // if right is zero
      stopMotorRight();
      if (left_speed > 0)
      {
        motor1.setSpeed(left_speed);
        motor2.setSpeed(left_speed);
        moveMotorLeftForward();
      }
      else
      {
        motor1.setSpeed(-left_speed);
        motor2.setSpeed(-left_speed);
        moveMotorLeftBackward();
      }
    }
    else if ((left_speed != 0) && (right_speed != 0))
    {
      // if none is zero
      
      if (left_speed > 0)
      {
        if (right_speed > 0)
        {
          motor1.setSpeed(left_speed);
          motor2.setSpeed(left_speed);
          motor3.setSpeed(right_speed);
          motor4.setSpeed(right_speed);
          moveMotorLeftForward();
          moveMotorRightForward();
        }
        else 
        {
          motor1.setSpeed(left_speed);
          motor2.setSpeed(left_speed);
          motor3.setSpeed(-right_speed);
          motor4.setSpeed(-right_speed);
          moveMotorLeftForward();
          moveMotorRightBackward();
        }
      }
      else if (left_speed < 0)
      {
        if (right_speed>0)
        {
          motor1.setSpeed(-left_speed);
          motor2.setSpeed(-left_speed);
          motor3.setSpeed(right_speed);
          motor4.setSpeed(right_speed);
          moveMotorLeftBackward();
          moveMotorRightForward();
        }
        else 
        {
          motor1.setSpeed(-left_speed);
          motor2.setSpeed(-left_speed);
          motor3.setSpeed(-right_speed);
          motor4.setSpeed(-right_speed);
          moveMotorLeftBackward();
          moveMotorRightBackward();
        }
      }
    }
  }
}

// Code dependent functions on robot version
void moveMotorsForward()
{
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void moveMotorsBackward()
{
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
}

void moveMotorLeftForward()
{
  motor1.run(FORWARD);
  motor2.run(FORWARD);
}

void moveMotorRightForward()
{
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void moveMotorLeftBackward()
{
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
}

void moveMotorRightBackward()
{
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
}

void stopMotors()
{
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}
void stopMotorLeft()
{
  motor1.run(RELEASE);
  motor2.run(RELEASE);
}
void stopMotorRight()
{
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}
