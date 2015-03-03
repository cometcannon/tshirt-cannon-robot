package robot;

import java.io.IOException;
import java.io.PrintStream;
import java.io.DataInputStream;

import java.net.Socket;

import manager.RobotState;

public class Networker
{
    final int YUN_PORT = 12313;

    RobotState robotState;

    Socket client;
    PrintStream output;
    DataInputStream input;

    public Networker(RobotState robotState)
    {
        this.robotState = robotState;

        try {
            client = new Socket("192.168.240.1", YUN_PORT);
            input = new DataInputStream(client.getInputStream());
            output = new PrintStream(client.getOutputStream());
        } catch (IOException e) {
            System.out.println(e);
        }
    }

    public synchronized void sendCommand(String type, byte magnitude)
    {
        byte buffer[] = new byte[100];

        int length = 0;

        buffer[length++] = 0x47;
        buffer[length++] = 0x41;
        buffer[length++] = 0x4e;
        buffer[length++] = 0x53;

        if (type.equals("FORWARD")) {
            buffer[length++] = 3;
            buffer[length++] = magnitude;
            buffer[length++] = 0;
            buffer[length++] = 0;
        } else if (type.equals("BACKWARD")) {
            buffer[length++] = 3;
            buffer[length++] = (byte) -magnitude;
            buffer[length++] = 0;
            buffer[length++] = 0;
        } else if (type.equals("LEFT")) {
            buffer[length++] = 3;
            buffer[length++] = 0;
            buffer[length++] = magnitude;
            buffer[length++] = 0;
        } else if (type.equals("RIGHT")) {
            buffer[length++] = 3;
            buffer[length++] = 0;
            buffer[length++] = (byte) -magnitude;
            buffer[length++] = 0;
        } else if (type.equals("ROTATE CW")) {
            buffer[length++] = 3;
            buffer[length++] = 0;
            buffer[length++] = 0;
            buffer[length++] = (byte) -magnitude;
        } else if (type.equals("ROTATE CCW")) {
            buffer[length++] = 3;
            buffer[length++] = 0;
            buffer[length++] = 0;
            buffer[length++] = magnitude;
        } else if (type.equals("MOTOR 0")) {
            buffer[length++] = 4;
            buffer[length++] = 0;
            buffer[length++] = magnitude;
        } else if (type.equals("MOTOR 1")) {
            buffer[length++] = 4;
            buffer[length++] = 1;
            buffer[length++] = magnitude;
        } else if (type.equals("MOTOR 2")) {
            buffer[length++] = 4;
            buffer[length++] = 2;
            buffer[length++] = magnitude;
        } else if (type.equals("MOTOR 3")) {
            buffer[length++] = 4;
            buffer[length++] = 3;
            buffer[length++] = magnitude;
        } else if (type.equals("KILL")) {
            buffer[length++] = 0;
        } else if (type.equals("KEEP ALIVE")) {
            buffer[length++] = 2;
        }

        try {
            output.write(buffer);
        } catch (IOException ex) {
            System.out.println("send failed");
        }
    }
}
