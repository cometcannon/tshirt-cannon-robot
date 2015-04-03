package edu.utdallas.cometcannon.robot;

import java.io.*;
import java.net.*;
import java.util.concurrent.*;

import edu.utdallas.cometcannon.robot.command.*;

public class Networker implements Runnable
{
    final int YUN_PORT = 12313;
    final String YUN_ADDRESS = "192.168.240.4";

    ArrayBlockingQueue<Command> robotCommandQueue;

    Socket client;
    PrintStream output;
    DataInputStream input;

    public Networker(ArrayBlockingQueue<Command> robotCommandQueue)
    {
        this.robotCommandQueue = robotCommandQueue;
    }
    
    @Override
    public void run()
    {
        try {
            client = new Socket(YUN_ADDRESS, YUN_PORT);
            input = new DataInputStream(client.getInputStream());
            output = new PrintStream(client.getOutputStream());
        } catch (IOException e) {
            System.out.println(e);
        }
        
        while (true) {
            Command nextCommand = robotCommandQueue.poll();
            byte payload[] = nextCommand.generatePayload();
                
            try {
                output.write(payload); 
            } catch (IOException e) {
                System.out.println(e);
            }
        }
    }
}
