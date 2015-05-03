package edu.utdallas.cometcannon.robot;

import java.io.*;
import java.net.*;
import java.util.concurrent.*;

import edu.utdallas.cometcannon.robot.command.*;
import edu.utdallas.cometcannon.robot.response.*;

public class Networker implements Runnable
{
    final int YUN_PORT = 12313;
    final String YUN_ADDRESS = "192.168.240.1";

    ArrayBlockingQueue<Command> robotCommandQueue;
    ArrayBlockingQueue<Response> robotResponseQueue;

    Socket client;
    PrintStream output;
    DataInputStream input;

    public Networker(ArrayBlockingQueue<Command> robotCommandQueue,
                     ArrayBlockingQueue<Response> robotResponseQueue)
    {
        this.robotCommandQueue = robotCommandQueue;
        this.robotResponseQueue = robotResponseQueue;
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

        NetworkReader reader = new NetworkReader();
        Thread readerThread = new Thread(reader);
        readerThread.start();

        while (true) {
            try {
                Command nextCommand = robotCommandQueue.poll(1, TimeUnit.MILLISECONDS);

                if (nextCommand != null) {
                    byte payload[] = nextCommand.generatePayload();
                    output.write(payload);
                }
            } catch(IOException | InterruptedException e){
                e.printStackTrace();
            }

        }
    }

    class NetworkReader implements Runnable
    {
        @Override
        public void run()
        {
            while (true) {
                try {
                    byte[] buffer = new byte[1000];
                    int bytesRead = input.read(buffer);
                    byte[] payload = new byte[bytesRead];
                    System.arraycopy(buffer, 0, payload, 0, bytesRead);
                    robotResponseQueue.offer(Response.parse(payload), 1000, TimeUnit.DAYS);
                } catch (IOException | InterruptedException e) {
                    System.out.println(e);
                }
            }
        }
    }
}
