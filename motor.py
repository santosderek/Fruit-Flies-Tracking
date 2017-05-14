# This file is created by: Ryan Reed
# Formatted by: Derek Santos

from Adafruit_MotorHAT import Adafruit_MotorHAT, Adafruit_DCMotor, Adafruit_StepperMotor
import atexit

class Motor:

        def __init__(self):
                self.hat = Adafruit_MotorHAT(addr = 0x60)
                self.stepper = self.hat.getStepper(1000,1)
                self.stepper.setSpeed(10000)

        def release_motors(self):
                self.hat.getMotor(1).run(Adafruit_MotorHAT.RELEASE)
                self.hat.getMotor(2).run(Adafruit_MotorHAT.RELEASE)
                self.hat.getMotor(3).run(Adafruit_MotorHAT.RELEASE)
                self.hat.getMotor(4).run(Adafruit_MotorHAT.RELEASE)

        def turnOffMotors(self):
                atexit.register(self.release_motors)

        def step(self):
                self.turnOffMotors()
                self.stepper.step(400, Adafruit_MotorHAT.BACKWARD, Adafruit_MotorHAT.DOUBLE)

if __name__ == '__main__':
        myMotor = Motor()
        myMotor.step()
