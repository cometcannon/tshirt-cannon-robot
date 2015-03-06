package robot;

import java.io.IOException;
import java.io.PrintStream;
import java.io.DataInputStream;

import java.net.Socket;

import manager.RobotState;
import robot.command.Command;

public class Networker implements Runnable
{
    final int YUN_PORT = 12313;

    RobotState robotState;

    Socket client;
    PrintStream output;
    DataInputStream input;

    public Networker(RobotState robotState)
    {
        this.robotState = robotState;
    }
    
    @Override
    public void run()
    {
        try {
            client = new Socket("192.168.240.1", YUN_PORT);
            input = new DataInputStream(client.getInputStream());
            output = new PrintStream(client.getOutputStream());
        } catch (IOException e) {
            System.out.println(e);
        }
        
        while (true) {
            if (robotState.isCommandAvailable()) {
                Command cmd = robotState.pollNextCommand();
                byte payload[] = cmd.generatePayload();

                try {
                    output.write(payload); 
                } catch (IOException ex) {
                    System.out.println("send failed");
                }
            }
        }
    }
}
