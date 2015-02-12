package robot;

import java.io.IOException;

import java.net.Socket;

import manager.RobotState;

public class Networker
{
    final int YUN_PORT = 12313;

    RobotState robotState;

    public Networker(RobotState robotState)
    {
        this.robotState = robotState;

        Socket client;

        try {
            client = new Socket("192.168.240.1", YUN_PORT);
        } catch (IOException e) {
            System.out.println(e);
        }
    }
}
